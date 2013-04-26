//
//  SFZAudioBuffer.cpp
//  OpenSFZ
//
//  Created by David Wallin on 4/22/13.
//  Copyright (c) 2013 David Wallin. All rights reserved.
//

#include "SFZAudioBuffer.h"


SFZAudioBuffer::SFZAudioBuffer(const int numChannels_, const int numSamples_)
{
    numSamples = numSamples_;

    int c = numChannels_;
    
    if(c > 2)
        c = 2;
    
    numChannels = c;
    
    for(int i=0; i<numChannels; i++)
    {
        channels[i] = new float[numSamples_];
    }
    owned = true;
}

SFZAudioBuffer::~SFZAudioBuffer()
{
    if(owned)
    {
        for(int i=0; i<numChannels; i++)
        {
            delete channels[i];
            channels[i] = 0;
        }
    }
}

SFZAudioBuffer::SFZAudioBuffer(const int numSamples_, float *channel1, float *channel2)
{
    owned = false;
    numChannels = 2;
    numSamples = numSamples_;
    channels[0] = channel1;
    channels[1] = channel2;
}