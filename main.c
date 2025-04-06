#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>   // library for memory allocation and other utility functions
#include <math.h>
#include <stdbool.h>  // provides bool type and true and false constants for more readable code

// Game constants
#define SCREEN_WIDTH 800             
#define SCREEN_HEIGHT 600
#define PADDLE_WIDTH 100
#define PADDLE_HEIGHT 20
#define BALL_RADIUS 10
#define BRICK_WIDTH 70
#define BRICK_HEIGHT 30
#define BRICK_ROWS 5
#define BRICK_COLS 10
#define BRICK_GAP 5

// Game state enum
typedef enum {       
    TITLE,
    GAMEPLAY,
    GAME_OVER,
    WIN               // Win state when all bricks are destroyed
} GameState;

// Brick structure
typedef struct {            // (struct) structure type called brick
    Rectangle rect;         // store position and size of the brick(x,y,height,width)
    bool active;            // boolean flag which indicated if a flag has been destroyed or still active
    Color color;            // stores the color of the brick 
} Brick;

typedef int (*ScoreFunc)(int currentScore, int points); // scorefunc is a data type that points to function that takes two integers and returns integer


// Score Functions
int RegularScore(int currentScore, int points) { // defines a function that follows same pattern as scorefunc and adds score with currentscore and returns the total
    return currentScore + points;
}

int BonusScore(int currentScore, int points) { // defines a function that follows same pattern as scorefunc and before adding it doubles the score with currentscore and returns the total
    return currentScore + (points * 2);
}


void FreeBricks(Brick** bricks, int rows) { // takes 2 arrays of bricks and number of rows and used to clean up memory when game finishes
    for (int i = 0; i < rows; i++){         // iterates through rows
        free(bricks[i]);                    // it calls free on each row to release memory
    }
    free(bricks);                           // after freeing each row freeds outer array that holds row pointers,completing memory cleanup
}

int main(void) {                    // entry point of program and returns integer 0 and without parameter(void)
    // Initialize window
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Breakout Game");
    SetTargetFPS(60);
    
    // Game variables
    GameState currentState = TITLE;
    int score = 0;
    int lives = 3;
    int activeBricks = BRICK_ROWS * BRICK_COLS; // 50 for 5 rows and 10 columns
    
    ScoreFunc scoreSystem = RegularScore; // creates a variable called scoresystem of type scorefunc and initialize to regular scoring system

    // Paddle
    Rectangle paddle = {                     // Rectangle paddle structure
        SCREEN_WIDTH/2 - PADDLE_WIDTH/2,     // screen_width/2 makes horizontal center of the screen and paddle width/2 ensures paddle is in centre
        SCREEN_HEIGHT - 50,                  // y coordinate positioned 50 pixels from the bottom
        PADDLE_WIDTH,
        PADDLE_HEIGHT
    };


    // Ball

    Vector2 ballPosition = { SCREEN_WIDTH/2, SCREEN_HEIGHT - 70}; // screen_width/2 makes the ball horizontally centered and 70 pixels from the bottom
    
    Vector2 ballSpeed = { 5.0f, -5.0f }; // it goes right 5 pixels and 5 pixels up per frame
   
    bool ballActive = false;  // boolean flag indicates ball is inactive until player launches it

     // BRICKS
    // Brick bricks[BRICK_ROWS][BRICK_COLS]; // creates 2d array rows columns
      Brick** bricks;       // declares variable called bricks that is a pointer to an array of Brick* pointers,main variable for holding 2d arrays
      bricks = (Brick**)malloc(BRICK_ROWS * sizeof(Brick*));  //allocates memory for brick* pointers, the size is 5 times of brick rows to brick pointers(brick*),mallic allocates that amount of memory and returns a void pointer to it which is casted to a pointer to a pointer to Brick**
      for (int i = 0; i < BRICK_ROWS; i++) {    // goes through each row 
        bricks[i] = (Brick*)malloc(BRICK_COLS * sizeof(Brick)); //allocates memory for bricks cols which is then casted to Brick* to match what we are storing in bricks[i]
    }
    // Initialize Bricks or bricks position and size

    for (int i = 0; i < BRICK_ROWS; i++) {     // outer loop goes through each row(5 rows)

        for (int j = 0; j < BRICK_COLS; j++) {  // inner loop goes through each column(10 column)

            bricks[i][j].rect = (Rectangle) {  // brick position and size, casting value that follows into rectangle type
                
               j * (BRICK_WIDTH + BRICK_GAP) + BRICK_GAP, // x coordinate based on column position
               
               i * (BRICK_HEIGHT + BRICK_GAP) + BRICK_GAP + 50, // y coordinate based on row position, 50 pixels offset from top

               BRICK_WIDTH,

               BRICK_HEIGHT
          
         };
         bricks[i][j].active = true; // sets initially active


         // Different colors for each row

         switch (i) {                                // color based on row index

             case 0: bricks[i][j].color = RED; break;
             case 1: bricks[i][j].color = ORANGE; break;
             case 2: bricks[i][j].color = YELLOW; break;
             case 3: bricks[i][j].color = GREEN; break;
             case 4: bricks[i][j].color = BLUE; break;
             default: bricks[i][j].color = WHITE;
         }
       
       } 
        
    }
    // Main game loop continues until window is closed or pressed esc
while (!WindowShouldClose()) {
    // Update
    switch (currentState) {            // used to handle different game logics depending on the state it is in
        case TITLE:
            if (IsKeyPressed(KEY_ENTER)) {     // checks if enter was pressed
                currentState = GAMEPLAY;        
            }
            break;
            
        case GAMEPLAY:
            // Move paddle
            if (IsKeyDown(KEY_LEFT)) paddle.x -= 7;      
            if (IsKeyDown(KEY_RIGHT)) paddle.x += 7;     
                
            // Paddle boundaries
            if (paddle.x < 0) paddle.x = 0;        //left edge
            if (paddle.x > SCREEN_WIDTH - PADDLE_WIDTH) paddle.x = SCREEN_WIDTH - PADDLE_WIDTH;   // right edge
                
            // Ball launch
            if (!ballActive) {
                ballPosition.x = paddle.x + PADDLE_WIDTH/2;  // upper part of the paddle
                if(IsKeyPressed(KEY_SPACE)) ballActive = true;
            }
                
            // Simple ball movement
            if(ballActive) {
                ballPosition.x += ballSpeed.x;   // updates ball x  position depending on x speed
                ballPosition.y += ballSpeed.y;   //  updates ball y  position depending on y speed
            
            for (int i = 0; i < BRICK_ROWS; i++){
                for (int j = 0; j < BRICK_COLS; j++){
                    if (bricks[i][j].active) {      // only checks the acive bricks
                        if (CheckCollisionCircleRec(  // calls raylib function check collision between ball and bricks
                            ballPosition,      // ball define by position and radius
                            BALL_RADIUS,
                            bricks[i][j].rect)) {  // bricks position and dimensions
                                bricks[i][j].active = false; //collision happens makes brick inactive
                                ballSpeed.y *= -1;   // reverses ball's vertical direction by multiply y speed with -1
                                score = scoreSystem(score, 10); // adds points to the function using current scoring system, passes current score and 10 points add
                                activeBricks--; // decreses the number of active bricks by 1

                                if (activeBricks == (BRICK_ROWS * BRICK_COLS) / 2) { // checks if half of bricks has been destroyed,which is 25 in this case
                                    scoreSystem = BonusScore; // changes scoring system to bonus scoring
                                    DrawText("BONUS SCORING ACTIVATED!", SCREEN_WIDTH/2 - MeasureText("BONUS SCORING ACTIVATED!", 20)/2, SCREEN_HEIGHT/2, 20, YELLOW); // display the notification
                                }

                             
                                
                                
                                
                                if (activeBricks <= 0) currentState = WIN; // if all bricks destroyed changes the game state to win
                            }

                    }
                }
            }
                
                
                // Ball collision with walls
                if (ballPosition.x <= BALL_RADIUS || ballPosition.x >= SCREEN_WIDTH - BALL_RADIUS) ballSpeed.x *= -1; // reverses horizontal direction when hitting left or right
                if (ballPosition.y <= BALL_RADIUS) ballSpeed.y *= -1;  // reverse verticaal direction
                
                // Ball out of bounds
                if (ballPosition.y >= SCREEN_HEIGHT) {     // checks if it has gone down bottom of the page
                    ballActive = false;                    // inactivates ball and place it to paddle
                    lives--;
                    ballPosition = (Vector2){ paddle.x + PADDLE_WIDTH/2, SCREEN_HEIGHT - 70};                                        // live goes down
                    if (lives <= 0) {
                        currentState = GAME_OVER;          // checkes if lives are not remaining then state moves to game over
                    }
                }
                
                if (CheckCollisionCircleRec( // checks collision between ball and paddle
                    ballPosition,
                    BALL_RADIUS,
                    (Rectangle){paddle.x, paddle.y, PADDLE_WIDTH, PADDLE_HEIGHT})) { // creates temporary paddle structure with rectangle
                        // calculating bounce angle depending where the ball is hitting the paddle
                        float hitPosition = (ballPosition.x - (paddle.x + PADDLE_WIDTH/2)) / (PADDLE_WIDTH/2); // calculates where on the paddle ball hits
                        ballSpeed.y = -fabs(ballSpeed.y); // fabs gets the absolute value and ball's vertical speed to negative,thats why it bounces upward
                        ballSpeed.x = hitPosition * 5; // determines ball's horizontal speed based on where it hit the paddle
                    }
                
            }
            break;
            
        case GAME_OVER:
        
            if (IsKeyPressed(KEY_ENTER)) {
                score = 0;
                lives = 3;
                scoreSystem = RegularScore;
                activeBricks = BRICK_ROWS * BRICK_COLS;
                ballActive = false;
                paddle.x = SCREEN_WIDTH/2 - PADDLE_WIDTH/2;
                ballPosition = (Vector2){ SCREEN_WIDTH/2, SCREEN_HEIGHT - 70 };
                ballSpeed = (Vector2){ 5.0f, -5.0f};

                
                for (int i = 0; i < BRICK_ROWS; i++) {
                    for (int j = 0; j < BRICK_COLS; j++) {
                        bricks[i][j].active = true; // loops through all bricks to set all the bricks active
                    }
                }
                
                currentState = TITLE;
            }
        
            break;
            
            case WIN:
            if (IsKeyPressed(KEY_ENTER)) {
                score = 0;
                lives = 3;
                scoreSystem = RegularScore;
                activeBricks = BRICK_ROWS * BRICK_COLS;
                ballActive = false;
                paddle.x = SCREEN_WIDTH/2 - PADDLE_WIDTH/2;
                ballPosition = (Vector2){ SCREEN_WIDTH/2, SCREEN_HEIGHT - 70 };
                ballSpeed = (Vector2){ 5.0f, -5.0f};
                
                for (int i = 0; i < BRICK_ROWS; i++) {
                    for (int j = 0; j < BRICK_COLS; j++) {
                        bricks[i][j].active = true;
                    }
                }
                
                currentState = TITLE;
            }
            break;
    }                         // ball brick collision logic
    // Draw
    BeginDrawing();
    ClearBackground(BLACK);


    switch (currentState) {
        case TITLE:
           DrawText("BREAKOUT", SCREEN_WIDTH/2 - MeasureText("BREAKOUT", 40)/2, SCREEN_HEIGHT/4, 40, WHITE); // positioned 1/4 th down the screen, centred horizontally,40 pixels
           DrawText("PRESS ENTER TO START", SCREEN_WIDTH/2 - MeasureText("PRESS ENTER TO START",20)/2, SCREEN_HEIGHT/2, 20, WHITE); // centered horizontally, 20 pixels
           break;
           case GAMEPLAY:       
        
    for (int i = 0; i < BRICK_ROWS; i++) {
        for (int j = 0; j < BRICK_COLS; j++) {
            if (bricks[i][j].active) {
                DrawRectangleRec(bricks[i][j].rect, bricks[i][j].color);   
                DrawRectangleLinesEx(bricks[i][j].rect, 1, BLACK);       
            }
        }
    }
    
    // Draw Paddle
    DrawRectangleRec(paddle, BLUE);
    
    // Draw Ball
    DrawCircleV(ballPosition, BALL_RADIUS, WHITE);

    DrawText(TextFormat("SCORE: %d", score), 10, 10, 20, WHITE);
    DrawText(TextFormat("LIVES: %d", lives), SCREEN_WIDTH - 100, 10, 20, WHITE);


    // Draw launch instruction
    if (!ballActive) {            
        DrawText("PRESS SPACE TO LAUNCH", SCREEN_WIDTH/2 - MeasureText("PRESS SPACE TO LAUNCH", 20)/2, SCREEN_HEIGHT - 30, 20, GRAY);
    }
    break;

    case GAME_OVER:

    DrawText("GAME OVER", SCREEN_WIDTH/2 - MeasureText("GAME OVER", 40)/2, SCREEN_HEIGHT/4, 40, RED);
    DrawText(TextFormat("FINAL SCORE: %d", score), SCREEN_WIDTH/2 - MeasureText(TextFormat("FINAL SCORE: %d", score), 30)/2, SCREEN_HEIGHT/2, 30, WHITE);
    DrawText("PRESS ENTER TO RESTART", SCREEN_WIDTH/2 - MeasureText("PRESS ENTER TO RESTART", 20)/2, SCREEN_HEIGHT*3/4, 20, WHITE);
    break; 
            
            
    case WIN:
    DrawText("YOU WIN!", SCREEN_WIDTH/2 - MeasureText("YOU WIN!", 40)/2, SCREEN_HEIGHT/4, 40, GREEN);
    DrawText(TextFormat("FINAL SCORE: %d", score), SCREEN_WIDTH/2 - MeasureText(TextFormat("FINAL SCORE: %d", score), 30)/2, SCREEN_HEIGHT/2, 30, WHITE);
    DrawText("PRESS ENTER TO RESTART", SCREEN_WIDTH/2 - MeasureText("PRESS ENTER TO RESTART", 20)/2, SCREEN_HEIGHT*3/4, 20, WHITE);
    break;
     }
 
    EndDrawing();
 }
 FreeBricks(bricks, BRICK_ROWS); //freeds all the memory allocated for bricks and prevents memory leaks  by cleaning up all malloc memory
 CloseWindow();
 return 0;

}
