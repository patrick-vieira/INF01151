//
// Created by vieir on 09/03/2022.
//

#include "client_communication_manager.h"

bool ClientCommunicationManager::login(string user_name) {

    this->session.openConnection();

    json message;
    message["type"] = LOGIN_REQUEST;
    message["user"] = user_name;

    this->session.sendMessage(message);

    // TODO verificar se mensagem chegou no server, se não false;
    return true;
}

bool ClientCommunicationManager::sendMessage(string user_input) {
    json message;
    message["type"] = NEW_MESSAGE;
    message["message"] = user_input;

    this->session.sendMessage(message);
    return false;
}


bool ClientCommunicationManager::followUser(string user_input) {
    json message;
    message["type"] = FOLLOW_REQUEST;
    message["user_name"] = user_input;

    this->session.sendMessage(message);
    return false;
}

bool ClientCommunicationManager::pingReply() {
    json message;
    message["type"] = PING;

    this->session.sendMessage(message);
    return false;
}

json ClientCommunicationManager::notificationAvailable() {
    json response = session.receiveMessage();
    return response;
}

void ClientCommunicationManager::logout() {
    this->session.closeConnection();
}

