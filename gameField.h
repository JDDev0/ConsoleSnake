#ifndef GAME_FIELD_H
#define GAME_FIELD_H
    enum fieldIDs {
        EMPTY, WALL, APPLE, BLUEBERRY, INSECT, SNAKE
    };
    
    enum direction {
        LEFT, UP, RIGHT, DOWN
    };

    struct field {
        int width;
        int height;
        enum fieldIDs **field;
        int insectPos[2];
        int snakeSize;
        int **snakePos;
        enum direction snakeDirection;
    };

    void initField(struct field *field, int width, int height);
    void removeField(struct field *field);

    char getCharFromField(struct field *field, int x, int y);
#endif
