#ifndef __ESCAPE_H__
#define __ESCAPE_H__

// arg 
// 2, clear the entire screen
// 1, clear the screen up to where the cursor is
// 0,  would clear the screen from the cursor up to the end of the screen
// 0 is the default argument for J
#define ES_CLEAR_SCREEN(mode) "\x1b[modeJ"

// size of the macro  ES_CLEAR_SCREEN
#define ES_CLEAR_SCREEN_SIZE 4

// clear the entire screen
#define ES_CLEAR_ENTIRE_SCREEN "\x1b[2J"
// size of the macro  ES_CLEAR_ENTIRE_SCREEN
#define ES_CLEAR_ENTIRE_SCREEN_SIZE  4

#define ES_POSITION_CURSOR(row, col) "\x1b[row;colH"
#define ES_POSITION_CURSOR_SIZE 3

#define ES_POSITION_CURSOR_ORIGIN "\x1b[H"
#define ES_POSITION_CURSOR_ORIGIN_SIZE 3
#define ES_POSITION_CURSOR_RIGHT_BOTTOM "\x1b[999C\x1b[999B"
#define ES_POSITION_CURSOR_RIGHT_BOTTOM_SIZE 12

#define ES_QUERY_CURSOR_POSITION "\x1b[6n"
#define ES_QUERY_CURSOR_POSITION_SIZE 4

#define ES_HIDE_CURSOR "\x1b[?25l"
#define ES_HIDE_CURSOR_SIZE 6
#define ES_SHOW_CURSOR "\x1b[?25h"
#define ES_SHOW_CURSOR_SIZE 6

#define ES_CLEAR_LINE "\x1b[K"
#define ES_CLEAR_LINE_SIZE 3
#endif