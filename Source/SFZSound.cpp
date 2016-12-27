#include "SFZSound.h"
#include "SFZRegion.h"
#include "SFZSample.h"
#include "SFZReader.h"

using namespace std;

SFZSound::SFZSound(std::string sz)
{
    setFile(Path(sz));
}

SFZSound::SFZSound(const Path& fileIn)
{
    setFile(fileIn);
}

void SFZSound::setFile(const Path &p)
{
	file = Path(p);
    

    
}



SFZSound::~SFZSound()
{
	int numRegions = (int)regions.size();
	for (int i = 0; i < numRegions; ++i) {
		delete regions[i];
    }
    
    regions.clear();
    

	for (map<string, SFZSample*>::iterator iter = samples.begin(); iter != samples.end(); iter++)
    {
		delete iter->second;
    }
}


bool SFZSound::appliesToNote(const int midiNoteNumber)
{
	// Just say yes; we can't truly know unless we're told the velocity as well.
	return true;
}


bool SFZSound::appliesToChannel(const int midiChannel)
{
	return true;
}


void SFZSound::addRegion(SFZRegion* region)
{
	regions.push_back(region);
}


SFZSample* SFZSound::addSample(string pathStr, string defaultPathStr)
{
    
	Path sampleFile;
	if (defaultPathStr.length() == 0)
		sampleFile = file.getSiblingFile(pathStr);
	else {
		Path defaultDir = file.getSiblingFile(defaultPathStr);
		sampleFile = defaultDir.getChildFile(pathStr);
		}
	std::string samplePath = sampleFile.getFullPath();
	SFZSample* sample = samples[samplePath];
	if (sample == NULL) {
		sample = new SFZSample(sampleFile.getFullPath());
		samples[samplePath] = sample;
    }
	return sample;
}




void SFZSound::addError(const string& message)
{
	errors.push_back(message);
}


void SFZSound::addUnsupportedOpcode(const string& opcode)
{
	unsupportedOpcodes[opcode] = opcode;
}


void SFZSound::loadRegions()
{
	SFZReader reader(this);
	reader.read(file.getFullPath());
}


void SFZSound::loadSamples(	double* progressVar)
{
	if (progressVar)
		*progressVar = 0.0;

	double numSamplesLoaded = 1.0, numSamples = samples.size();
    int preloadSize = 128;
    
#ifdef SFZ_LOWMEMORY
    if(numSamples > 100)
        return;
    else if(numSamples > 50)
        preloadSize = 1024;
    else
        preloadSize = 2048;
#else
    preloadSize = 4096;
#endif
    
	for (map<string,SFZSample*>::iterator iter = samples.begin(); iter != samples.end(); iter++ )
    {
		SFZSample* sample = iter->second;
        bool ok = false;
#ifdef SFZ_NO_STREAMING
        ok = sample->load();
#else
        ok = sample->preload(preloadSize);
#endif
		if (!ok)
			addError("Couldn't load sample \"" + sample->getShortName() + "\"");

		numSamplesLoaded += 1.0;
		if (progressVar)
			*progressVar = numSamplesLoaded / numSamples;
        
		//if (thread && thread->threadShouldExit())
		//	return;
    }

	if (progressVar)
		*progressVar = 1.0;
}


SFZRegion* SFZSound::getRegionFor(
	int note, int velocity, SFZRegion::Trigger trigger)
{
	int numRegions = (int)regions.size();
	for (int i = 0; i < numRegions; ++i) {
		SFZRegion* region = regions[i];
		if (region->matches(note, velocity, trigger))
			return region;
		}

	return NULL;
}

void SFZSound::checkMemoryUsage()
{
    unsigned int memoryUsage = SFZAudioReaderManager::getInstance()->getMemoryUsage();
//    unsigned int memoryLeft = getFreeMemory();
    
#ifdef DEBUG
    printf("Memory left %u, usage: %u\n", memoryLeft, memoryUsage);
#endif
    //memoryLeft < 30000000
    
    // Pick an old sample and release it..
    // FIXME: make this configurable?
    
    int attemptsLeft = 20;
    
    while(memoryUsage > 30000000 && (attemptsLeft--) > 0)    // while?
    {
        unsigned int oldest = UINT_MAX;
        SFZSample *oldestSample = NULL;
        map<string,SFZSample*>::iterator oldestIter;
        
        for (map<string,SFZSample*>::iterator iter = samples.begin(); iter != samples.end(); iter++ )
        {
            SFZSample* sample = iter->second;
            
            // check to see if it's still loaded, and if it's the oldest
            if(sample->getBuffer() && sample->getSampleOrder() < oldest &&
               sample->getBuffer()->getBufferSize() > 2048)
            {
                oldestSample = sample;
                oldestIter = iter;
                oldest = sample->getSampleOrder();
            }
        }
        
        if(oldestSample) {
            memoryUsage -= oldestSample->getBuffer()->getBufferSize() * 4 * oldestSample->getBuffer()->getNumChannels();
            
            //FIXME: I suppose we could also find a way to truncate the full sample down and free the memory instead.
            oldestSample->unload();
            oldestSample->preload(1024, true);

            
        } else {
            // could happen if there are no samples left..
            return;
        }

    }
}


int SFZSound::getNumRegions()
{
	return (int)regions.size();
}


SFZRegion* SFZSound::regionAt(int index)
{
	return regions[index];
}


string SFZSound::getErrorsString()
{
	string result;
	int numErrors = (int)errors.size();
	for (int i = 0; i < numErrors; ++i)
		result += errors[i] + "\n";

	if (unsupportedOpcodes.size() > 0) {
		result += "\nUnsupported opcodes:";
		bool shownOne = false;
		for (map<string,string>::iterator iter = unsupportedOpcodes.begin(); iter != unsupportedOpcodes.end(); iter++ ) {
			if (!shownOne) {
				result += " ";
				shownOne = true;
				}
			else
				result += ", ";
			result += iter->first;
        }
		result += "\n";
		}
	return result;
}


int SFZSound::numSubsounds()
{
	return 1;
}


string SFZSound::subsoundName(int whichSubsound)
{
	return string("");
}


void SFZSound::useSubsound(int whichSubsound)
{
}


int SFZSound::selectedSubsound()
{
	return 0;
}


void SFZSound::dump()
{
	int i;

	int numErrors = errors.size();
	if (numErrors > 0) {
		printf("Errors:\n");
		for (i = 0; i < numErrors; ++i) {

			printf("- %s\n", errors[i].c_str());
			}
		printf("\n");
		}

	if (unsupportedOpcodes.size() > 0) {
		printf("Unused opcodes:\n");
		for (map<string,string>::iterator iter = unsupportedOpcodes.begin(); iter != unsupportedOpcodes.end(); iter++ ) {
				printf("  %s\n", iter->second.c_str());
			}
		printf("\n");
		}

	printf("Regions (%d):\n", regions.size());
	int numRegions = regions.size();
	for (i = 0; i < numRegions; ++i)
		regions[i]->dump();
	printf("\n");

	printf("Samples: (%d)\n", samples.size());
	for (map<string,SFZSample*>::iterator iter = samples.begin(); iter != samples.end(); iter++)
		iter->second->dump();
}



