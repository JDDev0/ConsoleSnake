#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <inttypes.h>
#include "consoleLib.h"
#include "consoleMenu.h"
#include "gameField.h"
#include "helpMenu.h"

#define VERSION "1.0.1"

void resetGame(void);
void init(void);
void update(void);
void updateKey(int);
void updateMouse(void);
void updateSnake(void);
void createFood(void);
void drawField(void);
void drawSettings(void);
void drawStartMenu(void);
void setMap(int map);
void initVars(void);
int64_t calcCheckSum(int64_t highScore);
void readHighScore(void);
void saveHighScore(void);

//Set funcPtr to drawField after START_MENU
static void (*draw)(void);
static void (*drawOld)(void);

static int isHelp;

static int escCheck;

static const int gameMinWidth = 74;
static const int gameMinHeight = 23;

static enum {
    START_MENU, SETTINGS, IN_GAME, GAME_OVER
}screen;

static int map;
static const int mapCount = 5;
static struct field maps[5];
static struct field gameField;

static double vSnake = 0; //Unit/s
static double timerSnake = 0;
static int timerInsect = 0;
static double moveAfterTime;
static int blockInput; //Wait for direction change before next key
static int notGameOverAdditionalReactionMove; //If the player will have one additonal move to prevent game over

//Start menu
static struct menuComponent startMenu;
static char *entriesStartMenu[] = {
    "Play", "Settings"
};

//Settings
static struct menuComponent settingsNumVStart;
static struct menuComponent settingsNumVAddFactor;
static struct menuComponent settingsNumCountOfFood;
static struct tabOrder settingsTabOrder;
static int fCNS[] = {
    CL_COLOR_WHITE, CL_COLOR_WHITE
};
static int fCS[] = {
    CL_COLOR_CYAN, CL_COLOR_LIGHT_RED
};
static int bC[] = {
    CL_COLOR_NO_COLOR, CL_COLOR_NO_COLOR
};
static double vStart = 2.;
static double vAddFactor = 1.;
static int countOfFood = 1;

static int64_t score;
static int64_t highScore;
static FILE *highScoreFile;

static unsigned int fruitCount;

void resetGame(void) {
    reset();
    freeTableOfContents(&content);

    freeMenu(&startMenu);
    freeNumField(&settingsNumVStart);
    freeNumField(&settingsNumVAddFactor);
    freeNumField(&settingsNumCountOfFood);
    freeTabOrder(&settingsTabOrder);

    removeField(&gameField);
    for(int i = 0;i < mapCount;i++)
        removeField(maps + i);

    if(highScoreFile != NULL)
        fclose(highScoreFile);
}

int main(void) {
    init();

    while(1) {
        update();
        sleepMS(40);
    }

    return EXIT_SUCCESS;
}

void init(void) {
    initConsole();

    srand((unsigned)time(NULL));

    int widthCon, heightCon;
    getConsoleSize(&widthCon, &heightCon);
    if(widthCon < gameMinWidth || heightCon < gameMinHeight) {
        reset();
        printf("Console is to small (Min: %d x %d)!\n", gameMinWidth, gameMinHeight);

        exit(EXIT_FAILURE);
    }

    //High score
    readHighScore();

    //Add reset after console size check
    atexit(resetGame);

    //Menus
    initNumField(&settingsNumVStart, 0, 1, 15, (int)vStart, 1, 10, CL_COLOR_CYAN,
    CL_COLOR_NO_COLOR);
    initNumField(&settingsNumVAddFactor, 0, 3, 15, (int)vAddFactor*10, 5, 20,
    CL_COLOR_WHITE, CL_COLOR_NO_COLOR);
    settingsNumVAddFactor.isSelected = 0;
    initNumField(&settingsNumCountOfFood, 0, 5, 15, (int)countOfFood, 1, 10,
    CL_COLOR_WHITE, CL_COLOR_NO_COLOR);
    settingsNumCountOfFood.isSelected = 0;
    initTabOrder(&settingsTabOrder, 2, fCNS, bC, fCS, bC);
    addToTabOrder(&settingsTabOrder, &settingsNumVStart);
    addToTabOrder(&settingsTabOrder, &settingsNumVAddFactor);
    addToTabOrder(&settingsTabOrder, &settingsNumCountOfFood);

    initMenu(&startMenu, entriesStartMenu, 2, 0, 15, gameMinWidth, 2, CM_TA_CENTER,
    CM_SC_ARROW_LEFT_RIGHT, CL_COLOR_WHITE, CL_COLOR_NO_COLOR, CL_COLOR_CYAN,
    CL_COLOR_NO_COLOR);

    //Help menu
    initHelpMenu(gameMinWidth, gameMinHeight - 4);

    //Fields
    initField(&gameField, gameMinWidth, gameMinHeight - 1);
    for(int i = 0; i < mapCount;i++)
        initField(maps + i, gameMinWidth, gameMinHeight - 1);
    //Wall
    for(int i = 1;i < 5;i++) {
        for(int j = 0;j < maps[i].width;j++)
            maps[i].field[j][0] = maps[i].field[j][maps[i].height - 1] = WALL;
        if(i != 3)
            for(int j = 1;j < maps[i].height - 1;j++)
                maps[i].field[0][j] = maps[i].field[maps[i].width - 1][j] = WALL;
    }
    for(int i = 20;i < 51;i++)
        maps[2].field[i][8] = maps[2].field[i][12] = WALL;
    for(int i = 8;i < 15;i++)
        maps[3].field[20][i] = maps[3].field[50][i] = maps[4].field[20][i] = maps[4].field[50][i] = WALL;
    for(int i = 20;i < 51;i++)
        maps[4].field[i][7] = WALL;
    //Remove Walls
    for(int i = 9;i < 11;i++)
        maps[4].field[0][i] = maps[4].field[maps[4].width - 1][i] = EMPTY;
    //Snake
    for(int i = 0;i < mapCount;i++) {
        maps[i].snakeSize = 3;
        maps[i].snakeDirection = RIGHT;
        for(int j = 0;j < 3;j++) {
            maps[i].field[32 - j][10] = SNAKE;
            maps[i].snakePos[j][0] = 32 - j;
            maps[i].snakePos[j][1] = 10;
        }
    }

    initVars();
}

void update(void) {
    //Input
    if(hasInput()) { //If has key input: call updateKey
        updateKey(getKey());
    }
    updateMouse();

    //Snake
    updateSnake();

    //Graphics
    clrscr();
    draw();
}

void updateKey(int key) {
    int tmp;

    //Help
    if(isHelp && key == CL_KEY_ESC) {
        isHelp = 0;
        draw = drawOld;

        return;
    }
    //Block other inputs if [y]es/[n]o
    if(escCheck) {
        if(tolower(key) == 'y') {
            if(screen == START_MENU) {
                //Exit game
                exit(EXIT_SUCCESS);
            }else {
                if(score > highScore) {
                    highScore = score;
                    saveHighScore();
                }

                //Reset game (to start menu)
                initVars();
            }
        }else if(tolower(key) == 'n') {
            escCheck = 0;
        }
    }else {
        //Help
        if(key == CL_KEY_F1) {
            isHelp = !isHelp;
            if(isHelp) {
                drawOld = draw;
                draw = drawHelp;
            }else {
                draw = drawOld;
            }

            return;
        }
        if(isHelp) {
            if(key == CL_KEY_UP) {
                helpPage--;
                if(helpPage < 0) {
                    helpPage = maxHelpPage;
                }
            }else if(key == CL_KEY_DOWN) {
                helpPage++;
                if(helpPage > maxHelpPage) {
                    helpPage = 0;
                }
            }

            return;
        }

        switch(screen) {
            case START_MENU:
                switch(updateKeyMenu(&startMenu, key)) {
                    case 0:
                        initVars();

                        screen = IN_GAME;

                        //Set new draw function
                        draw = drawField;
                        break;
                    case 1:
                        screen = SETTINGS;
                        draw = drawSettings;
                        break;
                }
                break;
            case SETTINGS:
                if(updateKeyNumField(&settingsNumVStart, key, &tmp)) {
                    vStart = (double)tmp;
                }
                if(updateKeyNumField(&settingsNumVAddFactor, key, &tmp)) {
                    vAddFactor = (double)tmp/10.;
                }
                updateKeyNumField(&settingsNumCountOfFood, key, &countOfFood);
                updateKeyTabOrder(&settingsTabOrder, key);
                break;
            case IN_GAME:
                if(!blockInput) {
                    switch(key) {
                        case CL_KEY_LEFT:
                            if(gameField.snakeDirection%2) {
                                gameField.snakeDirection = LEFT;
                                blockInput = 1;
                            }
                            break;
                        case CL_KEY_UP:
                            if(!(gameField.snakeDirection%2)) {
                                gameField.snakeDirection = UP;
                                blockInput = 1;
                            }
                            break;
                        case CL_KEY_RIGHT:
                            if(gameField.snakeDirection%2) {
                                gameField.snakeDirection = RIGHT;
                                blockInput = 1;
                            }
                            break;
                        case CL_KEY_DOWN:
                            if(!(gameField.snakeDirection%2)) {
                                gameField.snakeDirection = DOWN;
                                blockInput = 1;
                            }
                            break;
                    }
                }
                break;
            case GAME_OVER:
                break;
        }

        //Exit game
        if(key == CL_KEY_ESC) {
            if(screen == SETTINGS) {
                screen = START_MENU;
                draw = drawStartMenu;

                return;
            }
            escCheck = 1;
        }
    }
}
void updateMouse(void) {
    int column, row;
    getMousePosClicked(&column, &row);

    if(column == -1) //No input
        return;

    //Help
    if(isHelp) {
        //"row - 2": Content start at row 2
        int tmpPage = getPageMouseClicked(&content, helpPage, row - 2);
        if(tmpPage != -1)
            helpPage = tmpPage;

        if(row == 22 && column < 8)
            updateKey(CL_KEY_DOWN);

        return;
    }

    if(escCheck) {
        if(row == 13) {
            if(screen == START_MENU) {
                if(column > 28 && column < 34)
                    updateKey('y');
                else if(column > 40 && column < 45)
                    updateKey('n');
            }else {
                if(column > 26 && column < 32)
                    updateKey('y');
                else if(column > 41 && column < 46)
                    updateKey('n');
            }
        }
    }else {
        switch(screen) {
            case START_MENU:
                switch(updateMouseMenu(&startMenu, column, row)) {
                    case 0:
                        initVars();

                        screen = IN_GAME;

                        //Set new draw function
                        draw = drawField;
                        break;
                    case 1:
                        screen = SETTINGS;
                        draw = drawSettings;
                        break;
                }
                if(row == 21 && column > 64 && column < 73)
                    updateKey(CL_KEY_F1);

                break;
            case SETTINGS:
                updateMouseNumField(&settingsNumVStart, column, row);
                updateMouseNumField(&settingsNumVAddFactor, column, row);
                updateMouseNumField(&settingsNumCountOfFood, column, row);
                break;
            case IN_GAME:
            case GAME_OVER:
                break;
        }
    }
}
void updateSnake(void) {
    enum fieldIDs tmpField = EMPTY;
    int oldPosX = 0, oldPosY = 0, tmp;
    int drawHeadAgain = 0;
    enum direction moveInsect;

    if(screen != IN_GAME || escCheck || draw != drawField)
        return;

    if(gameField.insectPos[0] != -1) {
        timerInsect++;

        if(timerInsect == 12) {
            timerInsect = 0;

            int alreadyCheck = 0;
			do {
				oldPosX = gameField.insectPos[0];
				oldPosY = gameField.insectPos[1];

				//Everything was checked but no EMPTY field -> no movement
				if(alreadyCheck == 15)
					break;

				moveInsect = rand()%4;

				switch(moveInsect) {
					case LEFT:
						oldPosX--;
						if(oldPosX < 0)
							oldPosX = gameField.width - 1;

						alreadyCheck |= 1;
						break;
					case UP:
						oldPosY--;
						if(oldPosY < 0)
							oldPosY = gameField.height - 1;

						alreadyCheck |= 2;
						break;
					case RIGHT:
						oldPosX++;
						if(oldPosX >= gameField.width)
							oldPosX = 0;

						alreadyCheck |= 4;
						break;
					case DOWN:
						oldPosY++;
						if(oldPosY >= gameField.height)
							oldPosY = 0;

						alreadyCheck |= 8;
						break;
				}
			}while(gameField.field[oldPosX][oldPosY] != EMPTY);
            gameField.field[gameField.insectPos[0]][gameField.insectPos[1]] = EMPTY;
            gameField.field[oldPosX][oldPosY] = INSECT;
            gameField.insectPos[0] = oldPosX;
            gameField.insectPos[1] = oldPosY;
        }
    }

    timerSnake -= 40.;
    if(timerSnake < 0) {
        timerSnake += moveAfterTime;

        blockInput = 0;

        switch(gameField.snakeDirection) {
            case LEFT:
                oldPosX = gameField.snakePos[0][0];
                oldPosY = gameField.snakePos[0][1];
                gameField.field[oldPosX][oldPosY] = EMPTY;

                tmp = oldPosX - 1;
                if(tmp < 0)
                    tmp = gameField.width - 1;
                tmpField = gameField.field[tmp][oldPosY];

                gameField.snakePos[0][0] = tmp;
                gameField.field[tmp][oldPosY] = SNAKE;
                break;
            case UP:
                oldPosX = gameField.snakePos[0][0];
                oldPosY = gameField.snakePos[0][1];
                gameField.field[oldPosX][oldPosY] = EMPTY;

                tmp = oldPosY - 1;
                if(tmp < 0)
                    tmp = gameField.height - 1;
                tmpField = gameField.field[oldPosX][tmp];

                gameField.snakePos[0][1] = tmp;
                gameField.field[oldPosX][tmp] = SNAKE;
                break;
            case RIGHT:
                oldPosX = gameField.snakePos[0][0];
                oldPosY = gameField.snakePos[0][1];
                gameField.field[oldPosX][oldPosY] = EMPTY;

                tmp = oldPosX + 1;
                if(tmp >= gameField.width)
                    tmp = 0;
                tmpField = gameField.field[tmp][oldPosY];

                gameField.snakePos[0][0] = tmp;
                gameField.field[tmp][oldPosY] = SNAKE;
                break;
            case DOWN:
                oldPosX = gameField.snakePos[0][0];
                oldPosY = gameField.snakePos[0][1];
                gameField.field[oldPosX][oldPosY] = EMPTY;

                tmp = oldPosY + 1;
                if(tmp >= gameField.height)
                    tmp = 0;
                tmpField = gameField.field[oldPosX][tmp];

                gameField.snakePos[0][1] = tmp;
                gameField.field[oldPosX][tmp] = SNAKE;
                break;
        }

        switch(tmpField) {
            case SNAKE:
                //Break if snake "eats" snake tail
                if(gameField.snakePos[0][0] == gameField.
                snakePos[gameField.snakeSize - 1][0] && gameField.snakePos[0][1] ==
                gameField.snakePos[gameField.snakeSize - 1][1]) {
                    drawHeadAgain = 1;
                    break;
                }
                /* no break */
            case WALL:
                if(notGameOverAdditionalReactionMove) {
                    screen = GAME_OVER;

                    if(score > highScore) {
                        highScore = score;
                        saveHighScore();
                    }

                    escCheck = 1;
                }else {
                    if(moveAfterTime < 30)
                        moveAfterTime = 30;
                    timerSnake = moveAfterTime;
                }
                notGameOverAdditionalReactionMove = 1;

                //No movement -> Reset snake head
                gameField.field[gameField.snakePos[0][0]]
                [gameField.snakePos[0][1]] = tmpField;
                gameField.field[oldPosX][oldPosY] = SNAKE;
                gameField.snakePos[0][0] = oldPosX;
                gameField.snakePos[0][1] = oldPosY;
                return;
            case EMPTY:
            case APPLE:
            case BLUEBERRY:
            case INSECT:
                break;
        }
        notGameOverAdditionalReactionMove = 0;

        //Move snake
        for(int i = 1;i < gameField.snakeSize;i++) {
            //Move snake in snakePos
            tmp = gameField.snakePos[i][0];
            gameField.snakePos[i][0] = oldPosX;
            oldPosX = tmp;
            tmp = gameField.snakePos[i][1];
            gameField.snakePos[i][1] = oldPosY;
            oldPosY = tmp;

            //Move snake in field
            gameField.field[oldPosX][oldPosY] = EMPTY;
            gameField.field[gameField.snakePos[i][0]]
            [gameField.snakePos[i][1]] = SNAKE;
        }

        if(drawHeadAgain)
            gameField.field[gameField.snakePos[0][0]][gameField.snakePos[0][1]] = SNAKE;

        switch(tmpField) {
            case INSECT:
                score += 30; //Will be += 50
                timerInsect = 0;
                gameField.insectPos[0] = -1;
                gameField.insectPos[1] = -1;
                /* no break */
            case BLUEBERRY:
                score += 10; //Wil be += 20
                /* no break */
            case APPLE:
                //oldPosX, oldPosY: Pos for new body of snake
                gameField.field[oldPosX][oldPosY] = SNAKE;
                gameField.snakePos[gameField.snakeSize][0] = oldPosX;
                gameField.snakePos[gameField.snakeSize][1] = oldPosY;
                gameField.snakeSize++;

                createFood();

                score += 10;
                if(score > (int64_t)9999999999)
                    score = (int64_t)9999999999;

                if(fruitCount > (unsigned int)(10 * (map/mapCount + 1))) {
                    fruitCount -= (unsigned int)(10 * (map/mapCount + 1));
                    map++;
                    if(map > 399)
                        map = 399;
                    setMap(map%mapCount);

                    break;
                }
                fruitCount++;

                vSnake += .6/(vSnake + .1) * ((double)map/50. + 1.) * vAddFactor;
                moveAfterTime = 1000./vSnake;
                if(moveAfterTime < 30)
                    moveAfterTime = 30;
                timerSnake = moveAfterTime;
                break;
            case EMPTY:
            case WALL:
            case SNAKE:
                break;
        }
    }
}
void createFood(void) {
    int food = rand()%100;
    int posX, posY;

    do {
        posX = rand()%gameMinWidth;
        posY = rand()%gameMinHeight;
    }while(gameField.field[posX][posY] != EMPTY);

    if(food < 60) {
        //Apple
        gameField.field[posX][posY] = APPLE;
    }else if(food < 90 || gameField.insectPos[0] != -1) {
        //Blueberry
        gameField.field[posX][posY] = BLUEBERRY;
    }else {
        //Insect
        gameField.field[posX][posY] = INSECT;
        gameField.insectPos[0] = posX;
        gameField.insectPos[1] = posY;
    }
}

void drawField(void) {
    setColor(CL_COLOR_WHITE, CL_COLOR_NO_COLOR);
    drawf("Score: %05d%05d\n", (int)(score/100000L), (int)(score%100000L));

    setCursorPos(gameMinWidth - 22, 0);
    drawf("High score: %05d%05d\n", (int)(highScore/100000L),
    (int)(highScore%100000L));

    setCursorPos(gameMinWidth/2 - 8, 0);
    drawf("Level: %03d\n", map + 1);

    for(int i = 0;i < gameField.height;i++) {
        for(int j = 0;j < gameField.width;j++) {
            drawf("%c", getCharFromField(&gameField, j, i));
        }
        drawf("\n");
    }

    //Exit
    if(escCheck) {
        setColor(CL_COLOR_BLACK, CL_COLOR_YELLOW);
        setCursorPos(27, 10);
        if(screen == GAME_OVER)
            drawf("    Game over!     ");
        else
            drawf("Back to start menu?");
        setCursorPos(27, 11);
        if(screen == GAME_OVER)
            drawf("Back to start menu?");
        else
            drawf("-------------------");
        setCursorPos(27, 12);
        if(screen == GAME_OVER)
            drawf("-------------------");
        else
            drawf("                   ");
        setCursorPos(27, 13);
        drawf("[y]es          [n]o");

        //Draw border
        setColor(CL_COLOR_LIGHT_BLACK, CL_COLOR_RED);
        setCursorPos(26, 9);
        drawf("                     ");
        setCursorPos(26, 14);
        drawf("                     ");
        for(int i = 10;i < 14;i++) {
            setCursorPos(26, i);
            drawf(" ");
            setCursorPos(46, i);
            drawf(" ");
        }
    }
}
void drawSettings(void) {
    resetColor();
    drawf("Start speed of snake:");
    drawNumField(&settingsNumVStart);

    resetColor();
    setCursorPos(0, 2);
    drawf("Speed increase factor:");
    drawNumField(&settingsNumVAddFactor);

    resetColor();
    setCursorPos(0, 4);
    drawf("Count of food at once:");
    drawNumField(&settingsNumCountOfFood);
}
void drawStartMenu(void) {
    //Draw border (top)
    setColor(CL_COLOR_WHITE, CL_COLOR_BLUE);
    drawf("/--------------------------------------------------------------------"
          "----\\\n");

    //Draw text
    setColor(CL_COLOR_LIGHT_YELLOW, CL_COLOR_NO_COLOR);
    setCursorPos(19, 1);
    drawf("-------------------------------------");
    setCursorPos(19, 2);
    drawf(".----   .   .   .---.   |  ./   .----\n");
    setCursorPos(19, 3);
    drawf("|       |\\  |   |   |   | /'    |\n");
    setCursorPos(19, 4);
    drawf("'---.   | \\ |   +---+   :{      +----\n");
    setCursorPos(23, 5);
    drawf("|   |  \\|   |   |   | \\.    |\n");
    setCursorPos(19, 6);
    drawf("----'   '   '   |   |   |  '\\   '----\n");
    setCursorPos(19, 7);
    drawf("-------------------------------------");
    setCursorPos(1, 13);
    drawf("-----------------------------------------------------------------------"
    "-");

    //Draw infos
    resetColor();
    char verStr[70]; //69 chars + '\0'
    sprintf(verStr, "Version: " VERSION);
    setCursorPos(71 - (int)strlen(verStr), 14); //69 chars, (3 chars empty)
    drawf("%s", verStr);

    drawMenu(&startMenu);

    resetColor();
    setCursorPos(1, 21);
    drawf("By ");
    setColor(CL_COLOR_NO_COLOR, CL_COLOR_YELLOW);
    drawf("JDDev0");

    resetColor();
    setCursorPos(65, 21);
    drawf("Help: ");
    setColor(CL_COLOR_LIGHT_RED, CL_COLOR_NO_COLOR);
    drawf("F1");

    //Draw border
    setColor(CL_COLOR_WHITE, CL_COLOR_BLUE);
    for(int i = 1;i < 22;i++) {
        setCursorPos(0, i);
        drawf("|");
        setCursorPos(73, i);
        drawf("|");
    }
    drawf("\n\\-----------------------------------------------------------------"
          "-------/");

    //Exit
    if(escCheck) {
        setColor(CL_COLOR_BLACK, CL_COLOR_YELLOW);
        setCursorPos(29, 10);
        drawf("   Exit game?   ");
        setCursorPos(29, 11);
        drawf("   ----------   ");
        setCursorPos(29, 12);
        drawf("                ");
        setCursorPos(29, 13);
        drawf("[y]es       [n]o");

        //Draw border
        setColor(CL_COLOR_LIGHT_BLACK, CL_COLOR_RED);
        setCursorPos(28, 9);
        drawf("                  ");
        setCursorPos(28, 14);
        drawf("                  ");
        for(int i = 10;i < 14;i++) {
            setCursorPos(28, i);
            drawf(" ");
            setCursorPos(45, i);
            drawf(" ");
        }
    }
}

void setMap(int map) {
    gameField.insectPos[0] = -1;
    gameField.insectPos[1] = -1;
    timerInsect = 0;
    vSnake = vStart;
    timerSnake = moveAfterTime = 1000./vSnake;
    blockInput = 0;

    for(int i = 0;i < gameField.width;i++)
        for(int j = 0;j < gameField.height;j++)
            gameField.field[i][j] = maps[map].field[i][j];
    gameField.snakeSize = maps[map].snakeSize;
    gameField.snakeDirection = maps[map].snakeDirection;
    for(int i = 0;i < maps[map].snakeSize;i++) {
        gameField.snakePos[i][0] = maps[map].snakePos[i][0];
        gameField.snakePos[i][1] = maps[map].snakePos[i][1];
    }

    for(int i = 0;i < countOfFood;i++)
        createFood();
}
void initVars(void) {
    //Fields
    map = 0;
    notGameOverAdditionalReactionMove = 0;
    setMap(map);

    //Help
    isHelp = 0;

    //Function pointer
    drawOld = draw = drawStartMenu;
    screen = START_MENU;

    fruitCount = score = 0;

    escCheck = 0;
}

int64_t calcCheckSum(int64_t highScore) {
    int64_t tmp = 42;
    while(highScore > 17) {
        highScore /= 5;
        tmp += highScore * 2;
        highScore -= 3;
    }

    tmp -= highScore/3;

    return tmp % 0x1000000;
}
void readHighScore(void) {
    int64_t checkSum;
    int64_t highScoreAdd;
    int checkSumTmp[3];
    int highScoreTmp[2];
    highScoreFile = fopen("score.data", "r+");
    if(highScoreFile == NULL) {
        reset();
        printf("Can't read score file!\n");

        exit(EXIT_FAILURE);
    }

    fscanf(highScoreFile, "%02x%05d%02x%05d%02x", checkSumTmp, highScoreTmp + 1,
    checkSumTmp + 2, highScoreTmp, checkSumTmp + 1);

    checkSum = ((int64_t)checkSumTmp[2] << 16) + ((int64_t)checkSumTmp[1] << 8) +
    (int64_t)checkSumTmp[0];
    highScoreAdd = (int64_t)highScoreTmp[1] * 100000L + (int64_t)highScoreTmp[0];

    if(calcCheckSum(highScoreAdd) != checkSum)
        highScore = 0;
    else
        highScore = highScoreAdd;
}
void saveHighScore(void) {
    int64_t checkSum = calcCheckSum(highScore);
    int checkSumTmp[3];
    int highScoreTmp[2];

    checkSumTmp[0] = checkSum & 0xFF;
    checkSumTmp[1] = (checkSum >> 8) & 0xFF;
    checkSumTmp[2] = (checkSum >> 16) & 0xFF;

    highScoreTmp[0] = (int)(highScore%100000L);
    highScoreTmp[1] = (int)(highScore/100000L);

    if(highScoreFile != NULL) {
        //Go to start of file
        rewind(highScoreFile);

        fprintf(highScoreFile, "%02x%05d%02x%05d%02x", checkSumTmp[0],
        highScoreTmp[1], checkSumTmp[2], highScoreTmp[0], checkSumTmp[1]);

        fflush(highScoreFile);
    }else {
        printf("Can't write score file!\n");
    }
}
