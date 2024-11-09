#ifndef MAIN_H
#define MAIN_H

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
#define FEATURES 9 // Number of features (board positions)
#define TITLE_GRID_SIZE 3

typedef enum { EMPTY, PLAYER_X, PLAYER_O } Cell;
typedef enum { PLAYER_X_TURN, PLAYER_O_TURN } PlayerTurn;
typedef enum { MENU, DIFFICULTY_SELECT, GAME, GAME_OVER, AI_ANALYSIS } GameState;
typedef enum { EASY, MEDIUM, HARD } Difficulty;

typedef struct {
    int wins;
    int losses;
    int draws;
    int totalGames;
} DifficultyStats;

typedef struct {
    int tp, tn, fp, fn; // True Positives, True Negatives, False Positives, False Negatives
} ConfusionMatrix;

typedef struct {
    int correctPredictions;
    int totalPredictions;
} AccuracyResult;

typedef struct {
    char symbol;  // 'X', 'O' or ' '
    float alpha;  // For fade effect
    bool active;
} GridSymbol;

GridSymbol titleSymbols[TITLE_GRID_SIZE][TITLE_GRID_SIZE];

extern DifficultyStats easyStats;
extern DifficultyStats mediumStats;
extern DifficultyStats hardStats;
extern Difficulty currentDifficulty;
extern PlayerTurn currentPlayerTurn;
extern bool gameOver;
extern Cell winner;
extern GameState gameState;
extern bool isTwoPlayer;
extern ConfusionMatrix confusionMatrix;
extern float trainingAccuracy;
extern float testingAccuracy;
extern float titleCellScales[TITLE_GRID_SIZE][TITLE_GRID_SIZE];
extern float titleRotations[TITLE_GRID_SIZE][TITLE_GRID_SIZE];
extern float titleAnimSpeed;
extern float buttonVibrationOffset;
extern float vibrationSpeed;
extern float vibrationAmount;

// Declare scroll variables
static float scrollY = 0.0f;
static const float scrollSpeed = 20.0f;

void InitGame();
void UpdateGame();
void UpdateGameOver();
void HandlePlayerTurn();
void AITurn();
void DrawGame();
void DrawDifficultySelect(void);
bool CheckWin(Cell player);
bool CheckDraw();
void DrawMenu();
void DrawGameOver();
void LoadAndEvaluateDataset(void);

int Minimax(Cell board[GRID_SIZE][GRID_SIZE], bool isMaximizing, int depth, int depthLimit);
int EvaluateBoard(Cell board[GRID_SIZE][GRID_SIZE]);

void DrawAIAnalysis();
void DrawDifficultySection(const char* difficulty, DifficultyStats stats, int* y, Color color, int padding, int textFontSize);
void DrawButton(Rectangle bounds, const char* text, int fontSize, bool isHovered);

// Linear Regression Functions
void TrainLinearRegression(float weights[FEATURES + 1], float learningRate, int epochs);
float PredictLinearRegression(float weights[FEATURES + 1], float features[FEATURES]);
void EvaluateLinearRegression(float weights[FEATURES + 1]);

#endif // MAIN_H