#include "SFZSample.h"

#include <assert.h>

unsigned int SFZSample::sampleOrderIndex = 0;

void SFZSample::bumpSampleOrder()
{
    sampleOrderIndex++;
    sampleOrder = sampleOrderIndex;
    
    if(sampleOrderIndex >= UINT_MAX)
        sampleOrderIndex = 0;
    
}

// FIXME: maybe the amount that we preload can depend on how many samples there are and how much memory it would take up..
// Also, maybe I could keep the 'header' info (length etc) saved, just not the wav buffer.
bool SFZSample::preload(int numSamples, bool bump)
{
    if(bump)
        bumpSampleOrder();
    
    SFZAudioReaderManager *manager = SFZAudioReaderManager::getInstance();
    Path filePath(fileName);
    
    if(filePath.exists())
    {
        loader = SFZAudioReaderManager::createReader(filePath.getExtension());
        if(loader)
        {
            loader->setFile(fileName, numSamples);
            
            if(filePath.getExtension() == "sf2")
            {
                ((SF2AudioReader *)loader)->setWaveChunkPosition(sf2Start, sf2Length);
            }


            manager->addReader(loader);
        }
        
        assert(loader);
    } else {
        printf("File not found %s\n", fileName.c_str());
    }
    fullyLoaded = false;
    
	return true;
}

bool SFZSample::load()
{
    if(fullyLoaded)
        return true;
    
    bumpSampleOrder();
    bool ok = false;
    fullyLoaded = false;
    
    // Check for case where we preloaded the whole file?
    Path filePath(fileName);
    
#ifdef SFZ_NO_STREAMING
    SFZAudioReaderManager *manager = SFZAudioReaderManager::getInstance();
    
    if(filePath.exists())
    {
        loader = SFZAudioReaderManager::createReader(Path(fileName).getExtension());
        loader->setFile(fileName, INT32_MAX);
        
        if(filePath.getExtension() == "sf2")
        {
            ((SF2AudioReader *)loader)->setWaveChunkPosition(sf2Start, sf2Length);
        }

        if(loader->beginLoad())
        {
            ok = true;
            while(!loader->isStreamFinished())
            {
                loader->stream();
            }
            loader->closeStream();
        }
    }
    
#else
    if(loader)
    {
        // preloaded -> loaded
        SFZAudioReaderManager *manager = SFZAudioReaderManager::getInstance();
        
        SFZBaseAudioReader *newloader = loader->createReaderForFull();
        if(newloader)
        {
            manager->releaseReader(loader);
            loader = newloader;
            manager->addReader(loader);
            ok = true;
        }
    } else {
        SFZAudioReaderManager *manager = SFZAudioReaderManager::getInstance();
        
        if(filePath.exists())
        {

            loader = SFZAudioReaderManager::createReader(filePath.getExtension());
            
            if(loader)
            {
                loader->setFile(fileName, INT32_MAX);
                if(Path(fileName).getExtension() == "sf2")
                {
                    ((SF2AudioReader *)loader)->setWaveChunkPosition(sf2Start, sf2Length);
                }

                
                // open the file here?
                ok = loader->beginLoad();
                if(ok)
                    manager->addReader(loader);
                else {
                    delete loader;
                    loader = NULL;
                }
            }
        } else {
            printf("File not found %s\n", fileName.c_str());
        }
    }
#endif
    

    sampleLength = 0;
    
    if(loader)
    {
        fullyLoaded = true;

        sampleRate = loader->getSampleRate();
        loopStart = loader->getLoopStart();
        loopEnd = loader->getLoopEnd();
    } else {
        sampleRate = 44100;
        loopStart = 0;
        loopEnd = 0;
    }
    
	return ok;
}

void SFZSample::unload()
{
    if(loader)
    {
        SFZAudioReaderManager *manager = SFZAudioReaderManager::getInstance();
        manager->releaseReader(loader);
    
        fullyLoaded = false;
        loader = 0;
    } else if(internalBuffer)
    {
        
    }

}


SFZSample::~SFZSample()
{
    // audio manager
    
    if(loader)
    {
        SFZAudioReaderManager *manager = SFZAudioReaderManager::getInstance();
        manager->releaseReader(loader);
        loader = NULL;
    }
}


std::string SFZSample::getShortName()
{
    // FIXME: this isn't short.
    Path p(fileName);
    return p.getFileName();

}


SFZAudioBuffer*	SFZSample::getBuffer()
{

    if(loader && loader->buffer)
    {
        sampleLength = loader->buffer->getNumSamples();
        return loader->buffer;
    }
    else if(internalBuffer)
    {
            sampleLength = internalBuffer->getNumSamples();
        return internalBuffer;
    } else 
        return NULL;
}



void SFZSample::dump()
{
    if(!loader)
        return;
    
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
	if (!loader || loader->buffer == NULL) {
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



