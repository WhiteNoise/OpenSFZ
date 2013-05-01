//
//  SFZAudioReader.cpp
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

#include <stdio.h>

#include "SFZAudioReader.h"
#include "stb_vorbis.h"

#include <assert.h>

using namespace std;

SFZAudioReader::SFZAudioReader()
: myData(NULL), myChannels(1), buffer(0), mySampleRate(44100.0f)
{
    headerIsRead = false;
    length = 0;
    loopStart = 0;
    loopEnd = 0;
    currentReadOffset = 0;
    
    vorbisAlloc.alloc_buffer = 0;
    vorbisAlloc.alloc_buffer_length_in_bytes = 0;
};

SFZAudioReader *SFZAudioReader::createReaderForFull()
{

    SFZAudioReader *newreader = new SFZAudioReader();
    newreader->maxLength = 0;
    
    if(headerIsRead)
    {
        newreader->buffer = new SFZAudioBuffer(myChannels, length);
        newreader->myFormat = myFormat;
        newreader->myByteRate = myByteRate;
        newreader->mySampleRate = mySampleRate;
        newreader->loopStart = loopStart;
        newreader->loopEnd = loopEnd;
        newreader->length = length;
        newreader->myPath = myPath;
        newreader->extension = extension;
        newreader->myBytesPerSample = myBytesPerSample;
        newreader->myBitsPerSample = myBitsPerSample;
        newreader->dataChunkPos = dataChunkPos;
        newreader->samplerChunk = samplerChunk;
        newreader->headerIsRead = true;
        newreader->myChannels = myChannels;
        newreader->buffer->initializeWith(*buffer);
        newreader->currentReadOffset = currentReadOffset;
        newreader->maxLength = length;
    } else {
        newreader->myPath = myPath;
        newreader->extension = extension;
        newreader->headerIsRead = false;
        newreader->beginLoad();
        newreader->stream();
        // get some data loaded
        
    }
    
    return newreader;

}
/*

//This is the SFZAudioReader load function. It just calls read.
bool SFZAudioReader::load(string fileName) {
	myPath = fileName;

    if(buffer) {
        delete buffer;
        buffer = 0;
    }
    
    Path p(fileName);
    std::string ext = p.getExtension();
    if(ext == "ogg")
        return readOgg();
    else if(p.getExtension() == "wav")
        return readWav();
    
    return false;
}
 */

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
    
    headerIsRead = true;
    
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
    
    delete temp;
    
	return result; // this should probably be something more descriptive
#endif
    return 0;
}


void SFZAudioReader::setFile(std::string fileName_, unsigned int maxLength_)
{
    myPath = fileName_;
    maxLength = maxLength_;
    
    Path p(myPath);
    extension = p.getExtension();
    
    
}

void SFZAudioReader::parseWAVHeader(ifstream &inFile)
{
    if(headerIsRead)
        return;
    
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
    long filePos = 36;
    
    
    inFile.seekg (0, inFile.end);
    unsigned long fileLength = inFile.tellg();
    
    // !datafound &&
    bool datafound = false;
    smplChunkSize = 0;
    smplChunkPos = 0;
    
    while(filePos < fileLength && !inFile.eof()) {
        int chunkSize;
        
        inFile.seekg(filePos, ios::beg);
        inFile.read((char*) &chunkID, sizeof(char) * 4);
        inFile.seekg(filePos + 4, ios::beg);
        inFile.read( (char*) &chunkSize, sizeof(int) ); // read the size of the data
        filePos += 8;
        if (strcmp(chunkID,"data") == 0 && !datafound) {
            myDataSize = chunkSize;
            dataChunkPos = filePos;
            datafound = true;
        } else if (strcmp(chunkID,"smpl") == 0) {
            smplChunkSize = chunkSize;
            smplChunkPos = filePos;
            
            inFile.seekg(filePos, ios::beg);
            
            parseSMPLChunk(inFile, chunkSize);
            
            loopStart = sampleLoop.dwStart;
            loopEnd = sampleLoop.dwEnd;
        }
        
        
        filePos += chunkSize;
        
    }
    
    myBytesPerSample = (myBitsPerSample / 8);
    
    assert(myBytesPerSample > 0);
    
    length=myDataSize/(myBytesPerSample * myChannels);

    if(maxLength == 0)
        maxLength = length;
    
    if(maxLength > length)
        maxLength = length;
    
    buffer = new SFZAudioBuffer(myChannels, maxLength);
    buffer->setNumSamples(0);
    buffer->clear();
    
    headerIsRead = true;
    

    
}

void SFZAudioReader::readWAVData(ifstream &inFile, unsigned int startOffset, unsigned int maxToRead)
{
    if(!headerIsRead)
        return;

    assert(buffer);
    if(startOffset >= maxLength)
    {
        // already done..
        return;
    }
    
    int samplesLeft = maxLength - startOffset;
    int samplesToRead = samplesLeft;
    
    
    if(maxToRead != 0 && samplesToRead > maxToRead)
        samplesToRead = maxToRead;
    
    if(samplesToRead + startOffset > maxLength)
        samplesToRead = maxLength - startOffset;
    
    if(samplesToRead < 0)
        return;
    
    
    // read the data chunk
    unsigned int blocksize = samplesToRead * (unsigned int)myBytesPerSample * (unsigned int)myChannels;
    myData = (char*) malloc(blocksize);      // myDataSize
    inFile.seekg(dataChunkPos + (startOffset*(unsigned int)myBytesPerSample * (unsigned int)myChannels), ios::beg);
    inFile.read(myData, blocksize);     // myDataSize
    
    int position = 0;
    
    
    if(myBitsPerSample == 16)
    {
        short int *temp;
        
        temp = (short int *)&myData[0];
        
        // FIXME: endian swap?
        
        for(int i=0; i<samplesToRead; i++)
            for(int j=0; j<myChannels; j++)
                buffer->channels[j][i + startOffset] = (float)temp[position++] / 32768.0f;
    }
    else if(myBitsPerSample == 24)
    {
        // FIXME: endian swap?
        
        for(int i=0; i<samplesToRead; i++)
            for(int j=0; j<myChannels; j++)
            {
                
                int32_t sample = (unsigned char)myData[position+2];
                sample = (sample << 8) | (unsigned char)myData[position+1];
                sample = (sample << 8) | (unsigned char)myData[position];
                sample <<= 8;
                
                buffer->channels[j][i + startOffset] = (float)((double)sample / (double)INT32_MAX);
                
                position += 3;
            }
    }
    else if(myBitsPerSample == 32)
    {
        // FIXME: endian swap?
        
        int32_t *temp;
        
        temp = (int32_t *)&myData[0];
        
        for(int i=0; i<samplesToRead; i++)
            for(int j=0; j<myChannels; j++)
                buffer->channels[j][i + startOffset] = (float)((double)temp[position++] / (double)INT32_MAX);
    }
    
    currentReadOffset += samplesToRead;
    buffer->setNumSamples(currentReadOffset);
    
    delete myData;
    myData = 0;

}

bool SFZAudioReader::readWav()
{
	bool result;
	ifstream inFile( myPath.c_str(), ios::in | ios::binary);
	result = true;
	if (inFile.good()) {

        parseWAVHeader(inFile);
        readWAVData(inFile, 0, 0);  // read entire wav..
		
	}else {
        //		cout << "ERROR: Could not load sample: " <<myPath << endl; //This line seems to be hated by windows
        printf("ERROR: Could not load sample.");
        result = false;
	}
	
	
	return result; // this should probably be something more descriptive
}


int32_t SFZAudioReader::parseSMPLChunk(ifstream &f, long dataLength)
{
    // FIXME: endian swap?
    
	f.read((char *)&samplerChunk, sizeof(TSamplerChunk));
	
	if (samplerChunk.cSampleLoops)
	{
        // just read the first one?
		f.read((char *)&sampleLoop, sizeof(TSampleLoop));
	}

}


long SFZAudioReader::getLength() {
    return maxLength;
}

// start pre-loading.. open file etc.
bool SFZAudioReader::beginLoad()
{

    if(extension == "wav")
    {
        if(currentFile.is_open())
            currentFile.close();
        
        currentFile.open(myPath.c_str(), ios::in | ios::binary);
        
        assert(currentFile.good());
        
        if (currentFile.good()) {

            parseWAVHeader(currentFile);
            return true;
        } else {
            return false;
        }
    } else if(extension == "ogg")
    {
        if(vorbisAlloc.alloc_buffer)
            delete vorbisAlloc.alloc_buffer;
        
        vorbisAlloc.alloc_buffer = 0;
        int error = 0;
        
        vorbisData = stb_vorbis_open_filename((char *)myPath.c_str(), &error, &vorbisAlloc);
        stb_vorbis_info vorbInfo = stb_vorbis_get_info(vorbisData);
        length = stb_vorbis_stream_length_in_samples(vorbisData);

        mySampleRate = vorbInfo.sample_rate;
        myChannels = vorbInfo.channels;


        // FIXME: DRY violation
        if(maxLength == 0)
            maxLength = length;
        
        if(maxLength > length)
            maxLength = length;
        
        if(!buffer)
        {
            buffer = new SFZAudioBuffer(myChannels, maxLength);
            buffer->setNumSamples(0);
            buffer->clear();
        }
        currentReadOffset = 0;
        headerIsRead = true;

        stream();
        stream();
        stream();
    }
    
    return false;
}

// stream the next N bytes..
bool SFZAudioReader::stream()
{
    
    if(extension == "wav")
    {
        assert(currentFile.good());
        
        if(currentFile.good())
            readWAVData(currentFile, currentReadOffset, 2048);
        
        
        return true;
    } else if(extension == "ogg")
    {

        assert(vorbisData);
        int numFrames = 4;
        
        
        for(int i=0; i<numFrames; i++)
        {
            float **sampleFrames;
            

            unsigned int samplesDecoded = stb_vorbis_get_frame_float(vorbisData, NULL, &sampleFrames);
            
            if(samplesDecoded)
            {
                unsigned int samplesToCopy = samplesDecoded;
                if(samplesToCopy > maxLength - currentReadOffset)
                    samplesToCopy = maxLength - currentReadOffset;
                
                if(samplesToCopy > 0)
                {
                

                    
                    for(int i=0; i<myChannels; i++)
                    {
                        for(int j=0; j<samplesToCopy; j++)
                        {
                            buffer->channels[i][j + currentReadOffset] = sampleFrames[i][j];
                        }
                    }
                    
                    currentReadOffset += samplesToCopy;
                    
                    // keep what's already in there..
                    if(buffer->getNumSamples() < currentReadOffset)
                        buffer->setNumSamples(currentReadOffset);
                }
            }
        }
    }
    
    return false;
    
}

// stop streaming.
void SFZAudioReader::closeStream()
{
    if(extension == "wav")
    {
        if(currentFile.is_open())
            currentFile.close();
    } else if(extension == "ogg")
    {

        if(vorbisData)
        {
            stb_vorbis_close(vorbisData);
            
            vorbisData = 0;
            
        }
    }
}
