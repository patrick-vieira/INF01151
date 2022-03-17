//
// Created by vieir on 07/03/2022.
//

#ifndef INF01151_TF_CLIENT_H
#define INF01151_TF_CLIENT_H


#include "../aux_shared/logger.h"
#include "session/session.h"
#include "user_gui/user_gui.h"
#include "client_communication_manager/client_communication_manager.h"

#include "client_thread_runner.h"

using namespace std;

class Session;
class UserGUI;

class Client : ClientThreadRunner {

    Logger logger;
    string menu_choices[3] = {"Write message", "Follow user", "Exit"};

private:
    string user;
    ClientCommunicationManager communicationManager;
    UserGUI gui;

public:
    Client(string user, Session session) : communicationManager(session), gui(user, menu_choices) {
        this->user = user;
        logger.message(INFO, "---> Client user:@%s starting on host: [%s] port: [%d]  <---", this->user.c_str(), session.getHost().c_str(), session.getPort());
    }

    int start();

    void MessageSenderImplementation() override;
    void MessageListennerImplementation() override;


private:

    /** validateUser()
      valida o numero de caracteres no nome do user
      min 4 max 20    **/
    bool validateUser();
};

#endif //INF01151_TF_CLIENT_H