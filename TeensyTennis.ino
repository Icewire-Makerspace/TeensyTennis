#include <OctoWS2811.h>
#include <OctoWS2811Draw.h>
#include <game.h>

// Set up LED display
#define HORIZONTAL_RESOLUTION 56
#define VERTICAL_RESOLUTION 24
const int ledsPerStrip = 168;
DMAMEM int displayMemory[ledsPerStrip*6];
int drawingMemory[ledsPerStrip*6];
void * reverseMemory[ledsPerStrip*6*4];
const int config = WS2811_GRB | WS2811_800kHz;
OctoWS2811 leds(ledsPerStrip, displayMemory, drawingMemory, reverseMemory, config);

// Set up drawing library
// Colors
#define RED 0xFF0000
#define GREEN 0x00FF00
#define BLUE 0x0000FF
#define YELLOW 0xFFFF00
#define PINK 0xFF1088
#define ORANGE 0xE05800
#define WHITE 0xFFFFFF
#define GRAY 0x646464
OctoWS2811Draw draw(&leds, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);

// Set up game
// The first to 5 points wins
#define WINNING_SCORE 5
#define PHYSICS_TO_PIXEL_RATIO 1
#define HORIZONTAL_WALL_LENGTH 55
#define VERTICAL_WALL_HEIGHT 3 // 3
#define PLAYER_LENGTH 1
#define PLAYER_HEIGHT 5
#define PLAYER_MAX_MOVE_SPEED 100
Game game(HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION, PHYSICS_TO_PIXEL_RATIO);

// Set up controllers
#define PLAYER_1_PIN 17
#define PLAYER_2_PIN 22
PlayerController controller[4];
const int playerPin[2] = {PLAYER_1_PIN, PLAYER_2_PIN};

// Set up player colors
const int playerColor[4] = {RED, BLUE, RED, BLUE};

// Run the game at 60FPS
const float REFRESH_RATE = 1000.0f/60.0f;
unsigned long lastRefresh;
const float frameRate = 1.0f/60.0f;

void setup() {
	// debugging
	Serial.begin(9600); 

	// Init display
	leds.begin();
	leds.show();
	
	// Init game with settings
	GameSettings settings;

	// Set up bounds
	// Floor
	settings.horizontalWallPoints[0] = physics::vector2d(0, 0);
	settings.horizontalWallLengths[0] = HORIZONTAL_WALL_LENGTH;

	// Ceiling
	settings.horizontalWallPoints[1] = physics::vector2d(0, 23);
	settings.horizontalWallLengths[1] = HORIZONTAL_WALL_LENGTH;

	// Bottom left wall
	settings.verticalWallPoints[0] = physics::vector2d(0, 0);
	settings.verticalWallLengths[0] = VERTICAL_WALL_HEIGHT;

	// Top left wall
	settings.verticalWallPoints[1] = physics::vector2d(0, 23 - VERTICAL_WALL_HEIGHT);
	settings.verticalWallLengths[1] = VERTICAL_WALL_HEIGHT;

	// Bottom right wall
	settings.verticalWallPoints[2] = physics::vector2d(55, 0);
	settings.verticalWallLengths[2] = VERTICAL_WALL_HEIGHT;

	// Top right wall
	settings.verticalWallPoints[3] = physics::vector2d(55, 23 - VERTICAL_WALL_HEIGHT);
	settings.verticalWallLengths[3] = VERTICAL_WALL_HEIGHT;

	// Set up ball
	settings.ballInitialPoint = physics::vector2d(28, 11);
	settings.ballDiameter = 2;
	settings.ballInitialVelocity = physics::vector2d(15.0f, 15.0f);
	settings.ballVelocityIncrease = 1.1f;
	settings.ballVelocityIncreaseInterval = 5.0f;
	settings.maxBallVelocity = 50.0f;

	// Set up players
	// Player 1
	settings.playerInitialPoint[0] = physics::vector2d(4, 9);
	settings.playerLength[0] = PLAYER_LENGTH;
	settings.playerHeight[0] = PLAYER_HEIGHT;
	settings.playerMaxMoveSpeed[0] = PLAYER_MAX_MOVE_SPEED;

	// Player 2
	settings.playerInitialPoint[1] = physics::vector2d(50, 9);
	settings.playerLength[1] = PLAYER_LENGTH;
	settings.playerHeight[1] = PLAYER_HEIGHT;
	settings.playerMaxMoveSpeed[1] = PLAYER_MAX_MOVE_SPEED;

	// Player 3
	settings.playerInitialPoint[2] = physics::vector2d(19, 9);
	settings.playerLength[2] = PLAYER_LENGTH;
	settings.playerHeight[2] = PLAYER_HEIGHT;
	settings.playerMaxMoveSpeed[2] = PLAYER_MAX_MOVE_SPEED;

	// Player 4
	settings.playerInitialPoint[3] = physics::vector2d(35, 9);
	settings.playerLength[3] = PLAYER_LENGTH;
	settings.playerHeight[3] = PLAYER_HEIGHT;
	settings.playerMaxMoveSpeed[3] = PLAYER_MAX_MOVE_SPEED;

	// Game speed
	settings.speed = 1.0f;
	
	// Wait time before ball starts moving at start of round
	settings.startDelay = 2.0f;
	
	game.setup(settings);

	// Assign controllers to players
	game.assignController(0, &controller[0]);
	game.assignController(1, &controller[1]);

	// Disable player 3 & 4
	game.deactivatePlayer(2);
	game.deactivatePlayer(3);
}

void loop() {
	unsigned long now = millis();
	if ((now - lastRefresh) >= REFRESH_RATE) {
		lastRefresh= now;

		updateGame(frameRate);

	}
}

// Our game loop
void updateGame(float dt) {
	if (game.winCondition()) {
		game.resetPlayersAndBall();
		if (game.getStats().leftScore >= WINNING_SCORE) {
			drawLeftWinScreen();
			delay(6000);
			// go back to main menu TODO
		} else if (game.getStats().leftScore >= WINNING_SCORE) {
			drawRightWinScreen();
			delay(6000);
			// go back to main menu TODO
		} else {
			drawScore();
			delay(3000);
		}
	} else {
		// Get controller input and convert to player position
		float playerExtentY = PLAYER_HEIGHT * 0.5f;
		for (int i = 0; i < 2; ++i) {
			// Convert the analog input to the player's desired position
			int val = analogRead(playerPin[i]);
			Serial.println(val);
			float desiredPositionY = map(val, 0, 1023, 0, 23);
			
			// Now set player's velocity such that he will be in the desired position next tick
			float currentPositionY = game.getPlayer(i).getPaddle()->getPosition().y;
			float distance = desiredPositionY - currentPositionY;
			float speed;
			
			// Smooth out low distances
			if (distance >= 0) {
				if (distance < 1) {
					distance = 0;
				} else if (distance < 2) {
					distance = 1;
				}
			} else {
				if (distance > -1) {
					distance = 0;
				} else if (distance > -2) {
					distance = -1;
				}
			}
			
			speed = (float)distance / dt;
			controller[i].setSpeed(speed);
		}

		game.tick(dt);
		// Draw the game
		drawGame();
	}
}

void drawGame() {
	draw.clearBuffer();
	
	// For positions, we are converting from float to int so there is a loss of precision,
	// hence the +1
	
	// Bounds
	draw.setColor(GRAY);
	for (int i = 0; i < NUM_HORIZONTAL_WALLS; ++i) {
		float x0 = game.getUtility().physicsToScreenX(game.XpositionOfHorizontalWall(i));
		float y0 = game.getUtility().physicsToScreenY(game.YpositionOfHorizontalWall(i));
		float x1 = game.getUtility().physicsToScreenX(game.XpositionOfHorizontalWall(i) + game.widthOfHorizontalWall(i));
		float y1 = y0;
		draw.line(x0, y0, x1, y1);
	}
	
	for (int i = 0; i < NUM_VERTICAL_WALLS; ++i) {
		float x0 = game.getUtility().physicsToScreenX(game.XpositionOfVerticalWall(i));
		float y0 = game.getUtility().physicsToScreenY(game.YpositionOfVerticalWall(i));
		float x1 = x0;
		float y1 = game.getUtility().physicsToScreenY(game.YpositionOfVerticalWall(i) - game.heightOfVerticalWall(i));
		draw.line(x0, y0, x1, y1);
	}

	// Ball
	draw.setColor(YELLOW);
	{
		float x = game.getUtility().physicsToScreenX(game.XpositionOfBall());
		float y = game.getUtility().physicsToScreenY(game.YpositionOfBall());
		float w = game.getUtility().physicsToScreen(game.diameterOfBall());
		float h = w;
		draw.rect(x+1, y+1, w, h);
	}

	// Players
	for (int i = 0; i < NUM_PLAYERS; ++i) {
		if (game.playerIsActive(i)) {
			draw.setColor(playerColor[i]);
			float x = game.getUtility().physicsToScreenX(game.XpositionOfPlayer(i));
			float y = game.getUtility().physicsToScreenY(game.YpositionOfPlayer(i));
			float w = game.getUtility().physicsToScreen(game.widthOfPlayer(i));
			float h = game.getUtility().physicsToScreen(game.heightOfPlayer(i));
			draw.rect(x, y+1, w+1, h);
		}
	}

	draw.drawBuffer();
}

void drawLeftWinScreen() {
	draw.clearBuffer();
	// TODO
	draw.drawBuffer();
}

void drawRightWinScreen() {
	draw.clearBuffer();
	// TODO
	draw.drawBuffer();
}

void drawScore() {
	draw.clearBuffer();
	// TODO
	draw.drawBuffer();
}