//
//  SFZAudioReader.h
//  OpenSFZ
//
//  Created by David Wallin on 4/23/13.
//  Copyright (c) 2013 David Wallin. All rights reserved.
//

#ifndef __OpenSFZ__SFZAudioReader__
#define __OpenSFZ__SFZAudioReader__

#include "SFZCommon.h"



class SFZAudioReader  {
	
private:
    std::string 	myPath;
	int 	myChunkSize;
	int	mySubChunk1Size;

	short 	myFormat;
	int   	myByteRate;
	short 	myBlockAlign;
	short 	myBitsPerSample;
	int	myDataSize;
    
	double position, recordPosition;
	double speed;
	double output;

	
public:

	short 	myChannels;
	int   	mySampleRate;

	long getLength();
	
	
	char* 	myData;
    SFZAudioBuffer *buffer;
	
	// get/set for the Path property
	
	~SFZAudioReader()
	{
		if (myData)
            delete (myData);

        if(buffer)
            delete buffer;
        
        printf("freeing SampleData");
        
	}
	
    SFZAudioReader();
    
	bool load(std::string fileName);

	
	
    
    
    void clear();
    
    void reset();
	
	
	// return a printable summary of the wav file
	char *getSummary()
	{
		char *summary = new char[250];
		sprintf(summary, " Format: %d\n Channels: %d\n SampleRate: %d\n ByteRate: %d\n BlockAlign: %d\n BitsPerSample: %d\n DataSize: %d\n", myFormat, myChannels, mySampleRate, myByteRate, myBlockAlign, myBitsPerSample, myDataSize);

		return summary;
	}
private:
	// read a wav file into this class
	bool readWav();
	//read an ogg file into this class using stb_vorbis
    bool readOgg();

    
    long length;
};


#endif /* defined(__OpenSFZ__SFZAudioReader__) */
