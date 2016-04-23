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
 *  Purpose: representation parameter for JPEG-2000
 *
 */

#include "dcmtk/config/osconfig.h"
#include "fmjpeg2k/djrparam.h"
#include "dcmtk/ofstd/ofstd.h"

FMJPEG2KRepresentationParameter::FMJPEG2KRepresentationParameter(
    Uint16 nearlosslessPSNR,
    OFBool losslessProcess)
: DcmRepresentationParameter()
, nearlosslessPSNR_(nearlosslessPSNR)
, losslessProcess_(losslessProcess)
{
}

FMJPEG2KRepresentationParameter::FMJPEG2KRepresentationParameter(const FMJPEG2KRepresentationParameter& arg)
: DcmRepresentationParameter(arg)
, nearlosslessPSNR_(arg.nearlosslessPSNR_)
, losslessProcess_(arg.losslessProcess_)
{
}

FMJPEG2KRepresentationParameter::~FMJPEG2KRepresentationParameter()
{
}  

DcmRepresentationParameter *FMJPEG2KRepresentationParameter::clone() const
{
  return new FMJPEG2KRepresentationParameter(*this);
}

const char *FMJPEG2KRepresentationParameter::className() const
{
  return "FMJP2KRepresentationParameter";
}

OFBool FMJPEG2KRepresentationParameter::operator==(const DcmRepresentationParameter &arg) const
{
  const char *argname = arg.className();
  if (argname)
  {
    OFString argstring(argname);
    if (argstring == className())
    {
      const FMJPEG2KRepresentationParameter& argll = OFreinterpret_cast(const FMJPEG2KRepresentationParameter &, arg);
      if (losslessProcess_ && argll.losslessProcess_) return OFTrue;
      else if (losslessProcess_ != argll.losslessProcess_) return OFFalse;
	  else if (nearlosslessPSNR_ != argll.nearlosslessPSNR_) return OFFalse;
      return OFTrue;
    }	
  }
  return OFFalse;
}
