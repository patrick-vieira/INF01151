//
// Created by vieir on 10/03/2022.
//

#include "runner.h"

void* Runner::runSender(void *arg) {

    gui.start();

    int selected = gui.select_menu();

    while (this->menu_choices[selected] != "Exit") {
        switch (selected) {
            case 0: {     // Send Message
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
//                string server_response = session.sendMessage(user_input);
//                gui.main_window_add_line("Server response [Follow]: " + server_response);
                this->communicationManager.followUser(user_input);
                break;
            }
            default:
                break;

        }
        selected = gui.select_menu();
    }
}

void* Runner::runReceiver(void *arg) {
    while(TRUE) {
        sleep(rand() % 5);

        string notification = this->communicationManager.notificationAvailable();
        gui.main_window_add_line("New message from  [@user_name]: " + notification);

    }
}
