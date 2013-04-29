//
//  SFZAudioReaderManager.h
//  OpenSFZ
//
//  Created by David Wallin on 4/28/13.
//  Copyright (c) 2013 David Wallin. All rights reserved.
//

#pragma once

#include "SFZCommon.h"

class SFZAudioReader;
class SFZAudioBuffer;

class SFZAudioReaderManager : NonCopyable
{
    
public:
    // Creates or gets the instance
    static SFZAudioReaderManager *getInstance();
    static void destroyInstance();
    
    // Start reading from this reader..
    void addReader(SFZAudioReader *r);
    void releaseReader(SFZAudioReader *r);
    
    // process any readers.. 
    void process();
    
private:
    SFZAudioReaderManager();
    ~SFZAudioReaderManager();
    
    LockFreeFifo<SFZAudioReader *> newReaders;
    LockFreeFifo<SFZAudioReader *> freedReaders;
    
    std::vector<SFZAudioReader *> readers;
    std::vector<SFZAudioReader *> completeReaders;
    
    static SFZAudioReaderManager *instance;
};