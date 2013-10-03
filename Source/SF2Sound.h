#ifndef SF2Sound_h
#define SF2Sound_h

#include "SFZCommon.h"
#include "SFZSound.h"


class SF2Sound : public SFZSound {
	public:
		SF2Sound(const Path& file);
		~SF2Sound();

		void	loadRegions();
		void	loadSamples(double* progressVar = NULL);

		struct Preset
        {
            std::string	name;
			int    	bank;
			int   	preset;
            std::vector<SFZRegion *>	regions;

			Preset(std::string nameIn, int bankIn, int presetIn)
				: name(nameIn), bank(bankIn), preset(presetIn) {}
			~Preset();

			void	addRegion(SFZRegion* region)
            {
				regions.push_back(region);
            }
        };
		void	addPreset(Preset* preset);

		int	numSubsounds();
        std::string	subsoundName(int whichSubsound);
		void	useSubsound(int whichSubsound);
		int 	selectedSubsound();

//		SFZSample*	sampleFor(unsigned long sampleRate);
    SFZSample*	sampleFor(unsigned int offset, const std::string &path);


	protected:
        std::vector<Preset *>	presets;
        std::map<unsigned int, SFZSample*>	samplesByOffset;
		int               	selectedPreset;
	};


#endif 	// !SF2Sound_h

