#include <stdio.h>
#include <ncurses.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LINES 100
#define MAX_COLUMNS 80

struct buf {
    char line[MAX_COLUMNS];
    int link;
};

struct buf Buffer[MAX_LINES];
int head;
int cursorX, cursorY;

void readprogramfromfile(const char* filename) {
    FILE* file = fopen(filename, "r");
    head = 0;
    int currentlink = 1;
    if (file == NULL) {
        fprintf(stderr, "Error opening file: %s\n", filename);
        return;
    }
    char buffer[MAX_COLUMNS];
    while (head < MAX_LINES && fgets(Buffer[head].line, MAX_COLUMNS, file) != NULL) {
        if (strlen(Buffer[head].line) != 1) {
            Buffer[head].line[strlen(Buffer[head].line) - 1] = '\0';
        }
        Buffer[head].link = currentlink++;
        head++;
    }

    fclose(file);
}

void displayprogram() {
    clear();
    for (int i = 0; i < head; i++) {
        mvprintw(i, 0, "%s", Buffer[i].line);
    }
    refresh();
}

void insertLine(int position, const char* newLine) {
    if (head >= MAX_LINES) {
        return;
    }
    for (int i = head; i > position; i--) {
        strcpy(Buffer[i].line, Buffer[i - 1].line);
        Buffer[i].link = i+1;
    }
    strcpy(Buffer[position].line, newLine);
    Buffer[position].link = position+1; 
    head++;
}

void expandProgram(const char* outputFilename) {
    FILE* outputFile = fopen(outputFilename, "w");
    if (outputFile == NULL) {
        perror("Geniþletilmiþ dosyaya yazma hatasý");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < head; i++) {
        if (strstr(Buffer[i].line, "@var")) {
            char variableName[20];
            sscanf(Buffer[i].line, "@var %s", variableName);
            fprintf(outputFile, "add_TT('%s');\n", variableName);
        }
        else if (strstr(Buffer[i].line, "@trace")) {
            char variableName[20];
            sscanf(Buffer[i].line, "@trace %s", variableName);
            fprintf(outputFile, "display_TT('%s');\n", variableName);
        }
        else {
            char* control = strchr(Buffer[i].line, '=');
            if (control != NULL) {
                char line[MAX_COLUMNS];
                strcpy(line, Buffer[i].line);
                const char delimiters[] = "=";
                char* variableName = strtok(line, delimiters);
                fprintf(outputFile, "%supdate_TT('%s', %s);\n", Buffer[i].line, variableName, variableName);
            }
            else {
                fprintf(outputFile, "%s\n", Buffer[i].line);
            }
        }
    }

    fclose(outputFile);
}

int main() {
    initscr(); // Initialize the ncurses library
    keypad(stdscr, TRUE);  // Enable special keys

    readprogramfromfile("myprog.c");
    cursorX = 0;  // Initialize cursor position
    cursorY = 0;

    int ch;
    while (1) {
        ch = getch();
        switch (ch) {
        case 'd':
            displayprogram();
            break;
        case 'x':
            expandProgram("expanded.c");
            endwin();
            return 0;
            break;
        case 'w':
            cursorY = (cursorY > 0) ? cursorY - 1 : 0;
            break;
        case 's':
            cursorY = (cursorY < head - 1) ? cursorY + 1 : head - 1;
            break;
        case 'i':
            mvprintw(LINES - 1, 0, "Enter new line: ");
            refresh();
            char newLine[MAX_COLUMNS];
            getstr(newLine);
            cursorY++;
            insertLine(cursorY, newLine);
            move(cursorY, cursorX);
            refresh();
            displayprogram();

        }
        displayprogram();
        move(cursorY, cursorX);
        refresh();
    }
    endwin();  // End the ncurses library
    return 0;
}
