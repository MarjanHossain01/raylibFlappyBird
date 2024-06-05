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
    Texture2D Texture; 
} Bird; 

Bird bird = {
    .width = 34,
    .height = 24,
    .position = {(float)width/2, (float)height/2},
    .velocity = {0, 0}, // Initialize velocity to {0, 0}
    .Texture = {0} // Initialize texture
};

typedef struct {
    Vector2 position;
    float speed;
    float windspeed;
    int alpha;
} raindrop;

raindrop rain[MAX_RAIN];

// Platform Properties
typedef struct Platform {
    int x;
    int y;
    int width;
    int height;
} Platform;

void exitGame(){
    EndDrawing();
    CloseWindow();
}

int mainMenu(Rectangle exit, Rectangle play, Rectangle retry, Texture2D button)
{
    Texture2D logo = LoadTexture("assets\\logo.png");
    DrawTexture(logo, 270, 50, WHITE);
    
    DrawTexture(button, width/2-125, height/2+50, WHITE);
    DrawRectangleRec(play, BLANK);
        DrawText("Play", width/2-90, play.y + play.height / 2 - 10, 20, WHITE);
        
   DrawTexture(button, width/2+30, height/2+50, WHITE);
   DrawRectangleRec(exit, BLANK);
        DrawText("Exit", width/2+70, exit.y + exit.height / 2 - 10, 20, WHITE);
   
   if( CheckCollisionPointRec(GetMousePosition(), play) )
   {
       SetMouseCursor(4);
       if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
           return 1;
       }
   }
   
   if( CheckCollisionPointRec(GetMousePosition(), exit) )
   {
       SetMouseCursor(4);
       if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
           exitGame();
       }
       
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
    
    char scoreText[20]; //Display score`
    snprintf(scoreText, 20, "Score: %d", *score);
    DrawText(scoreText, 25, 25, 58, WHITE);
    
    return *score;
}

int highScore(int *score, int* highscore)
{
    if((*score) > (*highscore))
    {
        (*highscore) = (*score);
    }
    char scoreText[20]; //Display score
    snprintf(scoreText, 20, "High Score: %d", *highscore);
    DrawText(scoreText, 25, 90, 30, RAYWHITE);
}

int gameOver(Platform Top_platforms[], Platform Bottom_platforms[], int* score, Rectangle retry, Rectangle exit, Texture2D button) {
    bird.velocity = (Vector2){0, 0}; // Stop bird from moving
    death = true;
    
    DrawText("Game Over!", 250, 250, 62, RAYWHITE);
    {
        DrawTexture(button, width/2-125, height/2+50, WHITE);
    DrawRectangleRec(retry, BLANK);
        DrawText("Retry", width/2-95, retry.y + retry.height / 2 - 10, 20, WHITE);
        
   DrawTexture(button, width/2+30, height/2+50, WHITE);
   DrawRectangleRec(exit, BLANK);
        DrawText("Exit", width/2+70, exit.y + exit.height / 2 - 10, 20, WHITE);
   
    }
    
    if (CheckCollisionPointRec(GetMousePosition(), retry)){
        SetMouseCursor(4);
        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
            *score = 0;
            Retry(Top_platforms, Bottom_platforms);
        }
    }
    if (CheckCollisionPointRec(GetMousePosition(), exit)){
        SetMouseCursor(4);
        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
            exitGame();
        }
    }
}

void Retry(Platform Top_platforms[], Platform Bottom_platforms[]) {
    bird.position.x = width / 2;
    bird.position.y = height / 2;
    death = false;
        
    for (int i = 0; i < 4; i++) {
        Top_platforms[i].x = 800 + i * 200;
        Bottom_platforms[i].x = 800 + i * 200;
    }
}

int main() 
{
    char quotes[5][70] = 
    {
        "It's only after we've lost everything that we're free to do anything.",
        "We accept the love we think we deserve.",
        "This is your life and it's ending one minute at a time.",
        "Maybe you're not meant to be complete.",
        "Look around you, and start thinking.",
        "We suffer more in imagination than in reality"
    };
    float seconds;
    
    InitWindow(width, height, "Ball Game");
    InitAudioDevice();
    
    for(int i = 0; i<MAX_RAIN; i++){
        rain[i].position.x = GetRandomValue(0, width);
        rain[i].position.y = GetRandomValue(height, 0);
        rain[i].speed = (float)GetRandomValue(50, 100) / 100.0f;
        rain[i].windspeed = (float)GetRandomValue(-50, 50) / 100.0f;
        rain[i].alpha = 0;
    }
    
    int score = 0;
    int highscore = 0;
    
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
    Texture2D background = LoadTexture("assets\\background.png");
    Texture2D button = LoadTexture("assets\\Texture.png");
    
    Rectangle play = {width/2-125, height/2+50, 130, 50};
    Rectangle exit = {width/2+30, height/2+50, 120, 50};
    Rectangle retry = {width/2-125, height/2+50, 130, 50};
    
    bird.Texture = LoadTexture("assets\\bird.png");
    Music bgm = LoadMusicStream("assets\\bgm.mp3");
    Music rainSfx = LoadMusicStream("assets\\rainSfx.mp3");
    Sound jump = LoadSound("assets\\jump.wav");
    
    
    // Draw the main menu to display buttons
    int startGame = mainMenu(exit, play, retry, button);
    
    SetTargetFPS(60);
    
    PlayMusicStream(bgm);
    PlayMusicStream(rainSfx);
    SetMusicVolume(rainSfx, 0.7);
    
    float lastPrint = 0.0;
    int flag=0;
    
    while (!WindowShouldClose()) {
        seconds = GetTime();
        if(flag == 1){
            DrawText("Example", width/2, height/2, 24, RED);
        }
        if(seconds - lastPrint >= 15.0){
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
            startGame = mainMenu(exit, play, retry, button); //Wait for user to select an option
        }
        else {
            for(int i=1; i<MAX_RAIN; i++){
                if (rain[i].alpha < 200) {
                    rain[i].alpha += 1;  // Adjust the increment value as needed
                    if (rain[i].alpha > 200) {
                        rain[i].alpha = 200;  // Cap the alpha value at 255
                    }
            }
                DrawRectangle(rain[i].position.x, rain[i].position.y, 1, 40, (Color){189, 201, 219, rain[i].alpha});
            }
            
            DrawTexture(bird.Texture, bird.position.x - bird.width/2, bird.position.y - bird.height/2, WHITE);
            
            Update_platforms(Top_platforms, Bottom_platforms);
            collision(Top_platforms, Bottom_platforms);
            
            scoreUpdate(Top_platforms, &score);
            highScore(&score, &highscore);
            
            // Setting Bird Properties
            bird.position.y += bird.velocity.y; 
            bird.velocity.y += 1; 
    
            // Game over // Retry and Exit
            if (collision(Top_platforms, Bottom_platforms)) {
                gameOver(Top_platforms, Bottom_platforms, &score, retry, exit, button);
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

