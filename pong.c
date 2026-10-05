#include "include/raylib.h"

#define WIDTH 1280
#define HEIGHT 720
#define FPS 60

#define BACKGROUND (Color) { 0, 48, 59, 255 }
#define FOREGROUND (Color) { 241, 242, 218, 255 }

#define PADDLE_WIDTH  30
#define PADDLE_HEIGHT 120
#define BALL_WIDTH    20
#define BALL_HEIGHT   20

#define BALL_MAX_DX   12
#define BALL_MAX_DY    8

#define PADDLE_MAX_DY 12

#define WIN_GOAL 3

enum {START, PLAYING, SERVING, WIN};

typedef struct {
    int x, y;
    int width, height;
    int score;
} Player;

typedef struct {
    int x, y;
    int dx, dy;
    int width, height;
} Ball;

int main(void)
{
    // Initialization ----------------------------------------------------------

    // Window
    InitWindow(WIDTH, HEIGHT, "Pong");
    SetTargetFPS(FPS);

    // Audio
    InitAudioDevice();
    Sound fxWin = LoadSound("sfx/win.wav");;
    Sound fxHit = LoadSound("sfx/hit.wav");
    Sound fxBounce = LoadSound("sfx/bounce.wav");
    Sound fxPoint = LoadSound("sfx/point.wav");

    // Default game state
    int game_state = START;

    // Create players
    Player player1 = {10, HEIGHT / 2 - PADDLE_HEIGHT / 2, PADDLE_WIDTH, PADDLE_HEIGHT, 0};
    Player player2 = {WIDTH - PADDLE_WIDTH - 10, HEIGHT / 2 - PADDLE_HEIGHT / 2, PADDLE_WIDTH, PADDLE_HEIGHT, 0};
    
    // Create the ball
    Ball ball = {WIDTH / 2 - BALL_WIDTH / 2, HEIGHT / 2 - BALL_HEIGHT / 2, 0, 0, BALL_WIDTH, BALL_HEIGHT};

    int serving_player = GetRandomValue(1, 2);
    int winner = 0;
    
    // -------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose()) { // Detect window close button or ESC key
	// Update --------------------------------------------------------------

	// Switching states
	if (IsKeyPressed(KEY_ENTER)) {
	    if (game_state == START || game_state == SERVING) {
		if (serving_player == 1) {
		    ball.dx = BALL_MAX_DX;
		    ball.dy = GetRandomValue(- BALL_MAX_DY, BALL_MAX_DY);
		} else {
		    ball.dx = - BALL_MAX_DX;
		    ball.dy = GetRandomValue(- BALL_MAX_DY, BALL_MAX_DY);
		}
		game_state = PLAYING;
	    } else if (game_state == WIN) {
		ball.x = WIDTH / 2 - BALL_WIDTH / 2;
		ball.y = HEIGHT / 2 - BALL_HEIGHT / 2;
		ball.dx = 0;
		ball.dy = 0;
		player1.score = player2.score = 0;
		game_state = START;
	    }
	}
	
	// Update Player 1
	if (IsKeyDown('S')) player1.y += PADDLE_MAX_DY;
	if (IsKeyDown('W')) player1.y -= PADDLE_MAX_DY;
	player1.y = player1.y < 10 ? 10 : player1.y;
	player1.y = player1.y > HEIGHT - PADDLE_HEIGHT - 10 ? HEIGHT - PADDLE_HEIGHT - 10 : player1.y;
	// Update Player 2
	if (IsKeyDown(KEY_DOWN)) player2.y += PADDLE_MAX_DY;
	if (IsKeyDown(KEY_UP)) player2.y -= PADDLE_MAX_DY;
	player2.y = player2.y < 10 ? 10 : player2.y;
	player2.y = player2.y > HEIGHT - PADDLE_HEIGHT - 10 ? HEIGHT - PADDLE_HEIGHT - 10 : player2.y;

	// Update the ball
	ball.x += ball.dx;
	ball.y += ball.dy;
	// Detect Collision with vertical walls
	if (ball.x <= 0) {
	    player2.score++;
	    ball.x = WIDTH / 2 - BALL_WIDTH / 2;
	    ball.y = HEIGHT / 2 - BALL_HEIGHT / 2;
	    ball.dx = 0;
	    ball.dy = 0;
	    serving_player = 1;
	    PlaySound(fxPoint);
	    game_state = SERVING;
	    if (player2.score == WIN_GOAL) {
		game_state = WIN;
		winner = 2;
		PlaySound(fxWin);
	    }
	} else if (ball.x >= WIDTH - BALL_WIDTH) {
	    player1.score++;
	    ball.x = WIDTH / 2 - BALL_WIDTH / 2;
	    ball.y = HEIGHT / 2 - BALL_HEIGHT / 2;
	    ball.dx = 0;
	    ball.dy = 0;
	    serving_player = 2;
	    PlaySound(fxPoint);
	    game_state = SERVING;
	    if (player1.score == WIN_GOAL) {
		game_state = WIN;
		winner = 1;
		PlaySound(fxWin);
	    }
	}
	// Detect Collision with horizontal walls
	if (ball.y <= 0 || ball.y >= HEIGHT - BALL_HEIGHT) {
	    ball.dy = ball.y <= 0 ? GetRandomValue(1, BALL_MAX_DY) : GetRandomValue(- BALL_MAX_DY, -1);
	    PlaySound(fxBounce);
	}
	// Detect Collision with players
	if (ball.x <= player1.x + PADDLE_WIDTH && ball.y >= player1.y && ball.y <= player1.y + PADDLE_HEIGHT) {
	    ball.dx = - ball.dx;
	    ball.dy = GetRandomValue(- BALL_MAX_DY, BALL_MAX_DY);
	    ball.x = PADDLE_WIDTH + 10 + 2;
	    PlaySound(fxHit);
	} else if (ball.x + BALL_WIDTH >= player2.x && ball.y >= player2.y && ball.y <= player2.y + PADDLE_HEIGHT) {
	    ball.dx = - ball.dx;
	    ball.dy = GetRandomValue(- BALL_MAX_DY, BALL_MAX_DY);
	    ball.x = WIDTH - BALL_WIDTH - PADDLE_WIDTH - 10 - 2;
	    PlaySound(fxHit);
	}
	
	// ---------------------------------------------------------------------

	// Draw ----------------------------------------------------------------
	
	BeginDrawing();

	ClearBackground(BACKGROUND);

	if (game_state == START) {
	    DrawText("PONG!", WIDTH / 2 - 140, 100, 100, FOREGROUND);
	    DrawText("Press Enter", WIDTH / 2 - 150, HEIGHT / 2 + 200, 50, FOREGROUND);
	}
	
	if (game_state == PLAYING || game_state == SERVING) {
	    // Draw scores
	    DrawText(TextFormat("%i\t%i", player1.score, player2.score),
		     572, 100, 80, FOREGROUND);
	}

	if (game_state == SERVING) {
	    DrawText(TextFormat("Player %d's Serve!", serving_player), WIDTH / 2 - 220, 200, 50, FOREGROUND);
	    DrawText("Press Enter to Serve", WIDTH / 2 - 140, HEIGHT - 200, 25, FOREGROUND);
	}

	if (game_state != WIN) {
	    // Draw the ball
	    DrawRectangle(ball.x, ball.y, ball.width, ball.height, FOREGROUND);
	    
	    // Draw the players
	    DrawRectangle(player1.x, player1.y, player1.width, player1.height, FOREGROUND);
	    DrawRectangle(player2.x, player2.y, player2.width, player2.height, FOREGROUND);
	} else {
	    DrawText(TextFormat("Player %d Wins!", winner), WIDTH / 2 - 200, HEIGHT / 2 - 50, 50, FOREGROUND);
	}
	
	EndDrawing();
	
	// ---------------------------------------------------------------------
    }

    // De-Initialization -------------------------------------------------------
    CloseWindow();		// Close window and OpenGL context
    // -------------------------------------------------------------------------

    return 0;
}
