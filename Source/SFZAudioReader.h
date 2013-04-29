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
class SFZAudioReader  {
	

public:
    
    int     loopStart;
    int     loopEnd;

	short 	myChannels;
	int   	mySampleRate;

	long getLength();
	
	

    SFZAudioBuffer *buffer;
	
	// get/set for the Path property
	
	~SFZAudioReader()
	{
        closeStream();
		
        if (myData)
            delete (myData);

        if(buffer)
            delete buffer;
        
        //printf("freeing SampleData");
        

        
	}
	
    SFZAudioReader();

    
    // 0 = no limit.
    void setFile(std::string fileName_, unsigned int maxLength_ = 0);
    
    // start pre-loading.. open file etc.
    bool beginLoad();
    
    // stream the next N bytes..
    bool stream();
    
    // stop streaming.
	void closeStream();
    
    
    bool isStreamFinished() { return currentReadOffset >= maxLength; };

    
	
    bool isHeaderRead() { return headerIsRead; };
	
	// return a printable summary of the wav file
	char *getSummary()
	{
		char *summary = new char[250];
		sprintf(summary, " Format: %d\n Channels: %d\n SampleRate: %d\n ByteRate: %d\n BlockAlign: %d\n BitsPerSample: %d\n DataSize: %d\n", myFormat, myChannels, mySampleRate, myByteRate, myBlockAlign, myBitsPerSample, myDataSize);

		return summary;
	}
    
    SFZAudioReader *createReaderForFull();
    
private:
    std::string 	myPath;
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
    
	// read a wav file into this class
	bool readWav();
	//read an ogg file into this class using stb_vorbis
    bool readOgg();

    
    unsigned int maxLength;
    unsigned int length;
    unsigned int currentReadOffset;
    
    std::string extension;
    
    bool headerIsRead;
    
    std::ifstream currentFile;
    
    
};


#endif /* defined(__OpenSFZ__SFZAudioReader__) */
