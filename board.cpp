#include "board.h"
#include "welcomeWindow.h"

// Initialize the static member textures
unordered_map<string, Texture> TextureManager::textures;

// Definition of getTexture function in TextureManager
Texture& TextureManager::getTexture(string textureName) {
    auto result = textures.find(textureName);
    if(result == textures.end()) {
        Texture newTexture;
        newTexture.loadFromFile("files/images/" + textureName + ".png");
        textures[textureName] = newTexture;
        return textures[textureName];
    }
    else {
        return result->second;
    }
}

Board::Board(RenderWindow& window) : gameWindow(window), rd(), gen(rd()) {
    initializeBoard();
}

void Board::initializeBoard() {
    // Load textures using TextureManager
    tileHiddenText = TextureManager::getTexture("tile_hidden");

    tileRevealedText = TextureManager::getTexture("tile_revealed");

    faceHappyText = TextureManager::getTexture("face_happy");

    faceLoseText = TextureManager::getTexture("face_lose");

    pauseText = TextureManager::getTexture("pause");

    debugText = TextureManager::getTexture("debug");

    leaderText = TextureManager::getTexture("leaderboard");

    flagText = TextureManager::getTexture("flag");

    mineText = TextureManager::getTexture("mine");

    num1Text = TextureManager::getTexture("number_1");

    num2Text = TextureManager::getTexture("number_2");

    num3Text = TextureManager::getTexture("number_3");

    num4Text = TextureManager::getTexture("number_4");

    num5Text = TextureManager::getTexture("number_5");

    num6Text = TextureManager::getTexture("number_6");

    num7Text = TextureManager::getTexture("number_7");

    num8Text = TextureManager::getTexture("number_7");

    digitsText = TextureManager::getTexture("digits");

    //load in cfg file
    ifstream configFile("config.cfg");
    if(configFile.is_open()){
        configFile >> tileCols >> tileRows >> mineCount;
    }
    //initialize tiles & gameboard
    gameBoard.resize(tileRows, vector<Sprite>(tileCols, Sprite(tileHiddenText)));
    for (int i = 0; i < tileRows; ++i) {
        for (int j = 0; j < tileCols; ++j) {
            gameBoard[i][j].setPosition(j * 32.f, i * 32.f); // Adjust the position based on the size of your tiles
        }
    }
    //initialize vectors
    flaggedTiles.resize(tileRows, vector<bool>(tileCols, false));
    mines.resize(tileRows, vector<bool>(tileCols, false));
    minesRevealed.resize(tileRows, vector<bool>(tileCols, false));
    placeMines();
}

void Board::draw() {
    // Draw revealed tiles and mines
    for (int i = 0; i < tileRows; ++i) {
        for (int j = 0; j < tileCols; ++j) {
            if (mines[i][j]) {
                // Draw revealed tile background
                Sprite revealedSprite(tileRevealedText);
                revealedSprite.setPosition(j * 32.f, i * 32.f);
                gameWindow.draw(revealedSprite);
                if(!flaggedTiles[i][j]) {
                    // Draw mine on top of revealed tile if tile is not flagged
                    Sprite mineSprite(mineText);
                    mineSprite.setPosition(j * 32.f, i * 32.f);
                    gameWindow.draw(mineSprite);
                }
            }
            else if (minesRevealed[i][j]) {
                // Draw revealed tile
                Sprite revealedSprite(tileRevealedText);
                revealedSprite.setPosition(j * 32.f, i * 32.f);
                gameWindow.draw(revealedSprite);
            }
            else if (gameBoard[i][j].getTexture() == &tileRevealedText) {
                // Draw revealed tile
                gameWindow.draw(gameBoard[i][j]);

                // Draw number if count is greater than 0
                int adjacentMines = countAdjacentMines(i, j);
                if (adjacentMines > 0) {
                    // Load the appropriate number texture
                    Texture& numberTexture = getNumberTexture(adjacentMines);

                    // Create a sprite and draw the number
                    Sprite numberSprite(numberTexture);
                    numberSprite.setPosition(j * 32.f, i * 32.f);
                    gameWindow.draw(numberSprite);
                }
            } else if (gameBoard[i][j].getTexture() == &tileRevealedText) {
                // Draw revealed tile
                gameWindow.draw(gameBoard[i][j]);
            }
        }
    }

    // Draw hidden tiles
    for (int i = 0; i < tileRows; ++i) {
        for (int j = 0; j < tileCols; ++j) {
            if (!minesRevealed[i][j] && gameBoard[i][j].getTexture() == &tileHiddenText) {
                // Draw hidden tile
                gameWindow.draw(gameBoard[i][j]);
            }
        }
    }

    // Draw flags
    for (const auto& flagSprite : flagSprites) {
        gameWindow.draw(flagSprite);
    }

    if(gameState == GameState::PLAYING){
        Sprite faceHappySprite(faceHappyText);
        faceHappySprite.setPosition(((tileCols / 2.f) * 32) - 32, (32 * (tileRows + 0.5)));
        gameWindow.draw(faceHappySprite);
    }
    else if(gameState == GameState::LOST) {
        Sprite loseSprite(faceLoseText);
        loseSprite.setPosition(((tileCols / 2.f) * 32) - 32, (32 * (tileRows + 0.5)));
        gameWindow.draw(loseSprite);
    }

    Sprite pauseSprite(pauseText);
    pauseSprite.setPosition((tileCols * 32) - 240, 32 * (tileRows + 0.5));
    gameWindow.draw(pauseSprite);

    Sprite debugSprite(debugText);
    debugSprite.setPosition((tileCols * 32) - 304, 32 * (tileRows + 0.5));
    gameWindow.draw(debugSprite);

    Sprite leaderSprite(leaderText);
    leaderSprite.setPosition((tileCols * 32) - 176, 32 * (tileRows + 0.5));
    gameWindow.draw(leaderSprite);

}

void Board::handleInput() {
    Event event;
    while (gameWindow.pollEvent(event)) {
        if (event.type == Event::Closed) {
            gameWindow.close();
        } else if (event.type == Event::MouseButtonPressed) {
            if (event.mouseButton.button == Mouse::Left) {
                handleLeftMouseClick(event.mouseButton.x, event.mouseButton.y);
            }
            else if (event.mouseButton.button == Mouse::Right) {
                handleRightMouseClick(event.mouseButton.x, event.mouseButton.y);
            }
        }
    }
}

void Board::revealTile(int row, int col) {
    if (row >= 0 && row < tileRows && col >= 0 && col < tileCols && gameBoard[row][col].getTexture() == &tileHiddenText) {
        // Skip revealing flagged tiles
        if (isTileFlagged(row, col)) {
            return;
        }

        gameBoard[row][col].setTexture(tileRevealedText);

        if (countAdjacentMines(row, col) == 0) {
            vector<pair<int, int>> neighbors;

            // Collect neighboring tiles
            for (int i = row - 1; i <= row + 1; ++i) {
                for (int j = col - 1; j <= col + 1; ++j) {
                    if (i >= 0 && i < tileRows && j >= 0 && j < tileCols) {
                        neighbors.push_back({i, j});
                    }
                }
            }
            // Shuffle the neighbors vector to randomize the order
            shuffle(neighbors.begin(), neighbors.end(), default_random_engine());

            // Recursively reveal neighboring tiles
            for (const auto& neighbor : neighbors) {
                revealTile(neighbor.first, neighbor.second);
            }
        }
    }
}
void Board::handleLeftMouseClick(int mouseX, int mouseY) {
    int row = mouseY / 32;
    int col = mouseX / 32;

    // Check if the reset button is clicked
    if (isResetButtonClicked(mouseX, mouseY)) {
        resetGame();  // Call a function to reset the game
        gameState = GameState::PLAYING;
        return;
    }

    if (gameState == GameState::PLAYING) {
        if (isDebugButtonClicked(mouseX, mouseY)) {
            debugGame();
        }
    }

    if (gameState == GameState::LOST) {
        return;
    }

    if (row >= 0 && row < tileRows && col >= 0 && col < tileCols) {
        if (!isTileFlagged(row, col)) {
            if (mines[row][col]) {
                minesRevealed[row][col] = true;
                for (int i = 0; i < tileRows; ++i) {
                    for (int j = 0; j < tileCols; ++j) {
                        // If the tile has a hidden mine, reveal it
                        if (mines[i][j] && !minesRevealed[i][j] && !flaggedTiles[i][j]) {
                            minesRevealed[i][j] = true;
                        }
                    }
                }
                gameState = GameState::LOST;
            } else {
                revealTile(row, col);
            }
        }
    }
}

void Board::handleRightMouseClick(int mouseX, int mouseY) {
    int row = mouseY / 32;
    int col = mouseX / 32;
    if(gameState == GameState::LOST){
        return;
    }

    if (row >= 0 && row < tileRows && col >= 0 && col < tileCols) {
        if (gameBoard[row][col].getTexture() == &tileHiddenText) {
            // Right-click on a hidden tile: place or remove flag
            if (!isTileFlagged(row, col)) {
                Sprite flagSprite(flagText);
                flagSprite.setPosition(col * 32.f, row * 32.f);
                flagSprites.push_back(flagSprite);
                setTileFlagged(row, col, true);
            } else {
                // Remove flag
                flagSprites.erase(
                        remove_if(flagSprites.begin(), flagSprites.end(),[row, col](const Sprite &flagSprite) {
                            return flagSprite.getPosition().x == col * 32.f && flagSprite.getPosition().y == row * 32.f;
                        }),flagSprites.end());
                setTileFlagged(row, col, false);
            }
        }
    }
}

bool Board::isTileFlagged(int row, int col) {
    return flaggedTiles[row][col]; // Check if the tile at (row, col) is flagged
}

void Board::setTileFlagged(int row, int col, bool flagged) {
    flaggedTiles[row][col] = flagged; // Set the flag status for the tile at (row, col)
}


void Board::placeMines() {
    uniform_int_distribution<> disRow(0, tileRows - 1);
    uniform_int_distribution<> disCol(0, tileCols - 1);

    // Reset mines vector
    mines.clear();
    mines.resize(tileRows, vector<bool>(tileCols, false));

    // Place mines based on random positions
    for (int i = 0; i < mineCount; ++i) {
        int row, col;
        do {
            row = disRow(gen);
            col = disCol(gen);
        } while (mines[row][col]);  // Ensure the position doesn't already have a mine

        mines[row][col] = true;
    }
}

int Board::countAdjacentMines(int row, int col) {
    int count = 0;

    for (int i = row - 1; i <= row + 1; ++i) {
        for (int j = col - 1; j <= col + 1; ++j) {
            // Check if the neighboring cell is within bounds
            if (i >= 0 && i < tileRows && j >= 0 && j < tileCols) {
                // Check if the neighboring cell contains a mine
                if (mines[i][j]) {
                    count++;
                }
            }
        }
    }
    return count;
}

bool Board::isResetButtonClicked(int mouseX, int mouseY) {
    float buttonX = ((tileCols / 2.f) * 32) - 32;
    float buttonY = (32 * (tileRows + 0.5));
    float buttonWidth = faceHappyText.getSize().x;
    float buttonHeight = faceHappyText.getSize().y;

    return mouseX >= buttonX && mouseX <= buttonX + buttonWidth &&
           mouseY >= buttonY && mouseY <= buttonY + buttonHeight;
}

bool Board::isDebugButtonClicked(int mouseX, int mouseY) {
    float buttonX = ((tileCols) * 32) - 304;
    float buttonY = (32 * (tileRows + 0.5));
    float buttonWidth = debugText.getSize().x;
    float buttonHeight = debugText.getSize().y;

    return mouseX >= buttonX && mouseX <= buttonX + buttonWidth &&
           mouseY >= buttonY && mouseY <= buttonY + buttonHeight;
}

void Board::debugGame() {
    // Iterate through all tiles
    for (int i = 0; i < tileRows; ++i) {
        for (int j = 0; j < tileCols; ++j) {
            // If the tile has a hidden mine, reveal it
            if (mines[i][j] && !minesRevealed[i][j]) {
                minesRevealed[i][j] = true;
            }
        }
    }
}

void Board::resetGame() {
    // Clear flagSprites
    flagSprites.clear();

    // Reinitialize vectors
    minesRevealed.clear();  // Clear the existing minesRevealed vector
    minesRevealed.resize(tileRows, vector<bool>(tileCols, false));

    // Reinitialize tiles & game board
    gameBoard.clear();
    gameBoard.resize(tileRows, vector<Sprite>(tileCols, Sprite(tileHiddenText)));

    for (int i = 0; i < tileRows; ++i) {
        for (int j = 0; j < tileCols; ++j) {
            gameBoard[i][j].setPosition(j * 32.f, i * 32.f);
        }
    }

    // Reinitialize flaggedTiles vector
    flaggedTiles.clear();
    flaggedTiles.resize(tileRows, vector<bool>(tileCols, false));

    // Place mines again
    placeMines();

    // Redraw hidden tiles
    for (int i = 0; i < tileRows; ++i) {
        for (int j = 0; j < tileCols; ++j) {
            if (!minesRevealed[i][j] && gameBoard[i][j].getTexture() == &tileHiddenText) {
                // Draw hidden tile
                gameWindow.draw(gameBoard[i][j]);
            }
        }
    }
}

// Helper function to get the number texture based on the count
Texture& Board::getNumberTexture(int count) {
    switch (count) {
        case 1:
            return num1Text;
        case 2:
            return num2Text;
        case 3:
            return num3Text;
        case 4:
            return num4Text;
        case 5:
            return num5Text;
        case 6:
            return num6Text;
        case 7:
            return num7Text;
        case 8:
            return num8Text;
    }
}