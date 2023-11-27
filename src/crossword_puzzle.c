#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <ncurses.h>


#define CHECKSUM_BYTES 2
#define FILE_MAGIC_BYTES 12

#define CIB_CHECKSUM_BYTES 2
#define MASKED_LOW_CHECKSUMS_BYTES 4
#define MASKED_HIGH_CHECKSUMS_BYTES 4

#define VERSION_STRING_BYTES 4
#define SCRAMBLED_CHECKSUM_BYTES 2

#define WIDTH_BYTES 1
#define HEIGHT_BYTES 1
#define NUM_OF_CLUES 2

#define UNKNOWN_BITMASK 2
#define SCRAMBLED_TAG_BYTES 2




#define OFFSET_X 4
#define OFFSET_Y 4


void draw_box(int x, int y, int width, int height)
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


void print_crossword(char **puzzle, int width, int height, int curr_x, int curr_y, int vertical) {
    draw_box(OFFSET_X, OFFSET_Y, width, height);
   
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

        i += vertical ? 0 : -1;
        j += vertical ? -1 : 0;
    }



    attron(COLOR_PAIR(1));
    mvaddch(OFFSET_Y + curr_y, OFFSET_X + (curr_x*3)-1, ' ');
    mvaddch(OFFSET_Y + curr_y, OFFSET_X + (curr_x*3), puzzle[curr_y][curr_x]);
    mvaddch(OFFSET_Y + curr_y, OFFSET_X + (curr_x*3)+1, ' ');
    attroff(COLOR_PAIR(1));
}


int main(int argc, char ** argv)
{
    if (argc < 2)
    {
        printf("Incomplete arguments provided\n");
        exit(0);
    }


    initscr();          // Start curses mode
    noecho();           // Don't echo input characters
    cbreak();           // Disable line buffering


    keypad(stdscr, TRUE);

    start_color();
    
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

    bkgd(COLOR_PAIR(1));


    int fd = open(argv[1], O_RDONLY);


    char checksum_buffer[CHECKSUM_BYTES];
    // Perform checksum
    if (read(fd, checksum_buffer, CHECKSUM_BYTES) == -1)
    {
        perror("Failed to read checksum");
        exit(-1);
    }


    // File Magic
    // Offset 0x02 - 0x0D Length 0xC
    char magic_buffer [FILE_MAGIC_BYTES];
    if (read(fd, magic_buffer, FILE_MAGIC_BYTES) == -1)
    {
        perror("Failed to read magic buffer");
        exit(-1);
    }
    printf("%s\n", magic_buffer);

    
    // CIB Checksum
    char CIB_checksum_buffer [CIB_CHECKSUM_BYTES];

    if (read(fd, CIB_checksum_buffer, CIB_CHECKSUM_BYTES) == -1)
    {
        perror("Failed to read magic buffer");
        exit(-1);
    }


    // Masked Low Checksums
    char masked_low_checksum_buffer [MASKED_LOW_CHECKSUMS_BYTES];

    if (read(fd, masked_low_checksum_buffer, MASKED_LOW_CHECKSUMS_BYTES) == -1)
    {
        perror("Failed to read magic buffer");
        exit(-1);
    }


    // Masked High Checksums
    char masked_high_checksum_buffer [MASKED_HIGH_CHECKSUMS_BYTES];

    if (read(fd, masked_high_checksum_buffer, MASKED_HIGH_CHECKSUMS_BYTES) == -1)
    {
        perror("Failed to read magic buffer");
        exit(-1);
    }


    // Version String 
    char version_string_buffer [VERSION_STRING_BYTES];
    if (read(fd, version_string_buffer, VERSION_STRING_BYTES) == -1)
    {
        perror("Failed to read magic buffer");
        exit(-1);
    }

    // Skip 0x1C - 0x1D 
    if (lseek(fd, 14, SEEK_CUR) == (off_t) - 1) {
        perror("Failed to skip this little bit");
        exit(-1);
    }


    // Scrambled Checksum
    char scrambled_checksum_buffer [SCRAMBLED_CHECKSUM_BYTES];
    if (read(fd, scrambled_checksum_buffer, SCRAMBLED_CHECKSUM_BYTES) == -1)
    {
        perror("Failed to read magic buffer");
        exit(-1);
    }

    // Width
    char width;
    if (read(fd, &width, WIDTH_BYTES) == -1)
    {
        perror("Failed to read magic buffer");
        exit(-1);
    }

    // Height
    char height;
    if (read(fd, &height, HEIGHT_BYTES) == -1)
    {
        perror("Failed to read magic buffer");
        exit(-1);
    }


    // # of Clues
    short num_of_clues;
    if (read(fd, &num_of_clues, NUM_OF_CLUES) == -1)
    {
        perror("Failed to read magic buffer");
        exit(-1);
    }

    // Unknown Bitmask
    short unknown_bitmask;
    if (read(fd, &unknown_bitmask, UNKNOWN_BITMASK) == -1)
    {
        perror("Failed to read unknown bitmask");
        exit(-1);
    }

    // Scrambled Tag
    short scrambled_tag;
    if (read(fd, &scrambled_tag, SCRAMBLED_TAG_BYTES) == -1)
    {
       perror("Failed to read scrambled tag");
       exit(-1);
    }


    printf("Successfully read all file bytes\n");

    printf("Initializing the puzzle structure\n");
    printf("Dimenstions: %d %d\n", (int)width, (int)height);

    char ** puzzle = malloc(height * sizeof(char *));
    for (int i = 0; i < height; i++)
    {
        puzzle[i] = malloc(width * sizeof(char));
    }

    printf("Successfully created puzzle structure\n");
    
    
    for (int i = 0; i < height; i++)
    {
        if (read(fd, puzzle[i], width) == -1)
        {
            perror("Error reading row of puzzle");
        }
    }


    int curr_x = 0;
    int curr_y = 0;
	int horizontal = 1;
    while (1)
    {
        print_crossword(puzzle, width, height, curr_x, curr_y, horizontal);

        int key_pressed = getch();            // Wait for user input to exit

        refresh();          // Print on the real screen
        if (key_pressed == KEY_DOWN) {
			if (horizontal) {
				horizontal = 0;
			} else {
            	int next_y = curr_y+1;
            	while (next_y < height && puzzle[next_y][curr_x] == '.') 
            	    next_y++;
            	if (next_y < height)
            	    curr_y = next_y;
			}

        } else if (key_pressed == KEY_UP) {
			if (horizontal) {
				horizontal = 0;
			} else {
				int next_y = curr_y-1;
            	while (next_y >= 0 && puzzle[next_y][curr_x] == '.')
                	next_y--;
            	if (next_y >= 0)
                	curr_y = next_y;
			}

        } else if (key_pressed == KEY_LEFT) {
			if (!horizontal) {
				horizontal = 1;
			} else {
				int next_x = curr_x-1;
            	while (next_x >= 0 && puzzle[curr_y][next_x] == '.')
               		next_x--;
            	if (next_x >= 0)
                	curr_x = next_x;
			}

        } else if (key_pressed == KEY_RIGHT) {
			if (!horizontal) {
				horizontal = 1;
			} else {
            	int next_x = curr_x+1;
            	while (next_x < width && puzzle[curr_y][next_x] == '.')
                next_x++;        
            	if (next_x < width) {
                	curr_x = next_x;
            	}
			}

        } else if (key_pressed == 27) {
            break;

		} else if (key_pressed == KEY_BACKSPACE || 
					key_pressed == KEY_DC || 
					key_pressed == 127) {

			puzzle[curr_y][curr_x] = ' ';
			
			if (horizontal)
			{
				int next_x = curr_x-1;
            	while (next_x >= 0 && puzzle[curr_y][next_x] == '.')
               		next_x--;
            	if (next_x >= 0)
                	curr_x = next_x;
			} else {
				
            	int next_y = curr_y-1;
            	while (next_y >= 0 && puzzle[next_y][curr_x] == '.') 
            	    next_y--;
            	if (next_y >= 0)
            	    curr_y = next_y;
			}
		
        } else {
            puzzle[curr_y][curr_x] = (char) key_pressed;
			if (horizontal)
			{
				int next_x = curr_x+1;
            	while (next_x < width && puzzle[curr_y][next_x] == '.')
               		next_x++;
            	if (next_x < width)
                	curr_x = next_x;
			} else {
				
            	int next_y = curr_y+1;
            	while (next_y < height && puzzle[next_y][curr_x] == '.') 
            	    next_y++;
            	if (next_y < height)
            	    curr_y = next_y;
			}
        }
    }

    endwin();           // End curses mode
}
