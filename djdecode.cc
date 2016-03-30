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
#include "djdecode.h"
#include "dcmtk/dcmdata/dccodec.h"  /* for DcmCodecStruct */
#include "djcparam.h"
#include "djcodecd.h"

// initialization of static members
OFBool FMJP2KDecoderRegistration::registered_                            = OFFalse;
DJP2KCodecParameter *FMJP2KDecoderRegistration::cp_                       = NULL;
DJP2KDecoder *FMJP2KDecoderRegistration::decoder_         = NULL;

void FMJP2KDecoderRegistration::registerCodecs(
    J2K_UIDCreation uidcreation,
    J2K_PlanarConfiguration planarconfig,
    OFBool ignoreOffsetTable)
{
  if (! registered_)
  {
    cp_ = new DJP2KCodecParameter(uidcreation, planarconfig, ignoreOffsetTable);
    if (cp_)
    {
      decoder_ = new DJP2KDecoder();
      if (decoder_) DcmCodecList::registerCodec(decoder_, NULL, cp_);
    
      registered_ = OFTrue;
    }
  }
}

void FMJP2KDecoderRegistration::cleanup()
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

OFString FMJP2KDecoderRegistration::getLibraryVersionString()
{
    return FMJP2K_JPEG_VERSION_STRING;
}
