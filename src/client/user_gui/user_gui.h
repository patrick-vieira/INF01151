//
// Created by vieir on 08/03/2022.
//

#ifndef INF01151_TF_USER_GUI_H
#define INF01151_TF_USER_GUI_H

#include <ncurses.h>
#include <string>
#include <vector>
#include <curses.h>

using namespace std;


class UserGUI {

private:

    int yMax, xMax;

    int menu_selected = 0;

    vector<string> main_win_content;
    WINDOW* main_win;
    int main_win_lines = 12;

    WINDOW* input_win;
    int input_win_lines = 3;

    WINDOW* menu_win;
    int menu_win_lines = 5;



public:
    UserGUI(string user_name, string menu_choices[]) {
        this->user_name = user_name;
        this->menu_choices = menu_choices;
    };
    void start();
    void stop();
    int select_menu();

    string request_user_input(string message);
    void main_window_add_line(string new_line);
    void refresh_main_window();

private:
    string getstring(WINDOW *window);

    string *menu_choices;
    string user_name;

    void createMainWindow(int yMax, int xMax);

    void createMenuWindow(int yMax, int xMax);

    void createUserInputWindow(int yMax, int xMax);
};


#endif //INF01151_TF_USER_GUI_H
