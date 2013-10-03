#include "SF2Sound.h"
#include "SF2Reader.h"
#include "SFZSample.h"

SF2Sound::Preset::~Preset()
{
    for(int i=0; i<regions.size(); i++)
    {
//        delete regions[i]->sample;
        delete regions[i];
    }
    regions.clear();
    
}

/////////////

SF2Sound::SF2Sound(const Path& file)
	: SFZSound(file)
{
}


SF2Sound::~SF2Sound()
{
	// "presets" owns the regions, so clear them out of "regions" so ~SFZSound()
	// doesn't try to delete them.
	regions.clear();
    
    for(int i=0; i<presets.size(); i++)
    {
        delete presets[i];
    }
    
    presets.clear();


	for (std::map<unsigned int, SFZSample*>::iterator iter = samplesByOffset.begin(); iter != samplesByOffset.end(); iter++)
		delete iter->second;

}


class PresetComparator {
	public:
		static int compareElements(const SF2Sound::Preset* first, const SF2Sound::Preset* second) {
			int cmp = first->bank - second->bank;
			if (cmp != 0)
				return cmp;
			return first->preset - second->preset;
			}
	};

void SF2Sound::loadRegions()
{
	SF2Reader reader(this, file);
	reader.read();

	// Sort the presets.
	//PresetComparator comparator;
	//presets.sort(comparator);

	useSubsound(0);
}


void SF2Sound::loadSamples(
	double* progressVar)
{
	SF2Reader reader(this, file);
    reader.findSamples();

    
    	for (std::map<unsigned int, SFZSample*>::iterator iter = samplesByOffset.begin(); iter != samplesByOffset.end(); iter++)
        {
            
            SFZSample* sample  = iter->second;
            // fix offset.
            sample->setSf2Start(sample->getSf2Start() + reader.sampleChunkStart);

            bool ok = false;
            
            #ifdef SFZ_NO_STREAMING
                ok = sample->load();
            #else
                ok = sample->preload(1024);
            #endif
            
            if (!ok)
                addError("Couldn't load sample \"" + sample->getShortName() + "\"");

        }
    
    printf("Loaded %d", samplesByOffset.size());

    


    

	if (progressVar)
		*progressVar = 1.0;
}


void SF2Sound::addPreset(SF2Sound::Preset* preset)
{
	presets.push_back(preset);
}


int SF2Sound::numSubsounds()
{
	return presets.size();
}


std::string SF2Sound::subsoundName(int whichSubsound)
{
	Preset* preset = presets[whichSubsound];
	std::string result;
	if (preset->bank != 0) {
		result += preset->bank;
		result += "/";
		}
	result += preset->preset;
	result += ": ";
	result += preset->name;
	return result;
}


void SF2Sound::useSubsound(int whichSubsound)
{
	selectedPreset = whichSubsound;
	regions.clear();
    
    for(int i=0; i<presets[whichSubsound]->regions.size(); i++)
        addRegion(presets[whichSubsound]->regions[i]);

}


int SF2Sound::selectedSubsound()
{
	return selectedPreset;
}


SFZSample* SF2Sound::sampleFor(unsigned int offset, const std::string &path)
{
	SFZSample* sample = samplesByOffset[offset];
	if (sample == NULL) {
		sample = new SFZSample(path);
		samplesByOffset[offset] = sample;
		}
	return sample;
}





