//problems;need win condition and finished lb
//win does not pull up leaderboard

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

Board::Board(RenderWindow& window) : gameWindow(window), rd(), gen(rd()), paused(false), isGamePaused(false), elapsedPausedTime(0) {
    initializeBoard();
    mineCounter = mineCount;
    digitsText = TextureManager::getTexture("digits");
    Sprite digits;
    digits.setTexture(digitsText);
    digitsMap = parseDigits(digits);
}

void Board::initializeBoard() {
    // Load textures using TextureManager
    tileHiddenText = TextureManager::getTexture("tile_hidden");

    tileRevealedText = TextureManager::getTexture("tile_revealed");

    faceHappyText = TextureManager::getTexture("face_happy");

    faceLoseText = TextureManager::getTexture("face_lose");

    faceWinText = TextureManager::getTexture("face_win");

    pauseText = TextureManager::getTexture("pause");

    playText = TextureManager::getTexture("play");

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
    // Draw revealed tiles and mines when the game is not paused
    if (gameState != GameState::PAUSE) {
        for (int i = 0; i < tileRows; ++i) {
            for (int j = 0; j < tileCols; ++j) {
                if (mines[i][j]) {
                    // Draw revealed tile background
                    Sprite revealedSprite(tileRevealedText);
                    revealedSprite.setPosition(j * 32.f, i * 32.f);
                    gameWindow.draw(revealedSprite);
                    if (!flaggedTiles[i][j]) {
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
    }
    else {
        // Draw all tiles as revealed when the game is paused
        for (int i = 0; i < tileRows; ++i) {
            for (int j = 0; j < tileCols; ++j) {
                // Draw revealed tile
                Sprite revealedSprite(tileRevealedText);
                revealedSprite.setPosition(j * 32.f, i * 32.f);
                gameWindow.draw(revealedSprite);
            }
        }
    }

    drawLeaderBoard();

    //draw timer
    drawGameTimer(gameWindow);

    if(gameState == GameState::PLAYING){
        Sprite faceHappySprite(faceHappyText);
        faceHappySprite.setPosition(((tileCols / 2.f) * 32) - 32, (32 * (tileRows + 0.5)));
        gameWindow.draw(faceHappySprite);

        Sprite pauseSprite(pauseText);
        pauseSprite.setPosition((tileCols * 32) - 240, 32 * (tileRows + 0.5));
        gameWindow.draw(pauseSprite);
    }
    else if(gameState == GameState::LOST) {
        Sprite pauseSprite(pauseText);
        pauseSprite.setPosition((tileCols * 32) - 240, 32 * (tileRows + 0.5));
        gameWindow.draw(pauseSprite);

        Sprite loseSprite(faceLoseText);
        loseSprite.setPosition(((tileCols / 2.f) * 32) - 32, (32 * (tileRows + 0.5)));
        gameWindow.draw(loseSprite);


    }

    else if(gameState == GameState::PAUSE) {
        Sprite playSprite(playText);
        playSprite.setPosition((tileCols * 32) - 240, 32 * (tileRows + 0.5));
        gameWindow.draw(playSprite);

        Sprite faceHappySprite(faceHappyText);
        faceHappySprite.setPosition(((tileCols / 2.f) * 32) - 32, (32 * (tileRows + 0.5)));
        gameWindow.draw(faceHappySprite);
    }
    else if(gameState == GameState::WIN) {
        Sprite pauseSprite(pauseText);
        pauseSprite.setPosition((tileCols * 32) - 240, 32 * (tileRows + 0.5));
        gameWindow.draw(pauseSprite);

        Sprite faceWinSprite(faceWinText);
        faceWinSprite.setPosition(((tileCols / 2.f) * 32) - 32, (32 * (tileRows + 0.5)));
        gameWindow.draw(faceWinSprite);

        drawLeaderBoard();
    }

    // Draw mine counter
    int counterX = 33;
    int counterY = 32 * (tileRows + 0.5) + 16;

    // Convert mineCounter to a string for easier manipulation
    string mineCounterString = to_string(abs(mineCounter));

    // Calculate the number of leading zeros needed
    int numLeadingZeros = max(3 - static_cast<int>(mineCounterString.length()), 0);

    // Draw leading zeros for negative mineCounter
    if (mineCounter < 0) {
        digitsMap[0].setPosition(counterX, counterY);
        gameWindow.draw(digitsMap[0]);
        counterX += 21;
        numLeadingZeros--;  // Decrease the count of remaining leading zeros
    }

// Draw remaining leading zeros
    for (int i = 0; i < numLeadingZeros; ++i) {
        digitsMap[0].setPosition(counterX, counterY);
        gameWindow.draw(digitsMap[0]);
        counterX += 21;
    }

    if (mineCounter >= 0) {
        // Draw positive counter digits
        for (int digit : to_string(mineCounter)) {
            digitsMap[digit - '0'].setPosition(counterX, counterY);
            gameWindow.draw(digitsMap[digit - '0']);
            counterX += 21;
        }
    }
    else {
        // Draw negative counter
        digitsMap[-1].setPosition(12, 32 * (tileRows + 0.5) + 16);
        gameWindow.draw(digitsMap[-1]);

        for (int digit : to_string(-mineCounter)) {
            digitsMap[digit - '0'].setPosition(counterX, counterY);
            gameWindow.draw(digitsMap[digit - '0']);
            counterX += 21;
        }
    }

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
        }
        else if (event.type == Event::MouseButtonPressed && paused && (isPlayButtonClicked(event.mouseButton.x, event.mouseButton.y) ||
                                                             isLeaderButtonClicked(event.mouseButton.x, event.mouseButton.y) ||
                                                             isDebugButtonClicked(event.mouseButton.x, event.mouseButton.y) ||
                                                             isResetButtonClicked(event.mouseButton.x, event.mouseButton.y))) {
            if (event.mouseButton.button == Mouse::Left) {
                handleLeftMouseClick(event.mouseButton.x, event.mouseButton.y);
            }
            else if (event.mouseButton.button == Mouse::Right) {
                handleRightMouseClick(event.mouseButton.x, event.mouseButton.y);
            }
        }
        else if(event.type == Event::MouseButtonPressed && !paused && !leaderboardWindow.isOpen()) {
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
        mineCounter = mineCount;
        gameState = GameState::PLAYING;
        paused = false;
        startTimer();
        return;
    }

    auto pauseTime = chrono::high_resolution_clock::now();
    if(gameState == GameState::PLAYING){
        if(isDebugButtonClicked(mouseX, mouseY)) {
            debugGame();
        }
    }
    if (gameState == GameState::PLAYING) {
        if (isPauseButtonClicked(mouseX, mouseY)) {
            //Toggle the paused state
            paused = !paused;
            isGamePaused = true;
            // If paused, stop the timer; if unpaused, start the timer
            if (paused) {
                gameState = GameState::PAUSE; // Stop the timer
                pausedTime = chrono::high_resolution_clock::now();
            }
        }
    }

    else if(gameState == GameState::PAUSE){
        if(isPlayButtonClicked(mouseX, mouseY)){
            auto unPausedTime = chrono::steady_clock::now();
            gameState = GameState::PLAYING;
            paused = false;
        }
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
            }
            else {
                revealTile(row, col);
                tileRevealedCounter++;
            }
        }

    }


    if(isLeaderButtonClicked(mouseX, mouseY)) {
        if(gameState == GameState::LOST) {
            previousGameState = GameState::LOST;
        }
        if(gameState == GameState::PLAYING) {
            paused = !paused;
        }
        // If paused, stop the timer; if unpaused, start the timer
        if (paused) {
            gameState = GameState::PAUSE; // Stop the timer
            pausedTime = chrono::high_resolution_clock::now();
        }
        screenHeight = (tileRows * 16) + 50;
        screenWidth = (tileCols * 16);
        leaderboardWindow.create(VideoMode(screenWidth, screenHeight), "Leaderboard");
    }
}

void Board::handleRightMouseClick(int mouseX, int mouseY) {
    int row = mouseY / 32;
    int col = mouseX / 32;
    if(gameState == GameState::LOST){
        return;
    }

    if (row >= 0 && row < tileRows && col >= 0 && col < tileCols && gameState == GameState::PLAYING && !paused) {
        if (gameBoard[row][col].getTexture() == &tileHiddenText) {
            // Right-click on a hidden tile: place or remove flag
            if (!isTileFlagged(row, col)) {
                Sprite flagSprite(flagText);
                flagSprite.setPosition(col * 32.f, row * 32.f);
                flagSprites.push_back(flagSprite);
                setTileFlagged(row, col, true);
                mineCounter--;
            } else {
                // Remove flag
                flagSprites.erase(
                        remove_if(flagSprites.begin(), flagSprites.end(),[row, col](const Sprite &flagSprite) {
                            return flagSprite.getPosition().x == col * 32.f && flagSprite.getPosition().y == row * 32.f;
                        }),flagSprites.end());
                setTileFlagged(row, col, false);
                mineCounter++;
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
            if (mines[i][j] && !paused && !flaggedTiles[i][j]) {
                minesRevealed[i][j] = !minesRevealed[i][j];
            }
        }
    }
}


void Board::resetGame() {
    //reset timer
    startTimer();

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

map<int, Sprite> Board::parseDigits(Sprite digits) {
    map<int, Sprite> digitsMap;

    // Add an entry for the negative sign
    IntRect rectNegative(10 * 21, 0, 21, 32);
    digits.setTextureRect(rectNegative);
    Sprite spriteNegative = digits;
    digitsMap.emplace(-1, spriteNegative);

    for (int i = 0; i < 10; i++) {
        IntRect rect(i * 21, 0, 21, 32);
        digits.setTextureRect(rect);
        Sprite sprite = digits;
        digitsMap.emplace(i, sprite);
    }

    return digitsMap;
}

bool Board::isPauseButtonClicked(int mouseX, int mouseY) {
    float buttonX = ((tileCols) * 32) - 240;
    float buttonY = (32 * (tileRows + 0.5));
    float buttonWidth = pauseText.getSize().x;
    float buttonHeight = pauseText.getSize().y;

    return mouseX >= buttonX && mouseX <= buttonX + buttonWidth &&
           mouseY >= buttonY && mouseY <= buttonY + buttonHeight;
}

bool Board::isPlayButtonClicked(int mouseX, int mouseY) {
    float buttonX = ((tileCols) * 32) - 240;
    float buttonY = (32 * (tileRows + 0.5));
    float buttonWidth = playText.getSize().x;
    float buttonHeight = playText.getSize().y;

    return mouseX >= buttonX && mouseX <= buttonX + buttonWidth &&
           mouseY >= buttonY && mouseY <= buttonY + buttonHeight;
}

void Board::updateTimer() {
    auto currentGameTime = chrono::high_resolution_clock::now();

    if(gameState == GameState::PLAYING){
        startTime += chrono::seconds(elapsedPausedTime);
        elapsedPausedTime = 0;
        auto totalGameDuration = chrono::duration_cast<chrono::seconds>(currentGameTime - startTime);
        long overallTotalTime = totalGameDuration.count();
        minutes = overallTotalTime / 60;
        seconds = overallTotalTime % 60;
    }
    else {
        elapsedPausedTime = chrono::duration_cast<chrono::seconds>(currentGameTime - pausedTime).count();
    }
}

void Board::startTimer() {
    startTime = chrono::high_resolution_clock::now();
}

void Board::drawGameTimer(RenderWindow& window) {
    // Calculate positions based on the provided layout
    int minutesX0 = (tileCols * 32) - 97;
    int minutesX1 = minutesX0 + 21;
    int secondsX0 = (tileCols * 32) - 54;
    int secondsX1 = secondsX0 + 21;
    int digitsY = 32 * (tileRows + 0.5) + 16;

    // Draw the timer digits on the window
    digitsMap[minutes / 10].setPosition(minutesX0, digitsY);
    gameWindow.draw(digitsMap[minutes / 10]);

    digitsMap[minutes % 10].setPosition(minutesX1, digitsY);
    gameWindow.draw(digitsMap[minutes % 10]);

    digitsMap[seconds / 10].setPosition(secondsX0, digitsY);
    gameWindow.draw(digitsMap[seconds / 10]);

    digitsMap[seconds % 10].setPosition(secondsX1, digitsY);
    gameWindow.draw(digitsMap[seconds % 10]);
}

void Board::drawLeaderBoard() {
    if(gameState == GameState::PLAYING){
       toggleGameStateInPlay = GameState::PLAYING;
       previousGameState = toggleGameStateInPlay;
       isGamePaused = false;
    }

    if (leaderboardWindow.isOpen()) {
        Event event;
        bool closeWindow = false;
        while (leaderboardWindow.pollEvent(event)) {
            if (event.type == Event::Closed) {
                closeWindow = true;
            }
        }
        if (closeWindow) {
            leaderboardWindow.close();
            //leaderBoardClickedBool = false;
            gameState = previousGameState;
            if(isGamePaused)
            {
                gameState = GameState::PAUSE;
            }
            else
            {
                paused = false;
            }
        }
        leaderboard.setString("LEADERBOARD");
        font.loadFromFile("files/font.ttf");
        leaderboard.setCharacterSize(20);
        leaderboard.setFillColor(Color::White);
        leaderboard.setFont(font);
        leaderboard.setStyle(Text::Underlined | Text::Bold);
        setText(leaderboard, screenWidth / 2.f, (screenHeight / 2.f) - 120);
        leaderboardWindow.clear(Color::Blue);
        leaderboardWindow.draw(leaderboard);
        leaderboardWindow.display();
    }
}

bool Board::isLeaderButtonClicked(int mouseX, int mouseY) {
    float buttonX = (tileCols * 32) - 176;
    float buttonY = (32 * (tileRows + 0.5));
    float buttonWidth = leaderText.getSize().x;
    float buttonHeight = leaderText.getSize().y;

    return mouseX >= buttonX && mouseX <= buttonX + buttonWidth &&
           mouseY >= buttonY && mouseY <= buttonY + buttonHeight;
}
//need to fix if all revealed tiles that arent mines are cleared
bool Board::checkWinCondition() {
    bool win = true;
    for (int i = 0; i < tileRows; ++i) {
        for (int j = 0; j < tileCols; ++j) {
            if (!mines[i][j] && gameBoard[i][j].getTexture() == &tileHiddenText) {
                win = false ;
                return win;
            }
        }
    }
    for (int i = 0; i < tileRows; ++i) {
        for (int j = 0; j < tileCols; ++j) {
            if (mines[i][j] && !flaggedTiles[i][j]) {
                if (win) {

                    // Set the tile as flagged
                    Sprite flagSprite(flagText);
                    flagSprite.setPosition(j * 32.f, i * 32.f);
                    flagSprites.push_back(flagSprite);
                    setTileFlagged(i, j, true);
                    mineCounter--;
                }
            }
        }
    }
    win = true;
    return win;
}

void Board::setText(Text &text, float x, float y) {
    FloatRect textRect = text.getLocalBounds();
    text.setOrigin(textRect.left + textRect.width / 2.0, textRect.top + textRect.height / 2.0f);
    text.setPosition(Vector2f(x, y));
}