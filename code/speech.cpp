#include "speech.hpp"
#include "timer.hpp"
#include "graphics.hpp"
#include "common.hpp"
#include "scripting.hpp"
#include "game.hpp"

SpeechSequence currSequence;
float lastSpeechStartTime = 0.0f;
int currSpeechIndex = 0;
bool speechActive = false;
SoundPtr currSpeechSound;
std::string onSpeechFinished = "";

void startSpeechSequence(const SpeechSequence &speech)
{
	MYASSERT(!speech.empty());
	stopSpeechSequence();
	currSequence = speech;
	lastSpeechStartTime = getTimeSeconds();
	currSpeechIndex = 0;
	speechActive = true;
}

void stopSpeechSequence()
{
	currSequence.clear();
	speechActive = false;
}

void updateSpeechSequence()
{
	if (currSequence.empty() || currSpeechIndex >= currSequence.size() ||
		(game->player->getEntity() && game->player->getEntity()->isDead()))
	{
		if (game->player->getEntity() && game->player->getEntity()->isDead())
		{
			game->setTalker("Nobody");
			resetSpeechSound();
		}

		stopSpeechSequence();
		return;
	}

	float timeNow     = getTimeSeconds();
	float timeElapsed = timeNow - lastSpeechStartTime;
	if (timeElapsed >= currSequence[currSpeechIndex].duration)
	{
		lastSpeechStartTime = timeNow;
		currSpeechIndex++;

		if (currSpeechIndex >= currSequence.size())
		{
			game->setTalker("Nobody");
			scripting->executeString(onSpeechFinished);
			onSpeechFinished = "";
			stopSpeechSequence();
			return;
		}

		game->setTalker(currSequence[currSpeechIndex].talker);
	}
}

void drawCurrentSubtitle()
{
	if (currSequence.empty() || currSpeechIndex >= currSequence.size())
		return;

	const Speech &currSpeech = currSequence[currSpeechIndex];
	graphics->drawTextRel(
		currSpeech.relX, 
		currSpeech.relY, 
		currSpeech.text, 
		false);
}

void resetSpeechSound()
{
	if (currSpeechSound)
		currSpeechSound->stop();

	currSpeechSound.reset();
}

bool isSpeechSoundPlaying()
{
	return currSpeechSound && currSpeechSound->isPlaying();
}

bool isSpeechSequenceActive()
{
	return speechActive;
}