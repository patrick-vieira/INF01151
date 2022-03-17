//
// Created by vieir on 08/03/2022.
//

#ifndef INF01151_TF_USER_H
#define INF01151_TF_USER_H


#include <list>
#include <string>
#include <netinet/in.h>
#include <nlohmann/json.hpp>
#include "../../aux_shared/message_types.h"
#include "../../aux_shared/logger.h"

using namespace std;

using json = nlohmann::json;

class User {
    Logger logger;

    string name;
    list<User> followers;

    list<struct sockaddr_in> sessions;
    list<int> tests;

    int uuid = rand()%100;

public:
    User() {}
    User(string name);
    User(string name, list<User> followers);
    // Copy constructor
//    User(const User &other) {
//        this->name = other.name;
//        this->followers = other.followers;
//        std::copy(other.sessions.begin(), other.sessions.end(), this->sessions);
//        this->uuid = other.uuid;
//    }

    void addFollower(User newFollower);

    void sendMessage(string message);

    string getName() {
        return name;
    }

    bool newSession(struct sockaddr_in address);

    int active_sessions_count() {
        return sessions.size();
    }

//    void removeSession(struct sockaddr_in address){
//        sessions.remove(address);
//    }

    json asJson(){

        auto json_followers = json::array();

        for (auto it = begin (this->followers); it != end (this->followers); ++it) {
            int index = distance(followers.begin(), it);
            User follower = *it;
            json_followers.push_back(follower.getName());
        }

        json json_user = {
            {"name", name},
            {"followers", json_followers}
        };

        return json_user;
    }

    json loginSuccess(sockaddr_in sender_cli_addr, int sockfd);

    json loginFail(sockaddr_in in, int i);

    void removeSession(list<struct sockaddr_in>::iterator it);

    list<struct sockaddr_in> * getSessions();
};


#endif //INF01151_TF_USER_H
