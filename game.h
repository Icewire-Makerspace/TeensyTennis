#ifndef GAME_H
#define GAME_H

#include "ball.h"
#include "wall.h"
#include "paddle.h"
#include "player.h"
#include "controller.h"
#include "physics\Math2d.h"

#define NUM_HORIZONTAL_WALLS 2
#define NUM_VERTICAL_WALLS 4
#define NUM_PLAYERS 4

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

	// Players
	physics::vector2d playerInitialPoint[NUM_PLAYERS];
	float playerLength[NUM_PLAYERS];
	float playerHeight[NUM_PLAYERS];
	float playerMoveSpeed[NUM_PLAYERS];

	float speed;

};

class Game {
public:
	Game();
	void setup(GameSettings _settings);
	void assignController(int playerNum, PlayerController* _controller);
	void tick(float dt);
	void activatePlayer(int num);
	void deactivatePlayer(int num);
	bool playerIsActive(int num);

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

	GameSettings settings;

private:
	Ball ball;
	HorizontalWall horizontalWalls[NUM_HORIZONTAL_WALLS];
	VerticalWall verticalWalls[NUM_VERTICAL_WALLS];
	Paddle paddles[NUM_PLAYERS];
	Player player[NUM_PLAYERS];
	PlayerController* controller[NUM_PLAYERS];
};

#endif