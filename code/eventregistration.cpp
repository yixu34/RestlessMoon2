#include "eventmanager.hpp"
#include "event.hpp"
#include "scripting.hpp"
#include "command.hpp"
#include "camera.hpp"
#include "console.hpp"
#include "audio.hpp"
#include "game.hpp"
#include "framemanager.hpp"
#include "renderer.hpp"

//
// Register all events and listeners here.
//

void registerEvents()
{
	scripting->registerHandlers();
	audio->registerHandlers();
	renderer->registerHandlers();
	frameManager->registerHandlers();
	game->registerHandlers();
	camera->registerHandlers();
	//console->registerHandlers();
}