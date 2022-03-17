//
// Created by vieir on 08/03/2022.
//

#ifndef INF01151_TF_USER_H
#define INF01151_TF_USER_H


#include <list>
#include <string>
#include <netinet/in.h>
#include <nlohmann/json.hpp>
#include <arpa/inet.h>
#include "../../aux_shared/message_types.h"
#include "../../aux_shared/logger.h"

using namespace std;

using json = nlohmann::json;

typedef struct client_address {
    char ip[INET_ADDRSTRLEN];
    int port;

    struct sockaddr_in sockaddrIn(){
        struct sockaddr_in temp_address;
        // recreate socket address
        inet_pton(AF_INET, ip, &(temp_address.sin_addr));
        temp_address.sin_port = htons(port);
        temp_address.sin_family = AF_INET;
        return temp_address;
    }

    struct client_address fromSockaddrIn(struct sockaddr_in cli_addr){
        inet_ntop(AF_INET, &(cli_addr.sin_addr), this->ip, INET_ADDRSTRLEN);
        this->port = htons(cli_addr.sin_port);

        return *this;
    }
} CLIENT_ADDRESS;

class User {
    Logger logger;

    string name;
    list<User> followers;

    list<CLIENT_ADDRESS> active_sessions;

public:
    User() {}
    User(string name);
    User(string name, list<User> followers);

    void addFollower(User newFollower);

    void sendMessage(string message);

    string getName() {
        return name;
    }

    bool newSession(struct sockaddr_in address, int sockfd);

    int active_sessions_count() {
        return active_sessions.size();
    }

    json asJson();

    json loginSuccessMessage();

    json loginFail();

    list<CLIENT_ADDRESS> *getActiveSessions(int sockfd);
private:
    bool ping(CLIENT_ADDRESS cli_addr, int sockfd);
};


#endif //INF01151_TF_USER_H
