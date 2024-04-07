#include <stdlib.h>
#include "gameField.h"
#include "consoleLib.h"

void initField(struct field *field, int width, int height) {
    field->width = width;
    field->height = height;

    field->field = malloc(sizeof(enum fieldIDs *) * (size_t)width);
    for(int i = 0;i < field->width;i++) {
        field->field[i] = malloc(sizeof(enum fieldIDs) * (size_t)height);
        for(int j = 0;j < field->height;j++)
            field->field[i][j] = EMPTY;
    }

    field->insectPos[0] = -1;
    field->insectPos[1] = -1;

    field->snakeSize = 0;
    field->snakePos = malloc(sizeof(int *) * (size_t)(width * height));
    for(int i = 0;i < width * height;i++)
        field->snakePos[i] = malloc(sizeof(int) * 2);
    field->snakeDirection = RIGHT;
}
void removeField(struct field *field) {
    for(int i = 0;i < field->width;i++)
        free(field->field[i]);
    free(field->field);
    for(int i = 0;i < field->width * field->height;i++)
        free(field->snakePos[i]);
    free(field->snakePos);
}

char getCharFromField(struct field *field, int x, int y) {
    switch(field->field[x][y]) {
        case EMPTY:
            setColor(CL_COLOR_NO_COLOR, CL_COLOR_GREEN);
            return ' ';
        case WALL:
            setColor(CL_COLOR_PINK, CL_COLOR_GREEN);
            return '#';
        case APPLE:
            setColor(CL_COLOR_LIGHT_RED, CL_COLOR_GREEN);
            return 'o';
        case BLUEBERRY:
            setColor(CL_COLOR_BLUE, CL_COLOR_GREEN);
            return 'o';
        case INSECT:
            setColor(CL_COLOR_LIGHT_BLACK, CL_COLOR_GREEN);
            return '%';
        case SNAKE:
            for(int i = 0;i < field->snakeSize;i++) {
                if(field->snakePos[i][0] == x && field->snakePos[i][1] == y) {
                    setColor(CL_COLOR_WHITE, CL_COLOR_GREEN);

                    if(i == 0)
                        return '@';
                    else if(i == field->snakeSize - 1)
                        return ':';
                    else
                        return 'o';
                }
            }
    }

    return '\0';
}
