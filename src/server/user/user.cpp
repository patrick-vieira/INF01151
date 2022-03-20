//
// Created by vieir on 08/03/2022.
//

#include "user.h"

User::User(string name) {
    this->name = name;
}

User::User(string name, list<string> followers_name, list<USER_SESSION> active_sessions) {
    this->name = name;
    this->followers_name = followers_name;
    this->active_sessions = active_sessions;
}

bool User::login(struct sockaddr_in cli_addr) {

    if(active_sessions.size() < 2){
        USER_SESSION as(cli_addr);
        active_sessions.push_back(as);
        return true;
    }

    return false;
}

void User::logout(struct sockaddr_in cli_addr) {

    USER_SESSION as(cli_addr);

    for (auto iter = active_sessions.begin(), end = active_sessions.end(); iter != end; ++iter) {
        if(strcmp(as.ip, iter->ip) == 0 && as.port == iter->port) {
            iter = active_sessions.erase(iter);
            pthread_cond_signal(&cond_message_avaliable);
        }
    }
}

void User::pingResponse(struct sockaddr_in cli_addr) {

    USER_SESSION as(cli_addr);

    for (auto iter = active_sessions.begin(), end = active_sessions.end(); iter != end; ++iter) {
        if (strcmp(as.ip, iter->ip) == 0 && as.port == iter->port) {
//            iter = active_sessions.erase(iter);
//            active_sessions.insert(iter, as);
            iter->setLastPingResponse();
            break;
        }
    }
}

list<USER_SESSION> User::getActiveSessions() {
    return active_sessions;
}

void User::addFollower(User* newFollower) {
    followers_name.push_back(newFollower->getName());
}

list<string> User::getFollowers() {
    return followers_name;
}

bool User::isConnected() {
    return !active_sessions.empty();
}

string User::getName()  {
    return name;
}

int User::active_sessions_count()  {
    return active_sessions.size();
}

void User::setConsumerThread(pthread_t consumer_thread) {
    pthread_cond_init(&cond_message_avaliable, NULL);
    this->consumer_thread = consumer_thread;
    this->consumer_thread_running = true;
}

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

    for (auto it = begin(this->followers_name); it != end(this->followers_name); ++it) {
        json follower;
        follower["follower"] = *it;
        json_followers.push_back(follower);
    }

    auto json_active_sessions = json::array();

    for (auto it = begin(this->active_sessions); it != end(this->active_sessions); ++it) {
        json json_address = {
                {"ip",   it->ip},
                {"port", it->port},
                {"last_ping_response", it->last_ping_response}
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

