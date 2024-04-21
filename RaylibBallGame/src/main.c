#include "raylib.h"
#include <stdio.h>
#include <time.h>

const int width = 800;
const int height = 600;

bool death = false;

// Bird Properties:
typedef struct Bird {
    float width;
    float height;
    Vector2 position;
    Vector2 velocity;
    Texture2D Texture;
} Bird;

Bird bird = {
    .width = 34,
    .height = 24,
    .position = {(float)width/2, (float)height/2},
    .velocity = {0, 0}, // Initialize velocity to {0, 0}
    .Texture = {0} // Initialize texture
};


// Platform Properties
typedef struct Platform {
    int x;
    int y;
    int width;
    int height;
    int velocity;
    bool active;
} Platform;

void retry(Platform Top_platforms[], Platform Bottom_platforms[]);
void exitGame();

void Update_platforms(Platform Top_platforms[], Platform Bottom_platforms[]) {
    int i;
    for (i = 0; i < 4; i++) {
        DrawRectangle(Top_platforms[i].x, Top_platforms[i].y, Top_platforms[i].width, Top_platforms[i].height, GREEN);
        if (Top_platforms[i].x + Top_platforms[i].width < 0) {
            Top_platforms[i].x = 820;
        }
        DrawRectangle(Bottom_platforms[i].x, 450, Bottom_platforms[i].width, 150, WHITE);
        if (Bottom_platforms[i].x + Bottom_platforms[i].width < 0) {
            Bottom_platforms[i].x = 820;
        }
    }
      
    for(i=0; i<4; i++)
    {
        if(death != true)
        {
            Top_platforms[i].x -= 4;   
            Bottom_platforms[i].x -= 4;
        }
    }
}

int scoreUpdate(Platform Top_platforms[], int* score)
{  
    static bool crossed[4] = {false}; // Track if the bird has crossed each platform
    for(int i = 0; i < 4; i++) {
        if(bird.position.x > Top_platforms[i].x + Top_platforms[i].width &&
           bird.position.x - bird.width/2 <= Top_platforms[i].x + Top_platforms[i].width &&
           !crossed[i]) // Check if the bird crossed the platform and hasn't already been counted
        {
           (*score)++;
           crossed[i] = true; // Mark the platform as crossed
        }
        else if (bird.position.x <= Top_platforms[i].x + Top_platforms[i].width) {
            crossed[i] = false; // Reset the crossed flag if the bird is still on the platform
        }
    }
    
    char scoreText[20];
    snprintf(scoreText, 20, "Score: %d", *score);
    DrawText(scoreText, 25, 25, 58, WHITE);
    
    return *score;
}


int collision(Platform Top_platforms[], Platform Bottom_platforms[]) {   
    if((bird.position.y + bird.height/2) >= height){
        return 1;
    }
    if((bird.position.y - bird.height/2) <= 0){
        return 1;
    }   
    
    for (int i = 0; i < 4; i++) {
        if (CheckCollisionCircleRec(bird.position, bird.height/2, (Rectangle){Top_platforms[i].x,
            Top_platforms[i].y, Top_platforms[i].width, Top_platforms[i].height})) {
            return 1;
        }
        if (CheckCollisionCircleRec(bird.position, bird.height/2, (Rectangle){Bottom_platforms[i].x,
            Bottom_platforms[i].y, Bottom_platforms[i].width, Bottom_platforms[i].height})) {
            return 1;
        }
    }
    return 0; // No collision detected
}

typedef struct Button {
    Rectangle rect;
    Color color;
    Color textcolor;
    char* text;
} Button;

struct Button Retry = {
    .rect = {width/2-125, height/2+50, 130, 50},
    .color = RED,
    .textcolor = RAYWHITE,
    .text = "Retry"
};

struct Button Exit = {
    .rect = {width/2+30, height/2+50, 120, 50},
    .color = RED,
    .textcolor = RAYWHITE,
    .text = "Exit"
};

struct Button play = {
    .rect = {width/2+60, height/2+50, 120, 50},
    .color = RED,
    .textcolor = RAYWHITE,
    .text = "Play"
};

struct Button High_score = {
    .rect = {width/2+60, height/2+ 50*2, 120, 50},
    .color = RED,
    .textcolor = RAYWHITE,
    .text = "High Score"
};

int gameOver(Platform Top_platforms[], Platform Bottom_platforms[], int* score) {
    bird.velocity = (Vector2){0, 0}; // Reset bird's velocity
    death = true;
    
    DrawText("Game Over!", 250, 250, 62, RAYWHITE);
    {
        DrawRectangleRec(Retry.rect, Retry.color);
        DrawText(Retry.text, Retry.rect.x + Retry.rect.width / 2 - MeasureText(Retry.text, 20) / 2, //Text
        Retry.rect.y + Retry.rect.height / 2 - 10, 20, Retry.textcolor);

        DrawRectangleRec(Exit.rect, Exit.color);
        DrawText(Exit.text, Exit.rect.x + Exit.rect.width / 2 - MeasureText(Exit.text, 20) / 2,  //Text
        Exit.rect.y + Exit.rect.height / 2 - 10, 20, Exit.textcolor);
    }
    
    if (CheckCollisionPointRec(GetMousePosition(), Retry.rect) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
        *score = 0;
        retry(Top_platforms, Bottom_platforms);
    }
    if (CheckCollisionPointRec(GetMousePosition(), Exit.rect) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
        exitGame();
    }
}

void retry(Platform Top_platforms[], Platform Bottom_platforms[]) {
    bird.position.x = width / 2;
    bird.position.y = height / 2;
    death = false;
        
    for (int i = 0; i < 4; i++) {
        Top_platforms[i].x = 800 + i * 200;
        Bottom_platforms[i].x = 800 + i * 200;
    }
}

void exitGame(){
    EndDrawing();
    CloseWindow();
}

int main() {
    int score = 0;
    Platform Top_platforms[4];
    Platform Bottom_platforms[4];
    for (int i = 0; i < 4; i++) {
        Top_platforms[i].x = 800 + i * 200;
        Top_platforms[i].y = 0;
        Top_platforms[i].width = 70;
        Top_platforms[i].height = 250;
        
        Bottom_platforms[i].x = 800 + i * 200;
        Bottom_platforms[i].y = 450;
        Bottom_platforms[i].width = 70;
        Bottom_platforms[i].height = 150;
    }
       
    InitWindow(width, height, "Ball Game");
    InitAudioDevice();
    
    Texture2D background = LoadTexture("F:\\Code\\Project\\RaylibBallGame\\assets\\background.png");
    Sound jump = LoadSound("F:\\Code\\Project\\RaylibBallGame\\assets\\boom.mp3");
    bird.Texture = LoadTexture("F:\\Code\\Project\\RaylibBallGame\\assets\\bird.png");

    
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();

        ClearBackground(BLUE);
        DrawTexture(background, 0, 0, RAYWHITE);
        
        DrawTexture(bird.Texture, bird.position.x - bird.width/2, bird.position.y - bird.height/2, WHITE);
        Update_platforms(Top_platforms, Bottom_platforms);
        collision(Top_platforms, Bottom_platforms);
        
        scoreUpdate(Top_platforms, &score);
        
        // Setting Bird Properties
        bird.position.y += bird.velocity.y; 
        bird.velocity.y += 1; 

        // Game over // Retry and Menu
        if (collision(Top_platforms, Bottom_platforms)) {
            gameOver(Top_platforms, Bottom_platforms, &score);
        }
        
        if ((IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_W)) && death != true) {
          bird.velocity.y = -15;
          PlaySound(jump);
        }

        EndDrawing();
    }
    CloseWindow();
    return 0;
} 
