#ifndef SF2Sound_h
#define SF2Sound_h

#include "OpenSFZ.h"
#include "SFZSound.h"


class SF2Sound : public SFZSound {
	public:
		SF2Sound(const File& file);
		~SF2Sound();

		void	loadRegions();
		void	loadSamples(
			AudioFormatManager* formatManager,
			double* progressVar = NULL, Thread* thread = NULL);

		struct Preset {
            std::string	name;
			int    	bank;
			int   	preset;
			OwnedArray<SFZRegion>	regions;

			Preset(std::string nameIn, int bankIn, int presetIn)
				: name(nameIn), bank(bankIn), preset(presetIn) {}
			~Preset() {}

			void	addRegion(SFZRegion* region) {
				regions.add(region);
				}
			};
		void	addPreset(Preset* preset);

		int	numSubsounds();
        std::string	subsoundName(int whichSubsound);
		void	useSubsound(int whichSubsound);
		int 	selectedSubsound();

		SFZSample*	sampleFor(unsigned long sampleRate);
		void	setSamplesBuffer(SFZAudioBuffer* buffer);

	protected:
		OwnedArray<Preset>	presets;
        std::map<unsigned long, SFZSample*>	samplesByRate;
		int               	selectedPreset;
	};


#endif 	// !SF2Sound_h

