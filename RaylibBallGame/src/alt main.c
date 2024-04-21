#include "raylib.h"
#include <stdio.h>
#include <stdbool.h>

#define MAX_PLATFORMS 10

// Ball Properties
typedef struct {
    Vector2 position;
    float radius;
    float velocity;
} Ball;

// Platform Properties
typedef struct {
    Rectangle rect;
    bool active;
} Platform;

int screenWidth = 800;
int screenHeight = 450;

Ball bird = {0};
Platform platforms[MAX_PLATFORMS] = {0};

bool gameOver = false;

// Function declarations
void InitGame();
void UpdateGame();
void DrawGame();
void UnloadGame();

int main(void) {
    InitWindow(screenWidth, screenHeight, "Flappy Bird");
    InitGame();

    SetTargetFPS(60);               

    while (!WindowShouldClose()) {
        UpdateGame();       
        DrawGame();         
    }

    UnloadGame();         

    CloseWindow();        

    return 0;
}

void InitGame() {
    // Initialize bird
    bird.position = (Vector2){ screenWidth / 4, screenHeight / 2 };
    bird.radius = 20.0f;
    bird.velocity = 0.0f;

    // Initialize platforms
    for (int i = 0; i < MAX_PLATFORMS; i++) {
        platforms[i].rect = (Rectangle){ screenWidth + 300 * i, GetRandomValue(100, screenHeight - 100), 80, 300 };
        platforms[i].active = true;
    }
}

void UpdateGame() {
    if (!gameOver) {
        // Update bird
        bird.velocity += 0.5f;
        bird.position.y += bird.velocity;

        // Check collisions
        for (int i = 0; i < MAX_PLATFORMS; i++) {
            if (platforms[i].active && CheckCollisionCircleRec(bird.position, bird.radius, platforms[i].rect)) {
                gameOver = true;
                break;
            }
        }

        // Update platforms
        for (int i = 0; i < MAX_PLATFORMS; i++) {
            if (platforms[i].active) {
                platforms[i].rect.x -= 2;

                // Reset platform position if it's out of the screen
                if (platforms[i].rect.x + platforms[i].rect.width < 0) {
                    platforms[i].rect.x = screenWidth + 300 * (MAX_PLATFORMS - 1);
                    platforms[i].rect.y = GetRandomValue(100, screenHeight - 100);
                }
            }
        }

        // Check for player input
        if (IsKeyPressed(KEY_SPACE)) {
            bird.velocity = -8.0f;
            PlaySound(GetRandomValue(1, 2) == 1 ? LoadSound("jump1.wav") : LoadSound("jump2.wav"));
        }

        // Check if bird is out of bounds
        if (bird.position.y >= screenHeight || bird.position.y <= 0) {
            gameOver = true;
        }
    }
}

void DrawGame() {
    BeginDrawing();

    ClearBackground(RAYWHITE);

    // Draw bird
    DrawCircleV(bird.position, bird.radius, RED);

    // Draw platforms
    for (int i = 0; i < MAX_PLATFORMS; i++) {
        if (platforms[i].active) {
            DrawRectangleRec(platforms[i].rect, GREEN);
        }
    }

    // Draw game over text if game is over
    if (gameOver) {
        DrawText("Game Over!", screenWidth / 2 - MeasureText("Game Over!", 40) / 2, screenHeight / 2 - 20, 40, BLACK);
        DrawText("Press R to Restart", screenWidth / 2 - MeasureText("Press R to Restart", 20) / 2, screenHeight / 2 + 20, 20, BLACK);
    }

    EndDrawing();
}

void UnloadGame() {
    // Unload all resources here...
}
