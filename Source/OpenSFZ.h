//
//  OpenSFZ.h
//  OpenSFZ
//
//  Created by David Wallin on 4/22/13.
//  Copyright (c) 2013 David Wallin. All rights reserved.
//

#ifndef OpenSFZ_OpenSFZ_h
#define OpenSFZ_OpenSFZ_h

#include <string>
#include <vector>
#include <map>

#include <stdio.h>

#include <fstream>

#include "Math.h"
#include "SFZAudioBuffer.h"
#include "Path.h"

#include "Synthesizer.h"

#define VORBIS 1

float decibelsToGain(float db)
{
    return powf(10.0f, db / 20.0f);
}

float getMidiNoteInHertz(int noteNumber)
{
    noteNumber -= 12 * 6 + 9; // now 0 = A
    return 440.0f * pow (2.0, noteNumber / 12.0);
}

typedef long int64;


#endif
