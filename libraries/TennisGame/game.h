#ifndef GAME_H
#define GAME_H

#include "ball.h"
#include "wall.h"
#include "paddle.h"
#include "player.h"
#include "controller.h"
#include "physics/math2d.h"
#include "collision2d.h"

#define NUM_HORIZONTAL_WALLS 2
#define NUM_VERTICAL_WALLS 4
#define MAX_NUM_PLAYERS 4

struct GameUtility {
	int screenWidth;
	int screenHeight;
	int physicsToPixelRatio;

	float screenToPhysics(float p) const {
		return p / physicsToPixelRatio;
	}

	float physicsToScreen(float p) const {
		return p * physicsToPixelRatio;
	}

	float physicsToScreenX(float x) const {
		return physicsToScreen(x);
	}

	float physicsToScreenY(float y) const {
		return (screenHeight - 1) - (physicsToScreen(y));
	}
};

struct GameSettings {
	// Bounds
	physics::vector2d horizontalWallPoints[NUM_HORIZONTAL_WALLS];
	float horizontalWallLengths[NUM_HORIZONTAL_WALLS];
	physics::vector2d verticalWallPoints[NUM_VERTICAL_WALLS];
	float verticalWallLengths[NUM_VERTICAL_WALLS];

	// Ball
	physics::vector2d ballInitialPoint;
	float ballDiameter;
	physics::vector2d ballInitialVelocity;
	float ballVelocityIncrease;
	float ballVelocityIncreaseInterval;
	float maxBallVelocity;

	// Players
	physics::vector2d playerInitialPoint[MAX_NUM_PLAYERS];
	float playerLength[MAX_NUM_PLAYERS];
	float playerHeight[MAX_NUM_PLAYERS];
	float playerMaxMoveSpeed[MAX_NUM_PLAYERS];

	float speed;
	float startDelay;
};

struct GameStats {
	int leftScore;
	int rightScore;
	unsigned int tick;
};

class Game {
public:
	Game(int _screenWidth, int _screenHeight, int _physicsToPixelRatio);
	void setup(GameSettings _settings);
	void resetPlayersAndBall();
	void resetScore();
	void assignController(int playerNum, PlayerController* _controller);
	bool winCondition();
	void tick(float dt);
	void activatePlayer(int num);
	void deactivatePlayer(int num);
	bool playerIsActive(int num);
	const Player& getPlayer(int num);
	const GameStats& getStats();
	const GameUtility& getUtility();
	void changeStartDelay(float _startDelay);
	float getStartDelay();
	bool ballIsPaused();
	float currentStartTime();

	// For drawing
	float XpositionOfHorizontalWall(int num);
	float YpositionOfHorizontalWall(int num);
	float widthOfHorizontalWall(int num);
	float XpositionOfVerticalWall(int num);
	float YpositionOfVerticalWall(int num);
	float heightOfVerticalWall(int num);
	float XpositionOfBall();
	float YpositionOfBall();
	float diameterOfBall();
	float XpositionOfPlayer(int num);
	float YpositionOfPlayer(int num);
	float widthOfPlayer(int num);
	float heightOfPlayer(int num);
	
private:
	void updatePlayers();
	void updatePhysics(float dt);

	GameUtility utility;
	GameSettings settings;
	GameStats stats;
	Ball ball;
	HorizontalWall horizontalWalls[NUM_HORIZONTAL_WALLS];
	VerticalWall verticalWalls[NUM_VERTICAL_WALLS];
	Paddle paddles[MAX_NUM_PLAYERS];
	Player player[MAX_NUM_PLAYERS];
	PlayerController* controller[MAX_NUM_PLAYERS];
	
	float startTimer;
	float ballVelocityIncreaseTimer;
	
	bool pauseBall;
};

#endif