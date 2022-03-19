//
// Created by vieir on 09/03/2022.
//

#include "client_communication_manager.h"

bool ClientCommunicationManager::login(string user_name) {

    this->session.openConnection();
    logged_user_name = user_name;

    json message;
    message["type"] = LOGIN_REQUEST;
    message["user"] = logged_user_name;

    this->session.sendMessage(message);

    // TODO verificar se mensagem chegou no server, se não false;
    return true;
}

void ClientCommunicationManager::logout() {
    json message;
    message["type"] = LOGOUT_REQUEST;
    message["user"] = logged_user_name;

    this->session.sendMessage(message);
    this->session.closeConnection();
}

bool ClientCommunicationManager::sendMessage(string user_input) {
    json message;
    message["type"] = NEW_MESSAGE;
    message["user"] = logged_user_name;
    message["message"] = user_input;

    this->session.sendMessage(message);
    return false;
}


bool ClientCommunicationManager::followUser(string user_input) {
    json message;
    message["type"] = FOLLOW_REQUEST;
    message["user"] = logged_user_name;
    message["user_name"] = user_input;

    this->session.sendMessage(message);
    return false;
}

bool ClientCommunicationManager::pingReply() {
    json message;
    message["type"] = PING;
    message["user"] = logged_user_name;

    this->session.sendMessage(message);
    return false;
}

json ClientCommunicationManager::notificationAvailable() {
    json response = session.receiveMessage();
    return response;
}

