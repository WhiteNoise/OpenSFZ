//
//  SFZAudioBuffer.h
//  OpenSFZ
//
//  Created by David Wallin on 4/22/13.
//  Copyright (c) 2013 David Wallin. All rights reserved.
//
//  Audio buffer class for loading samples and generating output into. Currently only supports
//  1 or 2 channels.

#pragma once

// SFZSample creates a reader and passes it to the AudioManager to have it filled. It's passed via lock free fifo.
// We mark the reader so that it only has a small 'preload' buffer'. This preload buffer is always a fixed size.
// The audio manager now owns this memory.
// When the sample is actually played, we create another reader using the currently loaded buffer and ask that to be filed
// the rest of the way. This is the buffer use for playback now.
// SFZSample calls 'release reader' on the first reader. When the AudioManager gets a turn, it will release this memory and close any files.
// When SFZSample is destroyed it calls 'release reader' on it's current reader. When the audio manager gets a turn it releases it.


#include "SFZCommon.h"

class SFZAudioBuffer
{
public:
    SFZAudioBuffer(const int numChannels_, unsigned int numSamples_);
    
    SFZAudioBuffer(unsigned int numSamples_, float *channel1, float *channel2);
    
    // non-owned reference to the other buffer (not unique memory)
    SFZAudioBuffer(const SFZAudioBuffer &other);
    ~SFZAudioBuffer();
    
    float *channels[2];
    float *channelPtr[2];
    
    unsigned int getNumSamples() { return numSamples; };
    int getNumChannels() { return numChannels; };
    
    void setOwned(bool b) { owned = b; };
    
    void initializeWith(const SFZAudioBuffer &other);
    
    void setNumSamples(atomic_t n);
    
    void clear();
private:
    // actual buffer size
    atomic_t bufferSize;
    
    // number of samples currently in the buffer.
    atomic_t numSamples;

    int numChannels;
    
    // should we delete the buffer memory when SFZAudioBuffer is destroyed?
    bool owned;
    
    

};

         


