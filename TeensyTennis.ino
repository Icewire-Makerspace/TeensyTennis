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
OctoWS2811Draw draw(&leds, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);

// Set up game
// The first to 3 points wins
#define WINNING_SCORE 3
#define PHYSICS_TO_PIXEL_RATIO 1
#define HORIZONTAL_WALL_LENGTH 55
#define VERTICAL_WALL_HEIGHT 3
#define PLAYER_LENGTH 1
#define PLAYER_HEIGHT 5
#define PLAYER_MAX_MOVE_SPEED 100.0f
#define BALL_INITIAL_SPEED 10.0f
#define BALL_FINAL_ROUND_SPEED 25.0f
#define BALL_MAX_SPEED 140.0f
#define BALL_INCREASE_SPEED_INTERVAL 2.5f
Game game(HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION, PHYSICS_TO_PIXEL_RATIO);

// Set up controllers
#define NUM_PLAYERS 4
#define PLAYER_1_PIN 17
#define PLAYER_2_PIN 18
#define PLAYER_3_PIN 19
#define PLAYER_4_PIN 22
PlayerController controller[NUM_PLAYERS];
const int playerPin[NUM_PLAYERS] = {PLAYER_1_PIN, PLAYER_2_PIN, PLAYER_4_PIN, PLAYER_3_PIN};

// Set up player colors
const char* TEAM_1_COLOR = "GREEN";
const char* TEAM_2_COLOR = "WHITE";
const int playerColor[NUM_PLAYERS] = {GREEN, WHITE, WHITE, GREEN};

// Run the game at 60FPS
const float REFRESH_RATE = 1000.0f/60.0f;
unsigned long lastRefresh;
const float frameRate = 1.0f/60.0f;

// Button
#define BUTTON_PIN 23
const int debounceTime = 10000;
volatile unsigned long lastTime;
volatile bool buttonPressed;

// States
enum State {
	MAIN_MENU,
	TWO_PLAYERS,
	FOUR_PLAYERS
};

State state;
void (*update)(float);

// Boundary changes colors
#define NORMAL_BOUNDARY_CHANGE_COLOR_SPEED 100.0f
#define FINAL_ROUND_BOUNDARY_CHANGE_COLOR_SPEED 600.0f
float boundaryChangeColorSpeed;
float boundR;
float boundG;
float boundB;
const uint32_t boundaryColors[5] = {0x992099, // pink
0x209920, // green
0x993200, // orange
0x999920, // yellow
0x209999}; // blue
int currentBoundaryColor;

#define INITIAL_START_DELAY 5.0f
#define ROUND_START_DELAY 2.0f

// Audio
#define BUZZER_PIN 12

void setup() {
	// Init display
	leds.begin();
	leds.show();
	
	GameSettings settings;

	// Set up bounds
	// Floor
	settings.horizontalWallPoints[0] = physics::vector2d(0, 0);

	// Ceiling
	settings.horizontalWallPoints[1] = physics::vector2d(0, 23);
	
	for (int i = 0; i < 2; ++i) {
		settings.horizontalWallLengths[i] = HORIZONTAL_WALL_LENGTH;
	}

	// Bottom left wall
	settings.verticalWallPoints[0] = physics::vector2d(0, 0);

	// Top left wall
	settings.verticalWallPoints[1] = physics::vector2d(0, 23 - VERTICAL_WALL_HEIGHT);

	// Bottom right wall
	settings.verticalWallPoints[2] = physics::vector2d(55, 0);

	// Top right wall
	settings.verticalWallPoints[3] = physics::vector2d(55, 23 - VERTICAL_WALL_HEIGHT);
	
	for (int i = 0; i < 4; ++i) {
		settings.verticalWallLengths[i] = VERTICAL_WALL_HEIGHT;
	}

	// Set up players
	// Player 1
	settings.playerInitialPoint[0] = physics::vector2d(4, 9);

	// Player 2
	settings.playerInitialPoint[1] = physics::vector2d(50, 9);

	// Player 3
	settings.playerInitialPoint[2] = physics::vector2d(35, 9);

	// Player 4
	settings.playerInitialPoint[3] = physics::vector2d(19, 9);
	
	for (int i = 0; i < NUM_PLAYERS; ++i) {
		settings.playerLength[i] = PLAYER_LENGTH;
		settings.playerHeight[i] = PLAYER_HEIGHT;
		settings.playerMaxMoveSpeed[i] = PLAYER_MAX_MOVE_SPEED;
	}
	
	// Set up ball
	settings.ballInitialPoint = physics::vector2d(26, 11);
	settings.ballDiameter = 2;
	settings.ballInitialVelocity = physics::vector2d(BALL_INITIAL_SPEED, BALL_INITIAL_SPEED);
	settings.ballVelocityIncrease = 1.1f;
	settings.ballVelocityIncreaseInterval = BALL_INCREASE_SPEED_INTERVAL;
	settings.maxBallVelocity = BALL_MAX_SPEED;

	// Game speed
	settings.speed = 1.0f;
	
	// Wait time before ball starts moving at start of round
	settings.startDelay = ROUND_START_DELAY;
	
	// Init game with settings
	game.setup(settings);

	// Assign controllers to players
	game.assignController(0, &controller[0]);
	game.assignController(1, &controller[1]);
	game.assignController(2, &controller[2]);
	game.assignController(3, &controller[3]);

	// Disable player 3 & 4
	game.deactivatePlayer(2);
	game.deactivatePlayer(3);
	
	// Callbacks for audio
	game.setBallCollidesWithWall(playLowBeep);
	game.setBallCollidesWithPaddle(playHighBeep);
	
	// Button
	pinMode(BUTTON_PIN, INPUT);
	lastTime = 0;
	buttonPressed = false;
	attachInterrupt(BUTTON_PIN, isrButton, FALLING);
	
	// Initial state is main menu
	goToMainMenu();
	
	// Set up boundary color
	boundR = 50;
	boundG = 50;
	boundB = 50;
	currentBoundaryColor = 0;
}

void isrButton() {
	noInterrupts();
	unsigned long time = micros();
	if (time - lastTime >= debounceTime) {
		lastTime = time;
		// Since the IR receiver can pick up some false positives
		if (digitalRead(BUTTON_PIN) == 0) {
			buttonPressed = true;
		}
	}
	interrupts();
}

void goToMainMenu() {
	state = MAIN_MENU;
	update = updateMainMenu;
}

void goToTwoPlayers() {
	state = TWO_PLAYERS;
	boundaryChangeColorSpeed = NORMAL_BOUNDARY_CHANGE_COLOR_SPEED;
	game.changeBallInitialVelocity(physics::vector2d(BALL_INITIAL_SPEED, BALL_INITIAL_SPEED));
	game.resetScore();
	game.resetPlayersAndBall();
	game.deactivatePlayer(2);
	game.deactivatePlayer(3);
	game.changeStartDelay(INITIAL_START_DELAY);
	update = updateGame;
}

void goToFourPlayers() {
	state = FOUR_PLAYERS;
	boundaryChangeColorSpeed = NORMAL_BOUNDARY_CHANGE_COLOR_SPEED;
	game.changeBallInitialVelocity(physics::vector2d(BALL_INITIAL_SPEED, BALL_INITIAL_SPEED));
	game.resetScore();
	game.resetPlayersAndBall();
	game.activatePlayer(2);
	game.activatePlayer(3);
	game.changeStartDelay(INITIAL_START_DELAY);
	update = updateGame;
}

void loop() {
	// Handle button pressed
	if (buttonPressed) {
		// Let's still make sure we don't hit the button more than once at a time
		delay(600);
		buttonPressed = false;
		switch (state) {
			case MAIN_MENU:
				goToTwoPlayers();
				break;
			case TWO_PLAYERS:
				goToFourPlayers();
				break;
			case FOUR_PLAYERS:
				goToMainMenu();
		}
	}

	unsigned long now = millis();
	if ((now - lastRefresh) >= REFRESH_RATE) {
		lastRefresh= now;
		update(frameRate);
	}
}

void updateMainMenu(float dt) {
	// Not enough time and space to do too much here so let's just show the title
	drawTitle();
}

// Our game loop
void updateGame(float dt) {
	// Check whether round was won
	if (game.winCondition()) {
		delay(500);
		if (game.getStats().leftScore >= WINNING_SCORE) {
			drawLeftWinScreen();
			delay(5000);
			goToMainMenu();
		} else if (game.getStats().rightScore >= WINNING_SCORE) {
			drawRightWinScreen();
			delay(5000);
			goToMainMenu();
		} else {
			game.changeStartDelay(ROUND_START_DELAY);
			drawScore();
			if (game.getStats().leftScore == WINNING_SCORE - 1 && game.getStats().leftScore == game.getStats().rightScore) {
				delay(2000);
				drawFinalRound();
				delay(2000);
				// Increase initial ball speed and boundary change color speed
				game.changeBallInitialVelocity(physics::vector2d(BALL_FINAL_ROUND_SPEED, BALL_FINAL_ROUND_SPEED));
				boundaryChangeColorSpeed = FINAL_ROUND_BOUNDARY_CHANGE_COLOR_SPEED;
			} else {
				delay(2000);
			}
			game.resetPlayersAndBall();
		} 
	} else {
		// Get controller input and convert to player position
		float playerExtentY = PLAYER_HEIGHT * 0.5f;
		for (int i = 0; i < NUM_PLAYERS; ++i) {
			if (game.playerIsActive(i)) {
				// Convert the analog input to the player's desired position
				int val = analogRead(playerPin[i]);
				float desiredPositionY = map(val, 0, 1023, 0, 23);
				
				// Now set player's velocity such that he will be in the desired position next tick
				float currentPositionY = game.getPlayer(i).getPaddle()->getPosition().y;
				float distance = desiredPositionY - currentPositionY;
				
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
				
				float speed = (float)distance / dt;
				controller[i].setSpeed(speed);
			}
		}
		
		// Simulate the game
		game.tick(dt);
		
		// Draw the game
		drawGame(dt);
	}
}

void drawGame(float dt) {
	draw.clearBuffer();
	
	// For positions, we are converting from float to int so there is a loss of precision,
	// hence the +1
	
	// Bounds
	uint32_t desiredColor = boundaryColors[currentBoundaryColor];
	
	float speed = boundaryChangeColorSpeed;
	
	int hitColor = 0;
	{
		uint8_t desiredColorR = desiredColor >> 16;
		if (boundR > desiredColorR) {
			boundR -= dt * speed;
			if (boundR < desiredColorR) {
				boundR = desiredColorR;
			}
		} else if (boundR < desiredColorR - 1) {
			boundR += dt * speed;
			if (boundR > desiredColorR) {
				boundR = desiredColorR;
			}
		} else {
			++hitColor;
		}
	}
	
	{
		uint8_t desiredColorG = desiredColor >> 8;
		if (boundG > desiredColorG) {
			boundG -= dt * speed;
			if (boundG < desiredColorG) {
				boundG = desiredColorG;
			}
		} else if (boundG < desiredColorG - 1) {
			boundG += dt * speed;
			if (boundG > desiredColorG) {
				boundG = desiredColorG;
			}
		} else {
			++hitColor;
		}
	}
	
	{
		uint8_t desiredColorB = desiredColor;
		if (boundB > desiredColorB) {
			boundB -= dt * speed;
			if (boundB < desiredColorB) {
				boundB = desiredColorB;
			}
		} else if (boundB < desiredColorB - 1) {
			boundB += dt * speed;
			if (boundB > desiredColorB) {
				boundB = desiredColorB;
			}
		} else {
			++hitColor;
		}
	}
	
	if (hitColor == 3) {
		// Change to a new color
		int newBoundaryColor = currentBoundaryColor;
		while (newBoundaryColor == currentBoundaryColor) {
			newBoundaryColor = random(5);
		}
		currentBoundaryColor = newBoundaryColor;
	}
	
	int r = boundR;
	int g = boundG;
	int b = boundB;
	
	draw.setColor((r << 16) + (g << 8) + b);
	
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
	
	if (game.ballIsPaused()) {
		draw.number((int)(game.getStartDelay() - game.currentStartTime() + 1), 24, 3);
	}

	draw.drawBuffer();
}

void drawLeftWinScreen() {
	draw.clearBuffer();
	draw.setColor(playerColor[0]);
	draw.string(TEAM_1_COLOR, 12, 5);
	draw.setColor(LIGHT_GRAY);
	draw.string("WINS", 12, 13);
	draw.setColor(playerColor[0]);
	draw.number(game.getStats().leftScore, 1, 16);
	draw.setColor(playerColor[1]);
	draw.number(game.getStats().rightScore, 47, 16);
	draw.drawBuffer();
}

void drawRightWinScreen() {
	draw.clearBuffer();
	draw.setColor(playerColor[1]);
	draw.string(TEAM_2_COLOR, 12, 5);
	draw.setColor(LIGHT_GRAY);
	draw.string("WINS", 12, 13);
	draw.setColor(playerColor[0]);
	draw.number(game.getStats().leftScore, 1, 16);
	draw.setColor(playerColor[1]);
	draw.number(game.getStats().rightScore, 47, 16);
	draw.drawBuffer();
}

void drawScore() {
	draw.clearBuffer();
	draw.setColor(playerColor[0]);
	draw.number(game.getStats().leftScore, 14, 9);
	draw.setColor(playerColor[1]);
	draw.number(game.getStats().rightScore, 34, 9);
	draw.drawBuffer();
}

void drawFinalRound() {
	draw.clearBuffer();
	draw.setColor(0x400000);
	draw.string("FINAL", 8, 4);
	draw.string("ROUND", 8, 14);
	draw.drawBuffer();
}

void drawTitle() {
	draw.clearBuffer();
	// Dim blue
	draw.setColor(0x000070);
	draw.string("ICEWIRE", 0, 1);
	draw.setColor(playerColor[1]);
	draw.string("TEENSY", 2, 9);
	draw.setColor(playerColor[0]);
	draw.string("TENNIS", 4, 17);
	draw.drawBuffer();
}

void playLowBeep() {
	noTone(BUZZER_PIN);
	tone(BUZZER_PIN, 392, 50);
}

void playHighBeep() {
	noTone(BUZZER_PIN);
	tone(BUZZER_PIN, 784, 50);
}