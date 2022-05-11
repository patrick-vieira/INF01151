//
// Created by vieir on 07/03/2022.
//
#include <iostream>
#include "client.h"


int Client::start() {

    this->validateUser();
    gui.start();
    if (this->communicationManager.login(this->user)){
        this->StartMessageSenderThread();
        this->StartMessageListennerThread();
    } else {
        gui.main_window_add_line("Login error. ");
        this->communicationManager.logout();
        this->gui.stop();
    }

    this->WaitForMessageSenderToExit();
    this->WaitForMessageListennerToExit();

    this->communicationManager.logout();
    this->gui.stop();
    return 0;
}

bool Client::validateUser() {
    if(this->user.size() > 20 || this->user.size() <  4)
        exit(-1);
    return false;
}


void Client::MessageSenderImplementation() {

    int selected = gui.select_menu(login_success);

    while (running && selected < 0 || this->menu_choices[selected] != "Exit") {
        switch (selected) {
            case 0: {     // Write message
                string user_input_message = "Say something: ";
                string user_input = gui.request_user_input(user_input_message);

                if(user_input.size() > 200) {
                    gui.main_window_add_line("Message size exceed 200 characters - message size: " + to_string(user_input.size()));
                    break;
                }

                this->communicationManager.sendMessage(user_input);
                break;
            }
            case 1: {     // Follow User
                string message = "User name to follow: ";
                string user_input = gui.request_user_input(message);
                this->communicationManager.followUser(user_input);
                break;
            }
            default:
                break;

        }
        selected = gui.select_menu(login_success);
    }
    running = false;
}

void Client::MessageListennerImplementation() {
    sleep(1); //TODO remover pequno sleep para não escrever ao mesmo tempo na tela
    while(running) {
        json notification = this->communicationManager.notificationAvailable();

        switch (notification["type"].get<int>()) {
            case LOGIN_RESPONSE_SUCCESS:
                gui.main_window_add_line(notification["message"].get<string>());
                login_success = true;
                sleep(1);
                break;
            case LOGIN_RESPONSE_ERROR:
                gui.main_window_add_line(notification["message"].get<string>());
                gui.main_window_add_line("exiting in 5 seconds");
                running = false;
                break;
            case NOTIFICATION:
                gui.main_window_add_line(notification["message"].get<string>());
                break;
            case PING:
                this->communicationManager.pingReply();
                break;
            case NO_MESSAGE:
                break;
            default:
                gui.main_window_add_line("Unknown message from server: " + to_string(notification));
                break;
        }
    }
}

