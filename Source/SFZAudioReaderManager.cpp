//
//  SFZAudioReaderManager.cpp
//  OpenSFZ
//
//  Created by David Wallin on 4/28/13.
//  Copyright (c) 2013 David Wallin. All rights reserved.
//

#include "SFZAudioReaderManager.h"
#include <algorithm>

SFZAudioReaderManager *SFZAudioReaderManager::instance = 0;

// Creates or gets the instance
SFZAudioReaderManager *SFZAudioReaderManager::getInstance()
{
    if(!instance)
    {
        instance = new SFZAudioReaderManager();
    }
    
    return instance;
}

void SFZAudioReaderManager::destroyInstance()
{
    if(instance)
    {
        delete instance;
    }
    
}

// Start reading from this reader..
void SFZAudioReaderManager::addReader(SFZBaseAudioReader *r)
{
    if(newReaders.canWrite())
    {
        newReaders.put(r);
    }
}

void SFZAudioReaderManager::releaseReader(SFZBaseAudioReader *r)
{
    if(freedReaders.canWrite())
        freedReaders.put(r);
}

// process any readers..
void SFZAudioReaderManager::process()
{
    while(freedReaders.canRead())
    {
        SFZBaseAudioReader *r = freedReaders.get();
        
        atomic_t _memUsage = memoryUsage;
        _memUsage -= r->buffer->getBufferSize() * 4 * r->getNumChannels();
        memoryUsage = _memUsage;
        
        r->closeStream();
        
        std::vector<SFZBaseAudioReader *>::iterator iter = find(readers.begin(), readers.end(), r);
                
        if(iter != readers.end())
            readers.erase(iter);
        
        iter = find(completeReaders.begin(), completeReaders.end(), r);
        
        if(iter != completeReaders.end())
            completeReaders.erase(iter);

        delete r;
    }
    
    while(newReaders.canRead())
    {
        SFZBaseAudioReader *r = newReaders.get();
        
        if(r->beginLoad())
        {
            r->stream();
            readers.push_back(r);
            
            atomic_t _memUsage = memoryUsage;
            
            _memUsage += r->buffer->getBufferSize() * 4 * r->getNumChannels();
            
            memoryUsage = _memUsage;
            
            
        } else {
            // stream did not load
            sfzDebugPrint("Error, stream didn't load");
            printf(r->getPath().c_str());
            r->closeStream();
            completeReaders.push_back(r);
        }
    }
    
    std::vector<SFZBaseAudioReader *>::iterator iter2;
    
    for(iter2 = readers.begin(); iter2 != readers.end(); )
    {
        if((*iter2)->isStreamFinished())
        {
            (*iter2)->closeStream();
            completeReaders.push_back(*iter2);
            
            iter2 = readers.erase(iter2);
            

        } else {
            (*iter2)->stream();
            
            iter2++;
        }
    }
}


SFZAudioReaderManager::SFZAudioReaderManager()
: newReaders(1024), freedReaders(1024), memoryUsage(0)
{
    
}

SFZAudioReaderManager::~SFZAudioReaderManager()
{
    process();
    
    for(int i=0; i<readers.size(); i++)
    {
        delete readers[i];

    }
    readers.clear();
}

SFZBaseAudioReader *SFZAudioReaderManager::createReader(const std::string &extension)
{
    if(extension == "wav")
        return new SFZWavAudioReader();
    else if(extension == "ogg")
        return new SFZOggAudioReader();
    else if(extension == "sf2")
        return new SF2AudioReader();
    
    return NULL;
}
