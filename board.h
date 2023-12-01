#ifndef MINESWEEPER_BOARD_H
#define MINESWEEPER_BOARD_H

#pragma once
#include <unordered_map>
#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>
#include <fstream>
#include <random>
#include <algorithm>
#include <chrono>

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
    private:
        random_device rd;
        mt19937 gen;
        Texture tileHiddenText;
        Texture tileRevealedText;
        Texture faceHappyText;
        Texture faceLoseText;
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
        void initializeBoard();
        void handleLeftMouseClick(int mouseX, int mouseY);
        void handleRightMouseClick(int mouseX, int mouseY);
        void setTileFlagged(int row, int col, bool flagged);
        void placeMines();
        void revealTile(int row, int col);
        void debugGame();
        void resetGame();
        bool isResetButtonClicked(int mouseX, int mouseY);
        bool isDebugButtonClicked(int mouseX, int mouseY);
        bool isTileFlagged(int row, int col);
        int countAdjacentMines(int row, int col);
        enum class GameState {PLAYING, LOST, WIN, PAUSE};
        GameState gameState = GameState::PLAYING;
        int tileRows;
        int tileCols;
        int mineCount;
};

#endif //MINESWEEPER_BOARD_H