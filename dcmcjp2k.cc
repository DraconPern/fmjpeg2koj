/*
 *
 *  Copyright (C) 2007-2014, OFFIS e.V.
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation were developed by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *
 *  Module:  dcmcjpls
 *
 *  Author:  Martin Willkomm, Uli Schlachter
 *
 *  Purpose: Compress DICOM file with JPEG-LS transfer syntax
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include "dcmtk/ofstd/ofstdinc.h"

#ifdef HAVE_GUSI_H
#include <GUSI.h>
#endif

#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmdata/cmdlnarg.h"
#include "dcmtk/ofstd/ofconapp.h"
#include "dcmtk/dcmdata/dcuid.h"      /* for dcmtk version name */
#include "dcmtk/dcmimage/diregist.h"  /* include to support color images */
#include "fmjpeg2k/djencode.h"   /* for class DJLSEncoderRegistration */
#include "fmjpeg2k/djrparam.h"   /* for class FMJP2KRepresentationParameter */

#ifdef USE_LICENSE_FILE
#include "oflice.h"
#endif

#ifndef OFFIS_CONSOLE_APPLICATION
#define OFFIS_CONSOLE_APPLICATION "fmcjp2k"
#endif

static OFLogger fmjp2kLogger = OFLog::getLogger(OFFIS_CONSOLE_APPLICATION);

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";

// ********************************************


#define SHORTCOL 3
#define LONGCOL 21

int main(int argc, char *argv[])
{

#ifdef HAVE_GUSI_H
  GUSISetup(GUSIwithSIOUXSockets);
  GUSISetup(GUSIwithInternetSockets);
#endif

  const char *opt_ifname = NULL;
  const char *opt_ofname = NULL;

  // input options
  E_FileReadMode opt_readMode = ERM_autoDetect;
  E_TransferSyntax opt_ixfer = EXS_Unknown;

  // JPEG-LS encoding options
  E_TransferSyntax opt_oxfer = EXS_JPEG2000LosslessOnly;
  OFBool opt_useLosslessProcess = OFTrue;

  OFCmdUnsignedInt opt_cblkwidth = 64;
  OFCmdUnsignedInt opt_cblkheight = 64;  

  OFBool opt_use_custom_options = OFFalse;

  // JPEG-LS options
  OFCmdUnsignedInt opt_nearlossless_psnr = 0;
  OFBool opt_prefer_cooked = OFFalse;
  
  // encapsulated pixel data encoding options
  OFCmdUnsignedInt opt_fragmentSize = 0; // 0=unlimited
  OFBool           opt_createOffsetTable = OFTrue;
  J2K_UIDCreation  opt_uidcreation = EJ2KUC_default;
  OFBool           opt_secondarycapture = OFFalse;

  // output options
  E_GrpLenEncoding opt_oglenc = EGL_recalcGL;
  E_EncodingType opt_oenctype = EET_ExplicitLength;
  E_PaddingEncoding opt_opadenc = EPD_noChange;
  OFCmdUnsignedInt opt_filepad = 0;
  OFCmdUnsignedInt opt_itempad = 0;

#ifdef USE_LICENSE_FILE
LICENSE_FILE_DECLARATIONS
#endif

  OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION, "Encode DICOM file to JPEG-LS transfer syntax", rcsid);
  OFCommandLine cmd;
  cmd.setOptionColumns(LONGCOL, SHORTCOL);
  cmd.setParamColumn(LONGCOL + SHORTCOL + 4);

  cmd.addParam("dcmfile-in",  "DICOM input filename to be converted");
  cmd.addParam("dcmfile-out", "DICOM output filename");

  cmd.addGroup("general options:", LONGCOL, SHORTCOL + 2);
    cmd.addOption("--help",                     "-h",     "print this help text and exit", OFCommandLine::AF_Exclusive);
    cmd.addOption("--version",                            "print version information and exit", OFCommandLine::AF_Exclusive);
    OFLog::addOptions(cmd);

#ifdef USE_LICENSE_FILE
LICENSE_FILE_DECLARE_COMMAND_LINE_OPTIONS
#endif

  cmd.addGroup("input options:");
    cmd.addSubGroup("input file format:");
      cmd.addOption("--read-file",              "+f",     "read file format or data set (default)");
      cmd.addOption("--read-file-only",         "+fo",    "read file format only");
      cmd.addOption("--read-dataset",           "-f",     "read data set without file meta information");
    cmd.addSubGroup("input transfer syntax:");
      cmd.addOption("--read-xfer-auto",         "-t=",    "use TS recognition (default)");
      cmd.addOption("--read-xfer-detect",       "-td",    "ignore TS specified in the file meta header");
      cmd.addOption("--read-xfer-little",       "-te",    "read with explicit VR little endian TS");
      cmd.addOption("--read-xfer-big",          "-tb",    "read with explicit VR big endian TS");
      cmd.addOption("--read-xfer-implicit",     "-ti",    "read with implicit VR little endian TS");

  cmd.addGroup("JPEG-2000 encoding options:");
    cmd.addSubGroup("JPEG-2000 process:");
      cmd.addOption("--encode-lossless",        "+el",    "encode JPEG-2000 lossless (default)");
      cmd.addOption("--encode-nearlossless",    "+en",    "encode JPEG-2000 TS near-lossless");

    cmd.addSubGroup("lossless compression:");
      cmd.addOption("--prefer-raw",             "+pr",    "prefer raw encoder mode (default)");
      cmd.addOption("--prefer-cooked",          "+pc",    "prefer cooked encoder mode ");

    cmd.addSubGroup("JPEG-2000 compression:");
	  cmd.addOption("--PSNR",                   "+ps", 1, "[s]ize: integer (default: 0 (automatic))",
                                                          "set JPEG-2000 encoding parameter psnr");
      cmd.addOption("--cblkwidth",              "+cw", 1, "[s]ize: integer (default: 64)",
                                                          "set JPEG-2000 encoding parameter code block size");
      cmd.addOption("--cblkheight",             "+ch", 1, "[s]ize: integer (default: 64)",
                                                          "set JPEG-2000 encoding parameter code block size");

  cmd.addGroup("encapsulated pixel data encoding options:");
    cmd.addSubGroup("pixel data fragmentation:");
      cmd.addOption("--fragment-per-frame",     "+ff",    "encode each frame as one fragment (default)");
      cmd.addOption("--fragment-size",          "+fs", 1, "[s]ize: integer",
                                                          "limit fragment size to s kbytes");
    cmd.addSubGroup("basic offset table encoding:");
      cmd.addOption("--offset-table-create",    "+ot",    "create offset table (default)");
      cmd.addOption("--offset-table-empty",     "-ot",    "leave offset table empty");
    cmd.addSubGroup("SOP Class UID:");
      cmd.addOption("--class-default",          "+cd",    "keep SOP Class UID (default)");
      cmd.addOption("--class-sc",               "+cs",    "convert to Secondary Capture Image\n(implies --uid-always)");
    cmd.addSubGroup("SOP Instance UID:");
      cmd.addOption("--uid-default",            "+ud",    "assign new UID if lossy compression (default)");
      cmd.addOption("--uid-always",             "+ua",    "always assign new UID");
      cmd.addOption("--uid-never",             "+un",    "never assign new UID");

  cmd.addGroup("output options:");
    cmd.addSubGroup("post-1993 value representations:");
      cmd.addOption("--enable-new-vr",          "+u",     "enable support for new VRs (UN/UT) (default)");
      cmd.addOption("--disable-new-vr",         "-u",     "disable support for new VRs, convert to OB");
    cmd.addSubGroup("group length encoding:");
      cmd.addOption("--group-length-recalc",    "+g=",    "recalculate group lengths if present (default)");
      cmd.addOption("--group-length-create",    "+g",     "always write with group length elements");
      cmd.addOption("--group-length-remove",    "-g",     "always write without group length elements");
    cmd.addSubGroup("length encoding in sequences and items:");
      cmd.addOption("--length-explicit",        "+e",     "write with explicit lengths (default)");
      cmd.addOption("--length-undefined",       "-e",     "write with undefined lengths");
    cmd.addSubGroup("data set trailing padding:");
      cmd.addOption("--padding-retain",         "-p=",    "do not change padding (default)");
      cmd.addOption("--padding-off",            "-p",     "no padding");
      cmd.addOption("--padding-create",         "+p",  2, "[f]ile-pad [i]tem-pad: integer",
                                                          "align file on multiple of f bytes\nand items on multiple of i byte");
    /* evaluate command line */
    prepareCmdLineArgs(argc, argv, OFFIS_CONSOLE_APPLICATION);
    if (app.parseCommandLine(cmd, argc, argv))
    {
      /* check exclusive options first */
      if (cmd.hasExclusiveOption())
      {
        if (cmd.findOption("--version"))
        {
          app.printHeader(OFTrue /*print host identifier*/);
          COUT << OFendl << "External libraries used:" << OFendl;
          COUT << "- " << FMJPEG2KEncoderRegistration::getLibraryVersionString() << OFendl;
          return 0;
        }
      }

      /* command line parameters */

      cmd.getParam(1, opt_ifname);
      cmd.getParam(2, opt_ofname);

      // general options
      OFLog::configureFromCommandLine(cmd, app);

#ifdef USE_LICENSE_FILE
LICENSE_FILE_EVALUATE_COMMAND_LINE_OPTIONS
#endif

      // input options
      // input file format
      cmd.beginOptionBlock();
      if (cmd.findOption("--read-file")) opt_readMode = ERM_autoDetect;
      if (cmd.findOption("--read-file-only")) opt_readMode = ERM_fileOnly;
      if (cmd.findOption("--read-dataset")) opt_readMode = ERM_dataset;
      cmd.endOptionBlock();

      // input transfer syntax
      cmd.beginOptionBlock();
      if (cmd.findOption("--read-xfer-auto"))
        opt_ixfer = EXS_Unknown;
      if (cmd.findOption("--read-xfer-detect"))
        dcmAutoDetectDatasetXfer.set(OFTrue);
      if (cmd.findOption("--read-xfer-little"))
      {
        app.checkDependence("--read-xfer-little", "--read-dataset", opt_readMode == ERM_dataset);
        opt_ixfer = EXS_LittleEndianExplicit;
      }
      if (cmd.findOption("--read-xfer-big"))
      {
        app.checkDependence("--read-xfer-big", "--read-dataset", opt_readMode == ERM_dataset);
        opt_ixfer = EXS_BigEndianExplicit;
      }
      if (cmd.findOption("--read-xfer-implicit"))
      {
        app.checkDependence("--read-xfer-implicit", "--read-dataset", opt_readMode == ERM_dataset);
        opt_ixfer = EXS_LittleEndianImplicit;
      }
      cmd.endOptionBlock();

      // JPEG-LS encoding options
      // JPEG-LS process options
      cmd.beginOptionBlock();
      if (cmd.findOption("--encode-lossless"))
      {
        opt_oxfer = EXS_JPEG2000LosslessOnly;
        opt_useLosslessProcess = OFTrue;
      }
      if (cmd.findOption("--encode-nearlossless"))
      {
        opt_oxfer = EXS_JPEG2000;
        opt_useLosslessProcess = OFFalse;
        opt_nearlossless_psnr = 0; // 0 for automatic
      }
      cmd.endOptionBlock();

      // JPEG-2000 psnr options
      if (cmd.findOption("--PSNR"))
      {
        app.checkConflict("--PSNR", "--encode-lossless", opt_oxfer == EXS_JPEG2000LosslessOnly);
        app.checkValue(cmd.getValueAndCheckMin(opt_nearlossless_psnr, 0));
      }

      // lossless compression options
      cmd.beginOptionBlock();
      if (cmd.findOption("--prefer-raw"))
      {
        opt_prefer_cooked = OFFalse;
      }
      if (cmd.findOption("--prefer-cooked"))
      {
        opt_prefer_cooked = OFTrue;
      }
      cmd.endOptionBlock();

      // thresholds and reset
      cmd.beginOptionBlock();
      if (cmd.findOption("--cblkwidth"))
      {
        app.checkValue(cmd.getValueAndCheckMinMax(opt_cblkwidth, OFstatic_cast(OFCmdUnsignedInt, 4), 1024));
        opt_use_custom_options = OFTrue;
      }
      if (cmd.findOption("--cblkheight"))
      {
        app.checkValue(cmd.getValueAndCheckMinMax(opt_cblkheight, OFstatic_cast(OFCmdUnsignedInt, 4), 1024));
        opt_use_custom_options = OFTrue;
      }      
      cmd.endOptionBlock();

      // encapsulated pixel data encoding options
      // pixel data fragmentation options
      cmd.beginOptionBlock();
      if (cmd.findOption("--fragment-per-frame")) opt_fragmentSize = 0;
      if (cmd.findOption("--fragment-size"))
      {
        app.checkValue(cmd.getValueAndCheckMin(opt_fragmentSize, OFstatic_cast(OFCmdUnsignedInt, 1)));
      }
      cmd.endOptionBlock();

      // basic offset table encoding options
      cmd.beginOptionBlock();
      if (cmd.findOption("--offset-table-create")) opt_createOffsetTable = OFTrue;
      if (cmd.findOption("--offset-table-empty")) opt_createOffsetTable = OFFalse;
      cmd.endOptionBlock();

      // SOP Class UID options
      cmd.beginOptionBlock();
      if (cmd.findOption("--class-default")) opt_secondarycapture = OFFalse;
      if (cmd.findOption("--class-sc")) opt_secondarycapture = OFTrue;
      cmd.endOptionBlock();

      // SOP Instance UID options
      cmd.beginOptionBlock();
      if (cmd.findOption("--uid-default")) opt_uidcreation = EJ2KUC_default;
      if (cmd.findOption("--uid-always")) opt_uidcreation = EJ2KUC_always;
      if (cmd.findOption("--uid-never")) opt_uidcreation = EJ2KUC_never;
      cmd.endOptionBlock();

      // output options
      // post-1993 value representations
      cmd.beginOptionBlock();
      if (cmd.findOption("--enable-new-vr")) dcmEnableGenerationOfNewVRs();
      if (cmd.findOption("--disable-new-vr")) dcmDisableGenerationOfNewVRs();
      cmd.endOptionBlock();

      // group length encoding
      cmd.beginOptionBlock();
      if (cmd.findOption("--group-length-recalc")) opt_oglenc = EGL_recalcGL;
      if (cmd.findOption("--group-length-create")) opt_oglenc = EGL_withGL;
      if (cmd.findOption("--group-length-remove")) opt_oglenc = EGL_withoutGL;
      cmd.endOptionBlock();

      // length encoding in sequences and items
      cmd.beginOptionBlock();
      if (cmd.findOption("--length-explicit")) opt_oenctype = EET_ExplicitLength;
      if (cmd.findOption("--length-undefined")) opt_oenctype = EET_UndefinedLength;
      cmd.endOptionBlock();

      // data set trailing padding
      cmd.beginOptionBlock();
      if (cmd.findOption("--padding-retain")) opt_opadenc = EPD_noChange;
      if (cmd.findOption("--padding-off")) opt_opadenc = EPD_withoutPadding;
      if (cmd.findOption("--padding-create"))
      {
        app.checkValue(cmd.getValueAndCheckMin(opt_filepad, 0));
        app.checkValue(cmd.getValueAndCheckMin(opt_itempad, 0));
        opt_opadenc = EPD_withPadding;
      }
      cmd.endOptionBlock();
    }

    /* print resource identifier */
    OFLOG_DEBUG(fmjp2kLogger, rcsid << OFendl);

    // register global compression codecs
    FMJPEG2KEncoderRegistration::registerCodecs(opt_use_custom_options,
      OFstatic_cast(Uint16, opt_cblkwidth), OFstatic_cast(Uint16, opt_cblkheight),
      opt_prefer_cooked, opt_fragmentSize, opt_createOffsetTable,
      opt_uidcreation, opt_secondarycapture);

    /* make sure data dictionary is loaded */
    if (!dcmDataDict.isDictionaryLoaded())
    {
        OFLOG_WARN(fmjp2kLogger, "no data dictionary loaded, "
           << "check environment variable: "
           << DCM_DICT_ENVIRONMENT_VARIABLE);
    }

    // open inputfile
    if ((opt_ifname == NULL) || (strlen(opt_ifname) == 0))
    {
      OFLOG_FATAL(fmjp2kLogger, "invalid filename: <empty string>");
      return 1;
    }

    OFLOG_INFO(fmjp2kLogger, "reading input file " << opt_ifname);

    DcmFileFormat fileformat;
    OFCondition error = fileformat.loadFile(opt_ifname, opt_ixfer, EGL_noChange, DCM_MaxReadLength, opt_readMode);
    if (error.bad())
    {
      OFLOG_FATAL(fmjp2kLogger, error.text() << ": reading file: " <<  opt_ifname);
      return 1;
    }
    DcmDataset *dataset = fileformat.getDataset();

    DcmXfer original_xfer(dataset->getOriginalXfer());
    if (original_xfer.isEncapsulated())
    {
      OFLOG_INFO(fmjp2kLogger, "DICOM file is already compressed, converting to uncompressed transfer syntax first");
      if (EC_Normal != dataset->chooseRepresentation(EXS_LittleEndianExplicit, NULL))
      {
        OFLOG_FATAL(fmjp2kLogger, "No conversion from compressed original to uncompressed transfer syntax possible!");
        return 1;
      }
    }

    OFString sopClass;
    if (fileformat.getMetaInfo()->findAndGetOFString(DCM_MediaStorageSOPClassUID, sopClass).good())
    {
        /* check for DICOMDIR files */
        if (sopClass == UID_MediaStorageDirectoryStorage)
        {
            OFLOG_FATAL(fmjp2kLogger, "DICOMDIR files (Media Storage Directory Storage SOP Class) cannot be compressed!");
            return 1;
        }
    }

    OFLOG_INFO(fmjp2kLogger, "Convert DICOM file to compressed transfer syntax");

    //create representation parameter
    FMJPEG2KRepresentationParameter rp(OFstatic_cast(Uint16, opt_nearlossless_psnr), opt_useLosslessProcess);
    DcmXfer opt_oxferSyn(opt_oxfer);

    // perform decoding process
    OFCondition result = dataset->chooseRepresentation(opt_oxfer, &rp);
    if (result.bad())
    {
      OFLOG_FATAL(fmjp2kLogger, result.text() << ": encoding file: " << opt_ifname);
      return 1;
    }
    if (dataset->canWriteXfer(opt_oxfer))
    {
      OFLOG_INFO(fmjp2kLogger, "Output transfer syntax " << opt_oxferSyn.getXferName() << " can be written");
    } else {
      OFLOG_FATAL(fmjp2kLogger, "No conversion to transfer syntax " << opt_oxferSyn.getXferName() << " possible!");
      return 1;
    }

    OFLOG_INFO(fmjp2kLogger, "creating output file " << opt_ofname);

    fileformat.loadAllDataIntoMemory();
    error = fileformat.saveFile(opt_ofname, opt_oxfer, opt_oenctype, opt_oglenc, opt_opadenc,
      OFstatic_cast(Uint32, opt_filepad), OFstatic_cast(Uint32, opt_itempad), EWM_updateMeta);

    if (error.bad())
    {
      OFLOG_FATAL(fmjp2kLogger, error.text() << ": writing file: " <<  opt_ofname);
      return 1;
    }

    OFLOG_INFO(fmjp2kLogger, "conversion successful");

    // deregister global codecs
    FMJPEG2KEncoderRegistration::cleanup();

    return 0;
}
