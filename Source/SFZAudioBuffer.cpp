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
    int c = numChannels_;
    
    if(c > 2)
        c = 2;
    
    for(int i=0; i<c; i++)
    {
        channels[i] = new float[numSamples_];
    }
}