#include "Game.h"

// initalization
bool Game::Init()
{
	int initSuccess = SDL_Init(SDL_INIT_EVERYTHING);
	if (initSuccess != 0) {
		SDL_Log("Failed to Initalize SDL, %s", SDL_GetError());
		return false;
	}
	
	mWindow = SDL_CreateWindow(
		"Pong!", 
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
		SCREEN_WIDTH, SCREEN_HEIGHT, 
		0
	);
	if (mWindow == nullptr) {
		SDL_Log("Failed to Create Window, %s", SDL_GetError());
		return false;
	}

	mRenderer = SDL_CreateRenderer(
		mWindow,
		-1,
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
	);
	if (mRenderer == nullptr) {
		SDL_Log("Failed to Create Renderer, %s", SDL_GetError());
		return false;
	}

	return true;
}

void Game::Close()
{
	SDL_DestroyRenderer(mRenderer);
	SDL_DestroyWindow(mWindow);
	SDL_Quit();
}

void Game::Start() 
{
	SpawnBall();
	tickCounter = SDL_GetTicks64();
	lastSpawnTick = SDL_GetTicks64();
	running = true;
	while (running) {
		ProcessInput();
		ProcessLogic();
		ProcessOutput();
	}
}

// helper functions
void Game::SpawnBall()
{
	float angle = 3.14f/4 + .3 / ((std::rand() % 100) / 10);
	if (std::rand() % 2 == 0)
		angle += 3.14f / 2;
	Ball newBall = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, BALL_SPEED * cosf(angle), BALL_SPEED * sinf(angle) };
	BallVector.push_back(newBall);
}

void Game::DrawBall(const Ball& ballObj)
{
	SDL_Rect ballGraphic;
	ballGraphic.x = static_cast<int>(ballObj.Position.x - BALL_SIZE / 2);
	ballGraphic.y = static_cast<int>(ballObj.Position.y - BALL_SIZE / 2);
	ballGraphic.w = BALL_SIZE;
	ballGraphic.h = BALL_SIZE;
	SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);
	SDL_RenderFillRect(mRenderer, &ballGraphic);
}

void Game::DrawPaddle(const Paddle& paddleObj)
{
	SDL_Rect paddleGraphic1;
	paddleGraphic1.x = static_cast<int>(paddleObj.Position.x - PADDLE_WIDTH / 2);
	paddleGraphic1.y = static_cast<int>(paddleObj.Position.y - PADDLE_HEIGHT / 2);
	paddleGraphic1.w = PADDLE_WIDTH;
	paddleGraphic1.h = PADDLE_HEIGHT;
	SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);
	SDL_RenderFillRect(mRenderer, &paddleGraphic1);
}

void Game::UpdatePaddle(Paddle& paddleObj, float dt)
{
	paddleObj.Position.y += paddleObj.Direction * PADDLE_SPEED * dt;
	if (paddleObj.Position.y + PADDLE_HEIGHT / 2 >= SCREEN_HEIGHT - WALL_THICKNESS)
		paddleObj.Position.y = SCREEN_HEIGHT - WALL_THICKNESS - PADDLE_HEIGHT / 2;
	if (paddleObj.Position.y - PADDLE_HEIGHT / 2 <= WALL_THICKNESS)
		paddleObj.Position.y = WALL_THICKNESS + PADDLE_HEIGHT / 2;
}

void Game::UpdateBall(Ball& ballObj, float dt)
{
	Vector2& ballPos = ballObj.Position;
	Vector2& ballVelocity = ballObj.Velocity;
	ballPos.x += ballObj.Velocity.x * dt;
	ballPos.y += ballObj.Velocity.y * dt;

	if (ballPos.y >= SCREEN_HEIGHT - WALL_THICKNESS - BALL_SIZE / 2 && ballVelocity.y > 0.0f)
		ballVelocity.y *= -1.0f;
	if (ballPos.y <= WALL_THICKNESS + BALL_SIZE / 2 && ballVelocity.y < 0.0f)
		ballVelocity.y *= -1.0f;

	if (CollidesWithPaddle(Paddle0, ballObj) && ballVelocity.x < 0.0f)
		ballVelocity.x *= -1.0f;

	if (CollidesWithPaddle(Paddle1, ballObj) && ballVelocity.x > 0.0f)
		ballVelocity.x *= -1.0f;
}

bool Game::CollidesWithPaddle(const Paddle& paddleObj, const Ball& ballObj)
{
	float diffX = std::abs(paddleObj.Position.x - ballObj.Position.x);
	float diffY = std::abs(paddleObj.Position.y - ballObj.Position.y);
	return diffX <= PADDLE_WIDTH / 2 + BALL_SIZE / 2 && diffY <= PADDLE_HEIGHT / 2 + BALL_SIZE / 2;
}


// game loop
void Game::ProcessInput()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type) {
			case SDL_QUIT:
				running = false;
				break;
		}
	}

	const uint8_t* keyboardState = SDL_GetKeyboardState(NULL);

	if (keyboardState[SDL_SCANCODE_ESCAPE])
		running = false;

	Paddle0.Direction = 0;
	if (keyboardState[SDL_SCANCODE_W])
		Paddle0.Direction += -1.0f;
	if (keyboardState[SDL_SCANCODE_S])
		Paddle0.Direction += 1.0f;

	Paddle1.Direction = 0;
	if (keyboardState[SDL_SCANCODE_I])
		Paddle1.Direction += -1.0f;
	if (keyboardState[SDL_SCANCODE_K])
		Paddle1.Direction += 1.0f;
}

void Game::ProcessLogic()
{
	uint64_t tick = SDL_GetTicks64();
	float dt = (tick - tickCounter) / 1000.0f;
	tickCounter = tick;

	// spawn ball after 5 seconds
	if (tick - lastSpawnTick > 2000) {
		SpawnBall();
		lastSpawnTick = tick;
	}

	// update paddle
	UpdatePaddle(Paddle0, dt);
	UpdatePaddle(Paddle1, dt);

	// update balls
	auto currentBall = BallVector.begin();

	// while loop as balls are removed while itterating
	while (currentBall != BallVector.end())
	{
		UpdateBall(*currentBall, dt);
		// remove ball if out of bounds
		if (currentBall->Position.x < -BALL_SIZE * 2 || currentBall->Position.x > SCREEN_WIDTH + BALL_SIZE * 2) {
			currentBall = BallVector.erase(currentBall);
			std::cout << "removed out of bounds ball" << std::endl;
		}
		else {
			currentBall++;
		}
	}
}

void Game::ProcessOutput()
{
	SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, 255);
	SDL_RenderClear(mRenderer);

	for (auto currentBall = BallVector.begin(); currentBall != BallVector.end(); currentBall++) {
		DrawBall(*currentBall);
	}

	DrawPaddle(Paddle0);
	DrawPaddle(Paddle1);

	// draw walls;
	SDL_Rect WallTop;
	WallTop.x = 0;
	WallTop.y = 0;
	WallTop.w = SCREEN_WIDTH;
	WallTop.h = WALL_THICKNESS;
	SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);
	SDL_RenderFillRect(mRenderer, &WallTop);

	SDL_Rect WallBtm;
	WallBtm.x = 0;
	WallBtm.y = SCREEN_HEIGHT - static_cast<int>(WALL_THICKNESS);
	WallBtm.w = SCREEN_WIDTH;
	WallBtm.h = WALL_THICKNESS;
	SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);
	SDL_RenderFillRect(mRenderer, &WallBtm);

	SDL_RenderPresent(mRenderer);
}