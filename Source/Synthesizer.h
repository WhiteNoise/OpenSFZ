//
//  Synthesizer.h
//  OpenSFZ
//
//  Created by David Wallin on 4/22/13.
//  Copyright (c) 2013 David Wallin. All rights reserved.
//

#ifndef __OpenSFZ__Synthesizer__
#define __OpenSFZ__Synthesizer__

#include "OpenSFZ.h"
class SynthesizerSound;

class SynthesizerVoice {
public:
    SynthesizerVoice();
    
    void setCurrentPlaybackSampleRate (double newRate);
    void clearCurrentNote();
    int getCurrentlyPlayingNote() { return currentlyPlayingNote; };
protected:
    double sampleRate;
    
    friend class Synthesiser;
    
    int currentlyPlayingNote;
    uint32_t noteOnTime;
    SynthesizerSound *currentlyPlayingSound;
    bool keyIsDown; // the voice may still be playing when the key is not down (i.e. sustain pedal)
    bool sostenutoPedalDown;
    

};

class SynthesizerSound {
public:
	SynthesizerSound();
    virtual ~SynthesizerSound();
    
    
};

class Synthesizer {
public:
	Synthesizer();
	virtual ~Synthesizer();
    void addVoice (SynthesizerVoice* newVoice);
	
    SynthesizerVoice* getVoice (int index) const;

    int getNumVoices() const          
	{ 
		return voices.size(); 
	}

	void clearVoices();
    
    
    //==============================================================================
    /** Deletes all sounds. */
    void clearSounds();
    
    /** Returns the number of sounds that have been added to the synth. */
    int getNumSounds() const                                        { return sounds.size(); }
    
    /** Returns one of the sounds. */
    SynthesizerSound* getSound (int index) const                    { return sounds [index]; }
    
    virtual void noteOn (int midiChannel,
                         int midiNoteNumber,
                         float velocity);
    
    virtual void noteOff (int midiChannel,
                          int midiNoteNumber,
                          bool allowTailOff);
    
    virtual void allNotesOff (int midiChannel,
                              bool allowTailOff);
    
    virtual void handlePitchWheel (int midiChannel,
                                   int wheelValue);
    virtual void handleController (int midiChannel,
                                   int controllerNumber,
                                   int controllerValue);
    void setCurrentPlaybackSampleRate (double sampleRate);
    
    virtual SynthesizerVoice* findFreeVoice (SynthesizerSound* soundToPlay,
                                             const bool stealIfNoneAvailable) const;
    void startVoice (SynthesizerVoice* voice,
                     SynthesizerSound* sound,
                     int midiChannel,
                     int midiNoteNumber,
                     float velocity);
    
    void renderNextBlock (SFZAudioBuffer& outputAudio,
                          int startSample,
                          int numSamples);
public:
    double sampleRate;
    
    std::vector<SynthesizerVoice *>voices;
    std::vector<SynthesizerSound *>sounds;
};

#endif /* defined(__OpenSFZ__Synthesizer__) */
