//
// Created by vieir on 10/03/2022.
//

#ifndef INF01151_TF_RUNNER_H
#define INF01151_TF_RUNNER_H

#include "../user_gui/user_gui.h"
#include "../communication_manager/communication_manager.h"

class Runner {

    CommunicationManager communicationManager;
    UserGUI gui;

    string menu_choices[3] = {"Write message", "Follow user", "Exit"};

public:
    Runner(CommunicationManager cm, UserGUI gui, string user) : communicationManager(cm), gui(gui) {
        printf("ok");
        this->communicationManager.login(user);
    }

    void* runSender(void *arg);
    void* runReceiver(void *arg);

};


#endif //INF01151_TF_RUNNER_H
