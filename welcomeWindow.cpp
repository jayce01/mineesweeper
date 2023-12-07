#include "welcomeWindow.h"
#include "board.h"

WelcomeWindow::WelcomeWindow() {
    getScreenSize();
    window.create(VideoMode(screenWidth, screenHeight), "Minesweeper", Style::Close);
    if (!font.loadFromFile("files/font.ttf")) {
        cout << "Font not loaded" << endl; // Handle font loading failure
    }

    welcomeText.setString("WELCOME TO MINESWEEPER!");
    welcomeText.setFont(font);
    welcomeText.setCharacterSize(24);
    welcomeText.setFillColor(Color::White);
    welcomeText.setStyle(Text::Underlined | Text::Bold);
    setText(welcomeText, screenWidth / 2.f, (screenHeight / 2.f) - 150);

    name.setString("enter your name:");
    name.setFont(font);
    name.setCharacterSize(20);
    name.setFillColor(Color::White);
    name.setStyle(Text::Bold);
    setText(name, screenWidth / 2.f, (screenHeight / 2.f) - 75);

    nameText.setString("");
    nameText.setFont(font);
    nameText.setCharacterSize(18);
    nameText.setFillColor(Color::Yellow);
    nameText.setStyle(Text::Bold);
    setText(nameText, screenWidth / 2.f, (screenHeight / 2.f) - 45);

    cursor.setSize(Vector2f(2, 18));
    cursor.setFillColor(Color::White);
    cursorVisible = true;
    cursorClock.restart();
}

void WelcomeWindow::getScreenSize() {
    ifstream configFile("config.cfg");
    if(configFile.is_open()) {
        configFile >> cols >> rows;
        configFile.close();
    }
    screenHeight = (rows * 32.f) + 100;
    screenWidth = (cols * 32.f);
}

void WelcomeWindow::run() {
    while(window.isOpen()) {
        handleEvents();
        update();
        draw();
        openGameWindow();
    }
}

string WelcomeWindow::getEnteredName() {
    return nameInput;
}

void WelcomeWindow::handleEvents(){
    Event event;
    while (window.pollEvent(event)) {
        if (event.type == Event::Closed) {
            window.close();
        }
        else if (event.type == Event::TextEntered) {
            if (event.text.unicode < 128) {
                char enteredChar = static_cast<char>(event.text.unicode);
                if (isalpha(enteredChar) || enteredChar == '\b') {
                    if (enteredChar == '\b' && !nameInput.empty()) {
                        nameInput.pop_back();
                        nameText.setString(nameInput);
                    }
                    else if (enteredChar != '\b') {
                        if (nameInput.size() < 10) {
                            if (nameInput.empty()) {
                                enteredChar = toupper(enteredChar);
                            }
                            else {
                                enteredChar = tolower(enteredChar);
                            }
                            nameInput += enteredChar;
                            nameText.setString(nameInput);
                        }
                    }
                }
            }
        }
    }
}

void WelcomeWindow::update() {
    if (cursorClock.getElapsedTime().asSeconds() > 0.5) {
        cursorVisible = !cursorVisible;
        cursorClock.restart();
    }
    float textPosition = nameText.getLocalBounds().width;
    float centerDifference = (screenWidth - textPosition) / 2;
    nameText.setPosition(centerDifference, nameText.getPosition().y);
}

void WelcomeWindow::setText(Text &text, float x, float y) {
    FloatRect textRect = text.getLocalBounds();
    text.setOrigin(textRect.left + textRect.width/2.0,textRect.top + textRect.height/2.0f);
    text.setPosition(Vector2f(x, y));
}

void WelcomeWindow::draw() {
    window.clear(Color::Blue);
    window.draw(welcomeText);
    window.draw(name);
    window.draw(nameText);

    if (cursorVisible) {
        cursor.setPosition(nameText.getPosition().x + nameText.getLocalBounds().width, (screenHeight / 2.f) - 45);
        window.draw(cursor);
    }
    window.display();
}

void WelcomeWindow::openGameWindow() {
    if (Keyboard::isKeyPressed(Keyboard::Enter) && !nameInput.empty() && !gameWindow.isOpen()) {
        gameWindow.create(VideoMode(screenWidth, screenHeight), "Minesweeper", Style::Close);
        Board board(gameWindow);
        window.close();
        board.startTimer();
        while (gameWindow.isOpen()){
            gameWindow.clear(Color::White);
            if(board.checkWinCondition()){
                board.gameState = Board::GameState::WIN;
            }
            board.updateTimer();
            board.handleInput();
            board.draw();
            Event gameEvent;
            while (gameWindow.pollEvent(gameEvent)) {
                if (gameEvent.type == Event::Closed) {
                    gameWindow.close();
                }
            }
            gameWindow.display();
        }
    }
}

