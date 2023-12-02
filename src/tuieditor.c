
#include <ncurses.h>
#include "crossword.h"


#define OFFSET_X 4
#define OFFSET_Y 4


void drawBox(int x, int y, int width, int height)
{
    attron(COLOR_PAIR(1));

    mvaddch(y-1, 
            (x - 2), 
            ACS_ULCORNER);

    mvaddch(y + (height), 
            (x - 2), 
            ACS_LLCORNER);

    mvaddch(y-1, 
            (x) + (width * 3)-1, 
            ACS_URCORNER);

    mvaddch(y + (height),
            (x) + (width * 3)-1,
            ACS_LRCORNER);
    
    for (int i = 0; i < (width*3); i++)
    {
        mvaddch(y-1, x+i-1, ACS_HLINE);
        mvaddch(y + height, x+i-1, ACS_HLINE);
    }


    for (int i = 0; i < height; i++)
    {
        mvaddch(y+i, x-2, ACS_VLINE);
        mvaddch(y+i, x+(width*3)-1, ACS_VLINE);
    }


    attroff(COLOR_PAIR(1));
}



void printCrossword(char **puzzle, int width, int height, int curr_x, int curr_y, int vertical) {
    drawBox(OFFSET_X, OFFSET_Y, width, height);
   
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            int color_pair = ((i + j) % 2 == 0) ? 2 : 3;
            if (i == curr_y && j == curr_x)
            {
                color_pair = 5;
            }
            if (puzzle[i][j] == '.') {
                mvaddch(OFFSET_Y + i, OFFSET_X + (j*3), ' ');
            } else {
                attron(COLOR_PAIR(color_pair));
                mvaddch(OFFSET_Y + i, OFFSET_X + (j*3)-1, ' ');
                mvaddch(OFFSET_Y + i, OFFSET_X + (j*3), puzzle[i][j]);
                mvaddch(OFFSET_Y + i, OFFSET_X + (j*3)+1, ' ');
                attroff(COLOR_PAIR(color_pair));
            }
        }
    }


    int j = curr_x;
    int i = curr_y;

    
    while (i < height && j < width && puzzle[i][j] != '.') {
        attron(COLOR_PAIR(6));
        mvaddch(OFFSET_Y + i, OFFSET_X + (j*3)-1, ' ');
        mvaddch(OFFSET_Y + i, OFFSET_X + (j*3), puzzle[i][j]);
        mvaddch(OFFSET_Y + i, OFFSET_X + (j*3)+1, ' ');
        attroff(COLOR_PAIR(6));

        i += vertical ? 0 : 1;
        j += vertical ? 1 : 0;
    }


    j = curr_x;
    i = curr_y;

    while (i >= 0 && j >= 0 && puzzle[i][j] != '.') {
        attron(COLOR_PAIR(6));
        mvaddch(OFFSET_Y + i, OFFSET_X + (j*3)-1, ' ');
        mvaddch(OFFSET_Y + i, OFFSET_X + (j*3), puzzle[i][j]);
        mvaddch(OFFSET_Y + i, OFFSET_X + (j*3)+1, ' ');
        attroff(COLOR_PAIR(6));

        i -= vertical ? 0 : 1;
        j -= vertical ? 1 : 0;
	}

	attron(COLOR_PAIR(1));
    mvaddch(OFFSET_Y + curr_y, OFFSET_X + (curr_x*3)-1, ' ');
    mvaddch(OFFSET_Y + curr_y, OFFSET_X + (curr_x*3), puzzle[curr_y][curr_x]);
    mvaddch(OFFSET_Y + curr_y, OFFSET_X + (curr_x*3)+1, ' ');
    attroff(COLOR_PAIR(1));

}



/*
 * return horizontal
 */
int processInputs(int input,
                  crossword_t * cw, 
                  int * curr_x, 
                  int * curr_y, 
                  int * horizontal)
{
    char ** puzzle = cw->puzzle_solution;

    //cw->puzzle_blank;
    if (input == KEY_DOWN) {
        if (*horizontal) {
            *horizontal = 0;
        } else {
            int next_y = (*curr_y)+1;
            while (next_y < cw->height && puzzle[next_y][*curr_x] == '.') 
                next_y++;
            if ((next_y) < cw->height)
                (*curr_y) = next_y;
        }

    } else if (input == KEY_UP) {
        if (*horizontal) {
            *horizontal = 0;
        } else {
            int next_y = (*curr_y)-1;
            while (next_y >= 0 && puzzle[next_y][*curr_x] == '.')
                next_y--;
            if (next_y >= 0)
                *curr_y = next_y;
        }

    } else if (input == KEY_LEFT) {
        if (!(*horizontal)) {
            *horizontal = 1;
        } else {
            int next_x = (*curr_x)-1;
            while (next_x >= 0 && puzzle[*curr_y][next_x] == '.')
                next_x--;
            if (next_x >= 0)
                (*curr_x) = next_x;
        }

    } else if (input == KEY_RIGHT) {
        if (!(*horizontal)) {
            *horizontal = 1;
        } else {
            int next_x = (*curr_x)+1;
            while (next_x < cw->width && puzzle[*curr_y][next_x] == '.')
            next_x++;        
            if (next_x < cw->width) {
                *curr_x = next_x;
            }
        }

    } else if (input == 27) {
        return 0;

    } else if (input == KEY_BACKSPACE || 
                input == KEY_DC || 
                input == 127) {

        puzzle[*curr_y][*curr_x] = ' ';
        
        if (*horizontal)
        {
            int next_x = (*curr_x)-1;
            while (next_x >= 0 && puzzle[*curr_y][next_x] == '.')
                next_x--;
            if (next_x >= 0)
                *curr_x = next_x;
        } else {
            
            int next_y = (*curr_y)-1;
            while (next_y >= 0 && puzzle[next_y][*curr_x] == '.') 
                next_y--;
            if (next_y >= 0)
                *curr_y = next_y;
        }
    
    } else {
        puzzle[*curr_y][*curr_x] = (char) input;
        if (*horizontal)
        {
            int next_x = *curr_x+1;
            while (next_x < cw->width && puzzle[*curr_y][next_x] == '.')
                next_x++;
            if (next_x < cw->width)
                *curr_x = next_x;
        } else {
            
            int next_y = (*curr_y)+1;
            while (next_y < cw->height && puzzle[next_y][*curr_x] == '.') 
                next_y++;
            if (next_y < cw->height)
                *curr_y = next_y;
        }
    }
    return 1;
}



void colorsInit()
{
    if (can_change_color()) {
        // Change the definition of COLOR_RED to a shade of orange
        init_color(COLOR_WHITE, 1000,1000,1000);
        init_color(5, 830, 830, 830);
        init_color(6, 870, 870, 870);
        init_color(COLOR_BLACK, 0, 0, 0); 
        init_color(7, 1000, 500, 500);
    }


    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(4, COLOR_BLACK, COLOR_WHITE);
    init_pair(2, COLOR_BLACK, 5);
    init_pair(3, COLOR_BLACK, 6);
    init_pair(5, COLOR_BLACK, COLOR_RED);
    init_pair(6, COLOR_BLACK, 7);


}


int tuiMainLoop(crossword_t * puzzle)
{
    initscr();          // Start curses mode
    noecho();           // Don't echo input characters
    cbreak();           // Disable line buffering
    keypad(stdscr, TRUE);
    start_color();
    
    colorsInit(); // DM

    bkgd(COLOR_PAIR(1));

    int curr_x = 0;
    int curr_y = 0;
	int horizontal = 1;


    do {
        printCrossword(puzzle->puzzle_solution,
                       puzzle->width,
                       puzzle->height,
                       curr_x, 
                       curr_y, 
                       horizontal);
        refresh();                  // Print on the real screen
                                    //
                                    //

    } while (processInputs(getch(), puzzle, &curr_x, &curr_y, &horizontal));

    endwin();           // End curses mode
    return 0;
}
