//
// Created by vieir on 08/03/2022.
//

#include "user_menu.h"

UserMenu::UserMenu() {

}

string getstring(WINDOW* window)
{
    std::string input;

    // let the terminal do the line editing
    nocbreak();
    echo();

    // this reads from buffer after <ENTER>, not "raw"
    // so any backspacing etc. has already been taken care of
    int ch = wgetch(window);

    while ( ch != '\n' )
    {
        input.push_back( ch );
        ch = getch();
    }

    // restore your cbreak / echo settings here

    return input;
}
void UserMenu::start(string user_name, string host, int port) {
    // start ncurses
    initscr();
    noecho();
    cbreak();

    // screen size
    int yMax, xMax;
    getmaxyx(stdscr, yMax, xMax);

    // create window menu
    WINDOW * menuwin = newwin(6, xMax-12, yMax-8, 5);
    box(menuwin, 0, 0);
    refresh();
    wrefresh(menuwin);
    mvwprintw(menuwin, 0, (xMax/2) - 8, "  Menu  ");
    keypad(menuwin, true);

    // create window menu
    WINDOW * mainwin = newwin(yMax/2, xMax-12, 2, 5);
    box(mainwin, 0, 0);
    refresh();
    wrefresh(mainwin);
    keypad(menuwin, true);

    // menu options
    string choices[3] = {"Write message", "Follow user", "Exit"};
    int choice;
    int selected = 0;


    bool selecting_menu = true;

    while (selecting_menu) {
        // print menu
        for (int i = 0; i < 3; i++) {
            if (i == selected)
                wattron(menuwin, A_REVERSE); // highligh
            int y = i + 1, x = 1;
            mvwprintw(menuwin, y, x, choices[i].c_str());
            wattroff(menuwin, A_REVERSE);  // highligh off
        }

        choice = wgetch(menuwin);

        switch (choice) {
            case KEY_UP:
                selected--;
                if (selected < 0) selected = 0;
                break;
            case KEY_DOWN:
                selected++;
                if (selected > 2) selected = 2;
                break;
            default:
                break;
        }

        if (choice == 10) {   // 10 is the enter key
            selecting_menu = false;
            choice = -1;
        }
    }



    bool running = true;

    while(running) {
        int y = 1;
        int x = 1;

        switch (selected) {
            case 0: {
                wclear(mainwin);
                box(mainwin, 0, 0);

                mvwprintw(mainwin, y, x, "Enter a message: ");
                string message = getstring(mainwin);

                mvwprintw(mainwin, ++y, x, "message sent: %s", message.c_str());
                mvwprintw(mainwin, ++++y, x, "Press Enter to send another message.", message.c_str());
                wrefresh(mainwin);

                getch();
                break;
            }
            case 1: {
                move(y + 1, x);
                printw("Enter the user name: ");
                string user = getstring(mainwin);

                refresh();
                move(y + 2, x);
                printw("following: %s", user.c_str());
                break;
            }
            case 2: {
                running = false;
                break;
            }
            default:
                break;
        }
    }


    // le
    getch();

    endwin();
}

