#include "raylib.h"
#include <stdio.h>

const int width = 800;
const int height = 600;

#define MAX_RAIN 200

bool death = false;

// Bird Properties:
typedef struct Bird { 
    float width; 
    float height; 
    Vector2 position; 
    Vector2 velocity; 
    Texture2D texture; 
} Bird; 

Bird bird = {
    .width = 34,
    .height = 24,
    .position = {(float)width / 2, (float)height / 2},
    .velocity = {0, 0},
    .texture = {0}
};

typedef struct {
    Vector2 position;
    float speed;
    float windspeed;
    int alpha;
} Raindrop;

Raindrop rain[MAX_RAIN];

// Platform Properties
typedef struct Platform {
    int x;
    int y;
    int width;
    int height;
} Platform;

void exitGame() {
    CloseWindow();
}

int mainMenu(Rectangle exit, Rectangle play, Rectangle retry, Texture2D button) {
    Texture2D logo = LoadTexture("assets/logo.png");
    DrawTexture(logo, 270, 50, WHITE);

    DrawTexture(button, width / 2 - 125, height / 2 + 50, WHITE);
    DrawRectangleRec(play, BLANK);
    DrawText("Play", width / 2 - 90, play.y + play.height / 2 - 10, 20, WHITE);

    DrawTexture(button, width / 2 + 30, height / 2 + 50, WHITE);
    DrawRectangleRec(exit, BLANK);
    DrawText("Exit", width / 2 + 70, exit.y + exit.height / 2 - 10, 20, WHITE);

    if (CheckCollisionPointRec(GetMousePosition(), play)) {
        SetMouseCursor(4);
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            return 1;
        }
    }

    if (CheckCollisionPointRec(GetMousePosition(), exit)) {
        SetMouseCursor(4);
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            exitGame();
        }
    }

    return 0;
}

void UpdatePlatforms(Platform topPlatforms[], Platform bottomPlatforms[]) {
    for (int i = 0; i < 4; i++) {
        DrawRectangle(topPlatforms[i].x, topPlatforms[i].y, topPlatforms[i].width, topPlatforms[i].height, GREEN);
        if (topPlatforms[i].x + topPlatforms[i].width < 0) {
            topPlatforms[i].x = 820;
        }
        DrawRectangle(bottomPlatforms[i].x, 450, bottomPlatforms[i].width, 150, WHITE);
        if (bottomPlatforms[i].x + bottomPlatforms[i].width < 0) {
            bottomPlatforms[i].x = 820;
        }
        if (!death) {
            topPlatforms[i].x -= 4;
            bottomPlatforms[i].x -= 4;
        }
    }
}

int checkCollision(Platform topPlatforms[], Platform bottomPlatforms[]) {
    if ((bird.position.y + bird.height / 2) >= height) {
        return 1;
    }
    if ((bird.position.y - bird.height / 2) <= 0) {
        return 1;
    }

    for (int i = 0; i < 4; i++) {
        if (CheckCollisionCircleRec(bird.position, bird.height / 2, (Rectangle){topPlatforms[i].x, topPlatforms[i].y, topPlatforms[i].width, topPlatforms[i].height})) {
            return 1;
        }
        if (CheckCollisionCircleRec(bird.position, bird.height / 2, (Rectangle){bottomPlatforms[i].x, bottomPlatforms[i].y, bottomPlatforms[i].width, bottomPlatforms[i].height})) {
            return 1;
        }
    }
    return 0;
}

int updateScore(Platform topPlatforms[], int* score) {
    static bool crossed[4] = {false};
    for (int i = 0; i < 4; i++) {
        if (bird.position.x > topPlatforms[i].x + topPlatforms[i].width &&
            bird.position.x - bird.width / 2 <= topPlatforms[i].x + topPlatforms[i].width &&
            !crossed[i]) {
            (*score)++;
            crossed[i] = true;
        } else if (bird.position.x <= topPlatforms[i].x + topPlatforms[i].width) {
            crossed[i] = false;
        }
    }

    char scoreText[20];
    snprintf(scoreText, 20, "Score: %d", *score);
    DrawText(scoreText, 25, 25, 58, WHITE);

    return *score;
}

void updateHighScore(int* score, int* highscore) {
    if ((*score) > (*highscore)) {
        (*highscore) = (*score);
    }
    char scoreText[20];
    snprintf(scoreText, 20, "High Score: %d", *highscore);
    DrawText(scoreText, 25, 90, 30, RAYWHITE);
}

void gameOver(Platform topPlatforms[], Platform bottomPlatforms[], int* score, Rectangle retry, Rectangle exit, Texture2D button) {
    bird.velocity = (Vector2){0, 0};
    death = true;

    DrawText("Game Over!", 250, 250, 62, RAYWHITE);
    DrawTexture(button, width / 2 - 125, height / 2 + 50, WHITE);
    DrawRectangleRec(retry, BLANK);
    DrawText("Retry", width / 2 - 95, retry.y + retry.height / 2 - 10, 20, WHITE);

    DrawTexture(button, width / 2 + 30, height / 2 + 50, WHITE);
    DrawRectangleRec(exit, BLANK);
    DrawText("Exit", width / 2 + 70, exit.y + exit.height / 2 - 10, 20, WHITE);

    if (CheckCollisionPointRec(GetMousePosition(), retry)) {
        SetMouseCursor(4);
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            *score = 0;
            Retry(topPlatforms, bottomPlatforms);
        }
    }
    if (CheckCollisionPointRec(GetMousePosition(), exit)) {
        SetMouseCursor(4);
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            exitGame();
        }
    }
}

void Retry(Platform topPlatforms[], Platform bottomPlatforms[]) {
    bird.position.x = width / 2;
    bird.position.y = height / 2;
    death = false;

    for (int i = 0; i < 4; i++) {
        topPlatforms[i].x = 800 + i * 200;
        bottomPlatforms[i].x = 800 + i * 200;
    }
}

void generateQuote(char quotes[][75], int* check, int* count, int* alpha) {
    int textWidth = MeasureText(quotes[*count], 20); // Measure the text width
    int xPos = (width - textWidth) / 2; // Calculate the centered x position
    
    DrawText(quotes[*count], xPos, height / 2, 20, (Color){200, 0, 0, (*alpha)});
    
    (*check)++;
    if (*check < 200) {
        (*alpha)++;
    } else if (*check >= 200) {
        (*alpha)--;
    }
    if (*check >= 510) {
        (*count)++;
        (*check) = 0;
    }
}

int main() {
    char quotes[][75] = {
        "It's only after we've lost everything that we're free to do anything.",
        "We accept the love we think we deserve.",
        "This is your life and it's ending one minute at a time.",
        "Maybe you're not meant to be perfect.",
        "Look around you, and start thinking.",
        "We suffer more in imagination than in reality.",
        "Your Success makes your identity,",
        "your failure make you stronger.",
        "There's an odd comfort in realizing you've lost, there's a victory in that."
    };
    float seconds;

    InitWindow(width, height, "Bird Game");
    InitAudioDevice();

    for (int i = 0; i < MAX_RAIN; i++) {
        rain[i].position.x = GetRandomValue(0, width);
        rain[i].position.y = GetRandomValue(height, 0);
        rain[i].speed = (float)GetRandomValue(50, 100) / 100.0f;
        rain[i].windspeed = (float)GetRandomValue(-50, 50) / 100.0f;
        rain[i].alpha = 0;
    }

    int quoteCheck = 0;
    int quoteAlpha = 0;
    int quoteCount = 0;

    int score = 0;
    int highscore = 0;

    // Initializing Platforms
    Platform topPlatforms[4];
    Platform bottomPlatforms[4];
    for (int i = 0; i < 4; i++) {
        topPlatforms[i].x = 800 + i * 200;
        topPlatforms[i].y = 0;
        topPlatforms[i].width = 70;
        topPlatforms[i].height = 250;

        bottomPlatforms[i].x = 800 + i * 200;
        bottomPlatforms[i].y = 450;
        bottomPlatforms[i].width = 70;
        bottomPlatforms[i].height = 150;
    }

    // Assets
    Texture2D background = LoadTexture("assets/background.png");
    Texture2D button = LoadTexture("assets/Texture.png");

    Rectangle play = {width / 2 - 125, height / 2 + 50, 130, 50};
    Rectangle exit = {width / 2 + 30, height / 2 + 50, 120, 50};
    Rectangle retry = {width / 2 - 125, height / 2 + 50, 130, 50};

    bird.texture = LoadTexture("assets/bird.png");
    Music bgm = LoadMusicStream("assets/bgm.mp3");
    Music rainSfx = LoadMusicStream("assets/rainSfx.mp3");
    Sound jump = LoadSound("assets/jump.wav");

    // Draw the main menu to display buttons
    int startGame = mainMenu(exit, play, retry, button);

    SetTargetFPS(60);

    PlayMusicStream(bgm);
    PlayMusicStream(rainSfx);
    SetMusicVolume(rainSfx, 0.7);

    float lastPrint = 0.0;
    int flag = 0;

    while (!WindowShouldClose()) {
        seconds = GetTime();
        if (seconds - lastPrint >= 15.0) {
            lastPrint = seconds;
            flag = 1;
        }

        UpdateMusicStream(bgm);
        UpdateMusicStream(rainSfx);
        rainSfx.looping = true;

        for (int i = 0; i < MAX_RAIN; i++) {
            rain[i].position.y += rain[i].speed + GetRandomValue(0, 20);
            rain[i].position.x += rain[i].windspeed;

            // Reset raindrop's position
            if (rain[i].position.y > height) {
                rain[i].position.y = GetRandomValue(-height, 0);
                rain[i].position.x = GetRandomValue(0, width);
            }
        }

        SetMouseCursor(0);
        BeginDrawing();
        ClearBackground(BLUE);
        DrawTexture(background, 0, 0, RAYWHITE);

        if (startGame == 0) {
            startGame = mainMenu(exit, play, retry, button);
        } else {
            for (int i = 0; i < MAX_RAIN; i++) {
                if (rain[i].alpha < 200) {
                    rain[i].alpha += 1;
                    if (rain[i].alpha > 200) {
                        rain[i].alpha = 200;
                    }
                }
                DrawRectangle(rain[i].position.x, rain[i].position.y, 1, 40, (Color){189, 201, 219, rain[i].alpha});
            }

            generateQuote(quotes, &quoteCheck, &quoteCount, &quoteAlpha);
            DrawTexture(bird.texture, bird.position.x - bird.width / 2, bird.position.y - bird.height / 2, WHITE);

            UpdatePlatforms(topPlatforms, bottomPlatforms);
            checkCollision(topPlatforms, bottomPlatforms);

            updateScore(topPlatforms, &score);
            updateHighScore(&score, &highscore);

            // Setting Bird Properties
            bird.position.y += bird.velocity.y;
            bird.velocity.y += 1;

            // Game over // Retry and Exit
            if (checkCollision(topPlatforms, bottomPlatforms)) {
                gameOver(topPlatforms, bottomPlatforms, &score, retry, exit, button);
            }

            if ((IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_W)) && !death) {
                bird.velocity.y = -15;
                PlaySound(jump);
            }
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
