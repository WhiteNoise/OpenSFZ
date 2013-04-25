//
//  Synthesizer.cpp
//  OpenSFZ
//
//  Created by David Wallin on 4/22/13.
//  Copyright (c) 2013 David Wallin. All rights reserved.
//

#include "Synthesizer.h"

SynthesizerVoice::SynthesizerVoice()
{
    sampleRate = 44100.0f;
    
    currentlyPlayingNote = -1;
    noteOnTime = 0;
    currentlyPlayingSound = 0;
    keyIsDown = false; // the voice may still be playing when the key is not down (i.e. sustain pedal)
    sostenutoPedalDown = false;
}

void SynthesizerVoice::setCurrentPlaybackSampleRate (double newRate)
{
    
}

void SynthesizerVoice::clearCurrentNote()
{
    currentlyPlayingNote = -1;
    currentlyPlayingSound = nullptr;
}

