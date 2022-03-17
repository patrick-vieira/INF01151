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
//    this->WaitForMessageListennerToExit();

    this->communicationManager.logout();
    this->gui.stop();
    return 0;
}

bool Client::validateUser() {
    return false;
}


void Client::MessageSenderImplementation() {
    int selected = gui.select_menu();

    while (this->menu_choices[selected] != "Exit") {
        switch (selected) {
            case 0: {     // Write message
                string user_input_message = "Say something: ";
                string user_input = gui.request_user_input(user_input_message);

                this->communicationManager.sendMessage(user_input);
                break;
            }
            case 1: {     // Follow User
                string message = "User name to follow: ";
                string user_input = gui.request_user_input(message);
                //TODO Validar o tamanho maximo da mensagem 128;
                //TODO send to server;
                this->communicationManager.followUser(user_input);
                break;
            }
            default:
                break;

        }
        selected = gui.select_menu();
    }
}

void Client::MessageListennerImplementation() {
    sleep(1); //TODO remover pequno sleep para não escrever ao mesmo tempo na tela
    while(TRUE) {
        json notification = this->communicationManager.notificationAvailable();

        switch (notification["type"].get<int>()) {
            case LOGIN_RESPONSE_SUCCESS:
                gui.main_window_add_line(notification["message"].get<string>());
                break;
            case LOGIN_RESPONSE_ERROR:
                gui.main_window_add_line(notification["message"].get<string>());
                break;
            case NOTIFICATION:
                gui.main_window_add_line(notification["message"].get<string>());
                break;
            case PING:
                gui.main_window_add_line(notification["message"].get<string>());
                this->communicationManager.pingReply();
                break;
            default:
                gui.main_window_add_line("Unknown message from server: " + to_string(notification));
                break;
        }
        if (notification["type"] == LOGIN_RESPONSE_ERROR){
            break;
        }
    }
}

