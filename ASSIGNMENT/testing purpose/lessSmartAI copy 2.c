#include "raylib.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <limits.h>


#define SCREEN_WIDTH 600
#define SCREEN_HEIGHT 600
#define BUTTON_WIDTH 200
#define BUTTON_HEIGHT 40
#define GRID_SIZE 3
#define CELL_SIZE (SCREEN_WIDTH / GRID_SIZE)


typedef enum { EMPTY, PLAYER_X, PLAYER_O } Cell;
typedef enum { PLAYER_X_TURN, PLAYER_O_TURN } PlayerTurn;
typedef enum { MENU, DIFFICULTY_SELECT, GAME, GAME_OVER } GameState;
typedef enum { EASY, MEDIUM, HARD } Difficulty;
typedef struct {
    int wins;
    int losses;
    int draws;
    int totalGames;
} DifficultyStats;

DifficultyStats easyStats = {0, 0, 0, 0};
DifficultyStats mediumStats = {0, 0, 0, 0};
DifficultyStats hardStats = {0, 0, 0, 0};

Difficulty currentDifficulty = MEDIUM; // Default difficulty
Cell grid[GRID_SIZE][GRID_SIZE];
PlayerTurn currentPlayerTurn = PLAYER_X_TURN;
bool gameOver = false;
Cell winner = EMPTY;
GameState gameState = MENU;
bool isTwoPlayer = false; // Flag to check if it's a two-player or single-player game

void InitGame();
void UpdateGame();
void HandlePlayerTurn();
void AITurn();
void DrawGame();
void DrawDifficultySelect(void);
bool CheckWin(Cell player);
bool CheckDraw();
void DrawMenu();
void DrawGameOver();
// void ResetGame();
void LoadAndEvaluateDataset(void);
void DisplayDifficultyStats(void);

int Minimax(Cell board[GRID_SIZE][GRID_SIZE], bool isMaximizing, int depth);
int EvaluateBoard(Cell board[GRID_SIZE][GRID_SIZE]);

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Tic-Tac-Toe");

    while (!WindowShouldClose())
    {
        if (gameState == MENU) {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                Vector2 mousePos = GetMousePosition();
                // Single Player button
                if (mousePos.x >= SCREEN_WIDTH/2 - 100 && mousePos.x <= SCREEN_WIDTH/2 + 100 &&
                    mousePos.y >= SCREEN_HEIGHT/2 && mousePos.y <= SCREEN_HEIGHT/2 + 40) {
                    isTwoPlayer = false;
                    // gameState = GAME;
                    // InitGame();
                    gameState = DIFFICULTY_SELECT;  // go to difficulty selection instead of game
                }
                // Two Player button
                else if (mousePos.x >= SCREEN_WIDTH/2 - 100 && mousePos.x <= SCREEN_WIDTH/2 + 100 &&
                        mousePos.y >= SCREEN_HEIGHT/2 + 60 && mousePos.y <= SCREEN_HEIGHT/2 + 100) {
                    isTwoPlayer = true;
                    gameState = GAME;
                    InitGame();
                }
                // AI Analysis button
                else if (mousePos.x >= SCREEN_WIDTH/2 - 100 && mousePos.x <= SCREEN_WIDTH/2 + 100 &&
                    mousePos.y >= SCREEN_HEIGHT/2 + 120 && mousePos.y <= SCREEN_HEIGHT/2 + 160) {
                    LoadAndEvaluateDataset();
                    DisplayDifficultyStats();  // Show stats when clicking Analysis button
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
                if (mousePos.x >= SCREEN_WIDTH/2 - BUTTON_WIDTH/2 && 
                    mousePos.x <= SCREEN_WIDTH/2 + BUTTON_WIDTH/2 &&
                    mousePos.y >= SCREEN_HEIGHT/2 + BUTTON_HEIGHT + 20 && 
                    mousePos.y <= SCREEN_HEIGHT/2 + BUTTON_HEIGHT + 20 + BUTTON_HEIGHT)
                {
                    gameState = MENU;
                }
            }
        }
        else if (gameState == DIFFICULTY_SELECT) {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                Vector2 mousePos = GetMousePosition();
                if (mousePos.x >= SCREEN_WIDTH/2 - BUTTON_WIDTH/2 && 
                    mousePos.x <= SCREEN_WIDTH/2 + BUTTON_WIDTH/2) {
                    // easy button
                    if (mousePos.y >= SCREEN_HEIGHT/2 && 
                        mousePos.y <= SCREEN_HEIGHT/2 + BUTTON_HEIGHT) {
                        currentDifficulty = EASY;
                        gameState = GAME;
                        InitGame();
                    }
                    // medium button
                    else if (mousePos.y >= SCREEN_HEIGHT/2 + BUTTON_HEIGHT + 20 && 
                             mousePos.y <= SCREEN_HEIGHT/2 + BUTTON_HEIGHT * 2 + 20) {
                        currentDifficulty = MEDIUM;
                        gameState = GAME;
                        InitGame();
                    }
                    // hard button
                    else if (mousePos.y >= SCREEN_HEIGHT/2 + (BUTTON_HEIGHT + 20) * 2 && 
                             mousePos.y <= SCREEN_HEIGHT/2 + (BUTTON_HEIGHT + 20) * 2 + BUTTON_HEIGHT) {
                        currentDifficulty = HARD;
                        gameState = GAME;
                        InitGame();
                    }
                }
            }
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        switch(gameState) {
            case MENU:
                DrawMenu();
                break;
            case DIFFICULTY_SELECT:
                DrawDifficultySelect();
                break;
            case GAME:
                DrawGame();
                break;
            case GAME_OVER:
                DrawGame();
                DrawGameOver();
                break;
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}

// function to draw difficulty selection screen
void DrawDifficultySelect() {
    const int titleFontSize = 40;
    const int buttonFontSize = 20;
    
    // title
    const char* title = "Select Difficulty";
    DrawText(title, SCREEN_WIDTH/2 - MeasureText(title, titleFontSize)/2, SCREEN_HEIGHT/3, 
             titleFontSize, BLACK);
    
    // easy button
    Rectangle easyBtn = { SCREEN_WIDTH/2 - BUTTON_WIDTH/2, SCREEN_HEIGHT/2, BUTTON_WIDTH, BUTTON_HEIGHT };
    DrawRectangleRec(easyBtn, LIGHTGRAY);
    DrawRectangleLinesEx(easyBtn, 2, BLACK);
    DrawText("Easy", SCREEN_WIDTH/2 - MeasureText("Easy", buttonFontSize)/2, SCREEN_HEIGHT/2 + BUTTON_HEIGHT/4, 
             buttonFontSize, BLACK);
    
    // medium button
    Rectangle mediumBtn = {
        SCREEN_WIDTH/2 - BUTTON_WIDTH/2,
        SCREEN_HEIGHT/2 + BUTTON_HEIGHT + 20,
        BUTTON_WIDTH,
        BUTTON_HEIGHT
    };
    DrawRectangleRec(mediumBtn, LIGHTGRAY);
    DrawRectangleLinesEx(mediumBtn, 2, BLACK);
    DrawText("Medium", SCREEN_WIDTH/2 - MeasureText("Medium", buttonFontSize)/2, SCREEN_HEIGHT/2 + BUTTON_HEIGHT + 20 + BUTTON_HEIGHT/4, 
             buttonFontSize, BLACK);
    
    // hard button
    Rectangle hardBtn = { SCREEN_WIDTH/2 - BUTTON_WIDTH/2, SCREEN_HEIGHT/2 + (BUTTON_HEIGHT + 20) * 2, BUTTON_WIDTH, BUTTON_HEIGHT };
    DrawRectangleRec(hardBtn, LIGHTGRAY);
    DrawRectangleLinesEx(hardBtn, 2, BLACK);
    DrawText("Hard", SCREEN_WIDTH/2 - MeasureText("Hard", buttonFontSize)/2, SCREEN_HEIGHT/2 + (BUTTON_HEIGHT + 20) * 2 + BUTTON_HEIGHT/4, 
             buttonFontSize, BLACK);

    // Handle button hover effects
    Vector2 mousePos = GetMousePosition();
    if ((mousePos.x >= SCREEN_WIDTH/2 - BUTTON_WIDTH/2 && 
         mousePos.x <= SCREEN_WIDTH/2 + BUTTON_WIDTH/2)) {
        // easy button hover
        if (mousePos.y >= SCREEN_HEIGHT/2 && 
            mousePos.y <= SCREEN_HEIGHT/2 + BUTTON_HEIGHT) {
            SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
        }
        // medium button hover
        else if (mousePos.y >= SCREEN_HEIGHT/2 + BUTTON_HEIGHT + 20 && 
                 mousePos.y <= SCREEN_HEIGHT/2 + BUTTON_HEIGHT * 2 + 20) {
            SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
        }
        // hard button hover
        else if (mousePos.y >= SCREEN_HEIGHT/2 + (BUTTON_HEIGHT + 20) * 2 && 
                 mousePos.y <= SCREEN_HEIGHT/2 + (BUTTON_HEIGHT + 20) * 2 + BUTTON_HEIGHT) {
            SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
        }
        else {
            SetMouseCursor(MOUSE_CURSOR_DEFAULT);
        }
    }
    else {
        SetMouseCursor(MOUSE_CURSOR_DEFAULT);
    }
}

void DisplayDifficultyStats() {
    printf("\n=== Difficulty Level Statistics ===\n");
    
    // Easy Stats
    printf("\nEasy Mode:");
    printf("\nAI Wins: %d", easyStats.wins);
    printf("\nAI Losses: %d", easyStats.losses);
    printf("\nAI Draws: %d", easyStats.draws);
    printf("\nAI Win Rate: %.2f%%\n", easyStats.totalGames > 0 ? (float)easyStats.wins/easyStats.totalGames * 100 : 0);
    
    // Medium Stats
    printf("\nMedium Mode:");
    printf("\nAI Wins: %d", mediumStats.wins);
    printf("\nAI Losses: %d", mediumStats.losses);
    printf("\nAI Draws: %d", mediumStats.draws);
    printf("\nAI Win Rate: %.2f%%\n", mediumStats.totalGames > 0 ? (float)mediumStats.wins/mediumStats.totalGames * 100 : 0);
    
    // Hard Stats
    printf("\nHard Mode:");
    printf("\nAI Wins: %d", hardStats.wins);
    printf("\nAI Losses: %d", hardStats.losses);
    printf("\nAI Draws: %d", hardStats.draws);
    printf("\nAI Win Rate: %.2f%%\n", hardStats.totalGames > 0 ? (float)hardStats.wins/hardStats.totalGames * 100 : 0);
}

// structures for tracking accuracy and confusion matrix values
typedef struct {
    int tp, tn, fp, fn; // True Positives, True Negatives, False Positives, False Negatives
} ConfusionMatrix;

typedef struct {
    int correctPredictions;
    int totalPredictions;
} AccuracyResult;

bool simulateGame(char* gameState, int expectedOutcome) {
    Cell simulatedGrid[GRID_SIZE][GRID_SIZE];
    
    // Convert string to grid state
    int index = 0;
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            char c = gameState[index++];
            if (c == 'x' || c == 'X') simulatedGrid[i][j] = PLAYER_X;
            else if (c == 'o' || c == 'O') simulatedGrid[i][j] = PLAYER_O;
            else if (c == 'b' || c == 'B') simulatedGrid[i][j] = EMPTY;
            else simulatedGrid[i][j] = EMPTY;
        }
    }
    
    int prediction = Minimax(simulatedGrid, true, 0);
    return (prediction > 0) == (expectedOutcome == 1);
}

// Split the dataset into training and testing sets and calculate accuracy
void evaluateAccuracy(FILE *file) { 
    ConfusionMatrix trainingCM = {0, 0, 0, 0};
    ConfusionMatrix testingCM = {0, 0, 0, 0};
    AccuracyResult trainAcc = {0, 0}, testAcc = {0, 0};
    
    char line[100];
    int lineCount = 0;

    while (fgets(line, sizeof(line), file)) {
        // int outcome = line[9] - '0'; // Example: Assume outcome is in column 10
        int outcome;
        if (strncmp(&line[9], "positive", 8) == 0) {
            outcome = 1;
        } else if (strncmp(&line[9], "negative", 8) == 0) {
            outcome = 0;
        }

        if (lineCount < 800) { // 80% Training
            if (simulateGame(line, outcome)) {
                trainAcc.correctPredictions++;
                if (outcome == 1) trainingCM.tp++; // True Positive
                else trainingCM.tn++; // True Negative
            } else {
                if (outcome == 1) trainingCM.fn++; // False Negative
                else trainingCM.fp++; // False Positive
            }
            trainAcc.totalPredictions++;
        } else { // 20% Testing
            if (simulateGame(line, outcome)) {
                testAcc.correctPredictions++;
                if (outcome == 1) testingCM.tp++; // True Positive
                else testingCM.tn++; // True Negative
            } else {
                if (outcome == 1) testingCM.fn++; // False Negative
                else testingCM.fp++; // False Positive
            }
            testAcc.totalPredictions++;
        }
        lineCount++;
    }

    // Calculate and print training and testing accuracy
    printf("Training Accuracy: %.2f%%\n", (float)trainAcc.correctPredictions / trainAcc.totalPredictions * 100);
    printf("Testing Accuracy: %.2f%%\n", (float)testAcc.correctPredictions / testAcc.totalPredictions * 100);

    // Print confusion matrix
    printf("Training Confusion Matrix:\n");
    printf("TP: %d, TN: %d, FP: %d, FN: %d\n", trainingCM.tp, trainingCM.tn, trainingCM.fp, trainingCM.fn);
    printf("Testing Confusion Matrix:\n");
    printf("TP: %d, TN: %d, FP: %d, FN: %d\n", testingCM.tp, testingCM.tn, testingCM.fp, testingCM.fn);
}

void LoadDataset() {
    FILE *file = fopen("tic-tac-toe.data", "r");
    if (file == NULL) {
        printf("Error opening file!\n");
        return;
    }

    // Print only summary statistics
    int totalGames = 0;
    char line[100];
    while (fgets(line, sizeof(line), file)) {
        totalGames++;
    }
    printf("\nTotal games in dataset: %d\n", totalGames);
    fclose(file);
}

// Call this function after loading the dataset
void LoadAndEvaluateDataset() {
    LoadDataset(); // Load dataset into memory
    FILE *file = fopen("tic-tac-toe.data", "r");
    if (file == NULL) {
        printf("Error opening file!\n");
        return;
    }
    else {
        evaluateAccuracy(file); // Evaluate accuracy with confusion matrix
        fclose(file);
    }
}

void HandlePlayerTurn()
{
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        Vector2 mousePos = GetMousePosition();
        int row = (int)(mousePos.y / CELL_SIZE);
        int col = (int)(mousePos.x / CELL_SIZE);

        if (row >= 0 && row < GRID_SIZE && col >= 0 && col < GRID_SIZE)
        {
            if (grid[row][col] == EMPTY)
            {
                grid[row][col] = (currentPlayerTurn == PLAYER_X_TURN) ? PLAYER_X : PLAYER_O;
                if (CheckWin(grid[row][col]))
                {
                    gameOver = true;
                    winner = grid[row][col];
                    gameState = GAME_OVER;
                    
                    // Track AI losses when player wins
                    if (winner == PLAYER_X && !isTwoPlayer) {
                        switch(currentDifficulty) {
                            case EASY: easyStats.losses++; easyStats.totalGames++; break;
                            case MEDIUM: mediumStats.losses++; mediumStats.totalGames++; break;
                            case HARD: hardStats.losses++; hardStats.totalGames++; break;
                        }
                    }
                }
                else if (CheckDraw())
                {
                    gameOver = true;
                    gameState = GAME_OVER;
                    switch(currentDifficulty) {
                        case EASY: easyStats.draws++; easyStats.totalGames++; break;
                        case MEDIUM: mediumStats.draws++; mediumStats.totalGames++; break;
                        case HARD: hardStats.draws++; hardStats.totalGames++; break;
                    }
                }
                else
                {
                    currentPlayerTurn = (currentPlayerTurn == PLAYER_X_TURN) ? PLAYER_O_TURN : PLAYER_X_TURN;
                }
            }
        }
    }
}

void UpdateGame()
{
    if (gameOver) return;

    // quit button click
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        Vector2 mousePos = GetMousePosition();
        if (mousePos.x >= SCREEN_WIDTH - 80 && mousePos.x <= SCREEN_WIDTH - 10 &&
            mousePos.y >= 10 && mousePos.y <= 40)
        {
            DisplayDifficultyStats();  // Show stats before returning to menu
            gameState = MENU;
            return;
        }
    }

    // Handle game moves
    if (currentPlayerTurn == PLAYER_X_TURN)
    {
        HandlePlayerTurn();
    }
    else if (currentPlayerTurn == PLAYER_O_TURN)
    {
        if (isTwoPlayer)
        {
            HandlePlayerTurn();
        }
        else
        {
            AITurn();
        }
    }
}

void AITurn()
{
    int bestScore = -1000;
    int bestRow = -1;
    int bestCol = -1;

    // Easy mode: 40% chance of random move
    if (currentDifficulty == EASY) {
        if (GetRandomValue(0, 100) < 40) {
            do {
                bestRow = GetRandomValue(0, 2);
                bestCol = GetRandomValue(0, 2);
            } while (grid[bestRow][bestCol] != EMPTY);
            grid[bestRow][bestCol] = PLAYER_O;
            
            // Update game state and return
            if (CheckWin(PLAYER_O)) {
                gameOver = true;
                winner = PLAYER_O;
                gameState = GAME_OVER;
                switch(currentDifficulty) {
                    case EASY: easyStats.wins++; easyStats.totalGames++; break;
                    case MEDIUM: mediumStats.wins++; mediumStats.totalGames++; break;
                    case HARD: hardStats.wins++; hardStats.totalGames++; break;
                }
            } else if (CheckDraw()) {
                gameOver = true;
                gameState = GAME_OVER;
                switch(currentDifficulty) {
                    case EASY: easyStats.draws++; easyStats.totalGames++; break;
                    case MEDIUM: mediumStats.draws++; mediumStats.totalGames++; break;
                    case HARD: hardStats.draws++; hardStats.totalGames++; break;
                }
            } else {
                currentPlayerTurn = PLAYER_X_TURN;
            }
            return;
        }
    }

    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if (grid[i][j] == EMPTY) {
                grid[i][j] = PLAYER_O;
                int score = Minimax(grid, false, 0);
                grid[i][j] = EMPTY;

                if (score > bestScore) {
                    bestScore = score;
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
        switch(currentDifficulty) {
            case EASY: easyStats.wins++; easyStats.totalGames++; break;
            case MEDIUM: mediumStats.wins++; mediumStats.totalGames++; break;
            case HARD: hardStats.wins++; hardStats.totalGames++; break;
        }
    } 
    else if (CheckDraw()) {
        gameOver = true;
        gameState = GAME_OVER;
        switch(currentDifficulty) {
            case EASY: easyStats.draws++; easyStats.totalGames++; break;
            case MEDIUM: mediumStats.draws++; mediumStats.totalGames++; break;
            case HARD: hardStats.draws++; hardStats.totalGames++; break;
        }
    } 
    else {
        currentPlayerTurn = PLAYER_X_TURN;
    }
}

bool CheckWin(Cell player)
{
    // check rows and columns
    for (int i = 0; i < GRID_SIZE; i++)
    {
        if (grid[i][0] == player && grid[i][1] == player && grid[i][2] == player) return true; // Row check
        if (grid[0][i] == player && grid[1][i] == player && grid[2][i] == player) return true; // Column check
    }

    // Check diagonals
    if (grid[0][0] == player && grid[1][1] == player && grid[2][2] == player) return true; // Main diagonal
    if (grid[0][2] == player && grid[1][1] == player && grid[2][0] == player) return true; // Anti diagonal

    return false; // No win found
}

bool CheckDraw()
{
    for (int i = 0; i < GRID_SIZE; i++)
    {
        for (int j = 0; j < GRID_SIZE; j++)
        {
            if (grid[i][j] == EMPTY) return false; // If there's an empty cell, it's not a draw
        }
    }
    return true; // All cells are filled
}

void DrawGame()
{
    Vector2 mousePos = GetMousePosition();
    
    // Quit button hover
    if (mousePos.x >= SCREEN_WIDTH - 80 && mousePos.x <= SCREEN_WIDTH - 10 &&
        mousePos.y >= 10 && mousePos.y <= 40) {
        SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
    }
    else {
        SetMouseCursor(MOUSE_CURSOR_DEFAULT);
    }

    // the grid and pieces
    for (int i = 0; i < GRID_SIZE; i++)
    {
        for (int j = 0; j < GRID_SIZE; j++)
        {
            Rectangle cell = {(float)(j * CELL_SIZE), (float)(i * CELL_SIZE), (float)CELL_SIZE, (float)CELL_SIZE};
            DrawRectangleRec(cell, LIGHTGRAY);

            if (grid[i][j] == PLAYER_X)
            {
                DrawText("X", cell.x + CELL_SIZE / 2 - 10, cell.y + CELL_SIZE / 2 - 10, 40, BLUE);
            }
            else if (grid[i][j] == PLAYER_O)
            {
                DrawText("O", cell.x + CELL_SIZE / 2 - 10, cell.y + CELL_SIZE / 2 - 10, 40, RED);
            }
        }
    }

    // grid lines
    for (int i = 1; i < GRID_SIZE; i++)
    {
        DrawLine(i * CELL_SIZE, 0, i * CELL_SIZE, SCREEN_HEIGHT, BLACK);
        DrawLine(0, i * CELL_SIZE, SCREEN_WIDTH, i * CELL_SIZE, BLACK);
    }

    // quit button
    Rectangle quitBtn = {
        SCREEN_WIDTH - 80, 10,  // position
        70, 30                  // size
    };
    DrawRectangleRec(quitBtn, RED);
    DrawRectangleLinesEx(quitBtn, 2, MAROON);  // border
    DrawText("Quit", SCREEN_WIDTH - 65, 15, 20, WHITE);

    // turn indicator
    if (!gameOver) {
        const char* turnText;
        if (currentPlayerTurn == PLAYER_X_TURN) {
            turnText = "Player X's Turn";
            DrawText(turnText, SCREEN_WIDTH/2 - MeasureText(turnText, 30)/2, 20, 30, BLUE);
        } 
        else {
            if (isTwoPlayer) {
                turnText = "Player O's Turn";
            } else {
                turnText = "AI's Turn";
            }
            DrawText(turnText, SCREEN_WIDTH/2 - MeasureText(turnText, 30)/2, 20, 30, RED);
        }
    }
}

void DrawMenu() {
    const int titleFontSize = 40;
    const int buttonFontSize = 20;
    const int buttonWidth = 200;
    const int buttonHeight = 40;
    
    // title
    const char* title = "Tic-Tac-Toe";
    Vector2 titlePos = {
        SCREEN_WIDTH/2 - MeasureText(title, titleFontSize)/2,
        SCREEN_HEIGHT/3
    };
    DrawText(title, titlePos.x, titlePos.y, titleFontSize, BLACK);
    
    // Single Player Button
    Rectangle singlePlayerBtn = {
        SCREEN_WIDTH/2 - buttonWidth/2,
        SCREEN_HEIGHT/2,
        buttonWidth,
        buttonHeight
    };
    DrawRectangleRec(singlePlayerBtn, LIGHTGRAY);
    const char* singlePlayerText = "Single Player";
    Vector2 singlePlayerPos = {
        SCREEN_WIDTH/2 - MeasureText(singlePlayerText, buttonFontSize)/2,
        SCREEN_HEIGHT/2 + buttonHeight/4
    };
    DrawText(singlePlayerText, singlePlayerPos.x, singlePlayerPos.y, buttonFontSize, BLACK);
    
    // Two Players Button
    Rectangle twoPlayerBtn = {
        SCREEN_WIDTH/2 - buttonWidth/2,
        SCREEN_HEIGHT/2 + buttonHeight + 20,
        buttonWidth,
        buttonHeight
    };
    DrawRectangleRec(twoPlayerBtn, LIGHTGRAY);
    const char* twoPlayerText = "Two Players";
    Vector2 twoPlayerPos = {
        SCREEN_WIDTH/2 - MeasureText(twoPlayerText, buttonFontSize)/2,
        SCREEN_HEIGHT/2 + buttonHeight + 20 + buttonHeight/4
    };
    DrawText(twoPlayerText, twoPlayerPos.x, twoPlayerPos.y, buttonFontSize, BLACK);

    // Analysis Button
    Rectangle analysisBtn = {
        SCREEN_WIDTH/2 - buttonWidth/2,
        SCREEN_HEIGHT/2 + (buttonHeight + 20) * 2,
        buttonWidth,
        buttonHeight
    };
    DrawRectangleRec(analysisBtn, LIGHTGRAY);
    const char* analysisText = "View AI Analysis";
    Vector2 analysisPos = {
        SCREEN_WIDTH/2 - MeasureText(analysisText, buttonFontSize)/2,
        SCREEN_HEIGHT/2 + (buttonHeight + 20) * 2 + buttonHeight/4
    };
    DrawText(analysisText, analysisPos.x, analysisPos.y, buttonFontSize, BLACK);

    Vector2 mousePos = GetMousePosition();
    
    // Single Player Button hover
    if (mousePos.x >= SCREEN_WIDTH/2 - 100 && mousePos.x <= SCREEN_WIDTH/2 + 100 &&
        mousePos.y >= SCREEN_HEIGHT/2 && mousePos.y <= SCREEN_HEIGHT/2 + 40) {
        SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
    }
    
    // Two Players Button hover
    else if (mousePos.x >= SCREEN_WIDTH/2 - 100 && mousePos.x <= SCREEN_WIDTH/2 + 100 &&
             mousePos.y >= SCREEN_HEIGHT/2 + 60 && mousePos.y <= SCREEN_HEIGHT/2 + 100) {
        SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
    }
    
    // AI Analysis Button hover
    else if (mousePos.x >= SCREEN_WIDTH/2 - 100 && mousePos.x <= SCREEN_WIDTH/2 + 100 &&
             mousePos.y >= SCREEN_HEIGHT/2 + 120 && mousePos.y <= SCREEN_HEIGHT/2 + 160) {
        SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
    }
    else {
        SetMouseCursor(MOUSE_CURSOR_DEFAULT);
    }
}

void DrawGameOver() {
    const int titleFontSize = 40;
    const int buttonFontSize = 20;
    
    // Result Text
    const char* resultText;
    Color resultColor;
    
    if (winner == PLAYER_X) {
        resultText = "Player X Wins!";
        resultColor = BLUE;
    } else if (winner == PLAYER_O) {
        resultText = "Player O Wins!";
        resultColor = RED;
    } else {
        resultText = "It's a Draw!";
        resultColor = DARKGRAY;
    }
    
    Vector2 resultPos = {
        SCREEN_WIDTH/2 - MeasureText(resultText, titleFontSize)/2,
        SCREEN_HEIGHT/3
    };
    DrawText(resultText, resultPos.x, resultPos.y, titleFontSize, resultColor);
    
    // Back to Menu Button with precise hover detection
    Rectangle menuBtn = {
        SCREEN_WIDTH/2 - BUTTON_WIDTH/2,
        SCREEN_HEIGHT/2 + BUTTON_HEIGHT + 20,
        BUTTON_WIDTH,
        BUTTON_HEIGHT
    };
    
    Vector2 mousePos = GetMousePosition();
    if (mousePos.x >= menuBtn.x && mousePos.x <= menuBtn.x + menuBtn.width &&
        mousePos.y >= menuBtn.y && mousePos.y <= menuBtn.y + menuBtn.height) {
        SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
    } else {
        SetMouseCursor(MOUSE_CURSOR_DEFAULT);
    }
    
    DrawRectangleRec(menuBtn, LIGHTGRAY);
    DrawRectangleLinesEx(menuBtn, 2, BLACK);
    
    const char* menuText = "Back to Menu";
    Vector2 menuPos = {
        SCREEN_WIDTH/2 - MeasureText(menuText, buttonFontSize)/2,
        menuBtn.y + BUTTON_HEIGHT/4
    };
    DrawText(menuText, menuPos.x, menuPos.y, buttonFontSize, BLACK);
}

void InitGame()
{
    for (int i = 0; i < GRID_SIZE; i++)
    {
        for (int j = 0; j < GRID_SIZE; j++)
        {
            grid[i][j] = EMPTY;
        }
    }
    gameOver = false;
    winner = EMPTY;
    currentPlayerTurn = PLAYER_X_TURN;
}

// Minimax algorithm
int Minimax(Cell board[GRID_SIZE][GRID_SIZE], bool isMaximizing, int depth)
{
    int score = EvaluateBoard(board);
    if (score == 10) return score - depth; // O (AI) is the maximizing player
    if (score == -10) return score + depth; // X (human) is the minimizing player
    if (CheckDraw()) return 0; // Draw

    if (isMaximizing)
    {
        int bestScore = -1000;
        for (int i = 0; i < GRID_SIZE; i++)
        {
            for (int j = 0; j < GRID_SIZE; j++)
            {
                if (board[i][j] == EMPTY)
                {
                    board[i][j] = PLAYER_O;
                    bestScore = fmax(bestScore, Minimax(board, false, depth + 1));
                    board[i][j] = EMPTY;
                }
            }
        }
        return bestScore;
    }
    else
    {
        int bestScore = 1000;
        for (int i = 0; i < GRID_SIZE; i++)
        {
            for (int j = 0; j < GRID_SIZE; j++)
            {
                if (board[i][j] == EMPTY)
                {
                    board[i][j] = PLAYER_X;
                    bestScore = fmin(bestScore, Minimax(board, true, depth + 1));
                    board[i][j] = EMPTY;
                }
            }
        }
        return bestScore;
    }
}

int EvaluateBoard(Cell board[GRID_SIZE][GRID_SIZE])
{
    // Check rows and columns for a win
    for (int row = 0; row < GRID_SIZE; row++)
    {
        if (board[row][0] == board[row][1] && board[row][0] == board[row][2])
        {
            if (board[row][0] == PLAYER_O) return 10;
            else if (board[row][0] == PLAYER_X) return -10;
        }
    }
    for (int col = 0; col < GRID_SIZE; col++)
    {
        if (board[0][col] == board[1][col] && board[0][col] == board[2][col])
        {
            if (board[0][col] == PLAYER_O) return 10;
            else if (board[0][col] == PLAYER_X) return -10;
        }
    }

    // Check diagonals for a win
    if (board[0][0] == board[1][1] && board[0][0] == board[2][2])
    {
        if (board[0][0] == PLAYER_O) return 10;
        else if (board[0][0] == PLAYER_X) return -10;
    }
    if (board[0][2] == board[1][1] && board[0][2] == board[2][0])
    {
        if (board[0][2] == PLAYER_O) return 10;
        else if (board[0][2] == PLAYER_X) return -10;
    }

    return 0; // No winner
}

// gcc -o lessSmartAI-Copy2 "lessSmartAI copy 2.c" -IC:\\msys64\\mingw64\\include -LC:\\msys64\\mingw64\\lib -lraylib -lopengl32 -lgdi32 -lwinmm