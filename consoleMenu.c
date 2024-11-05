#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "consoleMenu.h"
#include "consoleLib.h"

void initTableOfContents(struct tableOfContents *content, int columns, int rows) {
    if(content == NULL) {
        return;
    }

    content->pages = 0;
    content->columns = columns + 1; //'\0'
    content->rows = rows;
    content->rowNow = 0;
    content->lastSection = content->lastSubSection = content->lastSubSubSection = 0;
    content->chapterNames = NULL;
}
void freeTableOfContents(struct tableOfContents *content) {
    if(content == NULL) {
        return;
    }

    if(content->chapterNames != NULL) {
        for(int i = 0;i < content->pages;i++) {
            if(content->chapterNames[i] != NULL) {
                free(content->chapterNames[i]);
            }
        }
        free(content->chapterNames);
    }
}
void addPageIfNeeded(struct tableOfContents *content) {
    if(content == NULL) {
        return;
    }

    if(content->rows-1 == content->rowNow) {
        content->rowNow = 0;
        content->pages++;
        content->chapterNames = realloc(content->chapterNames, sizeof(char *) *
        (size_t)content->pages);
        content->chapterNames[content->pages - 1] = malloc(sizeof(char) * (size_t)(
        content->columns * content->rows));

        return;
    }
    if(content->pages == 0) {
        content->pages++;
        content->chapterNames = malloc(sizeof(char *) * (size_t)content->pages);
        content->chapterNames[0] = malloc(sizeof(char) * (size_t)(content->columns *
        content->rows));
    }
}
void addSection(struct tableOfContents *content, char *name, int page) {
    if(content == NULL) {
        return;
    }

    content->lastSection++;
    content->lastSubSection = 0;
    content->lastSubSubSection = 0;

    addPageIfNeeded(content);

    //Get chapter row
    char *chapter = content->chapterNames[content->pages - 1] +
                    content->rowNow * content->columns;
    int size = 0;
    sprintf(chapter, "%d %s", content->lastSection, name);
    size = (int)strlen(chapter);
    memset(chapter + size, '.', sizeof(char) * (size_t)(content->columns -
    size - 2));

    size = (int)(floor(log10((double)page)));
    sprintf(chapter + content->columns - size - 2, "%d", page);

    content->rowNow++;
}
void addSubSection(struct tableOfContents *content, char *name, int page) {
    if(content == NULL) {
        return;
    }

    content->lastSubSection++;
    content->lastSubSubSection = 0;

    addPageIfNeeded(content);

    //Get chapter row
    char *chapter = content->chapterNames[content->pages - 1] +
                    content->rowNow * content->columns;
    int size = (int)(floor(log10(abs(content->lastSection)))) + 2;
    memset(chapter, ' ', sizeof(char) * (size_t)size);
    sprintf(chapter + size, "%d.%d %s", content->lastSection,
    content->lastSubSection, name);
    size = (int)strlen(chapter);
    memset(chapter + size, '.', sizeof(char) * (size_t)(content->columns -
    size - 2));

    size = (int)(ceil(log10((double)page + 1.)));
    sprintf(chapter + content->columns - size - 1, "%d", page);

    content->rowNow++;
}
void addSubSubSection(struct tableOfContents *content, char *name, int page) {
    if(content == NULL) {
        return;
    }

    content->lastSubSubSection++;

    addPageIfNeeded(content);

    //Get chapter row
    char *chapter = content->chapterNames[content->pages - 1] +
                    content->rowNow * content->columns;
    int size = 2*(int)(floor(log10(abs(content->lastSection)))) +
    (int)(floor(log10(abs(content->lastSubSection)))) + 6;
    memset(chapter, ' ', sizeof(char) * (size_t)size);
    sprintf(chapter + size, "%d.%d.%d %s", content->lastSection,
    content->lastSubSection, content->lastSubSubSection, name);
    size = (int)strlen(chapter);
    memset(chapter + size, '.', sizeof(char) * (size_t)(content->columns -
    size - 2));

    size = (int)(ceil(log10((double)page + 1.)));
    sprintf(chapter + content->columns - size - 1, "%d", page);

    content->rowNow++;
}
static const enum consoleLibColor colors[] = {
    CL_COLOR_BLUE, CL_COLOR_GREEN, CL_COLOR_CYAN
};
void drawContent(struct tableOfContents *content, int page) {
    if(content == NULL || content->pages <= page) {
        return;
    }

    char *pos = content->chapterNames[page];
    int maxRow;
    if(page < content->pages - 1) {
        maxRow = content->rows - 1;
    }else {
        maxRow = content->rowNow;
    }
    for(int i = 0;i < maxRow;i++) {
        int dotCount = 0;
        int numStart = 0;

        int j = 0;
        while(pos[j] != ' ' || !numStart) {
            if(pos[j] != ' ' && !numStart) {
                numStart = 1;
            }

            if(pos[j] == '.') {
                dotCount++;
            }

            j++;
        }

        setColor(colors[dotCount], CL_COLOR_NO_COLOR);
        drawf("%s\n", pos);

        pos += content->columns;
    }
    resetColor();
}
int getPageMouseClicked(struct tableOfContents *content, int page, int row) {
    if(content == NULL || content->pages <= page || content->rows-1 <= row) {
        return -1;
    }
    //For last page
    if(page == content->pages-1 && content->rowNow <= row) {
        return -1;
    }

    //Get last char
    char *pos = content->chapterNames[page] + content->columns * (row + 1) - 1;
    //Get page number pos
    while(1) {
        //"page": now: return value
        int tmp = sscanf(pos, "%d", &page);
        if(tmp == 0) {
            pos++;

            break;
        }

        pos--;
    }
    sscanf(pos, "%d", &page);

    return page - 1; //Convert to start by 0
}

//Menu
void initTabOrder(struct tabOrder *tabOrder, int colorCount,
int *fgColorsNotSelected, int *bgColorsNotSelected, int *fgColorsSelected,
int *bgColorsSelected) {
    if(tabOrder == NULL)
        return;

    tabOrder->list = NULL;
    tabOrder->nodeCount = 0;
    tabOrder->pos = 0;
    tabOrder->colorCount = colorCount;
    tabOrder->fgColorsNotSelected = fgColorsNotSelected;
    tabOrder->bgColorsNotSelected = bgColorsNotSelected;
    tabOrder->fgColorsSelected = fgColorsSelected;
    tabOrder->bgColorsSelected = bgColorsSelected;
}
void freeTabOrder(struct tabOrder *tabOrder) {
    if(tabOrder == NULL)
        return;

    if(tabOrder->list != NULL) {
        struct tabOrderNode *node;

        while(1) {
            node = tabOrder->list;

            if(node->next != NULL) {
                //Go to list end - 1
                while(node->next->next != NULL) {
                    node = node->next;
                }
                //Free list end
                free(node->next);
                //Set next to NULL
                node->next = NULL;
            }else {
                //Free first element
                free(node);
                tabOrder->list = NULL;
                break;
            }
        }
    }
}
void addToTabOrder(struct tabOrder *tabOrder, struct menuComponent *component) {
    if(tabOrder == NULL)
        return;

    struct tabOrderNode *newTN = malloc(sizeof(struct tabOrderNode));
    struct tabOrderNode *node = tabOrder->list;
    newTN->val = component;
    newTN->next = NULL;

    tabOrder->nodeCount++;

    if(node == NULL) {
        tabOrder->list = newTN;

        return;
    }

    //Go to end
    while(node->next != NULL)
        node = node->next;

    node->next = newTN;
}
void updateKeyTabOrder(struct tabOrder *tabOrder, int key) {
    struct menuComponent *component;
    struct tabOrderNode *node = tabOrder->list;
    int colorCount;
    if(node == NULL || key != CL_KEY_TAB)
        return;

    //Go to pos
    for(int i = 0;i < tabOrder->pos;i++)
        node = node->next;
    component = node->val;

    if(component->colorCount > tabOrder->colorCount)
        colorCount = tabOrder->colorCount;
    else
        colorCount = component->colorCount;

    component->isSelected = 0;
    for(int i = 0;i < colorCount;i++) {
        component->fgColors[i] = tabOrder->fgColorsNotSelected[i];
        component->bgColors[i] = tabOrder->bgColorsNotSelected[i];
    }

    if(++tabOrder->pos == tabOrder->nodeCount)
        tabOrder->pos = 0;

    //Go to pos
    node = tabOrder->list;
    for(int i = 0;i < tabOrder->pos;i++)
        node = node->next;
    component = node->val;

    if(component->colorCount > tabOrder->colorCount)
        colorCount = tabOrder->colorCount;
    else
        colorCount = component->colorCount;

    component->isSelected = 1;
    for(int i = 0;i < colorCount;i++) {
        component->fgColors[i] = tabOrder->fgColorsSelected[i];
        component->bgColors[i] = tabOrder->bgColorsSelected[i];
    }
}

struct menuContent *getMenuContent(struct menuComponent *content) {
    return (struct menuContent *)(content->component);
}
void initMenu(struct menuComponent *content, char **entryNames, int entryCount,
int xStart, int yStart, int columns, int rows, enum menuContentTextAlignment
textAlignment, enum menuContentSelecetCursor cursorType, int fgColorNormal,
int bgColorNormal, int fgColorSelected, int bgColorSelected) {
    if(content == NULL || rows < entryCount)
        return;

    content->isSelected = 1;
    content->x = xStart;
    content->y = yStart;
    content->colorCount = 2;
    content->bgColors = malloc(sizeof(int) * 2);
    content->fgColors = malloc(sizeof(int) * 2);

    content->component = malloc(sizeof(struct menuContent));
    struct menuContent *ptrContent = getMenuContent(content);

    ptrContent->columns = columns;
    ptrContent->rows = rows;
    ptrContent->textAlignment = textAlignment;
    ptrContent->cursorType = cursorType;
    ptrContent->pos = 0;
    ptrContent->entryCount = entryCount;
    ptrContent->entryNames = entryNames;
    content->fgColors[0] = fgColorNormal;
    content->bgColors[0] = bgColorNormal;
    content->fgColors[1] = fgColorSelected;
    content->bgColors[1] = bgColorSelected;
}
void freeMenu(struct menuComponent *content) {
    if(content == NULL)
        return;

    free(content->fgColors);
    free(content->bgColors);
    if(content->component != NULL)
        free(content->component);
}
void drawMenu(struct menuComponent *content) {
    if(content == NULL || content->component == NULL)
        return;

    struct menuContent *ptrContent = getMenuContent(content);
    int y = content->y;
    int x = content->x;
    int width = ptrContent->columns;
    int strWidth;
    int xDraw;
    int offsetLeft = 0;
    int offsetRight = 0;
    switch(ptrContent->cursorType) {
        case CM_SC_STARS:
        case CM_SC_VERTICAL_BAR:
        case CM_SC_QUOTES:
        case CM_SC_APOSTROPHE:
            offsetLeft = 1;
            offsetRight = 1;
            break;
        case CM_SC_ARROW_LEFT:
            offsetLeft = 2;
            break;
        case CM_SC_ARROW_LEFT_RIGHT:
            offsetLeft = 2;
            offsetRight = 2;
            break;
        case CM_SC_ARROW_RIGHT:
            offsetRight = 2;
            break;
        case CM_SC_INVERT_COLOR:
            break;
    }

    for(int i = 0;i < ptrContent->entryCount;i++) {
        strWidth = (int)strlen(ptrContent->entryNames[i]);
        switch(ptrContent->textAlignment) {
            case CM_TA_LEFT:
                xDraw = x + offsetLeft;
                break;
            case CM_TA_CENTER:
                xDraw = (int)(((double)(offsetLeft - offsetRight + width -
                strWidth))*.5) + x;
                break;
            case CM_TA_RIGHT:
                xDraw = width - strWidth - offsetRight + x;
                break;
        }
        if(ptrContent->pos == i) {
            setColor(content->fgColors[1], content->bgColors[1]);

            switch(ptrContent->cursorType) {
                case CM_SC_STARS:
                    setCursorPos(xDraw - 1, y + i);
                    drawf("*");

                    setCursorPos(xDraw + strWidth, y + i);
                    drawf("*");
                    break;
                case CM_SC_ARROW_LEFT:
                    setCursorPos(xDraw - 2, y + i);
                    drawf("->");
                    break;
                case CM_SC_ARROW_LEFT_RIGHT:
                    setCursorPos(xDraw - 2, y + i);
                    drawf("->");
                    setCursorPos(xDraw + strWidth, y + i);
                    drawf("<-");
                    break;
                case CM_SC_ARROW_RIGHT:
                    setCursorPos(xDraw + strWidth, y + i);
                    drawf("<-");
                    break;
                case CM_SC_VERTICAL_BAR:
                    setCursorPos(xDraw - 1, y + i);
                    drawf("|");

                    setCursorPos(xDraw + strWidth, y + i);
                    drawf("|");
                    break;
                case CM_SC_QUOTES:
                    setCursorPos(xDraw - 1, y + i);
                    drawf("\"");

                    setCursorPos(xDraw + strWidth, y + i);
                    drawf("\"");
                    break;
                case CM_SC_APOSTROPHE:
                    setCursorPos(xDraw - 1, y + i);
                    drawf("\'");

                    setCursorPos(xDraw + strWidth, y + i);
                    drawf("\'");
                    break;
                case CM_SC_INVERT_COLOR:
                    setColor(content->bgColors[0], content->fgColors[0]);
                    break;
            }
        }else {
            setColor(content->fgColors[0], content->bgColors[0]);
        }

        setCursorPos(xDraw, y + i);
        drawf("%s", ptrContent->entryNames[i]);
    }
}
int updateKeyMenu(struct menuComponent *content, int key) {
    if(content == NULL || content->component == NULL || !content->isSelected)
        return -1;

    struct menuContent *ptrContent = getMenuContent(content);

    switch(key) {
        case CL_KEY_UP:
            ptrContent->pos--;
            if(ptrContent->pos == -1)
                ptrContent->pos = ptrContent->entryCount - 1;
            break;
        case CL_KEY_DOWN:
            ptrContent->pos++;
            if(ptrContent->pos == ptrContent->entryCount)
                ptrContent->pos = 0;
            break;
        case CL_KEY_ENTER:
            return ptrContent->pos;
    }

    return -1;
}
int updateMouseMenu(struct menuComponent *content, int column, int row) {
    if(content == NULL || content->component == NULL || column == -1 || row == -1 ||
    !content->isSelected)
        return - 1;

    struct menuContent *ptrContent = getMenuContent(content);

    int pos = row - content->y;
    if(pos < 0 || pos >= ptrContent->entryCount)
        return -1;

    int width = ptrContent->columns;
    int strWidth = (int)strlen(ptrContent->entryNames[pos]);
    int xElementStart = content->x;
    int offsetLeft = 0;
    int offsetRight = 0;
    switch(ptrContent->cursorType) {
        case CM_SC_STARS:
        case CM_SC_VERTICAL_BAR:
        case CM_SC_QUOTES:
        case CM_SC_APOSTROPHE:
            offsetLeft = 1;
            offsetRight = 1;
            break;
        case CM_SC_ARROW_LEFT:
            offsetLeft = 2;
            break;
        case CM_SC_ARROW_LEFT_RIGHT:
            offsetLeft = 2;
            offsetRight = 2;
            break;
        case CM_SC_ARROW_RIGHT:
            offsetRight = 2;
            break;
        case CM_SC_INVERT_COLOR:
            break;
    }
    switch(ptrContent->textAlignment) {
        case CM_TA_LEFT:
            xElementStart += offsetLeft;
            break;
        case CM_TA_CENTER:
            xElementStart += (int)(((double)(offsetLeft - offsetRight + width -
            strWidth))*.5);
            break;
        case CM_TA_RIGHT:
            xElementStart += width - strWidth - offsetRight;
            break;
    }
    int xElementEnd = xElementStart + strWidth;
    xElementStart--;

    if(ptrContent->pos == pos) {
        xElementStart -= offsetLeft;
        xElementEnd += offsetRight;
    }

    if(column > xElementStart && column < xElementEnd) {
        ptrContent->pos = pos;
        return pos;
    }

    return -1;
}

struct textField *getTextField(struct menuComponent *field) {
    return (struct textField *)(field->component);
}
void initTextField(struct menuComponent *field, int x, int y, int width,
int maxTextLength, int fgColor, int bgColor) {
    if(field == NULL)
        return;

    field->isSelected = 1;
    field->x = x;
    field->y = y;
    field->colorCount = 1;
    field->bgColors = malloc(sizeof(int));
    field->fgColors = malloc(sizeof(int));

    field->component = malloc(sizeof(struct textField));
    struct textField *ptrField = getTextField(field);

    ptrField->width = width;
    ptrField->maxTextLength = maxTextLength;
    field->fgColors[0] = fgColor;
    field->bgColors[0] = bgColor;
    ptrField->offsetText = 0;
    ptrField->textLength = 0;
    ptrField->text = malloc(sizeof(char) * (size_t)(maxTextLength + 1));
}
void freeTextField(struct menuComponent *field) {
    if(field == NULL)
        return;

    free(field->fgColors);
    free(field->bgColors);
    if(field->component != NULL) {
        free(getTextField(field)->text);
        free(field->component);
    }
}
void drawTextField(struct menuComponent *field) {
    if(field == NULL || field->component == NULL)
        return;

    struct textField *ptrField = getTextField(field);

    setColor(field->fgColors[0], field->bgColors[0]);

    int textToDraw;
    if(ptrField->textLength < ptrField->width)
        textToDraw = ptrField->textLength;
    else
        textToDraw = ptrField->width;

    for(int i = ptrField->width - 1;i >= textToDraw;i--) {
        setCursorPos(field->x + i, field->y);
        drawf("_");
    }

    for(int i = 0;i < textToDraw;i++) {
        setCursorPos(field->x + i, field->y);
        drawf("%c", ptrField->text[i + ptrField->offsetText]);
    }
}
void updateKeyTextField(struct menuComponent *field, int key, char *ret) {
    if(field == NULL || field->component == NULL || ret == NULL ||
    !field->isSelected)
        return;

    struct textField *ptrField = getTextField(field);

    if(key == CL_KEY_ENTER) {
        ptrField->text[ptrField->textLength] = '\0';
        memcpy(ret, ptrField->text, sizeof(char) * (size_t)
        (ptrField->textLength + 1));

        return;
    }

    if(key == CL_KEY_DELETE && ptrField->textLength > 0) {
        ptrField->textLength--;

        if(ptrField->textLength >= ptrField->width)
            ptrField->offsetText = ptrField->textLength - ptrField->width;
    }else if(key > 31 && key < 127) { //'SPACE' - '~'
        if(ptrField->textLength == ptrField->maxTextLength)
            return;

        ptrField->text[ptrField->textLength] = (char)key;
        ptrField->textLength++;

        if(ptrField->textLength > ptrField->width)
            ptrField->offsetText = ptrField->textLength - ptrField->width;
    }

    return;
}
void drawPasswordField(struct menuComponent *field) {
    if(field == NULL || field->component == NULL)
        return;

    struct textField *ptrField = getTextField(field);

    setColor(field->fgColors[0], field->bgColors[0]);

    int textToDraw;
    if(ptrField->textLength < ptrField->width)
        textToDraw = ptrField->textLength;
    else
        textToDraw = ptrField->width;

    for(int i = ptrField->width - 1;i >= textToDraw;i--) {
        setCursorPos(field->x + i, field->y);
        drawf("_");
    }

    for(int i = 0;i < textToDraw;i++) {
        setCursorPos(field->x + i, field->y);
        drawf("*");
    }
}

struct numField *getNumField(struct menuComponent *field) {
    return (struct numField *)(field->component);
}
void initNumField(struct menuComponent *field, int x, int y, int width,
int startNum, int minNum, int maxNum, int fgColor, int bgColor) {
    if(field == NULL)
        return;

    field->isSelected = 1;
    field->x = x;
    field->y = y;
    field->colorCount = 1;
    field->bgColors = malloc(sizeof(int));
    field->fgColors = malloc(sizeof(int));

    field->component = malloc(sizeof(struct numField));
    struct numField *ptrField = getNumField(field);

    ptrField->width = width;
    if(startNum < minNum || startNum > maxNum)
        ptrField->num = minNum;
    else
        ptrField->num = startNum;
    ptrField->minNum = minNum;
    ptrField->maxNum = maxNum;
    field->fgColors[0] = fgColor;
    field->bgColors[0] = bgColor;

    ptrField->tmpNumLen = 0;
    int numLenMin = (int)(ceil(log10((double)(abs(ptrField->minNum)) + 1.)));
    if(ptrField->minNum < 1)
        numLenMin++;
    int numLenMax = (int)(ceil(log10((double)(abs(ptrField->maxNum)) + 1.)));
    if(ptrField->maxNum < 1)
        numLenMax++;
    int numLen = (numLenMin > numLenMax)?numLenMin:numLenMax;
    ptrField->tmpNum = malloc(sizeof(char) * (size_t)(numLen + 1));
    memset(ptrField->tmpNum, 0, sizeof(char) * (size_t)(numLen + 1));
}
void freeNumField(struct menuComponent *field) {
    if(field == NULL)
        return;

    free(field->fgColors);
    free(field->bgColors);
    if(field->component != NULL) {
        free(getNumField(field)->tmpNum);
        free(field->component);
    }
}
void drawNumField(struct menuComponent *field) {
    if(field == NULL || field->component == NULL)
        return;

    struct numField *ptrField = getNumField(field);

    setColor(field->fgColors[0], field->bgColors[0]);
    for(int i = field->x + 6;i < field->x + ptrField->width - 6;i++) {
        setCursorPos(i, field->y);
        drawf(" ");
    }
    setCursorPos(field->x, field->y);
    drawf("|vv|v|");
    setCursorPos(field->x + ptrField->width - 6, field->y);
    drawf("|^|^^|");

    if(ptrField->tmpNumLen > 0) {
        setCursorPos(field->x + (int)((double)(ptrField->width -
        ptrField->tmpNumLen)*.5), field->y);
        drawf("%s", ptrField->tmpNum);
    }else {
        int numLen = (int)(ceil(log10((double)(abs(ptrField->num)) + 1.)));
        if(ptrField->num < 1)
            numLen++;

        setCursorPos(field->x + (int)((double)(ptrField->width - numLen)*.5),
        field->y);
        drawf("%d", ptrField->num);
    }
}
int updateKeyNumField(struct menuComponent *field, int key, int *ret) {
    if(field == NULL || field->component == NULL || ret == NULL ||
    !field->isSelected)
        return 0;

    struct numField *ptrField = getNumField(field);

    if(ptrField->tmpNumLen > 0) {
        int numLenMin = (int)(ceil(log10((double)(abs(ptrField->minNum)) + 1.)));
        if(ptrField->minNum < 1)
            numLenMin++;
        int numLenMax = (int)(ceil(log10((double)(abs(ptrField->maxNum)) + 1.)));
        if(ptrField->maxNum < 1)
            numLenMax++;
        int numLen = (numLenMin > numLenMax)?numLenMin:numLenMax;

        if(key == CL_KEY_DELETE) {
            ptrField->tmpNum[--ptrField->tmpNumLen] = '\0';
        }

        if(key == CL_KEY_ENTER || ptrField->tmpNumLen == 0 || ptrField->tmpNumLen ==
        numLen) {
            int tmpNum;
            int c = sscanf(ptrField->tmpNum, "%d", &tmpNum);

            ptrField->tmpNumLen = 0;
            memset(ptrField->tmpNum, 0, sizeof(char) * (size_t)(numLen + 1));

            if(tmpNum >= ptrField->minNum && tmpNum <= ptrField->maxNum && c == 1) {
                *ret = ptrField->num = tmpNum;

                return 1;
            }
        }else if(key >= '0' && key <= '9') {
            if(ptrField->tmpNum[0] == '+')
                ptrField->tmpNumLen = 0;
            ptrField->tmpNum[ptrField->tmpNumLen++] = (char)key;
        }

        return 0;
    }

    if(key == CL_KEY_ENTER) {
        *ret = ptrField->num;
        return 1;
    }else if(key == CL_KEY_UP) {
        if(ptrField->num < ptrField->maxNum)
            ptrField->num++;
    }else if(key == CL_KEY_DOWN) {
        if(ptrField->num > ptrField->minNum)
            ptrField->num--;
    }else if(key == '-' || key == '+' || (key >= '0' && key <= '9')) {
        ptrField->tmpNumLen = 1;
        ptrField->tmpNum[0] = (char)key;
    }

    return 0;
}
void updateMouseNumField(struct menuComponent *field, int column, int row) {
    column -= field->x;

    struct numField *ptrField = getNumField(field);

    if(field == NULL || field->component == NULL || column < 0 ||
    column >= ptrField->width || row != field->y || !field->isSelected)
        return;

    if(column > 0 && column < 3) {
        if(ptrField->num - 10 >= ptrField->minNum)
            ptrField->num -= 10;
        else
            ptrField->num = ptrField->minNum;
    }else if(column == 4) {
        if(ptrField->num > ptrField->minNum)
            ptrField->num--;
    }else if(column > ptrField->width - 4 && column < ptrField->width - 1) {
        if(ptrField->num + 10 <= ptrField->maxNum)
            ptrField->num += 10;
        else
            ptrField->num = ptrField->maxNum;
    }else if(column == ptrField->width - 5) {
        if(ptrField->num < ptrField->maxNum)
            ptrField->num++;
    }
}

void fillKeyField(char field[], int sizeField, char empty, const char minMax[],
int toUpper, int key) {
    if(key == CL_KEY_DELETE) {
        for(int i = sizeField - 1;i > -1;i--) {
            if(field[i] != empty) {
                field[i] = empty;

                return;
            }
        }
    }

    for(int i = 0;i < sizeField;i++) {
        if(field[i] == empty) {
            if(key >= minMax[2*i] && key <= minMax[2*i + 1]) {
                if(toUpper) {
                    field[i] = (char)toupper(key);
                }else {
                    field[i] = (char)key;
                }
            }

            return;
        }
    }
}
