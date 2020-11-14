//-----------------------------------------------------------------------------------------------------
// 1. Header Includes
//-----------------------------------------------------------------------------------------------------
#include <cstdlib>
#include <iostream> 
#include <exception>
#include <cstdio> 
#include <cmath>
#include <random>
#include <vector>
#include <string>
#include "TextureBuilder.h"
#include <glut.h>

//-----------------------------------------------------------------------------------------------------
// 2. Namespaces
//-----------------------------------------------------------------------------------------------------

using namespace std;
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

//-----------------------------------------------------------------------------------------------------
//  3. Global variables
//-----------------------------------------------------------------------------------------------------

// Direct Initialization instead of copy initialization for better performance.

// Window Details
GLsizei gWindowWidth(900); // Keep track of the window width to be used everywhere else.
GLsizei gWindowHeight(600); // Keep track of the window height to be used everywhere else.
int gWindowID(0); // GLUT window ID

// Dimensions Details
int numOfLanes(7), laneWidth(0), laneHeight(0), laneBorderHeight(0);
int verticalMazeBorderHeight(0), verticalMazeBorderWidth(0);
int horizontalMazeBorderHeight(0);
int topBarHeight(0);

vector<int> laneBordersYLocations(numOfLanes - 1), laneBordersXGaps(numOfLanes - 1), firstRectWidthVector(numOfLanes - 1), secondRectWidthVector(numOfLanes - 1);
int startingX(0);
boolean gapsDrawn = false;

// Texture Details
int backgroundRepetition = 1; // Background Image Texture Repetition
vector<GLuint> textureIDs(7); // Background Image Texture ID
vector<float> firstCircle = { 1.0, 1.0, 1.0 };
vector<float> secondCircle = { 1.0, 1.0, 1.0 };
vector<float> thirdCircle = { 1.0, 1.0, 1.0 };

// Duck translations
int duckTX(0), duckTY(0), duckWidth(50), duckHeight(50), initialX(30), initialY(20), currentX(30), currentY(20), currentLane(0);
int gapWidth(duckWidth + 10);
// Score
long score(0);
int timeLeft(90), timeInterval(14), timeLimit(90);
boolean gameFinished(false);

// Coins
boolean coinsDrawn = false;
double coinRotation(0.0);
int coinRadius(12);
vector<int> coinXLocations(numOfLanes - 1), coinYLocations(numOfLanes - 1), colorIdVector(numOfLanes - 1);
int colorSet = 0;
std::vector<bool> coinLost(numOfLanes - 1);

// Money
float moneyRadius(10.0);

//-----------------------------------------------------------------------------------------------------
// 4. Function Prototyping for the compiler
//-----------------------------------------------------------------------------------------------------

void initGLUT(int argc, char** argv);
void initGL(); // no void since this is deprecated C.
void display();
void quit(); // no void since this is deprecated C.
void animate(); // no void since this is deprecated C.
void drawRectangle(int x, int y, int w, int h);
void drawCircle(int x, int y, float r, boolean hasTexture, int id);
void drawLine(int x, int y, int width);
void drawLanes();
void drawSShape(int xTranslation, int yTranslation);
void changeColorScoreCoinExistence();
int generateRandomxGap();
void drawDuckFeet(); // no void since this is deprecated C.
void keyboard(unsigned char k, int x, int y);
void keyboardUp(unsigned char k, int x, int y);
void specialKeyboard(int k, int x, int y);
void printMessage(int x, int y, char* string);
void drawCoins(); // no void since this is deprecated C
void timer(int value);
//-----------------------------------------------------------------------------------------------------
// 5. Actual Function Declarations
//-----------------------------------------------------------------------------------------------------

int main(int argc, char** argr) {
    int RETURN_CODE(EXIT_SUCCESS);
    atexit(quit);

    try {
        initGLUT(argc, argr);
        initGL();
        glutMainLoop(); // Continue the loop until something happens.
    }
    // Catch any exceptions.
    catch (std::exception & error)
    {
        std::cerr << error.what() << std::endl;
        RETURN_CODE = EXIT_FAILURE;
    }

    quit();
    return(RETURN_CODE);
}

void initGLUT(int argc, char** argv) {
    // Step 1: Initialize the glut library.
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);

    // Step 2: Create the window.
    glutInitWindowSize(gWindowWidth, gWindowHeight);
    glutInitWindowPosition(30, 30);
    gWindowID = glutCreateWindow("Welcome to Scrooge Mcduck land!");
    if (!gWindowID) {
        printf("ERROR in File:%s, FUNCTION:%s, LINE:%d. Cannot create a GLUT windowed mode window and its OpenGL context.", __FILE__, __FUNCTION__, __LINE__);
    }

    // Step 3: Register the callback functions to be called when a specific event happens.
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);      // sets the Keyboard handler function; called when a key is pressed
    glutKeyboardUpFunc(keyboardUp);  // sets the KeyboardUp handler function; called when a key is released
    // glutMouseFunc();       // sets the Mouse handler function; called when a mouse button is clicked
    glutTimerFunc(0, timer, 0); // sets the Timer handler function; which runs every `Threshold` milliseconds (1st argument)
    glutSpecialFunc(specialKeyboard);			//call the keyboard special keys function
    // glutSpecialUpFunc(speUp);
    // glutPassiveMotionFunc(passM);
    glutIdleFunc(animate);

}

void initGL() {
    glEnable(GL_TEXTURE_2D);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    gluOrtho2D(0.0, gWindowWidth, 0.0, gWindowHeight);
    loadPPM(&textureIDs[0], "textures/scroogeMcDuck400.ppm", 400, 400, true);
    loadPPM(&textureIDs[1], "textures/sosoppm200.ppm", 200, 200, true);
    loadPPM(&textureIDs[2], "textures/totoppm200.ppm", 200, 200, true);
    loadPPM(&textureIDs[3], "textures/loloppm200.ppm", 200, 200, true);
    loadPPM(&textureIDs[4], "textures/moneyppm.ppm", 50, 50, true);
    loadPPM(&textureIDs[5], "textures/successppm.ppm", 600, 600, true);
}

void quit() {
    if (gWindowID) {
        glutDestroyWindow(gWindowID);
        gWindowID = 0;
    }
}

void display() {

    // Calculations---------------------------
    glClear(GL_COLOR_BUFFER_BIT);

    topBarHeight = (gWindowHeight / 10) - ((gWindowHeight / 10) % 10) - 10;
    horizontalMazeBorderHeight = topBarHeight / 4;
    verticalMazeBorderWidth = horizontalMazeBorderHeight;
    laneWidth = gWindowWidth - (2 * verticalMazeBorderWidth);

    if (!gameFinished) {
        // Background Image---------------------------
        glPushMatrix();
        glBindTexture(GL_TEXTURE_2D, textureIDs[0]);

        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(0, gWindowHeight, 0);
        glTexCoord2f(backgroundRepetition, 0.0f); glVertex3f(gWindowWidth, gWindowHeight, 0);
        glTexCoord2f(backgroundRepetition, backgroundRepetition); glVertex3f(gWindowWidth, 0, 0);
        glTexCoord2f(0.0f, backgroundRepetition); glVertex3f(0, 0, 0); //
        glEnd();

        glPopMatrix();

        // Height Calculations---------------------------
        // 5y + y = 6y (totalHeight allowed for lanes), where the single y is the totalHeight for lane borders, and the 5y
        // is the totalHeight for lanes.
        int totalHeight = gWindowHeight - topBarHeight - (2 * horizontalMazeBorderHeight);
        int y = totalHeight / 6;
        laneBorderHeight = y / (numOfLanes - 1);
        laneHeight = (5 * y) / (numOfLanes);
        int integerDifference = gWindowHeight - (numOfLanes * laneHeight) - ((numOfLanes - 1) * laneBorderHeight) - (2 * horizontalMazeBorderHeight) - topBarHeight;
        topBarHeight += integerDifference;
        verticalMazeBorderHeight = gWindowHeight - topBarHeight - (2 * horizontalMazeBorderHeight);

        // Maze Borders--------------------------- GL_Quad for rectangle and GL_TRIANGLE_FAN for the S shape in corner.
        glColor3f(0.0f, 0.0f, 0.0f);
        drawRectangle(0, 0, gWindowWidth, horizontalMazeBorderHeight);
        glColor3f(1.0f, 0.0f, 0.0f);
        drawRectangle(0, horizontalMazeBorderHeight, verticalMazeBorderWidth, verticalMazeBorderHeight);
        glColor3f(0.0f, 0.0f, 0.0f);
        drawRectangle((verticalMazeBorderWidth + laneWidth), horizontalMazeBorderHeight, verticalMazeBorderWidth, verticalMazeBorderHeight);
        glColor3f(1.0f, 0.0f, 0.0f);
        drawRectangle(0, (horizontalMazeBorderHeight + verticalMazeBorderHeight), gWindowWidth, horizontalMazeBorderHeight);

        glColor3f(1.0f, 1.0f, 0.0f);
        drawSShape(verticalMazeBorderWidth, 40);

        glColor3f(1.0f, 1.0f, 0.0f);
        drawSShape(verticalMazeBorderWidth, (gWindowHeight - topBarHeight) - 20);

        glColor3f(1.0f, 1.0f, 0.0f);
        drawSShape(laneWidth + verticalMazeBorderWidth, 40);

        glColor3f(1.0f, 1.0f, 0.0f);
        drawSShape(laneWidth + verticalMazeBorderWidth, (gWindowHeight - topBarHeight) - 20);

        // Top Bar---------------------------
        glColor3f(0.0f, 0.0f, 0.0f);
        drawRectangle(0, gWindowWidth - topBarHeight, gWindowWidth, topBarHeight);

        char* p0s[20], p1s[20], p2s[20];
        glColor3f(0.0f, 0.0f, 0.0f);

        int heightOfMessages = gWindowHeight - (topBarHeight / 2) - 10;
        sprintf((char*)p0s, "Score: %d", score);
        printMessage(gWindowWidth - 100, heightOfMessages, (char*)p0s);
        sprintf((char*)p1s, "Time: %d", timeLeft);
        printMessage(gWindowWidth - 200, heightOfMessages, (char*)p1s);
        sprintf((char*)p2s, "Current Lane: %d", currentLane + 1);
        printMessage(gWindowWidth - 350, heightOfMessages, (char*)p2s);

        // Lanes---------------------------
        startingX = verticalMazeBorderWidth;
        if (!gapsDrawn) drawLanes();
        else {
            glColor3f(0.365, 0.42, 0.6);
            for (int i = 0; i < numOfLanes - 1; i++) {
                drawRectangle(startingX, laneBordersYLocations[i], firstRectWidthVector[i], laneBorderHeight);
                drawRectangle(laneBordersXGaps[i] + gapWidth, laneBordersYLocations[i], secondRectWidthVector[i], laneBorderHeight);
            }
        }

        // 4 Minimum Coins
        if (!coinsDrawn) drawCoins();
        else {
            for (int i = 0; i < numOfLanes - 1; i++) {
                if (!coinLost[i]) drawCircle(coinXLocations[i], coinYLocations[i], coinRadius, false, colorIdVector[i]);
            }
        }

        // Three circles
        float radius = (topBarHeight / 2) - 5.0;
        int circleStartingX = radius + 5, circleStartingY = gWindowHeight - topBarHeight + radius + 5;
        vector<int> circleStartingXVector(3);
        for (int i = 0; i < 3; i++) {
            circleStartingXVector[i] = circleStartingX;
            circleStartingX += 2 * radius + 15;
        }

        // Border circles
        glColor3f(firstCircle[0], firstCircle[1], firstCircle[2]);
        drawCircle(circleStartingXVector[0], circleStartingY, radius + 4.0, false, 0);

        glColor3f(secondCircle[0], secondCircle[1], secondCircle[2]);
        drawCircle(circleStartingXVector[1], circleStartingY, radius + 4.0, false, 0);

        glColor3f(thirdCircle[0], thirdCircle[1], thirdCircle[2]);
        drawCircle(circleStartingXVector[2], circleStartingY, radius + 4.0, false, 0);
        glColor3f(1.0f, 1.0f, 1.0f);

        // Inner circles
        drawCircle(circleStartingXVector[0], circleStartingY, radius, true, 1);
        drawCircle(circleStartingXVector[1], circleStartingY, radius, true, 2);
        drawCircle(circleStartingXVector[2], circleStartingY, radius, true, 3);

        // Prize
        glPushMatrix();
        glBindTexture(GL_TEXTURE_2D, textureIDs[4]);
        int startPoint = (laneHeight / 2) + 20;
        int translationUp = 0;
        if (startPoint < 30) translationUp = 30 - startPoint;
        glTranslated(20, translationUp, 0);


        glBegin(GL_QUAD_STRIP);
        glTexCoord2f(0.0f, 0.0f); glVertex2f(startingX + 10, laneBordersYLocations[numOfLanes - 2] + laneBorderHeight + startPoint);
        glTexCoord2f(1.0f, 0.0f); glVertex2f(startingX + 30, laneBordersYLocations[numOfLanes - 2] + laneBorderHeight + startPoint);
        glTexCoord2f(0.0f, 1.0f); glVertex2f(startingX + 10, laneBordersYLocations[numOfLanes - 2] + laneBorderHeight + startPoint - 10);
        glTexCoord2f(1.0f, 1.0f); glVertex2f(startingX + 30, laneBordersYLocations[numOfLanes - 2] + laneBorderHeight + startPoint - 10);
        glVertex2f(startingX, laneBordersYLocations[numOfLanes - 2] + startPoint - 20);
        glVertex2f(startingX + 40, laneBordersYLocations[numOfLanes - 2] + startPoint - 20);
        glVertex2f(startingX + 10, laneBordersYLocations[numOfLanes - 2] + startPoint - 30);
        glVertex2f(startingX + 30, laneBordersYLocations[numOfLanes - 2] + startPoint - 30);
        glEnd();

        glPopMatrix();

        // Scrooge McDuck Character
        glColor3f(1.0f, 0.0f, 0.0f);
        glPushMatrix();
        glTranslated(duckTX, duckTY, 0);
        glBegin(GL_QUADS);
        glVertex2f(initialX, initialY); glVertex2f(initialX + 50, initialY); glVertex2f(initialX + 50, initialY + 50); glVertex2f(initialX, initialY + 50);
        glEnd();
        glPopMatrix();
        glColor3f(1.0f, 1.0f, 1.0f);
    }
    else {
        if (timeLeft > 0) {
            glPushMatrix();
            glBindTexture(GL_TEXTURE_2D, textureIDs[5]);

            glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 0.0f); glVertex3f(0, gWindowHeight, 0);
            glTexCoord2f(backgroundRepetition, 0.0f); glVertex3f(gWindowWidth, gWindowHeight, 0);
            glTexCoord2f(backgroundRepetition, backgroundRepetition); glVertex3f(gWindowWidth, 0, 0);
            glTexCoord2f(0.0f, backgroundRepetition); glVertex3f(0, 0, 0); //
            glEnd();

            glPopMatrix();
        }
    }
    glFlush();
}

void drawRectangle(int x, int y, int w, int h) {
    glPushMatrix();
    glBegin(GL_POLYGON);

    glVertex2f(x, y);
    glVertex2f(x + w, y);
    glVertex2f(x + w, y + h);
    glVertex2f(x, y + h);
    glEnd();
    glPopMatrix();
}

void drawLanes() {
    gapsDrawn = true;
    int startingY = horizontalMazeBorderHeight + laneHeight, xGap, xEnd;

    for (int i = 0; i < numOfLanes - 1; i++) {
        laneBordersYLocations[i] = startingY;

        xGap = generateRandomxGap(); xEnd = xGap + gapWidth; laneBordersXGaps[i] = xGap;
        secondRectWidthVector[i] = laneWidth + verticalMazeBorderWidth - xEnd;
        firstRectWidthVector[i] = laneWidth - gapWidth - secondRectWidthVector[i];


        glColor3f(0.365, 0.42, 0.6);
        drawRectangle(startingX, startingY, firstRectWidthVector[i], laneBorderHeight);
        drawRectangle(xGap + gapWidth, startingY, secondRectWidthVector[i], laneBorderHeight);
        startingY += laneHeight + laneBorderHeight;
    }

}

int generateRandomxGap() {
    return rand() % (verticalMazeBorderWidth + laneWidth - gapWidth - startingX) + startingX;
}

void drawCircle(int x, int y, float r, boolean hasTexture, int id) {
    glPushMatrix();
    glTranslated(x, y, 0);

    GLUquadric* quadObj = gluNewQuadric();
    if (hasTexture) { // Soso, toto, and lolo.
        glRotated(180, 0, 0, 1);
        gluQuadricTexture(quadObj, GL_TRUE);
        glBindTexture(GL_TEXTURE_2D, textureIDs[id]);
        gluDisk(quadObj, 0, r, 50, 50);
    }
    else {
        if (id > 0) { // Coins have color ids larger than 0.
            glRotated(coinRotation, 0, 1, 0);
            switch (id) {
            case 1: glColor3f(1.0f, 0.0f, 0.0f); break;
            case 2: glColor3f(0.0f, 1.0f, 0.0f); break;
            case 3: glColor3f(0.0f, 0.0f, 1.0f);
            }
        }

        gluDisk(quadObj, 0, r, 50, 50);
    }

    glPopMatrix();
}

void drawSShape(int xTranslation, int yTranslation)   // the filled one
{
    float twoPI = 2 * M_PI;
    glPushMatrix();
    glTranslated(xTranslation, yTranslation, 0);

    glBegin(GL_TRIANGLE_FAN);
    for (float i = M_PI; i <= twoPI + (M_PI / 4); i += 0.001)
        glVertex2f((sin(i) * moneyRadius), (cos(i) * moneyRadius));
    glEnd();

    glPushMatrix();
    glTranslated(0, -20.0, 0);
    glBegin(GL_TRIANGLE_FAN);
    for (float i = 0.0; i <= 5 * M_PI / 4; i += 0.001)
        glVertex2f((sin(i) * moneyRadius), (cos(i) * moneyRadius));
    glEnd();
    glPopMatrix();

    glBegin(GL_LINE);
    float x = (sin(M_PI) * moneyRadius), y = (cos(M_PI) * moneyRadius);
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex2f(50, 50);
    glVertex2f(50, 25);
    //  glColor3f(1.0f, 0.0f, 0.0f); glVertex2f(x + moneyRadius / 2, y + moneyRadius * 4 + 2.0);
    //  glColor3f(1.0f, 0.0f, 0.0f); glVertex2f(x + moneyRadius / 2, y);
    glEnd();

    glPopMatrix();
}



void drawCoins() {
    int coinX, coinY;
    int xDifferenceMin = startingX + coinRadius + 2, xDifferenceMax = laneWidth + startingX - coinRadius - 2 - xDifferenceMin;
    coinsDrawn = true;

    for (int i = 0; i < numOfLanes - 1; i++) {
        int yDifferenceMin, yDifferenceMax;
        coinX = rand() % xDifferenceMax + xDifferenceMin;

        if (i == 0) {
            yDifferenceMax = laneBordersYLocations[i] - (2 * coinRadius) - horizontalMazeBorderHeight - 4;
            coinY = rand() % yDifferenceMax + (horizontalMazeBorderHeight + coinRadius + 2);
        }
        else {
            yDifferenceMin = laneBordersYLocations[i - 1] + laneBorderHeight + coinRadius + 2;
            yDifferenceMax = laneBordersYLocations[i] - coinRadius - 2 - yDifferenceMin;
            coinY = rand() % yDifferenceMax + yDifferenceMin;
        }
        coinXLocations[i] = coinX; coinYLocations[i] = coinY;
        int colorId = rand() % 3 + 1; colorIdVector[i] = colorId;
        drawCircle(coinX, coinY, coinRadius, false, colorId);
    }
}

void drawDuckFeet() {
    //   glColor3f(1.0f, 0.0f, 0.0f);

    glBegin(GL_TRIANGLE_FAN);
    int halfWindow = gWindowWidth / 2, duckFeetHeight = horizontalMazeBorderHeight + 30;
    glVertex2f(halfWindow, duckFeetHeight);
    glVertex2f(halfWindow - 25, (duckFeetHeight + 30) - 8);
    glVertex2f(halfWindow - 20, (duckFeetHeight + 30) - 10.5);
    glVertex2f(halfWindow - 17, (duckFeetHeight + 30) - 17);
    glVertex2f(halfWindow - 10, (duckFeetHeight + 30) - 13.5);
    glVertex2f(halfWindow - 5, (duckFeetHeight + 30) - 16);
    glEnd();

    glColor3f(1.0f, 1.0f, 1.0f);
}
void animate() {
    coinRotation += 0.5;
    glutPostRedisplay();
}

void keyboard(unsigned char k, int x, int y)// Keyboard function, takes 3 parameters
                                       // k is the key pressed from the keyboard
                                       // x and y are mouse postion when the key was pressed.
{
    int topLeftY = currentY + duckHeight, topRightX = currentX + duckWidth;
    switch (k) {
    case 'd':
        if (topRightX + 5 <= (laneWidth + startingX)) duckTX += 5;
        else if ((laneWidth + startingX) - topRightX > 0) duckTX += (laneWidth + startingX) - topRightX;
        break;
    case 'a':
        if (currentX - 5 >= startingX) duckTX -= 5;
        else if (currentX - startingX > 0) duckTX -= currentX - startingX;
        break;
    case 'w':
        if (currentLane < numOfLanes - 1) {
            if (!(currentX >= laneBordersXGaps[currentLane] && topRightX <= laneBordersXGaps[currentLane] + gapWidth)) {
                if (topLeftY + 5 <= laneBordersYLocations[currentLane]) duckTY += 5;
                else if (laneBordersYLocations[currentLane] - topLeftY > 0) duckTY += laneBordersYLocations[currentLane] - topLeftY;
            }
            else duckTY += 5;
        }
        else {
            if (topLeftY + 5 <= (gWindowHeight - topBarHeight - horizontalMazeBorderHeight)) duckTY += 5;
            else if ((gWindowHeight - topBarHeight - horizontalMazeBorderHeight) - topLeftY > 0) duckTY += (gWindowHeight - topBarHeight - horizontalMazeBorderHeight) - topLeftY;
        }
        break;
    case 's':
        if (currentLane != 0) {
            if (currentY - 5 >= (laneBordersYLocations[currentLane - 1] + laneBorderHeight)) duckTY -= 5;
            else if (currentY - (laneBordersYLocations[currentLane - 1] + laneBorderHeight) > 0) duckTY -= currentY - (laneBordersYLocations[currentLane - 1] + laneBorderHeight);
        }
        else {
            if (currentY - 5 >= horizontalMazeBorderHeight) duckTY -= 5;
            else if (currentY - horizontalMazeBorderHeight > 0) duckTY -= currentY - horizontalMazeBorderHeight;
        }
        break;
    case 'r': secondCircle[1] = 0.0f; secondCircle[2] = 0.0f; break;
    case 'g': thirdCircle[0] = 0.0f; thirdCircle[2] = 0.0f; break;
    case 'b': firstCircle[0] = 0.0f; firstCircle[1] = 0.0f;
    }
    currentX = initialX + duckTX; currentY = initialY + duckTY;
    if (
        !coinLost[currentLane]
        && (currentX + duckWidth >= coinXLocations[currentLane] - coinRadius)
        && (currentX + duckWidth <= coinXLocations[currentLane] + coinRadius)) changeColorScoreCoinExistence();
    if (
        !coinLost[currentLane]
        && currentX <= coinXLocations[currentLane] - coinRadius
        && currentX >= coinXLocations[currentLane] + coinRadius) changeColorScoreCoinExistence();

    if (
        !coinLost[currentLane]
        && currentY <= coinYLocations[currentLane] + coinRadius
        && currentY >= coinYLocations[currentLane] - coinRadius) changeColorScoreCoinExistence();

    if (
        !coinLost[currentLane]
        && currentY + duckHeight >= coinYLocations[currentLane] - coinRadius
        && currentY + duckHeight <= coinYLocations[currentLane] + coinRadius) changeColorScoreCoinExistence();

    if (currentLane < numOfLanes - 1 && currentY > laneBordersYLocations[currentLane]) currentLane++;
    glutPostRedisplay(); // Redisplay to update the screen with the changes
}

void keyboardUp(unsigned char k, int x, int y)// Keyboard function, takes 3 parameters
                                       // k is the key pressed from the keyboard
                                       // x and y are mouse postion when the key was pressed.
{
    switch (k) {
    case 'r': secondCircle[1] = 1.0f; secondCircle[2] = 1.0f; break;
    case 'g': thirdCircle[0] = 1.0f; thirdCircle[2] = 1.0f; break;
    case 'b': firstCircle[0] = 1.0f; firstCircle[1] = 1.0f;
    }
    glutPostRedisplay(); // Redisplay to update the screen with the changes
}

void specialKeyboard(int k, int x, int y)// Keyboard special key function takes 3 parameters
                             // k is the special key pressed such as the keyboard arrows the f1,2,3 and so on
{
    int topLeftY = currentY + duckHeight, topRightX = currentX + duckWidth;
    switch (k) {
    case GLUT_KEY_RIGHT:
        if (topRightX + 5 <= (laneWidth + startingX)) duckTX += 5;
        else if ((laneWidth + startingX) - topRightX > 0) duckTX += (laneWidth + startingX) - topRightX;
        break;
    case GLUT_KEY_LEFT:
        if (currentX - 5 >= startingX) duckTX -= 5;
        else if (currentX - startingX > 0) duckTX -= currentX - startingX;
        break;
    case GLUT_KEY_UP:
        if (currentLane < numOfLanes - 1) {
            if (!(currentX >= laneBordersXGaps[currentLane] && topRightX <= laneBordersXGaps[currentLane] + gapWidth)) {
                if (topLeftY + 5 <= laneBordersYLocations[currentLane]) duckTY += 5;
                else if (laneBordersYLocations[currentLane] - topLeftY > 0) duckTY += laneBordersYLocations[currentLane] - topLeftY;
            }
            else duckTY += 5;
        }
        else {
            if (topLeftY + 5 <= (gWindowHeight - topBarHeight - horizontalMazeBorderHeight)) duckTY += 5;
            else if ((gWindowHeight - topBarHeight - horizontalMazeBorderHeight) - topLeftY > 0) duckTY += (gWindowHeight - topBarHeight - horizontalMazeBorderHeight) - topLeftY;
        }
        break;
    case GLUT_KEY_DOWN:
        if (currentLane != 0) {
            if (currentY - 5 >= (laneBordersYLocations[currentLane - 1] + laneBorderHeight)) duckTY -= 5;
            else if (currentY - (laneBordersYLocations[currentLane - 1] + laneBorderHeight) > 0) duckTY -= currentY - (laneBordersYLocations[currentLane - 1] + laneBorderHeight);
        }
        else {
            if (currentY - 5 >= horizontalMazeBorderHeight) duckTY -= 5;
            else if (currentY - horizontalMazeBorderHeight > 0) duckTY -= currentY - horizontalMazeBorderHeight;
        }
    }

    currentX = initialX + duckTX; currentY = initialY + duckTY;
    if (currentLane != numOfLanes - 1) {
        if (currentY > (laneBordersYLocations[currentLane] + laneBorderHeight)) currentLane++; printf("Current Lane: %d\n", currentLane);
        if (currentLane != numOfLanes - 1) {
            if (
                !coinLost[currentLane]
                && (currentX + duckWidth >= coinXLocations[currentLane] - coinRadius)
                && (currentX + duckWidth <= coinXLocations[currentLane] + coinRadius)) {
                changeColorScoreCoinExistence();
            }
            else if (
                !coinLost[currentLane]
                && (currentX <= coinXLocations[currentLane] - coinRadius)
                && (currentX >= coinXLocations[currentLane] + coinRadius)) {
                changeColorScoreCoinExistence();
            }
        }
        else {
            if ((currentX >= startingX && currentX <= startingX + 40)) gameFinished = true;
        }

    }
    else {
        if ((currentX >= startingX && currentX <= startingX + 40)) gameFinished = true;
    }

    glutPostRedisplay(); // Redisplay to update the screen with the changes
}

void changeColorScoreCoinExistence() {
    switch (colorSet) {
    case 1: firstCircle[2] = 0.0f; break;
    case 2: secondCircle[0] = 0.0f; break;
    case 3: thirdCircle[1] = 0.0f;
    }
    switch (colorIdVector[currentLane]) {
    case 1: secondCircle[0] = 1.0f; colorSet = 2; break;
    case 2: thirdCircle[1] = 1.0; colorSet = 3; break;
    case 3: firstCircle[2] = 1.0f; colorSet = 1;
    }
    score += 20;
    coinLost[currentLane] = true;
}

void drawLine(int x, int y, int width) {
    glPushMatrix();
    glBegin(GL_LINE);
    glVertex2f(x, y);
    glVertex2f(x + width, y);
    glEnd();
    glPopMatrix();
}


// The first two are the coordinates where the text will get displayed.
// The third coordinate is the string containing the text to display
void printMessage(int x, int y, char* string)
{
    int len, i;
    glRasterPos2f(x, y); // Set the position of the text in the window using the x and y coordinates
    len = (int)strlen(string); // get the length of the string to display

   //  Loop to display character by character
    for (i = 0; i < len; i++)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, string[i]);
    }
}

void timer(int value) {
    // Ask OpenGL to recall the display function to reflect the changes on the window
    if (timeLeft > 0) { // Change gap coordinates every time interval that decreases as well.
        timeLeft--;
        if (timeLimit - timeLeft == timeInterval) {
            timeLimit -= timeInterval;
            timeInterval--;
            int xGap, xEnd;
            for (int i = 0; i < numOfLanes - 1; i++) {
                xGap = generateRandomxGap(); laneBordersXGaps[i] = xGap; xEnd = xGap + duckWidth;
                secondRectWidthVector[i] = laneWidth + verticalMazeBorderWidth - xEnd;
                firstRectWidthVector[i] = laneWidth - duckWidth - secondRectWidthVector[i];
            }
        }
        glutPostRedisplay();
        glutTimerFunc(1000, timer, 0);
    }
    else {
        gameFinished = true;
        glutTimerFunc(0, NULL, 0);
    }
}



