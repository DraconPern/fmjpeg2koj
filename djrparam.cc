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
#include "djrparam.h"
#include "dcmtk/ofstd/ofstd.h"

FMJP2KRepresentationParameter::FMJP2KRepresentationParameter(
    Uint16 nearlosslessPSNR,
    OFBool losslessProcess)
: DcmRepresentationParameter()
, nearlosslessPSNR_(nearlosslessPSNR)
, losslessProcess_(losslessProcess)
{
}

FMJP2KRepresentationParameter::FMJP2KRepresentationParameter(const FMJP2KRepresentationParameter& arg)
: DcmRepresentationParameter(arg)
, nearlosslessPSNR_(arg.nearlosslessPSNR_)
, losslessProcess_(arg.losslessProcess_)
{
}

FMJP2KRepresentationParameter::~FMJP2KRepresentationParameter()
{
}  

DcmRepresentationParameter *FMJP2KRepresentationParameter::clone() const
{
  return new FMJP2KRepresentationParameter(*this);
}

const char *FMJP2KRepresentationParameter::className() const
{
  return "FMJP2KRepresentationParameter";
}

OFBool FMJP2KRepresentationParameter::operator==(const DcmRepresentationParameter &arg) const
{
  const char *argname = arg.className();
  if (argname)
  {
    OFString argstring(argname);
    if (argstring == className())
    {
      const FMJP2KRepresentationParameter& argll = OFreinterpret_cast(const FMJP2KRepresentationParameter &, arg);
      if (losslessProcess_ && argll.losslessProcess_) return OFTrue;
      else if (losslessProcess_ != argll.losslessProcess_) return OFFalse;
	  else if (nearlosslessPSNR_ != argll.nearlosslessPSNR_) return OFFalse;
      return OFTrue;
    }	
  }
  return OFFalse;
}
