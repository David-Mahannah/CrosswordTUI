
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "crossword.h"


/*
 * crossword.c
 * 
 * Functions for the crossword data structure
 *
 */

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

crossword_t * readPuzzle(char * path)
{
    int fd = open(path, O_RDONLY);

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


	// We could init this earlier but I dont want to free it in
	// the error cases.
	crossword_t * cw = malloc(sizeof(crossword_t));
	cw->width = width;
	cw->height = height;
	cw->num_clues = num_of_clues;
	cw->puzzle_solution = malloc(height * sizeof(char *));
	for (int i = 0; i < height; i++) {
		cw->puzzle_solution[i] = malloc(width * sizeof(char));
	}

    printf("Successfully read all file bytes\n");
    printf("Initializing the puzzle structure\n");
    printf("Dimenstions: %d %d\n", (int)width, (int)height);
    printf("Successfully created puzzle structure\n");
        
    for (int i = 0; i < height; i++) {
        if (read(fd, cw->puzzle_solution[i], width) == -1) {
            perror("Error reading row of puzzle");
        }
    }

	printf("Creating puzzle blank\n");
	cw->puzzle_blank = malloc(height * sizeof(char *));
	for (int i = 0; i < height; i++)
	{
		cw->puzzle_blank[i] = malloc(width * sizeof(char));
	}

    return cw;
}


