#ifndef __CURSESHELP_H
#define __CURSESHELP_H

#if defined(__unix__) || FORCELINUX
    #include <curses.h>
#elif (defined(_WIN32) || defined(WIN32)) && !FORCELINUX
    #include <ncurses/curses.h>
#endif

WINDOW *createWin(int h, int w, int y, int x);
WINDOW *createCenteredWindow(int h, int w);
WINDOW *createCenteredWindowOff(int h, int w, int yoffset, int xoffset);
void destroyWin(WINDOW * win);
int hOffset(int offset);
void showTempMessage(int h, int w, char *message);
void showTempMessageS(char* message);

#ifdef CURSESHELP_IMPLEMENTATION

#include <stdlib.h>
#include <string.h>

WINDOW *createWin(int h, int w, int y, int x) {
	WINDOW* win;
	win = newwin(h, w, y, x);
	box(win, 0, 0);
	wrefresh(win);
	return win;
}

WINDOW *createCenteredWindow(int h, int w) {
	int height = h;
  	int width = w;
    int starty = (LINES - height) / 2;
    int startx = (COLS - width) / 2;
    return createWin(height, width, starty, startx);
}

WINDOW *createCenteredWindowOff(int h, int w, int yoffset, int xoffset) {
	int height = h;
  	int width = w;
    int starty = (LINES - height) / 2;
    int startx = (COLS - width) / 2;
    return createWin(height, width, (starty + yoffset), (startx + xoffset));
}

void destroyWin(WINDOW * win) {
	wclear(win);
	wrefresh(win);
	delwin(win);
    //free(win);
}

int vOffset(int offset) {
    return (LINES / 2) + offset;
}

int hOffset(int offset) {
    return (COLS / 2) + offset;
}

void showTempMessage(int h, int w, char *message) {
	WINDOW *tempwin = createCenteredWindow(h , w);
	box(tempwin, 0, 0);
	mvwprintw(tempwin, 1, 1, "%s", message);
	wrefresh(tempwin);
	wgetch(tempwin);
	destroyWin(tempwin);
}

void showTempMessageS(char* message) {
    showTempMessage(3, strlen(message) + 2, message);
}

#endif
#endif