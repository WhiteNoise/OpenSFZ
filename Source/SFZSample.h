#ifndef SFZSample_h
#define SFZSample_h

#include "SFZCommon.h"
#include "SFZAudioReader.h"
#include "SFZAudioReaderManager.h"

class SFZSample {
	public:
    SFZSample(const std::string& fileIn)
			: loopStart(0), loopEnd(0), fileName(fileIn), sampleRate(44100.0f), loader(0), internalBuffer(0)
            {
                fullyLoaded = false;
            }
    SFZSample(double sampleRateIn)
        : sampleLength(0), loopStart(0), loopEnd(0), loader(0), sampleRate(sampleRateIn), internalBuffer(0)
        {
            // sf2's use this only?
            fullyLoaded = true;
        }
    ~SFZSample();

    bool	preload(int numSamples);
    bool	load();
    void    unload();
    void    bumpSampleOrder();
    
    bool getFullyLoaded() { return fullyLoaded; };

    SFZAudioBuffer*	getBuffer();
    double	getSampleRate() { return sampleRate; }
    std::string	getShortName();

    int getSampleOrder() { return sampleOrder; };
    
    SFZAudioBuffer* detachBuffer();
    void setBuffer(SFZAudioBuffer* newBuffer);
		void	dump();

		unsigned long	sampleLength, loopStart, loopEnd;

#ifdef DEBUG
		void	checkIfZeroed(const char* where);
#endif

	protected:
        static unsigned int sampleOrderIndex;
    
    unsigned int sampleOrder;
        bool fullyLoaded;
    
        std::string fileName;
		double	sampleRate;
    
        SFZBaseAudioReader *loader;
        SFZAudioBuffer *internalBuffer;
	};


#endif 	// !SFZSample_h

