//
// Created by vieir on 08/03/2022.
//

#include "user.h"

User::User(string name) {
    this->name = name;
}

User::User(string name, list<User*> followers) {
    this->name = name;
    this->followers = followers;
}

bool User::login(struct sockaddr_in cli_addr) {

    if(active_sessions.size() < 2){
        CLIENT_ADDRESS as;
        as.fromSockaddrIn(cli_addr);
        active_sessions.push_back(as);
        return true;
    }

    return false;
}

void User::logout(struct sockaddr_in cli_addr) {

    CLIENT_ADDRESS as;
    as.fromSockaddrIn(cli_addr);

    for (auto iter = active_sessions.begin(), end = active_sessions.end(); iter != end; ++iter) {
        if(strcmp(as.ip, iter->ip) && as.port, iter->port) {
            iter = active_sessions.erase(iter);
        }
    }
    consumer_thread_execute = !active_sessions.empty();
}

list<CLIENT_ADDRESS> User::getActiveSessions() {
    // remove disconected users;
//    for (auto iter = active_sessions.begin(), end = active_sessions.end(); iter != end; ++iter) {
//        if(!ping(*iter, sockfd))
//            iter = active_sessions.erase(iter);
//    }

    return active_sessions;
}

// NOT WORK, reason: send ping works fine, the problem is with the response, its received before on the producer trhead..
//bool User::ping(CLIENT_ADDRESS cli_addr, int sockfd){
//    json response_message_json;
//    response_message_json["type"] = PING;
//    response_message_json["message"] = "[SERVER]: Ping";
//    string flat_response_message = to_string(response_message_json);
//
//    sockaddr_in cli_addr_to_ping = cli_addr.sockaddrIn();
//
//    int n = sendto(sockfd, flat_response_message.c_str(), strlen(flat_response_message.c_str()), 0 ,(struct sockaddr *) &cli_addr_to_ping, sizeof(struct sockaddr));
//    if (n  < 0) {
//        logger.message(ERROR, "ERROR ping not send.");
//        return false;
//    }
//
//    char* ping_response_buffer = (char*) calloc(256, sizeof(char));
//    socklen_t  socklen = sizeof(struct sockaddr_in);
//    n = recvfrom(sockfd, ping_response_buffer, 256, 0, (struct sockaddr *) &cli_addr, &socklen);
//    if (n < 0){
//        logger.message(ERROR, "ERROR ping response not received.");
//        return false;
//    }
//
//    return true;
//}

json User::loginSuccessMessage() {
    json response_message_json;
    response_message_json["type"] = LOGIN_RESPONSE_SUCCESS;
    response_message_json["message"] = "Login success: Welcome " + this->getName();
    string flat_response_message = to_string(response_message_json);

    return response_message_json;
}


json User::loginFailMessage() {
    json response_message_json;
    response_message_json["type"] = LOGIN_RESPONSE_ERROR;
    response_message_json["message"] = "Login fail: Unable to connect with user: " + this->getName();
    string flat_response_message = to_string(response_message_json);

    return response_message_json;
}

json User::followingSuccessMessage(string user_name){
    json response_message_json;
    response_message_json["type"] = NOTIFICATION;
    response_message_json["message"] = "You are now following the user: " + user_name;
    string flat_response_message = to_string(response_message_json);

    return response_message_json;
}

json User::followingFailMessage(string user_name){
    json response_message_json;
    response_message_json["type"] = NOTIFICATION;
    response_message_json["message"] = "User not found: " + user_name;
    string flat_response_message = to_string(response_message_json);

    return response_message_json;
}

json User::notificationMessage(string message) {
    json response_message_json;
    response_message_json["type"] = NOTIFICATION;
    response_message_json["message"] = "[" + name +"]: " + message;
    string flat_response_message = to_string(response_message_json);

    return response_message_json;
}

json User::asJson() {
    auto json_followers = json::array();

    for (auto it = begin(this->followers); it != end(this->followers); ++it) {
        int index = distance(followers.begin(), it);
        User* follower = *it;
        json_followers.push_back(follower->getName());
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

void User::addFollower(User* newFollower) {
    followers.push_back(newFollower);
}

list<User *> User::getFollowers() {
    return followers;
}

bool User::connected() {
    return consumer_thread_execute;
}

string User::getName()  {
    return name;
}

int User::active_sessions_count()  {
    return active_sessions.size();
}

void User::setConsumerThread(pthread_t consumer_thread) {
    this->consumer_thread = consumer_thread;
    this->consumer_thread_execute = true;
}



