#include <stdint.h>

#ifndef CROSSWORD_H
#define CROSSWORD_H

typedef struct crossword {
	int8_t width;
	int8_t height;
	int8_t num_clues;
	char ** puzzle_solution;
	char ** puzzle_blank;
} crossword_t;


/*
 * 
 * 
 *
 */
crossword_t * readPuzzle();

/*
 * 
 * 
 * 
 */
void clear_puzzle(crossword_t * cw);


#endif
