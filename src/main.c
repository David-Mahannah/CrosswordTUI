#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <ncurses.h>

#include "crossword.h"
#include "tuieditor.h"


#define OFFSET_X 4
#define OFFSET_Y 4


int main(int argc, char ** argv)
{
    if (argc < 2)
    {
        printf("Incomplete arguments provided\n");
        exit(0);
    }

    crossword_t * puzzle = readPuzzle(argv[1]);

    if (tuiMainLoop(puzzle) == 1)
    {
        printf("TUI session exited due to error");
    }

    free(puzzle);

    exit(0);
}
