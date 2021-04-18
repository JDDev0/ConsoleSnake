#include "helpMenu.h"
#include "consoleLib.h"
#include "consoleMenu.h"
#include "gameField.h"

struct tableOfContents content;
int helpPage;
const int maxHelpPage = 5;

void initHelpMenu(int width, int height) {
    initTableOfContents(&content, width, height);

    addSection(&content, "Control", 2);
    addSubSection(&content, "Keyboard", 2);
    addSubSubSection(&content, "Help menu", 2);
    addSubSubSection(&content, "Exit window", 2);
    addSubSubSection(&content, "Start menu", 2);
    addSubSubSection(&content, "Game control", 3);
    addSubSection(&content, "Mouse input", 3);
    addSubSubSection(&content, "Help menu", 3);
    addSubSubSection(&content, "Exit window", 3);
    addSubSubSection(&content, "Start menu", 4);
    addSection(&content, "Menus", 5);
    addSubSection(&content, "Help menu", 5);
    addSubSection(&content, "Exit window", 5);
    addSubSection(&content, "Game screen", 5);
    addSection(&content, "Gameplay", 6);
    addSubSection(&content, "Move", 6);
    addSubSection(&content, "Game over", 6);
}

void drawHelp(void) {
    setColor(CL_COLOR_YELLOW, CL_COLOR_NO_COLOR);
    setUnderline(1);
    drawf("Help menu");

    setCursorPos(0, 2);
    switch(helpPage) {
        case 0:
            setUnderline(0);
            drawContent(&content, 0);

            break;
        case 1:
            setColor(CL_COLOR_BLUE, CL_COLOR_NO_COLOR);
            drawf("1 Control\n");

            setColor(CL_COLOR_GREEN, CL_COLOR_NO_COLOR);
            drawf("1.1 Keyboard\n");

            setUnderline(0);
            resetColor();
            drawf("Press: ");
            setColor(CL_COLOR_LIGHT_RED, CL_COLOR_NO_COLOR);
            drawf("KEY\n");

            drawf("F1");
            resetColor();
            drawf(": Open help menu");

            setUnderline(1);
            setColor(CL_COLOR_CYAN, CL_COLOR_NO_COLOR);
            setCursorPos(0, 7);
            drawf("1.1.1 Help menu\n");

            setUnderline(0);
            setColor(CL_COLOR_LIGHT_RED, CL_COLOR_NO_COLOR);
            drawf("ESC");
            resetColor();
            drawf("/");
            setColor(CL_COLOR_LIGHT_RED, CL_COLOR_NO_COLOR);
            drawf("F1");
            resetColor();
            drawf(": Exit help menu\n");

            setColor(CL_COLOR_LIGHT_RED, CL_COLOR_NO_COLOR);
            drawf("UP");
            resetColor();
            drawf("/");
            setColor(CL_COLOR_LIGHT_RED, CL_COLOR_NO_COLOR);
            drawf("DOWN");
            resetColor();
            drawf(": Switch page");

            setUnderline(1);
            setColor(CL_COLOR_CYAN, CL_COLOR_NO_COLOR);
            setCursorPos(0, 11);
            drawf("1.1.2 Exit window\n");

            setUnderline(0);
            setColor(CL_COLOR_LIGHT_RED, CL_COLOR_NO_COLOR);
            drawf("y");
            resetColor();
            drawf(": Yes (Exit)\n");
            setColor(CL_COLOR_LIGHT_RED, CL_COLOR_NO_COLOR);
            drawf("n");
            resetColor();
            drawf(": No (Not exit)");

            setUnderline(1);
            setColor(CL_COLOR_CYAN, CL_COLOR_NO_COLOR);
            setCursorPos(0, 15);
            drawf("1.1.3 Start menu\n");

            setUnderline(0);
            setColor(CL_COLOR_LIGHT_RED, CL_COLOR_NO_COLOR);
            drawf("ENTER");
            resetColor();
            drawf(": Start game\n");

            setColor(CL_COLOR_LIGHT_RED, CL_COLOR_NO_COLOR);
            drawf("ESC");
            resetColor();
            drawf(": Exit window");

            break;
        case 2:
            setColor(CL_COLOR_CYAN, CL_COLOR_NO_COLOR);
            drawf("1.1.4 Game control\n");

            setUnderline(0);
            setColor(CL_COLOR_LIGHT_RED, CL_COLOR_NO_COLOR);
            drawf("ESC");
            resetColor();
            drawf(": Exit window\n");

            setColor(CL_COLOR_LIGHT_RED, CL_COLOR_NO_COLOR);
            drawf("Arrow keys");
            resetColor();
            drawf(": Change snake direction\n");

            setUnderline(1);
            setColor(CL_COLOR_GREEN, CL_COLOR_NO_COLOR);
            setCursorPos(0, 6);
            drawf("1.2 Mouse input\n");

            setUnderline(0);
            resetColor();
            drawf("Left click: [");
            setColor(CL_COLOR_NO_COLOR, CL_COLOR_YELLOW);
            drawf("L");
            resetColor();
            drawf("] \"Position\"\n");
            drawf("Right click: [");
            setColor(CL_COLOR_NO_COLOR, CL_COLOR_YELLOW);
            drawf("R");
            resetColor();
            drawf("] \"Position\"\n");
            drawf("Middle click: [");
            setColor(CL_COLOR_NO_COLOR, CL_COLOR_YELLOW);
            drawf("M");
            resetColor();
            drawf("] \"Position\"");

            setUnderline(1);
            setColor(CL_COLOR_CYAN, CL_COLOR_NO_COLOR);
            setCursorPos(0, 11);
            drawf("1.2.1 Help menu\n");

            setUnderline(0);
            resetColor();
            drawf("[");
            setColor(CL_COLOR_NO_COLOR, CL_COLOR_YELLOW);
            drawf("L");
            resetColor();
            drawf("] \"Page: 00\": Switch page (The same as ");
            setColor(CL_COLOR_LIGHT_RED, CL_COLOR_NO_COLOR);
            drawf("DOWN");
            resetColor();
            drawf(")\n[");
            setColor(CL_COLOR_NO_COLOR, CL_COLOR_YELLOW);
            drawf("L");
            resetColor();
            drawf("] Chapter at first pages: Goto page");

            setUnderline(1);
            setColor(CL_COLOR_CYAN, CL_COLOR_NO_COLOR);
            setCursorPos(0, 15);
            drawf("1.2.2 Exit window\n");

            setUnderline(0);
            resetColor();
            drawf("[");
            setColor(CL_COLOR_NO_COLOR, CL_COLOR_YELLOW);
            drawf("L");
            resetColor();
            drawf("] \"[y]es\": Yes (The same as ");
            setColor(CL_COLOR_LIGHT_RED, CL_COLOR_NO_COLOR);
            drawf("y");
            resetColor();
            drawf(")\n[");
            setColor(CL_COLOR_NO_COLOR, CL_COLOR_YELLOW);
            drawf("L");
            resetColor();
            drawf("] \"[n]o\": No (The same as ");
            setColor(CL_COLOR_LIGHT_RED, CL_COLOR_NO_COLOR);
            drawf("n");
            resetColor();
            drawf(")");
            break;
        case 3:
            setColor(CL_COLOR_CYAN, CL_COLOR_NO_COLOR);
            drawf("1.2.3 Start menu\n");

            setUnderline(0);
            resetColor();
            drawf("[");
            setColor(CL_COLOR_NO_COLOR, CL_COLOR_YELLOW);
            drawf("L");
            resetColor();
            drawf("] \"ENTER\": Start game (The same as ");
            setColor(CL_COLOR_LIGHT_RED, CL_COLOR_NO_COLOR);
            drawf("ENTER");
            resetColor();
            drawf(")\n[");
            setColor(CL_COLOR_NO_COLOR, CL_COLOR_YELLOW);
            drawf("L");
            resetColor();
            drawf("] \"Help: F1\": Open help menu (The same as ");
            setColor(CL_COLOR_LIGHT_RED, CL_COLOR_NO_COLOR);
            drawf("F1");
            resetColor();
            drawf(")");

            break;
        case 4:
            setColor(CL_COLOR_BLUE, CL_COLOR_NO_COLOR);
            drawf("2 Menus\n");
            setColor(CL_COLOR_GREEN, CL_COLOR_NO_COLOR);
            drawf("2.1 Help menu\n");

            setUnderline(0);
            resetColor();
            drawf("\"Page: x of y\": x: (Current page), y: (Last page)\n\"");
            setColor(CL_COLOR_BLUE, CL_COLOR_NO_COLOR);
            setUnderline(1);
            drawf("x Title");
            setUnderline(0);
            resetColor();
            drawf("\":     Heading 1 (Chapter Name)\n\"");
            setColor(CL_COLOR_GREEN, CL_COLOR_NO_COLOR);
            setUnderline(1);
            drawf("x.x Title");
            setUnderline(0);
            resetColor();
            drawf("\":   Heading 2 (Chapter.Chapter Name)\n\"");
            setColor(CL_COLOR_CYAN, CL_COLOR_NO_COLOR);
            setUnderline(1);
            drawf("x.x.x Title");
            setUnderline(0);
            resetColor();
            drawf("\": Heading 3 (Chapter.Chapter.Chapter Name)\n");

            setUnderline(1);
            setColor(CL_COLOR_GREEN, CL_COLOR_NO_COLOR);
            setCursorPos(0, 9);
            drawf("2.2 Exit window\n");

            setUnderline(0);
            resetColor();
            drawf("Confirm exit\n");

            setUnderline(1);
            setColor(CL_COLOR_GREEN, CL_COLOR_NO_COLOR);
            setCursorPos(0, 12);
            drawf("2.3 Game screen\n");

            setUnderline(0);
            resetColor();
            drawf("Top: Score/Level/High score");

            setColor(CL_COLOR_PINK, CL_COLOR_NO_COLOR);
            setCursorPos(0, 15);
            drawf("Game field:");

            resetColor();
            setCursorPos(0, 16);
            drawf("Empty:\nWall:\nFood:   (Apple)   (Blueberry)   (Insect)\nSnake:");

            setColor(CL_COLOR_NO_COLOR, CL_COLOR_GREEN);
            setCursorPos(7, 16);
            drawf(" ");

            setColor(CL_COLOR_PINK, CL_COLOR_GREEN);
            setCursorPos(6, 17);
            drawf("#");

            setColor(CL_COLOR_LIGHT_RED, CL_COLOR_GREEN);
            setCursorPos(6, 18);
            drawf("o");
            setColor(CL_COLOR_BLUE, CL_COLOR_GREEN);
            setCursorPos(16, 18);
            drawf("o");
            setColor(CL_COLOR_LIGHT_BLACK, CL_COLOR_GREEN);
            setCursorPos(30, 18);
            drawf("%%");

            setColor(CL_COLOR_LIGHT_GREEN, CL_COLOR_GREEN);
            setCursorPos(7, 19);
            drawf("@");
            setCursorPos(9, 19);
            drawf("o");
            setCursorPos(11, 19);
            drawf(":");

            break;
        case 5:
            setColor(CL_COLOR_BLUE, CL_COLOR_NO_COLOR);
            drawf("3 Gameplay\n");
            setColor(CL_COLOR_GREEN, CL_COLOR_NO_COLOR);
            drawf("3.1 Move\n");

            setUnderline(0);
            resetColor();
            drawf("Move  the snake  to the  food for  getting larger  and  new  hi"
            "gh  scores.");

            setUnderline(1);
            setColor(CL_COLOR_GREEN, CL_COLOR_NO_COLOR);
            setCursorPos(0, 6);
            drawf("3.2 Game over\n");

            setUnderline(0);
            resetColor();
            drawf("If you eat yourself or a wall, you will die.");

            break;
    }

    setCursorPos(0, 22);
    resetColor();
    drawf("Page: ");
    setColor(CL_COLOR_CYAN, CL_COLOR_NO_COLOR);
    drawf("%d", helpPage + 1);
    resetColor();
    drawf(" of ");
    setColor(CL_COLOR_CYAN, CL_COLOR_NO_COLOR);
    drawf("%d", maxHelpPage + 1);
}
