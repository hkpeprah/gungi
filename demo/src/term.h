#ifndef __TERM__
#define __TERM__
#include <cstdio>

#define OPEN_ESCAPE              "\033["
#define RESTORE_CURSOR           "\033[u"
#define SAVE_CURSOR              "\033[s"
#define MOVE_CURSOR              "\033[%d;%dH"
#define RESET_SCROLL             "\033[;r"
#define SET_SCROLL               "\033[%d;%dr"
#define SCROLL_UP                "\033[%dS"
#define SCROLL_DOWN              "\033[%dT"
#define BACKSPACE                "\b \b"
#define MOVE_CUR_UP              "\033[%dF"
#define MOVE_CUR_DOWN            "\033[%dE"
#define MOVE_CUR_LEFT            "\033[%dD"
#define MOVE_CUR_RIGHT           "\033[%dC"
#define ERASE_SCREEN             "\033[2J\033[0;0H"
#define ERASE_LINE               "\033[2K"
#define CHANGE_COLOR             "\033[%dm"
#define NEW_LINE                  "\r\n"
#define SET_WINDOW               "\033]2;\"%s\"ST"
#define SET_COLS_80              "\033[?3l"
#define SET_COLS_132             "\033[?3h"
#define MOVE_TO_COL              "\033[%dG"
#define HIDE_CURSOR              "\033[?25l"
#define SHOW_CURSOR              "\033[?25h"
#define ENABLE_LINE_WRAP         "\033[7h"
#define DISABLE_LINE_WRAP        "\033[7l"
#define RESET_TERMINAL           "\033c"
#define REPORT_WINDOW_TITLE      "\033[21t"
#define REPORT_WINDOW_SIZE       "\033[19t"

#define restore_cursor()         puts(RESTORE_CURSOR)
#define save_cursor()            puts(SAVE_CURSOR)
#define move_cursor(x, y)        printf(MOVE_CURSOR, y, x)
#define set_scroll(top, btm)     printf(SET_SCROLL, top, btm)
#define reset_scroll()           puts(RESET_SCROLL)
#define scroll_up(x)             printf(SCROLL_UP, x)
#define scroll_down(x)           printf(SCROLL_DOWN, x)
#define backspace()              puts(BACKSPACE)
#define move_cur_up(x)           printf(MOVE_CUR_UP, x)
#define move_cur_down(x)         printf(MOVE_CUR_DOWN, x)
#define move_cur_left(x)         printf(MOVE_CUR_LEFT, x)
#define move_cur_right(x)        printf(MOVE_CUR_RIGHT, x)
#define erase_screen()           puts(ERASE_SCREEN)
#define erase_line()             puts(ERASE_LINE)
#define erase_line_forward()     puts("\033[0K")
#define clear_screen()           erase_screen()
#define change_color(x)          printf(CHANGE_COLOR, x)
#define end_color()              change_color(0)
#define newline()                puts(NEW_LINE)
#define set_line_wrap(col)       printf("\033[7;%dh", col)
#define set_window(name)         printf(SET_WINDOW, name)

#define TERMINAL_WIDTH           80
#define TERMINAL_HEIGHT          63

#endif /* __TERM__ */
