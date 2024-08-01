#pragma once
#include <stdint.h>
#include <cstdlib>
#include <cmath>
#include "SDL.h"
#include <iostream>
#include <vector>
#include <ctime>

const int SCREEN_WIDTH = 1024;
const int SCREEN_HEIGHT = 718;
const int WALL_THICKNESS = 24;
const int PADDLE_HEIGHT = SCREEN_HEIGHT / 4;
const int PADDLE_WIDTH = WALL_THICKNESS;
const int PADDLE_SPEED = 256;
const int BALL_SIZE = 24;
const int BALL_SPEED = 384;

struct Vector2 { float x = 0.0; float y = 0.0; };
struct Ball { Vector2 Position; Vector2 Velocity; };
struct Paddle { Vector2 Position; float Direction; };

class Game final
{
public:

	// returns true if initalized, false if failure
	bool Init();
	// destroys the window and renderer, and shuts down SDL
	void Close();
	// beings the game loop
	void Start();

private:
	// helper
	void SpawnBall();
	void DrawBall(const Ball& ballObj);
	void DrawPaddle(const Paddle& paddleObj);
	void UpdatePaddle(Paddle& paddleObj, float dt);
	void UpdateBall(Ball& ballObj, float dt);
	bool CollidesWithPaddle(const Paddle& paddleObj, const Ball& ballObj);

	// gameloop functions
	void ProcessInput();
	void ProcessLogic();
	void ProcessOutput();

	SDL_Window* mWindow = nullptr;
	SDL_Renderer* mRenderer = nullptr;

	// game state
	bool running = false;
	uint64_t tickCounter = 0;
	uint64_t lastSpawnTick = 0;

	// game objects
	std::vector<Ball> BallVector;
	Paddle Paddle0 = { {PADDLE_WIDTH / 2, SCREEN_HEIGHT / 2 }, 0 };
	Paddle Paddle1 = { {SCREEN_WIDTH - PADDLE_WIDTH / 2, SCREEN_HEIGHT / 2}, 0 };
};
