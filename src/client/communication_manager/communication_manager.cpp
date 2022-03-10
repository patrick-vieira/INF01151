//
// Created by vieir on 09/03/2022.
//

#include "communication_manager.h"

bool CommunicationManager::login(string user_name) {

    this->session.openConnection();

    json message;
    message["type"] = LOGIN_REQUEST;
    message["user"] = user_name;

    this->session.sendMessage(message);

    return true;
}

bool CommunicationManager::sendMessage(string user_input) {
    json message;
    message["type"] = NEW_MESSAGE;
    message["message"] = user_input;

    this->session.sendMessage(message);
    return false;
}


bool CommunicationManager::followUser(string user_input) {
    json message;
    message["type"] = FOLLOW_REQUEST;
    message["user_name"] = user_input;

    this->session.sendMessage(message);


    return false;
}

json CommunicationManager::notificationAvailable() {
    json response = session.receiveMessage();
    return response;
}

void CommunicationManager::logout() {
    this->session.closeConnection();
}

