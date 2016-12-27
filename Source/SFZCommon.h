//
//  OpenSFZ.h
//  OpenSFZ
//
//  Created by David Wallin on 4/22/13.
//  Copyright (c) 2013 David Wallin. All rights reserved.
//

#ifndef OpenSFZ_OpenSFZ_h
#define OpenSFZ_OpenSFZ_h

#define VORBIS 1

#include <string>
#include <vector>
#include <map>

#include <stdio.h>
#include <Math.h>

#include <fstream>
#include <sstream>

#include <assert.h>

#include "stb_vorbis.h"


typedef std::atomic_int atomic_t;

#ifdef __APPLE__

#include "TargetConditionals.h"

#if TARGET_OS_IPHONE
#define SFZ_LOWMEMORY 1

#elif TARGET_IPHONE_SIMULATOR
#define SFZ_LOWMEMORY 1

#elif TARGET_OS_MAC
// Other kinds of Mac OS
#else
// Unsupported platform
#endif

#endif


typedef long int64;

#include "InputStream.h"
#include "SFZAudioBuffer.h"
#include "SFZAudioReader.h"
#include "Path.h"
#include "NonCopyable.h"
#include "LockFreeFifo.h"


#include "Synthesizer.h"




extern float decibelsToGain(float db);
extern float getMidiNoteInHertz(int noteNumber);
extern void print_free_memory ();
unsigned int getFreeMemory();
extern void sfzDebugPrint(const char *sz);

//#define SFZ_NO_STREAMING 1


#endif
