#ifndef SFZSample_h
#define SFZSample_h

#include "SFZCommon.h"
#include "SFZAudioReader.h"
#include "SFZAudioReaderManager.h"

class SFZSample {
	public:
    SFZSample(const std::string& fileIn)
			: loopStart(0), loopEnd(0), fileName(fileIn), sampleRate(44100.0f), loader(0), internalBuffer(0), sf2Start(0), sf2Length(0)
            {
                fullyLoaded = false;
                sampleOrder = 0;
                
                bumpSampleOrder();
            }
    SFZSample(double sampleRateIn)
        : sampleLength(0), loopStart(0), loopEnd(0), loader(0), sampleRate(sampleRateIn), internalBuffer(0), sf2Start(0), sf2Length(0)
        {
            // sf2's use this only?
            fullyLoaded = false;
            sampleOrder = 0;
            bumpSampleOrder();
        }
    ~SFZSample();

    bool	preload(int numSamples, bool bump = true);
    bool	load();
    void    unload();
    void    bumpSampleOrder();
    
    void setSf2Start(unsigned int s) { sf2Start = s; };
    unsigned int getSf2Start() { return sf2Start; };
    void setSf2Length(unsigned int s) { sf2Length = s; };
    
    bool getFullyLoaded() { return fullyLoaded; };

    SFZAudioBuffer*	getBuffer();
    double	getSampleRate() { return sampleRate; };
    
    void setSampleRate(double d) { sampleRate = d; };
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
    
    unsigned int sf2Start;
    unsigned int sf2Length;
    
        SFZBaseAudioReader *loader;
        SFZAudioBuffer *internalBuffer;
	};


#endif 	// !SFZSample_h

