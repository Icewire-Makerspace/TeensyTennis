#include "game.h"
#include "collision2d.h"

Game::Game(int _screenWidth, int _screenHeight, int _physicsToPixelRatio) {
	utility.screenWidth = _screenWidth;
	utility.screenHeight = _screenHeight;
	utility.physicsToPixelRatio = _physicsToPixelRatio;
	
	for (int i = 0; i < MAX_NUM_PLAYERS; ++i) {
		controller[i] = 0;
	}
}

void Game::setup(GameSettings _settings) {
	// Settings
	settings = _settings;

	for (int i = 0; i < NUM_HORIZONTAL_WALLS; ++i) {
		horizontalWalls[i].setup(settings.horizontalWallPoints[i], settings.horizontalWallLengths[i]);
	}

	for (int i = 0; i < NUM_VERTICAL_WALLS; ++i) {
		verticalWalls[i].setup(settings.verticalWallPoints[i], settings.verticalWallLengths[i]);
	}
	
	resetPlayersAndBall();
	resetScore();
}

void Game::resetPlayersAndBall() {
	startTimer = 0;
	pauseBall = true;
	ballVelocityIncreaseTimer = 0;
	ball.setup(settings.ballInitialPoint, settings.ballDiameter, settings.ballInitialVelocity);

	// Randomize ball take-off
	if (stats.tick && stats.tick % 2 == 0) {
		ball.reverseVelocityX();
	}
	if (stats.tick && stats.tick % 3 == 0) {
		ball.reverseVelocityY();
	}

	for (int i = 0; i < MAX_NUM_PLAYERS; ++i) {
		paddles[i].setup(settings.playerInitialPoint[i], settings.playerLength[i], settings.playerHeight[i]);
		player[i].setup(&paddles[i], settings.playerMaxMoveSpeed[i]);
	}
}

void Game::resetScore() {
	stats.leftScore = 0;
	stats.rightScore = 0;
	stats.tick = 0;
}

void Game::assignController(int playerNum, PlayerController* _controller) {
	controller[playerNum] = _controller;
}

bool Game::winCondition() {
	if (ball.getPosition().x + ball.getRadius() + 1 < 0) {
		++stats.rightScore;
		return true;
	} else if (ball.getPosition().x - ball.getRadius() - 1 > utility.screenToPhysics(utility.screenWidth - 1)) {
		++stats.leftScore;
		return true;
	}
	return false;
}

void Game::tick(float dt) {
	++stats.tick;
	
	if (startTimer >= 0) {
		startTimer += dt;
		if (startTimer >= settings.startDelay) {
			startTimer = -1;
			pauseBall = false;
		}
	}
	
	if (!pauseBall) {
		ballVelocityIncreaseTimer += dt;
		if (ballVelocityIncreaseTimer >= settings.ballVelocityIncreaseInterval) {
			ballVelocityIncreaseTimer = 0;
			ball.increaseVelocity(settings.ballVelocityIncrease);
			
			if (ball.getVelocityX() > settings.maxBallVelocity) {
				ball.setVelocityX(settings.maxBallVelocity);
			} else if (ball.getVelocityX() < -settings.maxBallVelocity) {
				ball.setVelocityX(-settings.maxBallVelocity);
			}
			
			if (ball.getVelocityY() > settings.maxBallVelocity) {
				ball.setVelocityY(settings.maxBallVelocity);
			} else if (ball.getVelocityY() < -settings.maxBallVelocity) {
				ball.setVelocityY(-settings.maxBallVelocity);
			}
		}
	}
	
	updatePlayers();
	updatePhysics(dt * settings.speed);
}

void Game::activatePlayer(int num) {
	player[num].active = true;
}

void Game::deactivatePlayer(int num) {
	player[num].active = false;
}

bool Game::playerIsActive(int num) {
	return player[num].active;
}

const Player& Game::getPlayer(int num) {
	return player[num];
}

const GameStats& Game::getStats() {
	return stats;
}

const GameUtility& Game::getUtility() {
	return utility;
}

void Game::changeStartDelay(float _startDelay) {
	settings.startDelay = _startDelay;
}

float Game::getStartDelay() {
	return settings.startDelay;
}

bool Game::ballIsPaused() {
	return pauseBall;
}

float Game::currentStartTime() {
	return startTimer;
}

void Game::changeBallInitialVelocity(physics::vector2d velocity) {
	settings.ballInitialVelocity = velocity;
}

float Game::XpositionOfHorizontalWall(int num) {
	return horizontalWalls[num].getPosition().x - horizontalWalls[num].getExtent();
}

float Game::YpositionOfHorizontalWall(int num) {
	return horizontalWalls[num].getPosition().y;
}

float Game::widthOfHorizontalWall(int num) {
	return horizontalWalls[num].getExtent() * 2.0f;
}

float Game::XpositionOfVerticalWall(int num) {
	return verticalWalls[num].getPosition().x;
}

float Game::YpositionOfVerticalWall(int num) {
	return verticalWalls[num].getPosition().y + verticalWalls[num].getExtent();
}

float Game::heightOfVerticalWall(int num) {
	return verticalWalls[num].getExtent() * 2.0f;
}

float Game::XpositionOfBall() {
	return ball.getPosition().x - ball.getRadius();
}

float Game::YpositionOfBall() {
	return ball.getPosition().y + ball.getRadius();
}

float Game::diameterOfBall() {
	return ball.getRadius() * 2.0f;
}

float Game::XpositionOfPlayer(int num) {
	return player[num].getPaddle()->getPosition().x - player[num].getPaddle()->getExtents().x;
}

float Game::YpositionOfPlayer(int num) {
	return player[num].getPaddle()->getPosition().y + player[num].getPaddle()->getExtents().y;
}

float Game::widthOfPlayer(int num) {
	return player[num].getPaddle()->getExtents().x * 2.0f;
}

float Game::heightOfPlayer(int num) {
	return player[num].getPaddle()->getExtents().y * 2.0f;
}

void Game::updatePlayers() {
	for (int i = 0; i < MAX_NUM_PLAYERS; ++i) {
		if (player[i].active && controller[i]) {
			player[i].changeSpeedTo(controller[i]->getSpeed());
		}
	}
}

void Game::updatePhysics(float dt) {
	// Ball collision check variables
	physics::vector2d collisionPositionOfBall;
	float timeOfCollision;
	float ballDt = dt;

	// Paddle collision check variables
	physics::vector2d collisionPositionOfPaddle;
	physics::vector2d paddleToBallCollisionPosition;
	
	// Ball collision check and update
	if (!pauseBall) {
		bool collision = true;
		int collisionLoopCount = 0;
		int maxCollisionLoops = 3;

		while (collision && collisionLoopCount < maxCollisionLoops) {
			collision = false;

			// Players
			const Paddle* paddle;
			for (int i = 0; i < MAX_NUM_PLAYERS; ++i) {
				if (player[i].active) {
					paddle = player[i].getPaddle();
					if (physics::movingBoxesCollide(ballDt, *ball.getPhysicsObject(), *paddle->getPhysicsObject(), collisionPositionOfBall, collisionPositionOfPaddle, timeOfCollision)) {
						collision = true;

						// This is a bug fix so that the ball will never collide with a paddle such that the collision position is around a wall
						//physics::vector2d pos(player[i].getPaddle()->getPosition().x, physics::paddedCollisionPosition(collisionPositionOfPaddle, paddle->getPhysicsObject()->extents, ball.getPosition(), ball.getPhysicsObject()->extents).y); unnecessary
						physics::vector2d pos(player[i].getPaddle()->getPosition().x, collisionPositionOfPaddle.y);
						player[i].setPosition(pos);
						player[i].stop();

						ball.setPosition(physics::paddedCollisionPosition(collisionPositionOfBall, ball.getPhysicsObject()->extents, paddle->getPosition(), paddle->getExtents()));
						paddleToBallCollisionPosition = collisionPositionOfBall - collisionPositionOfPaddle;
				
						// Traditional physics except just bounce off top of paddle
						float paddleToBallLengthY = paddleToBallCollisionPosition.y;
						if (paddleToBallLengthY < 0) {
							paddleToBallLengthY = -paddleToBallLengthY;
						}
						if (paddleToBallLengthY >= ball.getPhysicsObject()->extents.y + paddle->getPhysicsObject()->extents.y) {
							if (paddleToBallCollisionPosition.y >= 0) {
								if (ball.getPhysicsObject()->velocity.y < 0) {
									ball.reverseVelocityY();
								}
							} else {
								if (ball.getPhysicsObject()->velocity.y > 0) {
									ball.reverseVelocityY();
								}
							}
						} else {
							if (paddleToBallCollisionPosition.x >= 0) {
								if (ball.getPhysicsObject()->velocity.x < 0) {
									ball.reverseVelocityX();
								}
							} else {
								if (ball.getPhysicsObject()->velocity.x > 0) {
									ball.reverseVelocityX();
								}
							}
							ball.setVelocityY(paddleToBallCollisionPosition.y * 11.0f);
						}

						// TMP interdasting physics but uses math.h
						/*
						float ballVelocityMag = sqrt(powf(ball.getPhysicsObject()->velocity.x, 2.0f) + powf(ball.getPhysicsObject()->velocity.y, 2.0f));
						float paddleToBallMag = sqrt(powf(paddleToBallCollisionPosition.x, 2.0f) + powf(paddleToBallCollisionPosition.y, 2.0f));
						paddleToBallCollisionPosition = (paddleToBallCollisionPosition / paddleToBallMag) * ballVelocityMag;
						if (paddleToBallCollisionPosition.x > 0 && paddleToBallCollisionPosition.x < 1) {
							paddleToBallCollisionPosition.x = 1;
						}
						if (paddleToBallCollisionPosition.x < 0 && paddleToBallCollisionPosition.x > -1) {
							paddleToBallCollisionPosition.x = -1;
						}
						ball.setVelocityX(paddleToBallCollisionPosition.x);
						ball.setVelocityY(paddleToBallCollisionPosition.y);
						*/

						ballDt = ballDt * (1.0f - timeOfCollision);
					}
				}
			}

			// Vertical walls
			for (int i = 0; i < NUM_VERTICAL_WALLS; ++i) {
				if (physics::movingBoxCollidesWithVerticalLine(ballDt, *ball.getPhysicsObject(), *verticalWalls[i].getPhysicsObject(), collisionPositionOfBall, timeOfCollision)) {
					collision = true;

					ball.setPosition(physics::paddedCollisionPosition(collisionPositionOfBall, ball.getPhysicsObject()->extents, verticalWalls[i].getPosition(), verticalWalls[i].getExtents()));

					if (ball.getPosition().x >= verticalWalls[i].getPosition().x) {
						if (ball.getPhysicsObject()->velocity.x < 0) {
							ball.reverseVelocityX();
						}
					} else {
						if (ball.getPhysicsObject()->velocity.x > 0) {
							ball.reverseVelocityX();
						}
					}
					//ball.reverseVelocityX(); TMP remove

					ballDt = ballDt * (1.0f - timeOfCollision);
				}
			}

			// Horizontal walls
			for (int i = 0; i < NUM_HORIZONTAL_WALLS; ++i) {
				if (physics::movingBoxCollidesWithHorizontalLine(ballDt, *ball.getPhysicsObject(), *horizontalWalls[i].getPhysicsObject(), collisionPositionOfBall, timeOfCollision)) {
					collision = true;

					ball.setPosition(physics::paddedCollisionPosition(collisionPositionOfBall, ball.getPhysicsObject()->extents, horizontalWalls[i].getPosition(), horizontalWalls[i].getExtents()));

					if (ball.getPosition().y >= horizontalWalls[i].getPosition().y) {
						if (ball.getPhysicsObject()->velocity.y < 0) {
							ball.reverseVelocityY();
						}
					} else {
						if (ball.getPhysicsObject()->velocity.y > 0) {
							ball.reverseVelocityY();
						}
					}
					//ball.reverseVelocityY(); TMP remove

					ballDt = ballDt * (1.0f - timeOfCollision);
				}
			}

			++collisionLoopCount;
		}

		// Update
		ball.updatePhysics(ballDt);
	}

	// Players collision check and update
	const Paddle* paddle;
	for (int i = 0; i < MAX_NUM_PLAYERS; ++i) {
		if (player[i].active) {
			paddle = player[i].getPaddle();

			// Horizontal walls
			for (int j = 0; j < 2; ++j) {
				if (physics::movingBoxCollidesWithHorizontalLine(dt, *paddle->getPhysicsObject(), *horizontalWalls[j].getPhysicsObject(), collisionPositionOfPaddle, timeOfCollision)) {
					player[i].setPosition(physics::paddedCollisionPosition(collisionPositionOfPaddle, paddle->getPhysicsObject()->extents, horizontalWalls[j].getPosition(), horizontalWalls[j].getExtents()));
					player[i].stop();
				}
			}

			// Update
			player[i].update(dt);
		}
	}
}