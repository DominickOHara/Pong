#include "SDL.h"
#include "Game.h"

int main(int argc, char* argv[])
{
	Game gameInstance;
	if (gameInstance.Init())
		gameInstance.Start();
	gameInstance.Close();
	return 0;
}