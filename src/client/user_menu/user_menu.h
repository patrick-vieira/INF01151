//
// Created by vieir on 08/03/2022.
//

#ifndef INF01151_TF_USER_MENU_H
#define INF01151_TF_USER_MENU_H

#include <ncurses.h>
#include <string>

using namespace std;

class UserMenu {

public:
    UserMenu();

    void start(string user_name, string host, int port);
};


#endif //INF01151_TF_USER_MENU_H
