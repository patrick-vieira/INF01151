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

bool User::newSession(struct sockaddr_in cli_addr, int sockfd) {

    if(getActiveSessions(sockfd)->size() < 2){
        CLIENT_ADDRESS as;
        as.fromSockaddrIn(cli_addr);
        active_sessions.push_back(as);
        return true;
    }

    return false;
}

list<CLIENT_ADDRESS> * User::getActiveSessions(int sockfd) {
    // remove disconected users;
    for (auto iter = active_sessions.begin(), end = active_sessions.end(); iter != end; ++iter) {
        if(!ping(*iter, sockfd))
            iter = active_sessions.erase(iter);
    }

    return &active_sessions;
}


bool User::ping(CLIENT_ADDRESS cli_addr, int sockfd){
    json response_message_json;
    response_message_json["type"] = PING;
    response_message_json["message"] = "[SERVER]: Ping";
    string flat_response_message = to_string(response_message_json);

    sockaddr_in cli_addr_to_ping = cli_addr.sockaddrIn();

    int n = sendto(sockfd, flat_response_message.c_str(), strlen(flat_response_message.c_str()), MSG_CONFIRM ,(struct sockaddr *) &cli_addr_to_ping, sizeof(struct sockaddr));
    if (n  < 0) {
        logger.message(ERROR, "ERROR ping not send.");
        return false;
    }

    char* ping_response_buffer = (char*) calloc(256, sizeof(char));
    socklen_t  socklen = sizeof(struct sockaddr_in);
    n = recvfrom(sockfd, ping_response_buffer, 256, 0, (struct sockaddr *) &cli_addr, &socklen);
    if (n < 0){
        logger.message(ERROR, "ERROR ping response not received.");
        return false;
    }

    return true;
}

json User::loginSuccessMessage() {
    json response_message_json;
    response_message_json["type"] = LOGIN_RESPONSE_SUCCESS;
    response_message_json["message"] = "Login success: Welcome " + this->getName();
    string flat_response_message = to_string(response_message_json);

    return response_message_json;
}


json User::loginFail() {
    json response_message_json;
    response_message_json["type"] = LOGIN_RESPONSE_ERROR;
    response_message_json["message"] = "Login fail: Unable to connect with user: " + this->getName();
    string flat_response_message = to_string(response_message_json);

    return response_message_json;
}

json User::asJson() {
    auto json_followers = json::array();

    for (auto it = begin(this->followers); it != end(this->followers); ++it) {
        int index = distance(followers.begin(), it);
        User follower = *it;
        json_followers.push_back(follower.getName());
    }

    auto json_active_sessions = json::array();

    for (auto it = begin(this->active_sessions); it != end(this->active_sessions); ++it) {
        json json_address = {
                {"ip",   it->ip},
                {"port", it->port}
        };
        json_active_sessions.push_back(json_address);
    }

    json json_user = {
            {"name",            name},
            {"followers",       json_followers},
            {"active_sessions", json_active_sessions}
    };

    return json_user;
}


