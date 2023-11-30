#ifndef WELCOME_WINDOW_H
#define WELCOME_WINDOW_H

#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>
#include <fstream>

using namespace std;
using namespace sf;

class WelcomeWindow {
    public:
        WelcomeWindow();
        void run();
        string getEnteredName();

    private:
        RenderWindow window;
        RenderWindow gameWindow;
        Font font;
        Text welcomeText;
        Text name;
        Text nameText;
        RectangleShape cursor;

        string nameInput;
        bool cursorVisible;
        Clock cursorClock;
        float screenWidth;
        float screenHeight;
        int rows;
        int cols;

        void handleEvents();
        void update();
        void draw();
        void openGameWindow();
        void getScreenSize();
        void setText(Text &text, float x, float y);
};

#endif // WELCOME_WINDOW_H