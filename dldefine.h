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
 *  Purpose: Contains preprocessor definitions
 *
 */


#ifndef DL2KDEFINE_H
#define DL2KDEFINE_H

#include "dcmtk/config/osconfig.h"

#include "dcmtk/ofstd/ofdefine.h"


#ifdef fmjp2k_EXPORTS
#define FMJP2K_EXPORT DCMTK_DECL_EXPORT
#else
#define FMJP2K_EXPORT DCMTK_DECL_IMPORT
#endif


#endif
