#ifndef SPEECH_HPP
#define SPEECH_HPP

//
// Represents a sequence of subtitles, and one long sound file that
// contains all of the spoken dialogue for the subtitles.
//

#include <vector>
#include <string>

#include "sound.hpp"

struct Speech
{
	Speech(
		const std::string &talker, 
		const std::string &text, 
		float duration, 
		float relX, 
		float relY)
		: talker(talker), text(text), duration(duration), relX(relX), relY(relY)
	{}

	std::string talker;
	std::string text;
	float duration;
	float relX;
	float relY;
};

typedef std::vector<Speech> SpeechSequence;

void startSpeechSequence(const SpeechSequence &speech);
void stopSpeechSequence();
void updateSpeechSequence();
void drawCurrentSubtitle();
void resetSpeechSound();
bool isSpeechSoundPlaying();
bool isSpeechSequenceActive();

extern SoundPtr currSpeechSound;
extern std::string onSpeechFinished;

#endif