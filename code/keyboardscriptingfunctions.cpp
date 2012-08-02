#include "scripting.hpp"
#include "input.hpp"
#include "camera.hpp"
#include "game.hpp"
#include "bindings.hpp"
#include "humanplayer.hpp"
#include "framemanager.hpp"
#include "speech.hpp"

//////////////////////////////////////////////////////////////////////////
// Input binding functions.
//////////////////////////////////////////////////////////////////////////

// Changes the camera mode between land and air object.
// Use this for debugging only!
//SCRIPTING_FUNCTION(setCameraMode)
//{
//    std::string cameraType = scripting->getArgument<std::string>(1);
//    if (cameraType == "land")
//        camera->setType(Camera::camera_type_land_object);
//    else
//        camera->setType(Camera::camera_type_aircraft);
//
//    return 0;
//}

SCRIPTING_FUNCTION(bind)
{
	std::string keyName     = scripting->getArgument<std::string>(1);
	std::string commandName = scripting->getArgument<std::string>(2);

	bindings.bind(keyName, commandName);

	return 0;
}

SCRIPTING_FUNCTION(unbind)
{
	std::string keyName = scripting->getArgument<std::string>(1);

	bindings.unbind(keyName);

	return 0;
}

SCRIPTING_FUNCTION(unbindall)
{
	bindings.unbindall();

	return 0;
}

//////////////////////////////////////////////////////////////////////////

SCRIPTING_FUNCTION(yawLeft)
{
	bool activated = scripting->getArgument<bool>(1);
    game->currControllable->yawLeft(activated);
    return 0;
}

SCRIPTING_FUNCTION(yawRight)
{
	bool activated = scripting->getArgument<bool>(1);
    game->currControllable->yawRight(activated);
    return 0;
}

SCRIPTING_FUNCTION(pitchUp)
{
	bool activated = scripting->getArgument<bool>(1);
    game->currControllable->pitchUp(activated);
    return 0;
}

SCRIPTING_FUNCTION(pitchDown)
{
	bool activated = scripting->getArgument<bool>(1);
    game->currControllable->pitchDown(activated);
    return 0;
}

SCRIPTING_FUNCTION(strafeLeft)
{
	bool activated = scripting->getArgument<bool>(1);
    game->currControllable->strafeLeft(activated);
    return 0;
}

SCRIPTING_FUNCTION(strafeRight)
{
	bool activated = scripting->getArgument<bool>(1);
    game->currControllable->strafeRight(activated);
    return 0;
}

SCRIPTING_FUNCTION(jump)
{
	bool activated = scripting->getArgument<bool>(1);
    game->currControllable->flyUp(activated);
    return 0;
}

SCRIPTING_FUNCTION(crouch)
{
	bool activated = scripting->getArgument<bool>(1);
    game->currControllable->flyDown(activated);
    return 0;
}

SCRIPTING_FUNCTION(dash)
{
	bool activated = scripting->getArgument<bool>(1);
    game->currControllable->dash(activated);
	return 0;
}

SCRIPTING_FUNCTION(walkForward)
{
    bool activated = scripting->getArgument<bool>(1);
    game->currControllable->walkForward(activated);
    return 0;
}
SCRIPTING_FUNCTION(walkBackward)
{
	bool activated = scripting->getArgument<bool>(1);
    game->currControllable->walkBackward(activated);
    return 0;
}

SCRIPTING_FUNCTION(reload)
{
   	//bool activated = scripting->getArgument<bool>(1);
	//game->player->setReloading(activated);
	game->player->reload();
    return 0;
}

SCRIPTING_FUNCTION(shoot)
{
    bool activated = scripting->getArgument<bool>(1);

	// If the player shoots while dead, then reload the level!
	if (activated && game->isVisible() &&
		game->player->getEntity() && game->player->getEntity()->isDead())
	{
		game->reloadCurrentLevel();
	}
	else
		game->player->setShooting(activated && game->isVisible());
    return 0;
}

SCRIPTING_FUNCTION(nextWeapon)
{
	if (scripting->getNumArgs() == 0)
		game->player->changeToNextWeapon();
	else
	{
		bool activated = scripting->getArgument<bool>(1);
		game->player->setChangeToNextWeapon(activated);
	}
    return 0;
}

SCRIPTING_FUNCTION(previousWeapon)
{
	if (scripting->getNumArgs() == 0)
		game->player->changeToPreviousWeapon();
	else
	{
		bool activated = scripting->getArgument<bool>(1);
		game->player->setChangeToPreviousWeapon(activated);
	}
    return 0;
}
SCRIPTING_FUNCTION(throwGrenade)
{
    bool activated = scripting->getArgument<bool>(1);
    game->player->setThrowingGrenade(activated);
    return 0;
}
SCRIPTING_FUNCTION(toogleWeaponOne)
{
    bool activated = scripting->getArgument<bool>(1);
    if(activated)
        game->player->setWeapon(0);
    else
        game->player->setWeapon(-1);
    return 0;
}

SCRIPTING_FUNCTION(toogleWeaponTwo)
{
    bool activated = scripting->getArgument<bool>(1);
    if(activated)
        game->player->setWeapon(1);
    else
        game->player->setWeapon(-1);
    return 0;
}

SCRIPTING_FUNCTION(toogleWeaponThree)
{
    bool activated = scripting->getArgument<bool>(1);
    if(activated)
        game->player->setWeapon(2);
    else
        game->player->setWeapon(-1);
    return 0;
}

SCRIPTING_FUNCTION(toogleWeaponFour)
{
    bool activated = scripting->getArgument<bool>(1);
    if(activated)
        game->player->setWeapon(3);
    else
        game->player->setWeapon(-1);
    return 0;
}

SCRIPTING_FUNCTION(toogleWeaponFive)
{
    bool activated = scripting->getArgument<bool>(1);
    if(activated)
        game->player->setWeapon(4);
    else
        game->player->setWeapon(-1);
    return 0;
}

SCRIPTING_FUNCTION(toogleWeaponSix)
{
    bool activated = scripting->getArgument<bool>(1);
    if(activated)
        game->player->setWeapon(5);
    else
        game->player->setWeapon(-1);
    return 0;
}

SCRIPTING_FUNCTION(toogleWeaponSeven)
{
    bool activated = scripting->getArgument<bool>(1);
    if(activated)
        game->player->setWeapon(6);
    else
        game->player->setWeapon(-1);
    return 0;
}

SCRIPTING_FUNCTION(toogleWeaponEight)
{
    bool activated = scripting->getArgument<bool>(1);
    if(activated)
        game->player->setWeapon(7);
    else
        game->player->setWeapon(-1);
    return 0;
}

SCRIPTING_FUNCTION(toogleWeaponNine)
{
    bool activated = scripting->getArgument<bool>(1);
    if(activated)
        game->player->setWeapon(8);
    else
        game->player->setWeapon(-1);
    return 0;
}

SCRIPTING_FUNCTION(toogleWeaponTen)
{
    bool activated = scripting->getArgument<bool>(1);
    if(activated)
        game->player->setWeapon(9);
    else
        game->player->setWeapon(-1);
    return 0;
}

SCRIPTING_FUNCTION(cancel)
{
	if (isSpeechSequenceActive() || game->areEntitiesFrozen() || 
		isSpeechSoundPlaying())
	{
		resetSpeechSound();
		stopSpeechSequence();
		game->freezeEntities(0.0f);
		game->setTalker("Nobody");
		scripting->executeString(onSpeechFinished);
		onSpeechFinished = "";
	}
	else if (game->isGameInProgress() && game->isVisible())
	{
		game->setEnabled(false);
		frameManager->getFrame("continue_game")->setEnabled(true);
	}
	return 0;
}

void registerKeyboardScriptingFunctions()
{
//	REGISTER_SCRIPTING_FUNCTION(setCameraMode);
    REGISTER_SCRIPTING_FUNCTION(bind);
    REGISTER_SCRIPTING_FUNCTION(unbind);
    REGISTER_SCRIPTING_FUNCTION(unbindall);

    REGISTER_SCRIPTING_FUNCTION(yawLeft);
    REGISTER_SCRIPTING_FUNCTION(yawRight);
    REGISTER_SCRIPTING_FUNCTION(pitchUp);
    REGISTER_SCRIPTING_FUNCTION(pitchDown);
    REGISTER_SCRIPTING_FUNCTION(strafeLeft);
    REGISTER_SCRIPTING_FUNCTION(strafeRight);
    REGISTER_SCRIPTING_FUNCTION(jump);
    REGISTER_SCRIPTING_FUNCTION(crouch);
    REGISTER_SCRIPTING_FUNCTION(dash);
    REGISTER_SCRIPTING_FUNCTION(walkForward);
    REGISTER_SCRIPTING_FUNCTION(walkBackward);
    REGISTER_SCRIPTING_FUNCTION(reload);
    REGISTER_SCRIPTING_FUNCTION(shoot);
	REGISTER_SCRIPTING_FUNCTION(nextWeapon);
	REGISTER_SCRIPTING_FUNCTION(previousWeapon);
    REGISTER_SCRIPTING_FUNCTION(throwGrenade);

    REGISTER_SCRIPTING_FUNCTION(toogleWeaponOne);
    REGISTER_SCRIPTING_FUNCTION(toogleWeaponTwo);
    REGISTER_SCRIPTING_FUNCTION(toogleWeaponThree);
    REGISTER_SCRIPTING_FUNCTION(toogleWeaponFour);
    REGISTER_SCRIPTING_FUNCTION(toogleWeaponFive);
    REGISTER_SCRIPTING_FUNCTION(toogleWeaponSix);
    REGISTER_SCRIPTING_FUNCTION(toogleWeaponSeven);
    REGISTER_SCRIPTING_FUNCTION(toogleWeaponEight);
    REGISTER_SCRIPTING_FUNCTION(toogleWeaponNine);
    REGISTER_SCRIPTING_FUNCTION(toogleWeaponTen);

	REGISTER_SCRIPTING_FUNCTION(cancel);
}
