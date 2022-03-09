//
// Created by vieir on 07/03/2022.
//

#include <iostream>
#include "client.h"

Client::Client(string user, Session session) : session(session), gui(user, menu_choices) {
    this->user = user;
}

int Client::start() {
    logger.message(INFO, "---> Client user:@%s starting on host: [%s] port: [%d]  <---", this->user.c_str(), this->session.getHost().c_str(), this->session.getPort());

    this->validateUser();

    session.openConnection();
    gui.start();

    int selected = gui.select_menu();

    while (this->menu_choices[selected] != "Exit") {
        switch (selected) {
            case 0: {     // Send Message
                this->messageSender();
                break;
            }
            case 1: {     // Follow User
                string message = "User name to follow: ";
                string user_input = gui.request_user_input(message);
                //TODO send to server;
                string server_response = session.sendMessage(user_input);
                gui.main_window_add_line("Server response [Follow]: " + server_response);
                break;
            }
            default:
                break;

        }
        selected = gui.select_menu();
    }

    gui.stop();
    session.closeConnection();
    return 0;
}

bool Client::validateUser() {
    return false;
}


bool Client::messageSender() {
    string user_input_message = "Say something: ";
    string user_input = gui.request_user_input(user_input_message);
    //TODO send to server;
    string server_response = session.sendMessage(user_input);
    gui.main_window_add_line("Server response [Message]: " + server_response);
    return false;
}

bool Client::follow(string user_name) {
    return false;
}

bool Client::messageListener() {
    return false;
}
