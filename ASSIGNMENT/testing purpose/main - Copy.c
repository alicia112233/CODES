#include "C:\msys64\mingw64\include\raylib.h"
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#define SCREEN_WIDTH 600
#define SCREEN_HEIGHT 600
#define GRID_SIZE 3
#define CELL_SIZE (SCREEN_WIDTH / GRID_SIZE)
#define DATASET_SIZE 958
#define FEATURE_COUNT 9
#define TRAINING_RATIO 0.8
#define LABEL_SIZE 10  // adjust size as needed

typedef enum { EMPTY, PLAYER_X, PLAYER_O } Cell;
typedef enum { PLAYER_X_TURN, PLAYER_O_TURN } PlayerTurn;
typedef enum { MENU, GAME, GAME_OVER } GameState;

typedef struct {
    char features[FEATURE_COUNT];  // 'x', 'o', or 'b'
    // char label;                    // 
    char label[LABEL_SIZE];  // make label a string with enough space for the label name, 'positive' for win, 'negative' for lose
} GameData;

Cell grid[GRID_SIZE][GRID_SIZE];
PlayerTurn currentPlayerTurn = PLAYER_X_TURN;
bool gameOver = false;
Cell winner = EMPTY;
GameState gameState = MENU;
bool isTwoPlayer = false; // Flag to check if it's a two-player or single-player game

GameData dataset[DATASET_SIZE];
int total_games = 0, win_count = 0, lose_count = 0;

// Function declarations
void InitGame();
void UpdateGame();
void HandlePlayerTurn();
void AITurn();
void DrawGame();
bool CheckWin(Cell player);
bool CheckDraw();
void DrawMenu();
void DrawGameOver();
void ResetGame();
void load_dataset(char *filename);
float naive_bayes_predict(Cell board[GRID_SIZE][GRID_SIZE]);
void split_dataset();

// Main function
int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Tic-Tac-Toe");

    load_dataset("tic-tac-toe.data");

    while (!WindowShouldClose())
    {
        if (gameState == MENU)
        {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                Vector2 mousePos = GetMousePosition();
                if (mousePos.x > SCREEN_WIDTH / 2 - 100 && mousePos.x < SCREEN_WIDTH / 2 + 100)
                {
                    // For 1 Player button
                    if (mousePos.y > SCREEN_HEIGHT / 2 && mousePos.y < SCREEN_HEIGHT / 2 + 40)
                    {
                        isTwoPlayer = false;
                        gameState = GAME;
                        InitGame();
                    }
                    // For 2 Players button
                    else if (mousePos.y > SCREEN_HEIGHT / 2 + 80 && mousePos.y < SCREEN_HEIGHT / 2 + 120)
                    {
                        isTwoPlayer = true;
                        gameState = GAME;
                        InitGame();
                    }
                }
            }
        }

        else if (gameState == GAME)
        {
            UpdateGame();
        }
        else if (gameState == GAME_OVER)
        {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                Vector2 mousePos = GetMousePosition();
                if (mousePos.x > SCREEN_WIDTH / 2.0 - 100 && mousePos.x < SCREEN_WIDTH / 2.0 + 100 &&
                    mousePos.y > SCREEN_HEIGHT / 2.0 + 40 && mousePos.y < SCREEN_HEIGHT / 2.0 + 80)
                {
                    gameState = MENU;
                }
            }
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (gameState == MENU)
        {
            DrawMenu();
        }
        else if (gameState == GAME)
        {
            DrawGame();
        }
        else if (gameState == GAME_OVER)
        {
            DrawGame();
            DrawGameOver();
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}

// Function to load the dataset from the file
void load_dataset(char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error opening file.\n");
        return;
    }

    char line[256];
    int index = 0;
    
    while (fgets(line, sizeof(line), file)) {
        sscanf(line, "%c,%c,%c,%c,%c,%c,%c,%c,%c,%s",
               &dataset[index].features[0], &dataset[index].features[1],
               &dataset[index].features[2], &dataset[index].features[3],
               &dataset[index].features[4], &dataset[index].features[5],
               &dataset[index].features[6], &dataset[index].features[7],
               &dataset[index].features[8], dataset[index].label);
        index++;
    }

    fclose(file);
    
    // Calculate total win/lose counts
    for (int i = 0; i < DATASET_SIZE; i++) {
        if (strcmp(dataset[i].label, "positive") == 0) win_count++;
        if (strcmp(dataset[i].label, "negative") == 0) lose_count++;
    }
    total_games = win_count + lose_count;
}
// Function to predict using Naive Bayes
// float naive_bayes_predict(Cell board[GRID_SIZE][GRID_SIZE]) {
//     float prob_win = (float)win_count / total_games;
//     float prob_lose = (float)lose_count / total_games;
    
//     float likelihood_win = 1.0;
//     float likelihood_lose = 1.0;

//     for (int i = 0; i < FEATURE_COUNT; i++) {
//         int row = i / 3;
//         int col = i % 3;
//         char cell = board[row][col];
        
//         int win_feature_count = 0, lose_feature_count = 0;
//         for (int j = 0; j < DATASET_SIZE; j++) {
//             if (strcmp(dataset[j].label, "positive") == 0 && dataset[j].features[i] == cell) {
//                 win_feature_count++;
//             }
//             if (strcmp(dataset[j].label, "negative") == 0 && dataset[j].features[i] == cell) {
//                 lose_feature_count++;
//             }
//         }
        
//         // Avoid division by zero
//         if (win_count > 0) likelihood_win *= (float)win_feature_count / win_count;
//         if (lose_count > 0) likelihood_lose *= (float)lose_feature_count / lose_count;

//         // Debugging prints to show likelihood calculation
//         printf("Feature %d (Cell %d,%d): '%c', Likelihood Win: %f, Likelihood Lose: %f\n", i, row, col, cell, likelihood_win, likelihood_lose);
//     }
    
//     // Calculate posterior probabilities
//     float posterior_win = prob_win * likelihood_win;
//     float posterior_lose = prob_lose * likelihood_lose;

//     // Print posterior probabilities to confirm Naive Bayes usage
//     printf("Posterior Win: %f, Posterior Lose: %f\n", posterior_win, posterior_lose);

//     // Return the most likely outcome
//     return (posterior_win > posterior_lose) ? 1.0 : -1.0;
// }

float naive_bayes_predict(Cell board[GRID_SIZE][GRID_SIZE]) {
    float prob_win = (float)(win_count + 1) / (total_games + 2); // Laplace smoothing
    float prob_lose = (float)(lose_count + 1) / (total_games + 2);

    float log_likelihood_win = 0.0; // Using logs instead of direct multiplication
    float log_likelihood_lose = 0.0;

    for (int i = 0; i < FEATURE_COUNT; i++) {
        int row = i / 3;
        int col = i % 3;
        char cell = board[row][col] == PLAYER_X ? 'x' : (board[row][col] == PLAYER_O ? 'o' : 'b'); // Map Cell enum to 'x', 'o', 'b'
        
        int win_feature_count = 1; // Laplace smoothing start count
        int lose_feature_count = 1;

        for (int j = 0; j < DATASET_SIZE; j++) {
            if (strcmp(dataset[j].label, "positive") == 0 && dataset[j].features[i] == cell) {
                win_feature_count++;
            }
            if (strcmp(dataset[j].label, "negative") == 0 && dataset[j].features[i] == cell) {
                lose_feature_count++;
            }
        }

        // Adjust with smoothed probabilities
        log_likelihood_win += log((float)win_feature_count / (win_count + 3)); // Adding 3 for each possible 'x', 'o', 'b'
        log_likelihood_lose += log((float)lose_feature_count / (lose_count + 3));

        // Debugging prints to show likelihood calculation
        printf("Feature %d (Cell %d,%d): '%c', Log Likelihood Win: %f, Log Likelihood Lose: %f\n", 
               i+1, row+1, col+1, cell, log_likelihood_win, log_likelihood_lose);
    }

    // Calculate log posteriors
    float log_posterior_win = log(prob_win) + log_likelihood_win;
    float log_posterior_lose = log(prob_lose) + log_likelihood_lose;

    // Print posterior probabilities to confirm Naive Bayes usage
    printf("Log Posterior Win: %f, Log Posterior Lose: %f\n", log_posterior_win, log_posterior_lose);

    // Return the most likely outcome
    return (log_posterior_win > log_posterior_lose) ? 1.0 : -1.0;
}


// AI Turn logic using Naive Bayes
void AITurn() {
    printf("AI is making a move using Naive Bayes...\n");
    int bestRow = -1, bestCol = -1;
    float bestPrediction = -1000.0;

    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if (grid[i][j] == EMPTY) {
                grid[i][j] = PLAYER_O;
                float prediction = naive_bayes_predict(grid);
                grid[i][j] = EMPTY;

                if (prediction > bestPrediction) {
                    bestPrediction = prediction;
                    bestRow = i;
                    bestCol = j;
                }
            }
        }
    }

    grid[bestRow][bestCol] = PLAYER_O;

    if (CheckWin(PLAYER_O)) {
        gameOver = true;
        winner = PLAYER_O;
        gameState = GAME_OVER;
        printf("Player O Wins!\n");  // Print here for AI win
    } 
    else if (CheckDraw()) {
        gameOver = true;
        gameState = GAME_OVER;
        printf("It's a draw!\n");  // Print here for draw
    } 
    else {
        currentPlayerTurn = PLAYER_X_TURN;
    }
}

void UpdateGame() {
    if (gameOver) return;

    if (currentPlayerTurn == PLAYER_X_TURN) {
        HandlePlayerTurn(); // Human's turn
    }
    else if (currentPlayerTurn == PLAYER_O_TURN) {
        if (isTwoPlayer) {
            HandlePlayerTurn(); // Player 2's turn in 2 Player mode
        }
        else {
            AITurn(); // AI's turn in 1 Player mode
        }
    }
}

void HandlePlayerTurn() {
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 mousePos = GetMousePosition();
        int row = (int)(mousePos.y / CELL_SIZE);
        int col = (int)(mousePos.x / CELL_SIZE);

        if (grid[row][col] == EMPTY) {
            if (currentPlayerTurn == PLAYER_X_TURN) {
                grid[row][col] = PLAYER_X;
                if (CheckWin(PLAYER_X)) {
                    gameOver = true;
                    winner = PLAYER_X;
                    gameState = GAME_OVER;
                    printf("Player X Wins!\n");
                }
                else if (CheckDraw()) {
                    gameOver = true;
                    gameState = GAME_OVER;
                    printf("It's a Draw!\n");
                }
                else {
                    currentPlayerTurn = PLAYER_O_TURN;
                }
            }
            else if (currentPlayerTurn == PLAYER_O_TURN && isTwoPlayer) {
                grid[row][col] = PLAYER_O;
                if (CheckWin(PLAYER_O)) {
                    gameOver = true;
                    winner = PLAYER_O;
                    gameState = GAME_OVER;
                    printf("Player O Wins!\n");
                }
                else if (CheckDraw()) {
                    gameOver = true;
                    gameState = GAME_OVER;
                    printf("It's a Draw!\n");
                }
                else {
                    currentPlayerTurn = PLAYER_X_TURN;
                }
            }
        }
    }
}

bool CheckWin(Cell player) {
    for (int i = 0; i < GRID_SIZE; i++) {
        if (grid[i][0] == player && grid[i][1] == player && grid[i][2] == player) return true;
        if (grid[0][i] == player && grid[1][i] == player && grid[2][i] == player) return true;
    }
    if (grid[0][0] == player && grid[1][1] == player && grid[2][2] == player) return true;
    if (grid[0][2] == player && grid[1][1] == player && grid[2][0] == player) return true;
    return false;
}

bool CheckDraw() {
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if (grid[i][j] == EMPTY) return false;
        }
    }
    return true;
}

void DrawGame() {
    for (int i = 0; i <= GRID_SIZE; i++) {
        DrawLine(i * CELL_SIZE, 0, i * CELL_SIZE, SCREEN_HEIGHT, BLACK);
        DrawLine(0, i * CELL_SIZE, SCREEN_WIDTH, i * CELL_SIZE, BLACK);
    }

    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if (grid[i][j] == PLAYER_X) {
                DrawText("X", j * CELL_SIZE + CELL_SIZE / 2 - MeasureText("X", 100) / 2, i * CELL_SIZE + CELL_SIZE / 2 - 50, 100, RED);
            }
            else if (grid[i][j] == PLAYER_O) {
                DrawText("O", j * CELL_SIZE + CELL_SIZE / 2 - MeasureText("O", 100) / 2, i * CELL_SIZE + CELL_SIZE / 2 - 50, 100, BLUE);
            }
        }
    }
    
    if (!gameOver) {
        const char *message = (currentPlayerTurn == PLAYER_X_TURN) ? "Player X's turn" : "Player O's turn";
        DrawText(message, SCREEN_WIDTH / 2 - MeasureText(message, 20) / 2, 10, 20, BLACK);
    }
}

void DrawMenu() {
    // Logo centered vertically at 1/3 of screen height
    DrawCircle(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 3, 80, BLUE);
    DrawText("X", SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 3 - 40, 60, RED);
    DrawText("O", SCREEN_WIDTH / 2 + 10, SCREEN_HEIGHT / 3 - 40, 60, WHITE);
    
    // Title positioned halfway between logo and buttons
    DrawText("Tic-Tac-Toe", SCREEN_WIDTH / 2 - MeasureText("Tic-Tac-Toe", 40) / 2, SCREEN_HEIGHT / 2 - 80, 40, BLACK);
    
    // Buttons centered in lower half
    DrawRectangle(SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2, 200, 40, LIGHTGRAY);
    DrawText("1 Player", SCREEN_WIDTH / 2 - MeasureText("1 Player", 20) / 2, SCREEN_HEIGHT / 2 + 10, 20, BLACK);
    
    DrawRectangle(SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 + 80, 200, 40, LIGHTGRAY);
    DrawText("2 Players", SCREEN_WIDTH / 2 - MeasureText("2 Players", 20) / 2, SCREEN_HEIGHT / 2 + 90, 20, BLACK);
}

void DrawGameOver() {
    const char *message;
    if (winner == PLAYER_X) {
        message = "Player X Wins!";
    }
    else if (winner == PLAYER_O) {
        message = "Player O Wins!";
    }
    else {
        message = "It's a Draw!";
    }

    DrawText(message, SCREEN_WIDTH / 2 - MeasureText(message, 40) / 2, SCREEN_HEIGHT / 2 - 40, 40, BLACK);
    DrawRectangle(SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 + 40, 200, 40, LIGHTGRAY);
    DrawText("Main Menu", SCREEN_WIDTH / 2 - MeasureText("Main Menu", 20) / 2, SCREEN_HEIGHT / 2 + 50, 20, BLACK);
}

void InitGame() {
    srand(time(NULL)); // Seed the random number generator

    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            grid[i][j] = EMPTY;
        }
    }

    currentPlayerTurn = PLAYER_X_TURN;
    gameOver = false;
    winner = EMPTY;
}

// gcc -v (to check ur gcc bits)
// gcc -o main-Copy "main - Copy.c" -IC:\\msys64\\mingw64\\include -LC:\\msys64\\mingw64\\lib -lraylib -lopengl32 -lgdi32 -lwinmm //