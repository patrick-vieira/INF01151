//
// Created by vieir on 08/03/2022.
//

#include "user_menu.h"

string UserGUI::getstring(WINDOW* window)
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

//void UserGUI::start(string user_name, string host, int port) {
//    // start ncurses
//    initscr();
//    noecho();
//    cbreak();
//
//    // screen size
//    int yMax, xMax;
//    getmaxyx(stdscr, yMax, xMax);
//
//    // create window menu
//    WINDOW * menuwin = newwin(6, xMax-12, yMax-8, 5);
//    box(menuwin, 0, 0);
//    refresh();
//    wrefresh(menuwin);
//    mvwprintw(menuwin, 0, (xMax/2) - 8, "  Menu  ");
//    keypad(menuwin, true);
//
//    // create window menu
//    WINDOW * mainwin = newwin(yMax/2, xMax-12, 2, 5);
//    box(mainwin, 0, 0);
//    refresh();
//    wrefresh(mainwin);
//    keypad(menuwin, true);
//
//    // menu options
//    string choices[3] = {"Write message", "Follow user", "Exit"};
//    int choice;
//    int selected = 0;
//
//
//    bool selecting_menu = true;
//
//    while (selecting_menu) {
//        // print menu
//        for (int i = 0; i < 3; i++) {
//            if (i == selected)
//                wattron(menuwin, A_REVERSE); // highligh
//            int y = i + 1, x = 1;
//            mvwprintw(menuwin, y, x, choices[i].c_str());
//            wattroff(menuwin, A_REVERSE);  // highligh off
//        }
//
//        choice = wgetch(menuwin);
//
//        switch (choice) {
//            case KEY_UP:
//                selected--;
//                if (selected < 0) selected = 0;
//                break;
//            case KEY_DOWN:
//                selected++;
//                if (selected > 2) selected = 2;
//                break;
//            case 10:   // 10 is the enter key
//                selecting_menu = false;
//                choice = -1;
//                break;
//            default:
//                break;
//        }
//    }
//
//
//
//    bool running = true;
//
//    while(running) {
//        int y = 1;
//        int x = 1;
//
//        switch (selected) {
//            case 0: {
//                wclear(mainwin);
//                box(mainwin, 0, 0);
//
//                mvwprintw(mainwin, y, x, "Enter a message: ");
//                string message = getstring(mainwin);
//
//                mvwprintw(mainwin, ++y, x, "message sent: %s", message.c_str());
//                mvwprintw(mainwin, ++++y, x, "Press Enter to send another message.", message.c_str());
//                wrefresh(mainwin);
//
//                getch();
//                break;
//            }
//            case 1: {
//                move(y + 1, x);
//                printw("Enter the user name: ");
//                string user = getstring(mainwin);
//
//                refresh();
//                move(y + 2, x);
//                printw("following: %s", user.c_str());
//                break;
//            }
//            case 2: {
//                running = false;
//                break;
//            }
//            default:
//                break;
//        }
//    }
//
//
//    // le
//    getch();
//
//    endwin();
//}



void UserGUI::start() {
    // start ncurses
    initscr();
    noecho();
    cbreak();

    // screen size
    int yMax, xMax;
    getmaxyx(stdscr, yMax, xMax);

    // create window main
    this->createMainWindow(yMax, xMax);

    // create window user input
    this->createUserInputWindow(yMax, xMax);

    // create window menu
    this->createMenuWindow(yMax, xMax);

}

void UserGUI::createMainWindow(int yMax, int xMax) {
    // create window main
    this->main_win = newwin(this->main_win_lines, xMax-12, 2, 5);
    box(this->main_win, 0, 0);
    refresh();

    string window_title_text = this->client->getUser();
    int window_title_pos = (xMax/2) - window_title_text.length() ;
    mvwprintw(this->main_win, 0, window_title_pos, "  %s  ", window_title_text.c_str());

    wrefresh(this->main_win);
    keypad(this->main_win, true);
}

void UserGUI::createUserInputWindow(int yMax, int xMax) {
    // create window input
    this->input_win = newwin(this->input_win_lines, xMax-12, this->main_win_lines + 2, 5);
    box(this->input_win, 0, 0);
    refresh();
    string window_title_text = "User Input";
    int window_title_pos = (xMax/2) - window_title_text.length();
    mvwprintw(this->input_win, 0, window_title_pos, "  %s  ", window_title_text.c_str());

    wrefresh(this->input_win);
    keypad(this->input_win, true);
}

void UserGUI::createMenuWindow(int yMax, int xMax) {
    // create window input
    this->menu_win = newwin(this->menu_win_lines, xMax-12, this->main_win_lines + this->input_win_lines + 2, 5);
    box(this->menu_win, 0, 0);
    refresh();
    string window_title_text = "Menu";
    int window_title_pos = (xMax/2) - window_title_text.length();
    mvwprintw(this->menu_win, 0, window_title_pos, "  %s  ", window_title_text.c_str());

    wrefresh(this->menu_win);
    keypad(this->menu_win, true);
}


void UserGUI::stop() {
    getch();
    endwin();
}

int UserGUI::select_menu() {
    bool selecting_menu = true;

    int choice;

    while (selecting_menu) {
        // print menu
        for (int i = 0; i < 3; i++) {
            if (i == this->menu_selected)
                wattron(this->menu_win, A_REVERSE); // highligh
            int y = i + 1, x = 1;
            mvwprintw(this->menu_win, y, x, this->menu_choices[i].c_str());
            wattroff(this->menu_win, A_REVERSE);  // highligh off
        }

        choice = wgetch(this->menu_win);

        switch (choice) {
            case KEY_UP:
                this->menu_selected--;
                if (this->menu_selected < 0) this->menu_selected = 0;
                break;
            case KEY_DOWN:
                this->menu_selected++;
                if (this->menu_selected > 2) this->menu_selected = 2;
                break;
            case 10:   // 10 is the enter key
                selecting_menu = false;
                choice = -1;
                break;
            default:
                break;
        }
    }

    return this->menu_selected;
}

string UserGUI::request_user_input(string message) {

    int y = 1;
    int x = 1;

    wclear(this->input_win);
    box(this->input_win, 0, 0);

    mvwprintw(this->input_win, y, x, message.c_str());
    string user_input = getstring(this->input_win);

    return user_input;
}

void UserGUI::main_window_add_line(string new_line) {
    this->main_win_content.push_back(new_line);
    this->refresh_main_window();
}

void UserGUI::refresh_main_window() {
    int y = 1;
    int x = 1;

    wclear(this->main_win);
    box(this->main_win, 0, 0);

    for (auto it = this->main_win_content.begin(); it != this->main_win_content.end(); ++it) {
        int index = std::distance(this->main_win_content.begin(), it);
        mvwprintw(this->main_win, y+index, x, it->data());
    }

    wrefresh(this->main_win);
}



