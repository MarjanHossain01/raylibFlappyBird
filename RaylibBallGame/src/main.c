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
} Platform;

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

struct Button Play = {
    .rect = {width/2-125, height/2+50, 130, 50},
    .color = RED,
    .textcolor = RAYWHITE,
    .text = "Play"
};

//Function Declarations
int mainMenu();
void Update_platforms(Platform Top_platforms[], Platform Bottom_platforms[]); //Obstacle generation
int collision(Platform Top_platforms[], Platform Bottom_platforms[]); //Collision detection
int scoreUpdate(Platform Top_platforms[], int* score); //Score Function
int gameOver(Platform Top_platforms[], Platform Bottom_platforms[], int* score); // Game over stage
void retry(Platform Top_platforms[], Platform Bottom_platforms[]); //Retry
void exitGame(); //Exit

int mainMenu()
{
    Texture2D logo = LoadTexture("F:\\Code\\Project\\RaylibBallGame\\assets\\logo.png");
    DrawTexture(logo, 270, 50, WHITE);
    
    DrawRectangleRec(Play.rect, Play.color);
        DrawText(Play.text, Play.rect.x + Play.rect.width / 2 - MeasureText(Play.text, 20) / 2,
        Play.rect.y + Play.rect.height / 2 - 10, 20, Play.textcolor);
        
   DrawRectangleRec(Exit.rect, Exit.color);
        DrawText(Exit.text, Exit.rect.x + Exit.rect.width / 2 - MeasureText(Exit.text, 20) / 2,
        Exit.rect.y + Exit.rect.height / 2 - 10, 20, Exit.textcolor);
   
   if(CheckCollisionPointRec(GetMousePosition(), Play.rect) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
   {
       return 1;
   }
   
   if(CheckCollisionPointRec(GetMousePosition(), Exit.rect) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
   {
       exitGame();
   }
   return 0;
}

void Update_platforms(Platform Top_platforms[], Platform Bottom_platforms[]) {
    int i;
    for (i = 0; i < 4; i++) {
        DrawRectangle(Top_platforms[i].x, Top_platforms[i].y, Top_platforms[i].width, Top_platforms[i].height, GREEN);
        if (Top_platforms[i].x + Top_platforms[i].width < 0) { // if platforms go outside of the screen they will be teleported to the right side
            Top_platforms[i].x = 820;
        }
        DrawRectangle(Bottom_platforms[i].x, 450, Bottom_platforms[i].width, 150, WHITE);
        if (Bottom_platforms[i].x + Bottom_platforms[i].width < 0) {
            Bottom_platforms[i].x = 820;
        }
        if(death != true)
        {
            Top_platforms[i].x -= 4;   
            Bottom_platforms[i].x -= 4;
        }
    }
}

int collision(Platform Top_platforms[], Platform Bottom_platforms[]) {   
    if((bird.position.y + bird.height/2) >= height){ // if bird touches the bottom of the screen
        return 1;
    }
    if((bird.position.y - bird.height/2) <= 0){ // if bird touches the top of the screen
        return 1;
    }   
    
    for (int i = 0; i < 4; i++) { // If bird touches any rectangles
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

int scoreUpdate(Platform Top_platforms[], int* score)
{  
    static bool crossed[4] = {false}; // check if the bird has crossed each platform
    for(int i = 0; i < 4; i++) {
        if(bird.position.x > Top_platforms[i].x + Top_platforms[i].width &&
           bird.position.x - bird.width/2 <= Top_platforms[i].x + Top_platforms[i].width &&
           !crossed[i]) //if the ball passes the rectangle and it already hasnt been crossed, score will go up
        {
           (*score)++;
           crossed[i] = true; // Mark the platform as crossed
        }
        else if (bird.position.x <= Top_platforms[i].x + Top_platforms[i].width) {
            crossed[i] = false; //unmark the check if the bird is still on the platform
        }
    }
    
    char scoreText[20]; //Display score
    snprintf(scoreText, 20, "Score: %d", *score);
    DrawText(scoreText, 25, 25, 58, WHITE);
    
    return *score;
}

int gameOver(Platform Top_platforms[], Platform Bottom_platforms[], int* score) {
    bird.velocity = (Vector2){0, 0}; // Stop bird from moving
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
    InitWindow(width, height, "Ball Game");
    InitAudioDevice();
    
    int score = 0;
    
    //Initializing Platforms
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
    
    //Assets
    Texture2D background = LoadTexture("F:\\Code\\Project\\RaylibBallGame\\assets\\background.png");
    Sound jump = LoadSound("F:\\Code\\Project\\RaylibBallGame\\assets\\boom.mp3");
    bird.Texture = LoadTexture("F:\\Code\\Project\\RaylibBallGame\\assets\\bird.png");
    
    // Draw the main menu once to display buttons
    int startGame = mainMenu();
    
    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLUE);
        DrawTexture(background, 0, 0, RAYWHITE);
        
        if (startGame == 0) {
            startGame = mainMenu(); // Update startGame based on menu input
        }
        else {
            DrawTexture(bird.Texture, bird.position.x - bird.width/2, bird.position.y - bird.height/2, WHITE);
        
            Update_platforms(Top_platforms, Bottom_platforms);
            collision(Top_platforms, Bottom_platforms);
            
            scoreUpdate(Top_platforms, &score);
            
            // Setting Bird Properties
            bird.position.y += bird.velocity.y; 
            bird.velocity.y += 1; 
    
            // Game over // Retry and Exit
            if (collision(Top_platforms, Bottom_platforms)) {
                gameOver(Top_platforms, Bottom_platforms, &score);
            }
            
            if ((IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_W)) && death != true) {
                bird.velocity.y = -15;
                PlaySound(jump);
            }
        }
        
        EndDrawing();
    }
    
    CloseWindow();
    return 0;
}