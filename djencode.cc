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
*  Purpose: singleton class that registers encoders for all supported JPEG-2000 processes.
*
*/

#include "dcmtk/config/osconfig.h"
#include "djencode.h"
#include "dcmtk/dcmdata/dccodec.h"  /* for DcmCodecStruct */
#include "djcparam.h"
#include "djcodece.h"

// initialization of static members
OFBool FMJP2KEncoderRegistration::registered_                             = OFFalse;
DJP2KCodecParameter *FMJP2KEncoderRegistration::cp_                        = NULL;
DJP2KLosslessEncoder *FMJP2KEncoderRegistration::losslessencoder_          = NULL;
DJP2KNearLosslessEncoder *FMJP2KEncoderRegistration::nearlosslessencoder_  = NULL;


void FMJP2KEncoderRegistration::registerCodecs(
	OFBool jp2k_optionsEnabled,
	Uint16 jp2k_cblkwidth,
	Uint16 jp2k_cblkheight,
	OFBool preferCookedEncoding,
	Uint32 fragmentSize,
	OFBool createOffsetTable,
	J2K_UIDCreation uidCreation,
	OFBool convertToSC)
{
	if (! registered_)
	{
		cp_ = new DJP2KCodecParameter(jp2k_optionsEnabled, jp2k_cblkwidth, jp2k_cblkheight,
			preferCookedEncoding, fragmentSize, createOffsetTable, uidCreation, 
			convertToSC, EJ2KPC_restore, OFFalse);

		if (cp_)
		{
			losslessencoder_ = new DJP2KLosslessEncoder();
			if (losslessencoder_) DcmCodecList::registerCodec(losslessencoder_, NULL, cp_);
			nearlosslessencoder_ = new DJP2KNearLosslessEncoder();
			if (nearlosslessencoder_) DcmCodecList::registerCodec(nearlosslessencoder_, NULL, cp_);
			registered_ = OFTrue;
		}
	}
}

void FMJP2KEncoderRegistration::cleanup()
{
	if (registered_)
	{
		DcmCodecList::deregisterCodec(losslessencoder_);
		DcmCodecList::deregisterCodec(nearlosslessencoder_);
		delete losslessencoder_;
		delete nearlosslessencoder_;
		delete cp_;
		registered_ = OFFalse;
#ifdef DEBUG
		// not needed but useful for debugging purposes
		losslessencoder_ = NULL;
		nearlosslessencoder_ = NULL;
		cp_     = NULL;
#endif
	}
}

OFString FMJP2KEncoderRegistration::getLibraryVersionString()
{
	return FMJPEG2K_JPEG_VERSION_STRING;
}
