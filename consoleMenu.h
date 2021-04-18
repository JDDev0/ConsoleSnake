#ifndef CONSOLE_MENU_H
#define CONSOLE_MENU_H
    #define CONSOLE_MENU_VER "v0.0.2"

    struct tableOfContents {
        int pages;
        int columns;
        int rows;
        int rowNow;
        int lastSection;
        int lastSubSection;
        int lastSubSubSection;
        char **chapterNames; //[pages][colums * rows]
    };

    void initTableOfContents(struct tableOfContents *content, int columns, int rows);
    void freeTableOfContents(struct tableOfContents *content);
    void addSection(struct tableOfContents *content, char *name, int page);
    void addSubSection(struct tableOfContents *content, char *name, int page);
    void addSubSubSection(struct tableOfContents *content, char *name, int page);
    void drawContent(struct tableOfContents *content, int page);
    int getPageMouseClicked(struct tableOfContents *content, int page, int row);

    //Menu
    struct menuComponent {
        int isSelected;
        int x;
        int y;
        int colorCount;
        int *fgColors;
        int *bgColors;
        void *component; //Pointer to menu item
    };

    //Menu tab order
    struct tabOrderNode {
        struct menuComponent *val;
        struct tabOrderNode *next;
    };
    struct tabOrder {
        struct tabOrderNode *list;
        int nodeCount;
        int pos;
        int colorCount;
        int *fgColorsNotSelected;
        int *bgColorsNotSelected;
        int *fgColorsSelected;
        int *bgColorsSelected;
    };

    void initTabOrder(struct tabOrder *tabOrder, int colorCount,
    int *fgColorsNotSelected, int *bgColorsNotSelected, int *fgColorsSelected,
    int *bgColorsSelected);
    void freeTabOrder(struct tabOrder *tabOrder);
    void addToTabOrder(struct tabOrder *tabOrder, struct menuComponent *component);
    void updateKeyTabOrder(struct tabOrder *tabOrder, int key);

    //Menu items
    enum menuContentTextAlignment {
        CM_TA_LEFT, CM_TA_CENTER, CM_TA_RIGHT
    };
    enum menuContentSelecetCursor {
        CM_SC_STARS, CM_SC_ARROW_LEFT, CM_SC_ARROW_LEFT_RIGHT, CM_SC_ARROW_RIGHT,
        CM_SC_VERTICAL_BAR, CM_SC_QUOTES, CM_SC_APOSTROPHE, CM_SC_INVERT_COLOR
    };
    struct menuContent {
        int columns;
        int rows;
        enum menuContentTextAlignment textAlignment;
        enum menuContentSelecetCursor cursorType;
        int pos;
        int entryCount;
        char **entryNames;
    };
    struct textField {
        int width;
        int maxTextLength;
        int textLength;
        int offsetText;
        char *text;
    };
    struct numField {
        int width;
        int num;
        int minNum;
        int maxNum;
        int tmpNumLen;
        char *tmpNum;
    };

    void initMenu(struct menuComponent *content, char **entryNames, int entryCount,
    int xStart, int yStart, int columns, int rows, enum menuContentTextAlignment
    textAlignment, enum menuContentSelecetCursor cursorType, int fgColorNormal,
    int bgColorNormal, int fgColorSelected, int bgColorSelected);
    void freeMenu(struct menuComponent *content);
    void drawMenu(struct menuComponent *content);
    int updateKeyMenu(struct menuComponent *content, int key);
    int updateMouseMenu(struct menuComponent *content, int column, int row);

    void initTextField(struct menuComponent *field, int x, int y, int width,
    int maxTextLength, int fgColor, int bgColor);
    void freeTextField(struct menuComponent *field);
    void drawTextField(struct menuComponent *field);
    void updateKeyTextField(struct menuComponent *field, int key, char *ret);
    #define initPasswordField initTextField
    #define freePasswordField freeTextField
    void drawPasswordField(struct menuComponent *field);
    #define updateKeyPasswordField updateKeyTextField

    void initNumField(struct menuComponent *field, int x, int y, int width,
    int startNum, int minNum, int maxNum, int fgColor, int bgColor);
    void freeNumField(struct menuComponent *field);
    void drawNumField(struct menuComponent *field);
    int updateKeyNumField(struct menuComponent *field, int key, int *ret);
    void updateMouseNumField(struct menuComponent *field, int column, int row);

    void fillKeyField(char field[], int sizeField, char empty, const char minMax[],
    int toUpper, int key);
#endif
