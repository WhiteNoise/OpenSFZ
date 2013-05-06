//
//  SFZAudioReaderManager.h
//  OpenSFZ
//
//  Created by David Wallin on 4/28/13.
//  Copyright (c) 2013 David Wallin. All rights reserved.
//

#pragma once

#include "SFZCommon.h"

#ifndef SFZ_NO_STREAMING

class SFZBaseAudioReader;
class SFZAudioBuffer;

class SFZAudioReaderManager : NonCopyable
{
    
public:
    // Creates or gets the instance
    static SFZAudioReaderManager *getInstance();
    static void destroyInstance();
    
    // Start reading from this reader..
    void addReader(SFZBaseAudioReader *r);
    void releaseReader(SFZBaseAudioReader *r);
    
    // process any readers.. 
    void process();
    
    // attempt to estimate how much memory we're using
    atomic_t getMemoryUsage() { return memoryUsage; };
    
    static SFZBaseAudioReader *createReader(const std::string &extension);
private:
    atomic_t memoryUsage;
    SFZAudioReaderManager();
    ~SFZAudioReaderManager();
    
    LockFreeFifo<SFZBaseAudioReader *> newReaders;
    LockFreeFifo<SFZBaseAudioReader *> freedReaders;
    
    std::vector<SFZBaseAudioReader *> readers;
    std::vector<SFZBaseAudioReader *> completeReaders;
    
    static SFZAudioReaderManager *instance;
};
#endif