#include "SFZSample.h"

#include <assert.h>

bool SFZSample::preload()
{
    SFZAudioReaderManager *manager = SFZAudioReaderManager::getInstance();
    
    loader = new SFZAudioReader();
    loader->setFile(fileName, 10000);
    
    manager->addReader(loader);
    fullyLoaded = false;
    
	return true;
}

bool SFZSample::load()
{
    // Check for case where we preloaded the whole file? 
#ifdef SFZ_NO_STREAMING
    loader = new SFZAudioReader();
    loader->setFile(fileName, 0);

    if(loader->beginLoad())
    {
    
        while(loader->isStreamFinished())
        {
            loader->stream();
        }
        loader->closeStream();
    }
    
#else
    if(loader)
    {
        SFZAudioReaderManager *manager = SFZAudioReaderManager::getInstance();
        
        SFZAudioReader *newloader = loader->createReaderForFull();
        manager->releaseReader(loader);
        loader = newloader;
        manager->addReader(loader);
        
    } else {
        SFZAudioReaderManager *manager = SFZAudioReaderManager::getInstance();
        loader = new SFZAudioReader();
        loader->setFile(fileName, 0);
        
        
        manager->addReader(loader);
    }
#endif
    
    fullyLoaded = true;
    sampleRate = loader->mySampleRate;
    sampleLength = loader->getLength();
    loopStart = loader->loopStart;
    loopEnd = loader->loopEnd;
    
	return true;
}


SFZSample::~SFZSample()
{
    // audio manager 
}


std::string SFZSample::getShortName()
{
    // FIXME: this isn't short.
    Path p(fileName);
    return p.getFileName();

}


SFZAudioBuffer*	SFZSample::getBuffer()
{

    if(loader)
    {
        sampleLength = loader->buffer->getNumSamples();
        return loader->buffer;
    }
    else if(internalBuffer)
    {
            sampleLength = internalBuffer->getNumSamples();
        return internalBuffer;
    }
        return NULL;
}



void SFZSample::dump()
{
	printf("%s\n %s\n", fileName.c_str(), loader->getSummary());
}

void SFZSample::setBuffer(SFZAudioBuffer* newBuffer)
{
    if(internalBuffer)
        delete internalBuffer;
    
	internalBuffer = newBuffer;
	sampleLength = internalBuffer->getNumSamples();
}


SFZAudioBuffer* SFZSample::detachBuffer()
{
    SFZAudioBuffer* result = internalBuffer;
    internalBuffer = NULL;
    return result;
}



#ifdef DEBUG
void SFZSample::checkIfZeroed(const char* where)
{
	if (loader->buffer == NULL) {
		printf("SFZSample::checkIfZeroed(%s): no buffer!", where);
		return;
		}

	int samplesLeft = loader->buffer->getNumSamples();
	unsigned long nonzero = 0, zero = 0;
	float* p = loader->buffer->channels[0];
	for (; samplesLeft > 0; --samplesLeft) {
		if (*p++ == 0.0)
			zero += 1;
		else
			nonzero += 1;
		}
	if (nonzero > 0)
		printf("Buffer not zeroed at %s (%lu vs. %lu).", where, nonzero, zero);
	else
		printf("Buffer zeroed at %s!  (%lu zeros)", where, zero);
}
#endif 	// JUCE_DEBUG



