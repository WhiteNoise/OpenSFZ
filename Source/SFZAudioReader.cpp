   //
//  SFZAudioReader.cpp
//  OpenSFZ
//
//  Created by David Wallin on 4/23/13.
//  Copyright (c) 2013 David Wallin. All rights reserved.
//
#include <stdio.h>

#include "SFZAudioReader.h"
#include "stb_vorbis.h"

#include <assert.h>

using namespace std;

SFZAudioReader::SFZAudioReader()
: myData(NULL), position(0), recordPosition(0), myChannels(1), buffer(0), mySampleRate(44100.0f)
{
    length = 0;
};


//This is the SFZAudioReader load function. It just calls read.
bool SFZAudioReader::load(string fileName) {
	myPath = fileName;

    if(buffer)
        delete buffer;
    
    Path p(fileName);
    std::string ext = p.getExtension();
    if(ext == "ogg")
        return readOgg();
    else if(p.getExtension() == "wav")
        return read();
    
    return false;
}

// This is for OGG loading
bool SFZAudioReader::readOgg() {
#ifdef VORBIS
    bool result;
    short *temp;
    
    int channelx;
    //    cout << fileName << endl;
    myDataSize = stb_vorbis_decode_filename(const_cast<char*>(myPath.c_str()), &channelx, &temp);
    result = myDataSize > 0;
    
    printf("\nchannels = %d\nlength = %d",channelx,myDataSize);
    printf("\n");
    
    myChannels=(short)channelx;
    length=myDataSize;
    mySampleRate=44100;
    
    assert(myDataSize > 0);
    
    buffer = new SFZAudioBuffer(myChannels, myDataSize);
    
    if (myChannels>1) {
        int position=0;
        for (int i=0;i<myDataSize; i++) {
            for(int j=0; j<myChannels; j++)
            {
                buffer->channels[j][i] = (float)temp[position] / 32768.0f;
                position++;
            }
        }
    }
	return result; // this should probably be something more descriptive
#endif
    return 0;
}


//This is the main read function.
bool SFZAudioReader::read()
{
	bool result;
	ifstream inFile( myPath.c_str(), ios::in | ios::binary);
	result = true;
	if (inFile) {
		bool datafound = false;
		inFile.seekg(4, ios::beg);
		inFile.read( (char*) &myChunkSize, 4 ); // read the ChunkSize
		
		inFile.seekg(16, ios::beg);
		inFile.read( (char*) &mySubChunk1Size, 4 ); // read the SubChunk1Size
		
		//inFile.seekg(20, ios::beg);
		inFile.read( (char*) &myFormat, sizeof(short) ); // read the file format.  This should be 1 for PCM
		
		//inFile.seekg(22, ios::beg);
		inFile.read( (char*) &myChannels, sizeof(short) ); // read the # of channels (1 or 2)
		
		//inFile.seekg(24, ios::beg);
		inFile.read( (char*) &mySampleRate, sizeof(int) ); // read the samplerate
		
		//inFile.seekg(28, ios::beg);
		inFile.read( (char*) &myByteRate, sizeof(int) ); // read the byterate
		
		//inFile.seekg(32, ios::beg);
		inFile.read( (char*) &myBlockAlign, sizeof(short) ); // read the blockalign
		
		//inFile.seekg(34, ios::beg);
		inFile.read( (char*) &myBitsPerSample, sizeof(short) ); // read the bitspersample
                
		
		//ignore any extra chunks
		char chunkID[5]="";
		chunkID[4] = 0;
		int filePos = 36;
		while(!datafound && !inFile.eof()) {
			inFile.seekg(filePos, ios::beg);
			inFile.read((char*) &chunkID, sizeof(char) * 4);
			inFile.seekg(filePos + 4, ios::beg);
			inFile.read( (char*) &myDataSize, sizeof(int) ); // read the size of the data
			filePos += 8;
			if (strcmp(chunkID,"data") == 0) {
				datafound = true;
			}else{
				filePos += myDataSize;
			}
		}
        
        // FIXME: need to handle sample looping chunks, etc..
		
		// read the data chunk
		myData = (char*) malloc(myDataSize * sizeof(char));
		inFile.seekg(filePos, ios::beg);
		inFile.read(myData, myDataSize);
		length=myDataSize/(2 * myChannels);
		inFile.close(); // close the input file
		
        buffer = new SFZAudioBuffer(myChannels, length);
        int position = 0;
        
        
        
        short int *temp;
        
        if(myBitsPerSample == 16)
        {
            temp = (short int *)&myData[0];
            
                for(int i=0; i<length; i++)
                    for(int j=0; j<myChannels; j++)
                        buffer->channels[j][i] = (float)temp[position++] / 32768.0f;
        }
        
        
        delete myData;
        myData = 0;
		
	}else {
        //		cout << "ERROR: Could not load sample: " <<myPath << endl; //This line seems to be hated by windows
        printf("ERROR: Could not load sample.");
        result = false;
	}
	
	
	return result; // this should probably be something more descriptive
}




long SFZAudioReader::getLength() {
    return length;
}



void SFZAudioReader::clear() {
    memset(myData, 0, myDataSize);
}

void SFZAudioReader::reset() {
    position=0;
}