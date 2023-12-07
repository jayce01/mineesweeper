#ifndef MINESWEEPER_BOARD_H
#define MINESWEEPER_BOARD_H

#pragma once
#include <unordered_map>
#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <random>
#include <algorithm>
#include <chrono>
#include <map>

using namespace std;
using namespace sf;

class TextureManager {
    static unordered_map<string, Texture> textures;

    public:
        static Texture& getTexture(string textureName);
};

class Board {
    public:
        Board(RenderWindow& window);
        void draw();
        void handleInput();
        void drawGameTimer(RenderWindow& window);
        void startTimer();
        void updateTimer();
        void drawLeaderBoard();
        bool checkWinCondition();
        enum class GameState {PLAYING, LOST, WIN, PAUSE};
        GameState gameState = GameState::PLAYING;
        GameState previousGameState;
        GameState toggleGameStateInPlay;
    private:
        Text leaderboard;
        Font font;
        RenderWindow leaderboardWindow;
        random_device rd;
        mt19937 gen;
        Texture tileHiddenText;
        Texture tileRevealedText;
        Texture faceHappyText;
        Texture faceLoseText;
        Texture faceWinText;
        Texture pauseText;
        Texture debugText;
        Texture leaderText;
        Texture flagText;
        Texture mineText;
        Texture num1Text;
        Texture num2Text;
        Texture num3Text;
        Texture num4Text;
        Texture num5Text;
        Texture num6Text;
        Texture num7Text;
        Texture num8Text;
        Texture digitsText;
        Texture playText;
        Texture& getNumberTexture(int count);
        vector<vector<Sprite>> gameBoard;
        vector<vector<bool>> flaggedTiles;
        vector<vector<bool>> mines;
        vector<vector<bool>> minesRevealed;
        vector<Sprite> flagSprites;
        map<int, Sprite> digitsMap;
        map<int, Sprite> parseDigits(Sprite digits);
        RenderWindow& gameWindow;
        void initializeBoard();
        void handleLeftMouseClick(int mouseX, int mouseY);
        void handleRightMouseClick(int mouseX, int mouseY);
        void setTileFlagged(int row, int col, bool flagged);
        void setText(Text &text, float x, float y);
        void placeMines();
        void revealTile(int row, int col);
        void debugGame();
        void resetGame();
        bool isResetButtonClicked(int mouseX, int mouseY);
        bool isDebugButtonClicked(int mouseX, int mouseY);
        bool isPauseButtonClicked(int mouseX, int mouseY);
        bool isPlayButtonClicked(int mouseX, int mouseY);
        bool isLeaderButtonClicked(int mouseX, int mouseY);
        bool isTileFlagged(int row, int col);
        bool checkRemainingTileCount();
        int countAdjacentMines(int row, int col);
        chrono::time_point<chrono::high_resolution_clock> startTime;
        chrono::high_resolution_clock::time_point pausedTime;
        int tileRows;
        int tileCols;
        int mineCount;
        int mineCounter;
        bool paused;
        int minutes;
        int seconds;
        long elapsedPausedTime;
        bool isGamePaused;
        int tileRevealedCounter;
        float screenHeight;
        float screenWidth;
};

#endif //MINESWEEPER_BOARD_H