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


SFZBaseAudioReader::SFZBaseAudioReader()
{
    loopStart = 0;
    loopEnd = 0;
    
    myChannels = 1;
 	mySampleRate = 44100;
    buffer = 0;
    maxLength = INT_MAX;
}

SFZBaseAudioReader::~SFZBaseAudioReader()
{
    
}

const char *SFZBaseAudioReader::getSummary()
{
    static char chr[256];
    
    sprintf(chr, "Channels: %d\nSamplerate: %d\n", myChannels, mySampleRate);
    
    return chr;
}
//////////////////////////////////////////////////////////////////


SFZWavAudioReader::SFZWavAudioReader()
: myData(NULL)
{
    headerIsRead = false;
    length = 0;
    loopStart = 0;
    loopEnd = 0;
    currentReadOffset = 0;
    
    myFormat = 0;
    myByteRate = 0;
    mySampleRate = 0;
    myBytesPerSample = 0;
    myBitsPerSample = 0;
    dataChunkPos = 0;
    myChannels = 1;
    maxLength = 0;
    
};

SFZWavAudioReader::~SFZWavAudioReader()
{
    closeStream();
    
    if (myData)
        delete (myData);
    
    if(buffer)
        delete buffer;
    
    //printf("freeing SampleData");
    
}

SFZBaseAudioReader *SFZWavAudioReader::createReaderForFull()
{

    SFZWavAudioReader *newreader = new SFZWavAudioReader();
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

        newreader->myBytesPerSample = myBytesPerSample;
        newreader->myBitsPerSample = myBitsPerSample;
        newreader->dataChunkPos = dataChunkPos;
        newreader->samplerChunk = samplerChunk;
        newreader->headerIsRead = true;
        newreader->myChannels = myChannels;
        
        if(buffer)
            newreader->buffer->initializeWith(*buffer);
        
        newreader->currentReadOffset = currentReadOffset;
        newreader->maxLength = length;
    } else {
        newreader->myPath = myPath;
        newreader->headerIsRead = false;
        newreader->beginLoad();
        newreader->stream();
        // get some data loaded
        
    }
    
    return (SFZBaseAudioReader *)newreader;

}







void SFZWavAudioReader::setFile(std::string fileName_, unsigned int maxLength_)
{
    if(currentFile.is_open())
        currentFile.close();
    
    myPath = fileName_;
    maxLength = maxLength_;
    
    
    
}

void SFZWavAudioReader::parseWAVHeader(ifstream &inFile)
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
    {
        buffer = 0;
    } else {
        if(maxLength > length)
            maxLength = length;
        
        buffer = new SFZAudioBuffer(myChannels, maxLength);
        buffer->setNumSamples(0);
        buffer->clear();
    }
    
    headerIsRead = true;
    

    
}

void SFZWavAudioReader::readWAVData(ifstream &inFile, unsigned int startOffset, unsigned int maxToRead)
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
    
    if(myFormat == eSampleInt16)
    {
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
                {
                    int32_t v = temp[position++];
                    //v = __builtin_bswap32 (v);

                    buffer->channels[j][i + startOffset] = (float)((double) v / (double)INT32_MAX);
                }
        }
    } else if(myFormat == eSampleFloat32)
    {
        if(myBitsPerSample == 32)
        {
            float_t *temp;
            
            temp = (float_t *)&myData[0];
            
            for(int i=0; i<samplesToRead; i++)
                for(int j=0; j<myChannels; j++)
                {
                    float_t v = temp[position++];
                    
                    buffer->channels[j][i + startOffset] = v;
                }
            
        }
    } else {
        // unsupported format.
        
    }
    
    currentReadOffset += samplesToRead;
    buffer->setNumSamples(currentReadOffset);
    
    delete myData;
    myData = 0;

}
/*
bool SFZWavAudioReader::readWav()
{
	bool result;
	ifstream inFile( myPath.c_str(), ios::in | ios::binary);
	result = true;
	if (inFile.good()) {

        parseWAVHeader(inFile);
        readWAVData(inFile, 0, 0);  // read entire wav..
		
	}else {
        //		cout << "ERROR: Could not load sample: " <<myPath << endl; //This line seems to be hated by windows
        sfzDebugPrint("ERROR: Could not load sample.");
        result = false;
	}
	
	
	return result; // this should probably be something more descriptive
}

*/

int32_t SFZWavAudioReader::parseSMPLChunk(ifstream &f, long dataLength)
{
    // FIXME: endian swap?
    
	f.read((char *)&samplerChunk, sizeof(TSamplerChunk));
	
	if (samplerChunk.cSampleLoops)
	{
        // just read the first one?
		f.read((char *)&sampleLoop, sizeof(TSampleLoop));
	}

}


// start pre-loading.. open file etc.
bool SFZWavAudioReader::beginLoad()
{
    print_free_memory();
        
    if(!currentFile.is_open())
        currentFile.open(myPath.c_str(), ios::in | ios::binary);
    
    assert(currentFile.good());
    
    if (currentFile.good()) {

        parseWAVHeader(currentFile);
        return true;
    } else {
        return false;
    }
    
    return false;
}

// stream the next N bytes..
bool SFZWavAudioReader::stream()
{
    
    assert(currentFile.good());
    
    if(currentFile.good())
    {
        readWAVData(currentFile, currentReadOffset, 1024);

        return true;
    }
    
    return false;
    
}

// stop streaming.
void SFZWavAudioReader::closeStream()
{
    if(currentFile.is_open())
        currentFile.close();

}

///////////////////////////////////////

SFZOggAudioReader::SFZOggAudioReader()
{
    vorbisAlloc.alloc_buffer = 0;
    vorbisAlloc.alloc_buffer_length_in_bytes = 0;

    vorbisData = 0;
}

SFZOggAudioReader::~SFZOggAudioReader()
{
    closeStream();
    
    vorbisData = 0;
}

void SFZOggAudioReader::setFile(std::string fileName_, unsigned int maxLength_)
{    
    myPath = fileName_;
    maxLength = maxLength_;
    
    
    
}


// start pre-loading.. open file etc.
bool SFZOggAudioReader::beginLoad()
{
    print_free_memory();

    if(vorbisAlloc.alloc_buffer)
        delete vorbisAlloc.alloc_buffer;
    
    vorbisAlloc.alloc_buffer = 0;
    int error = 0;
    
    vorbisData = stb_vorbis_open_filename((char *)myPath.c_str(), &error, &vorbisAlloc);
    
    if(vorbisData)
    {
        stb_vorbis_info vorbInfo = stb_vorbis_get_info(vorbisData);
        length = stb_vorbis_stream_length_in_samples(vorbisData);
        
        mySampleRate = vorbInfo.sample_rate;
        myChannels = vorbInfo.channels;
        
        currentReadOffset = 0;

        
        if(maxLength == 0)
        {
            buffer = 0;
            closeStream();
        } else {
            
            if(maxLength > length)
                maxLength = length;
            
            if(!buffer)
            {
                buffer = new SFZAudioBuffer(myChannels, maxLength);
                buffer->setNumSamples(0);
                buffer->clear();
            }
            
            stream();
            
        }
        
        return true;
    } else {
        sfzDebugPrint("Error loading ogg. Out of memory?\n");
        return false;
    }
    
    
    return false;
}

// stream the next N bytes..
bool SFZOggAudioReader::stream()
{
        


    if(vorbisData)
    {
        
        if(currentReadOffset < maxLength && buffer)
        {
            const int numFrames = 4;
            
            for(int i=0; i<numFrames; i++)
            {
                if(currentReadOffset < maxLength)
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
        }
    
        return true;
    }
    
    return false;
}

// stop streaming.
void SFZOggAudioReader::closeStream()
{
    
    if(vorbisData)
    {
        stb_vorbis_close(vorbisData);
        
        if(vorbisAlloc.alloc_buffer)
            free( vorbisAlloc.alloc_buffer );
        vorbisAlloc.alloc_buffer = 0;
        vorbisData = 0;
        
    }
}

SFZBaseAudioReader *SFZOggAudioReader::createReaderForFull()
{
    SFZOggAudioReader *newreader = new SFZOggAudioReader();
    newreader->maxLength = 0;
    
    if(vorbisData)
    {
        newreader->buffer = new SFZAudioBuffer(myChannels, length);
        newreader->mySampleRate = mySampleRate;
        newreader->loopStart = loopStart;
        newreader->loopEnd = loopEnd;
        newreader->length = length;
        newreader->myPath = myPath;
        newreader->myChannels = myChannels;
        
        if(buffer)
            newreader->buffer->initializeWith(*buffer);
        
        newreader->currentReadOffset = currentReadOffset;
        newreader->maxLength = length;
    } else {
        newreader->myPath = myPath;
        newreader->beginLoad();
        newreader->stream();
        // get some data loaded
        
    }
    
    return (SFZBaseAudioReader *)newreader;
}


/*
// This is for OGG loading
bool SFZBaseAudioReader::readOgg() {
#ifdef VORBIS
    bool result;
    short *temp;
    
    int channelx;
    //    cout << fileName << endl;
    myDataSize = stb_vorbis_decode_filename(const_cast<char*>(myPath.c_str()), &channelx, &temp);
    result = myDataSize > 0;
    
#ifdef DEBUG
    char szBuff[256];
    sprintf(szBuff, "\nchannels = %d\nlength = %d\n",channelx,myDataSize);
    sfzDebugPrint(szBuff);
#endif
    
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
*/



SF2AudioReader::SF2AudioReader()
{
    currentFile = 0;
    currentReadOffset = 0;
    myChannels = 1;
}

SF2AudioReader::~SF2AudioReader()
{
    if(currentFile)
        delete currentFile;
    currentFile = 0;
    
    waveOffset = 0;
    waveLength = 0;
    
}

void SF2AudioReader::setFile(std::string fileName_, unsigned int maxLength_)
{
    if(currentFile)
        delete currentFile;
    currentFile = 0;
    
    myPath = fileName_;
    maxLength = maxLength_;
}

void SF2AudioReader::setWaveChunkPosition(unsigned int waveOffset_, unsigned int waveLength_)
{
    waveOffset = waveOffset_;
    waveLength = waveLength_;
}

// start pre-loading.. open file etc.
bool SF2AudioReader::beginLoad()
{
    Path p(myPath);
    
    //if(!p.exists())
    //    return false;
    
    currentFile = p.createInputStream();
    currentFile->setPosition(waveOffset);
    

    
    if(maxLength > waveLength)
        maxLength = waveLength;
    
    if(!buffer)
    {
        buffer = new SFZAudioBuffer(myChannels, maxLength);
        buffer->setNumSamples(0);
        buffer->clear();
    }
    
    stream();
    
    return true;
    
}

// stream the next N bytes..
bool SF2AudioReader::stream()
{
    
    
    if(maxLength > waveLength)
        maxLength = waveLength;
    
    
	// Read and convert.
	short shortBuffer[1024];
	unsigned long samplesLeft = maxLength - currentReadOffset;
    
    // only read 1024 at a time.
    if(samplesLeft > 1024)
        samplesLeft = 1024;
    
    
	float* out = &buffer->channels[0][currentReadOffset];
		// Read the buffer.
    currentFile->read(shortBuffer, samplesLeft * sizeof(short));
    
    // Convert from signed 16-bit to float.
    unsigned long samplesToConvert = samplesLeft;
    short* in = shortBuffer;
    for (; samplesToConvert > 0; --samplesToConvert) {
        // If we ever need to compile for big-endian platforms, we'll need to
        // byte-swap here.
        *out++ = *in++ / 32767.0;
    }
    
    atomic_t newOffset = currentReadOffset + samplesLeft;
    currentReadOffset = newOffset;
    
    if(buffer->getNumSamples() < currentReadOffset)
        buffer->setNumSamples(currentReadOffset);

    
    return true;
}

// stop streaming.
void SF2AudioReader::closeStream()
{
    if(currentFile)
        delete currentFile;
    currentFile = 0;
}

SFZBaseAudioReader *SF2AudioReader::createReaderForFull()
{
    SF2AudioReader *newreader = new SF2AudioReader();
    newreader->maxLength = 0;
    

    newreader->buffer = new SFZAudioBuffer(1, waveLength);
    newreader->mySampleRate = mySampleRate;
    newreader->myChannels = myChannels;

    newreader->loopStart = loopStart;
    newreader->loopEnd = loopEnd;
    newreader->waveLength = waveLength;
    newreader->waveOffset = waveOffset;

    newreader->myPath = myPath;
    
    if(buffer)
        newreader->buffer->initializeWith(*buffer);
    
    newreader->currentReadOffset = currentReadOffset;
    newreader->maxLength = waveLength;

    return (SFZBaseAudioReader *)newreader;
}