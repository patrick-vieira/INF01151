//
// Created by vieir on 08/03/2022.
//

#include "user.h"

User::User(string name) {
    this->name = name;
}

User::User(string name, list<User> followers) {
    this->name = name;
    this->followers = followers;
}

bool User::newSession(struct sockaddr_in cli_addr) {
    if(sessions.size() < 2){
        sessions.push_back(cli_addr);
        return true;
    }

    return false;
}

list<struct sockaddr_in> * User::getSessions() {
    return &sessions;
}

void User::removeSession(list<struct sockaddr_in>::iterator it) {
    sessions.erase(it);

//    for (auto it = begin (sessions); it != end (sessions); ++it) {
//        if(*it == *it2)
//            sessions.erase(it);
//    }
}



json User::loginSuccess(sockaddr_in cli_addr, int sockfd) {
    json response_message_json;
    response_message_json["type"] = LOGIN_RESPONSE_SUCCESS;
    response_message_json["message"] = "Login success: Welcome " + this->getName();
    string flat_response_message = to_string(response_message_json);

    return response_message_json;

//    int n = sendto(sockfd, flat_response_message.c_str(), strlen(flat_response_message.c_str()), 0,
//                   (struct sockaddr *) &cli_addr, sizeof(struct sockaddr));
//    if (n < 0)
//        logger.message(ERROR, "ERROR sending login success response");
}

json User::loginFail(sockaddr_in cli_addr, int sockfd) {
    json response_message_json;
    response_message_json["type"] = LOGIN_RESPONSE_ERROR;
    response_message_json["message"] = "Login fail: Unable to connect with user: " + this->getName();
    string flat_response_message = to_string(response_message_json);

    return response_message_json;
//
//    int n = sendto(sockfd, flat_response_message.c_str(), strlen(flat_response_message.c_str()), 0,
//                   (struct sockaddr *) &cli_addr, sizeof(struct sockaddr));
//    if (n < 0)
//        logger.message(ERROR, "ERROR sending login fail response");
}

