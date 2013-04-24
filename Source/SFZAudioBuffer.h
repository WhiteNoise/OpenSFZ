//
//  SFZAudioBuffer.h
//  OpenSFZ
//
//  Created by David Wallin on 4/22/13.
//  Copyright (c) 2013 David Wallin. All rights reserved.
//

#ifndef __OpenSFZ__SFZAudioBuffer__
#define __OpenSFZ__SFZAudioBuffer__


class SFZAudioBuffer
{
public:
    SFZAudioBuffer(const int numChannels_, const int numSamples_);
    float *channels[2];
    
    int getNumSamples() { return numSamples; };
    int getNumChannels() { return numChannels; };
private:
    int numSamples;
    int numChannels;

};

         




#endif /* defined(__OpenSFZ__SFZAudioBuffer__) */
