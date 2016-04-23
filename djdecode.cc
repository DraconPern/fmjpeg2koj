/*
 *
 *  Copyright (C) 2015, Ing-Long Eric Kuo
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation were developed by
 *
 *
 *
 *  Module:  fmjpeg2k
 *
 *  Author:  Ing-Long Eric Kuo
 *
 *  Purpose: singleton class that registers decoders for all supported JPEG-2000 processes.
 *
 */

#include "dcmtk/config/osconfig.h"
#include "fmjpeg2k/djdecode.h"
#include "dcmtk/dcmdata/dccodec.h"  /* for DcmCodecStruct */
#include "fmjpeg2k/djcparam.h"
#include "fmjpeg2k/djcodecd.h"

// initialization of static members
OFBool FMJPEG2KDecoderRegistration::registered_                            = OFFalse;
DJPEG2KCodecParameter *FMJPEG2KDecoderRegistration::cp_                       = NULL;
DJPEG2KDecoder *FMJPEG2KDecoderRegistration::decoder_         = NULL;

void FMJPEG2KDecoderRegistration::registerCodecs(
    J2K_UIDCreation uidcreation,
    J2K_PlanarConfiguration planarconfig,
    OFBool ignoreOffsetTable)
{
  if (! registered_)
  {
    cp_ = new DJPEG2KCodecParameter(uidcreation, planarconfig, ignoreOffsetTable);
    if (cp_)
    {
      decoder_ = new DJPEG2KDecoder();
      if (decoder_) DcmCodecList::registerCodec(decoder_, NULL, cp_);
    
      registered_ = OFTrue;
    }
  }
}

void FMJPEG2KDecoderRegistration::cleanup()
{
  if (registered_)
  {
    DcmCodecList::deregisterCodec(decoder_);    
    delete decoder_;    
    delete cp_;
    registered_ = OFFalse;
#ifdef DEBUG
    // not needed but useful for debugging purposes
    decoder_ = NULL;    
    cp_      = NULL;
#endif
  }
}

OFString FMJPEG2KDecoderRegistration::getLibraryVersionString()
{
    return FMJPEG2K_JPEG_VERSION_STRING;
}
