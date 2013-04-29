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
void SFZAudioReaderManager::addReader(SFZAudioReader *r)
{
    if(newReaders.canWrite())
    {
        newReaders.put(r);
    }
}

void SFZAudioReaderManager::releaseReader(SFZAudioReader *r)
{
    if(freedReaders.canWrite())
        freedReaders.put(r);
}

// process any readers..
void SFZAudioReaderManager::process()
{
    while(freedReaders.canRead())
    {
        SFZAudioReader *r = freedReaders.get();
        
        r->closeStream();
        
        std::vector<SFZAudioReader *>::iterator iter = find(readers.begin(), readers.end(), r);
                
        if(iter != readers.end())
            readers.erase(iter);
        
        iter = find(completeReaders.begin(), completeReaders.end(), r);
        
        if(iter != completeReaders.end())
            completeReaders.erase(iter);

        delete r;
    }
    
    while(newReaders.canRead())
    {
        SFZAudioReader *r = newReaders.get();
        
        r->beginLoad();
        r->stream();
        
        readers.push_back(r);
        
    }
    
    std::vector<SFZAudioReader *>::iterator iter2;
    
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
: newReaders(1024), freedReaders(1024)
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
