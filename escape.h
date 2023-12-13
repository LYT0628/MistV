#ifndef __ESCAPE_H__
#define __ESCAPE_H__

#define ES_PREFIX  "\x1b["
#define ES_PREFIX_0 '\x1b'
#define ES_PREFIX_1 '['
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

#define ES_POSITION_CURSOR(r, c) "\x1b[r;cH"
#define ES_POSITION_CURSOR_SIZE 3
#define ES_POSITION_CURSOR_FORMAT "\x1b[%d;%dH"

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

// bold (1), underscore (4), blink (5), and inverted colors (7).
#define ES_TEXT_FORMAT_INVERTED_COLOR "\x1b[7m"
#define ES_TEXT_FORMAT_INVERTED_COLOR_SIZE 4

#define ES_TEXT_FORMAT_RESET "\x1b[m"
#define ES_TEXT_FORMAT_RESET_SIZE 3

// color
#define ES_COLOR_RESET_SIZE 5
#define ES_COLOR_RESET "\x1b[39m"
#define ES_COLOR_BLACK "\x1b[30m"
#define ES_COLOR_RED "\x1b[31m"
#define ES_COLOR_RED_SIZE 5
#define ES_COLOR_WHITE "\x1b[37m"
#endif