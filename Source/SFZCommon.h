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

#ifdef __APPLE__
typedef volatile int32_t atomic_t;
#endif

#ifdef _WIN32_
typedef volatile uint32 atomic_t;
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


//#define SFZ_NO_STREAMING 1


#endif
