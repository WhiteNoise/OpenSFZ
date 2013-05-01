#ifndef SFZSynth_h
#define SFZSynth_h

#include "SFZCommon.h"

class SFZSynth : public Synthesizer {
	public:
		SFZSynth(int numVoices = 32);

        // note: Velocity should be in the 0...1 range
		void	noteOn(
			const int midiChannel, const int midiNoteNumber, const float velocity);
		void	noteOff(
			const int midiChannel, const int midiNoteNumber,
			const bool allowTailOff);

		int	numVoicesUsed();
		std::string   	voiceInfoString();

	protected:
		unsigned char	noteVelocities[128];
	};

#endif 	// !SFZSynth_h

