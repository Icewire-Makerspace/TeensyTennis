#include "game.h"
#include "collision2d.h"

Game::Game(int _screenWidth, int _screenHeight, int _physicsToPixelRatio) {
	utility.screenWidth = _screenWidth;
	utility.screenHeight = _screenHeight;
	utility.physicsToPixelRatio = _physicsToPixelRatio;
	
	for (int i = 0; i < MAX_NUM_PLAYERS; ++i) {
		controller[i] = 0;
	}
	
	ballCollidesWithWall = 0;
	ballCollidesWithPaddle = 0;
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

void Game::setBallCollidesWithWall(void (*_ballCollidesWithWall)()) {
	ballCollidesWithWall = _ballCollidesWithWall;
}

void Game::setBallCollidesWithPaddle(void (*_ballCollidesWithPaddle)()) {
	ballCollidesWithPaddle = _ballCollidesWithPaddle;
}

void Game::updatePlayers() {
	for (int i = 0; i < MAX_NUM_PLAYERS; ++i) {
		if (player[i].active && controller[i]) {
			player[i].changeVerticalSpeedTo(controller[i]->getSpeed());
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

		// A value of 3 is sufficient for objects moving at a quick speed, but this should be increased as the collision speed between two objects exceeds the bounds around that collision
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
						if (ballCollidesWithPaddle) {
							ballCollidesWithPaddle();
						}

						player[i].setPosition(collisionPositionOfPaddle);
						ball.setPosition(collisionPositionOfBall);
						paddleToBallCollisionPosition = collisionPositionOfBall - collisionPositionOfPaddle;
				
						// Traditional physics except just bounce off top of paddle
						float paddleToBallLengthY = paddleToBallCollisionPosition.y;
						if (paddleToBallLengthY < 0) {
							paddleToBallLengthY = -paddleToBallLengthY;
						}
						if (paddleToBallLengthY >= ball.getPhysicsObject()->extents.y + paddle->getPhysicsObject()->extents.y) {
							if (paddleToBallCollisionPosition.y >= 0) {
								// This is so the paddle will never shove the ball through another object
								if (player[i].getPaddle()->getVelocity().y > 0) {
									player[i].changeVerticalSpeedTo(0);
								}

								if (ball.getPhysicsObject()->velocity.y < 0) {
									ball.reverseVelocityY();
								}
							} else {
								// This is so the paddle will never shove the ball through another object
								if (player[i].getPaddle()->getVelocity().y < 0) {
									player[i].changeVerticalSpeedTo(0);
								}

								if (ball.getPhysicsObject()->velocity.y > 0) {
									ball.reverseVelocityY();
								}
							}
						} else {
							if (paddleToBallCollisionPosition.x >= 0) {
								// This is so the paddle will never shove the ball through another object
								if (player[i].getPaddle()->getVelocity().x > 0) {
									player[i].changeHorizontalSpeedTo(0);
								}

								if (ball.getPhysicsObject()->velocity.x < 0) {
									ball.reverseVelocityX();
								}
							} else {
								// This is so the paddle will never shove the ball through another object
								if (player[i].getPaddle()->getVelocity().x < 0) {
									player[i].changeHorizontalSpeedTo(0);
								}

								if (ball.getPhysicsObject()->velocity.x > 0) {
									ball.reverseVelocityX();
								}
							}
							// Magic number!
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
					if (ballCollidesWithWall) {
						ballCollidesWithWall();
					}
					
					ball.setPosition(collisionPositionOfBall);

					if (ball.getPosition().x >= verticalWalls[i].getPosition().x) {
						if (ball.getPhysicsObject()->velocity.x < 0) {
							ball.reverseVelocityX();
						}
					} else {
						if (ball.getPhysicsObject()->velocity.x > 0) {
							ball.reverseVelocityX();
						}
					}

					ballDt = ballDt * (1.0f - timeOfCollision);
				}
			}

			// Horizontal walls
			for (int i = 0; i < NUM_HORIZONTAL_WALLS; ++i) {
				if (physics::movingBoxCollidesWithHorizontalLine(ballDt, *ball.getPhysicsObject(), *horizontalWalls[i].getPhysicsObject(), collisionPositionOfBall, timeOfCollision)) {
					collision = true;
					if (ballCollidesWithWall) {
						ballCollidesWithWall();
					}

					ball.setPosition(collisionPositionOfBall);

					if (ball.getPosition().y >= horizontalWalls[i].getPosition().y) {
						if (ball.getPhysicsObject()->velocity.y < 0) {
							ball.reverseVelocityY();
						}
					} else {
						if (ball.getPhysicsObject()->velocity.y > 0) {
							ball.reverseVelocityY();
						}
					}

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
			for (int j = 0; j < NUM_HORIZONTAL_WALLS; ++j) {
				if (physics::movingBoxCollidesWithHorizontalLine(dt, *paddle->getPhysicsObject(), *horizontalWalls[j].getPhysicsObject(), collisionPositionOfPaddle, timeOfCollision)) {

					// Since collision with the ball can adjust the paddle's position, we want to make sure the paddle doesn't go through the wall
					player[i].setPosition(physics::paddedCollisionPosition(collisionPositionOfPaddle, paddle->getPhysicsObject()->extents, horizontalWalls[j].getPosition(), horizontalWalls[j].getExtents()));

					// Stop the paddle from entering the wall
					if (collisionPositionOfPaddle.y >= horizontalWalls[j].getPosition().y) {
						if (player[i].getPaddle()->getVelocity().y < 0) {
							player[i].changeVerticalSpeedTo(0);
						}
					} else {
						if (player[i].getPaddle()->getVelocity().y > 0) {
							player[i].changeVerticalSpeedTo(0);
						}
					}
				}
			}

			// Vertical walls
			for (int j = 0; j < NUM_VERTICAL_WALLS; ++j) {
				if (physics::movingBoxCollidesWithVerticalLine(dt, *paddle->getPhysicsObject(), *verticalWalls[j].getPhysicsObject(), collisionPositionOfPaddle, timeOfCollision)) {

					// Since collision with the ball can adjust the paddle's position, we want to make sure the paddle doesn't go through the wall
					player[i].setPosition(physics::paddedCollisionPosition(collisionPositionOfPaddle, paddle->getPhysicsObject()->extents, verticalWalls[j].getPosition(), verticalWalls[j].getExtents()));

					// Stop the paddle from entering the wall
					if (collisionPositionOfPaddle.x >= verticalWalls[j].getPosition().x) {
						if (player[i].getPaddle()->getVelocity().x < 0) {
							player[i].changeHorizontalSpeedTo(0);
						}
					} else {
						if (player[i].getPaddle()->getVelocity().x > 0) {
							player[i].changeHorizontalSpeedTo(0);
						}
					}
				}
			}

			// Update
			player[i].update(dt);
		}
	}
}