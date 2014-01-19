//
//  SFZAudioReader.h
//  OpenSFZ
//
//  Created by David Wallin on 4/23/13.
//
//  This file originally comes from Maximillian
//  https://github.com/micknoise/Maximilian
//  but includes updates to read more types of files, loop points etc.
//
//  Utilizes the stb_vorbis code for ogg reading.
//  http://www.nothings.org/stb_vorbis/

#ifndef __OpenSFZ__SFZAudioReader__
#define __OpenSFZ__SFZAudioReader__

#include "SFZCommon.h"


class SFZAudioBuffer;
class InputStream;

class SFZBaseAudioReader
{
public:
    
    SFZBaseAudioReader();
    virtual ~SFZBaseAudioReader();

    virtual void setFile(std::string fileName_, unsigned int maxLength_ = INT32_MAX) = 0;
    
    // start pre-loading.. open file etc.
    virtual bool beginLoad() = 0;
    
    // stream the next N bytes..
    virtual bool stream() = 0;
    
    // stop streaming.
	virtual void closeStream() = 0;
    
    virtual bool isStreamFinished() = 0;

    virtual SFZBaseAudioReader *createReaderForFull() = 0;
    
    const char *getSummary();
    
    bool isValid() { return myChannels != 0; };
    
    int getNumChannels() { return myChannels; };
    int getSampleRate() { return mySampleRate; };
    int getLoopStart() { return loopStart; };
    int getLoopEnd() { return loopEnd; };
    SFZAudioBuffer *buffer;
    
    std::string getPath() { return myPath; };
protected:
    int     loopStart;
    int     loopEnd;
    
	short 	myChannels;
	int   	mySampleRate;

    
    std::string 	myPath;

    unsigned int maxLength;
    
    enum WavSampleBitFormat
    {
        eSampleInt16 = 0x01, ///< samples are represented by 16 bits int numbers
        eSampleFloat32 = 0x03,///< samples are represented by float numbers (32 bits)
        eSampleFormatALAW = 0x06, // not supported
        eSampleFormatMULAW = 0x07, // not supported
        eSampleFormatExtensible = 0xFFFE
    };
};

class SFZWavAudioReader : public SFZBaseAudioReader
{
public:
	SFZWavAudioReader();
	virtual ~SFZWavAudioReader();



    void setFile(std::string fileName_, unsigned int maxLength_ = 0);
    
    // start pre-loading.. open file etc.
    bool beginLoad();
    
    // stream the next N bytes..
    bool stream();
    
    // stop streaming.
	void closeStream();
    
    bool isStreamFinished() { return currentReadOffset >= maxLength; };
    
    SFZBaseAudioReader *createReaderForFull();
    
private:

	int 	myChunkSize;
	int	mySubChunk1Size;
    
	short 	myFormat;
	int   	myByteRate;
	short 	myBlockAlign;
	short 	myBitsPerSample;
    short   myBytesPerSample;

	int	myDataSize;
    
    long dataChunkPos;
    long smplChunkPos;
    
    long smplChunkSize;
        
    struct TSamplerChunk
	{
		int32_t dwManufacturer;
		int32_t dwProduct;
		int32_t dwSamplePeriod;
		int32_t dwMIDIUnityNote;
		int32_t dwMIDIPitchFraction;
		int32_t dwSMPTEFormat;
		int32_t dwSMPTEOffset;
		int32_t cSampleLoops;
		int32_t cbSamplerData;
		
		TSamplerChunk() :
        dwManufacturer(0),
        dwProduct(0),
        dwSamplePeriod(0),
        dwMIDIUnityNote(0),
        dwMIDIPitchFraction(0),
        dwSMPTEFormat(0),
        dwSMPTEOffset(0),
        cSampleLoops(0),
        cbSamplerData(0)
		{
		}
	};
    
	struct TSampleLoop
	{
		int32_t dwIdentifier;
		int32_t dwType;
		int32_t dwStart;
		int32_t dwEnd;
		int32_t dwFraction;
		int32_t dwPlayCount;
		
		TSampleLoop() :
        dwIdentifier(0),
        dwType(0),
        dwStart(0),
        dwEnd(0),
        dwFraction(0),
        dwPlayCount(0)
		{
		}
	};
	
	TSamplerChunk samplerChunk;
	TSampleLoop sampleLoop;
	char* 	myData;
    
    void parseWAVHeader(std::ifstream &inFile);
    void readWAVData(std::ifstream &inFile, unsigned int startOffset, unsigned int maxToRead = 0);
    int32_t parseSMPLChunk(std::ifstream &f, long dataLength);

    unsigned int length;
    unsigned int currentReadOffset;
        
    bool headerIsRead;
    
    std::ifstream currentFile;

    
};

class SFZOggAudioReader : public SFZBaseAudioReader  {
public:
    SFZOggAudioReader();
    virtual ~SFZOggAudioReader();
    
    virtual void setFile(std::string fileName_, unsigned int maxLength_ = INT32_MAX);
    
    // start pre-loading.. open file etc.
    virtual bool beginLoad();
    
    // stream the next N bytes..
    virtual bool stream();
    
    // stop streaming.
	virtual void closeStream();
    
    bool isStreamFinished() { return currentReadOffset >= maxLength; };
    
    virtual SFZBaseAudioReader *createReaderForFull();

    
protected:
    unsigned int currentReadOffset;
    
    stb_vorbis_alloc vorbisAlloc;
    stb_vorbis *vorbisData;
    
    // length of the ogg stream.
    unsigned int length;

};

class SF2AudioReader : public SFZBaseAudioReader {
public:
    
    SF2AudioReader();
    virtual ~SF2AudioReader();
    
    virtual void setFile(std::string fileName_, unsigned int maxLength_ = INT32_MAX);
    
    virtual void setWaveChunkPosition(unsigned int waveOffset_, unsigned int waveLength_);
    
    // start pre-loading.. open file etc.
    virtual bool beginLoad();
    
    // stream the next N bytes..
    virtual bool stream();
    
    // stop streaming.
	virtual void closeStream();
    
    bool isStreamFinished() { return currentReadOffset >= maxLength; };
    
    virtual SFZBaseAudioReader *createReaderForFull();
    
    
protected:
    InputStream *currentFile;
    
    unsigned int waveOffset;
    unsigned int waveLength;
    
    unsigned int currentReadOffset;
    
};


#endif /* defined(__OpenSFZ__SFZAudioReader__) */
