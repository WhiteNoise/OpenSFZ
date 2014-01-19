//
//  SFZAudioBuffer.cpp
//  OpenSFZ
//
//  Created by David Wallin on 4/22/13.
//  Copyright (c) 2013 David Wallin. All rights reserved.
//

#include "SFZAudioBuffer.h"


SFZAudioBuffer::SFZAudioBuffer(const int numChannels_, const unsigned int numSamples_)
{
    bufferSize = 0;
    numSamples = 0;
    numChannels = 0;
    owned = false;
    
    channels[0] = 0;
    channels[1] = 0;
    channelPtr[0] = 0;
    channelPtr[1] = 0;

    
    bufferSize = numSamples = numSamples_;

    if(numSamples < 0 || numSamples > 158760000)
    {

        sfzDebugPrint("ERROR with sample size\n");

        owned = false;
        return;
    }
    
    int c = numChannels_;
    
    if(c > 2)
        c = 2;
    
    numChannels = c;
    
    for(int i=0; i<numChannels; i++)
    {
        channelPtr[i] = channels[i] = new float[numSamples + 32];
        
        for(unsigned int j=0; j< numSamples + 32; j++)
            channels[i][j] = 0.0f;
        
        channels[i] += 16;
    }
    owned = true;
}

SFZAudioBuffer::SFZAudioBuffer(const SFZAudioBuffer &other)
{

    owned = false;
    
    numChannels = other.numChannels;
    numSamples = other.numSamples;
    
    for(int i=0; i<numChannels; i++)
    {
        channelPtr[i] = channels[i] = other.channels[i];
    }
    
    bufferSize = numSamples;
    
}

SFZAudioBuffer::~SFZAudioBuffer()
{
    if(owned)
    {
        for(int i=0; i<numChannels; i++)
        {
            delete channelPtr[i];
            channelPtr[i] = 0;
            channels[i] = 0;
        }
        
        numChannels = 0;
    }
}


void SFZAudioBuffer::clear()
{
    for(int i=0; i<numChannels; i++)
    {
        memset(channels[i], 0, sizeof(float) * bufferSize);
    }
}

SFZAudioBuffer::SFZAudioBuffer(const unsigned int numSamples_, float *channel1, float *channel2)
{
    owned = false;
    numChannels = 2;
    bufferSize = numSamples = numSamples_;
    channels[0] = channel1;
    channels[1] = channel2;
}

void SFZAudioBuffer::initializeWith(const SFZAudioBuffer &other)
{
    // keep our buffer the same size.

    clear();
    
    assert(numChannels == other.numChannels);
    

    
    numSamples = other.numSamples;
    
    if(numSamples > bufferSize)
    {
        numSamples = bufferSize;
    }
    
    
    for(unsigned int i=0; i<numChannels; i++)
    {

        for(unsigned int j=0; j<numSamples; j++)
        {
            channels[i][j] = other.channels[i][j];

        }
    }
    
    
}

void SFZAudioBuffer::setNumSamples(atomic_t n)
{
    numSamples = n;
}