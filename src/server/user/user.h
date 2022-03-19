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
    list<User*> followers;

    pthread_t consumer_thread = 0;

    bool consumer_thread_execute = false;

public:
    list<CLIENT_ADDRESS> active_sessions;
    pthread_cond_t 	cond_message_avaliable;

    User() {}
    User(string name);
    User(string name, list<User*> followers);

    list<User*> getFollowers();

    void addFollower(User* newFollower);
    void sendMessage(string message);

    string getName();
    bool login(struct sockaddr_in address);
    void logout(struct sockaddr_in address);

    int active_sessions_count();


    json loginSuccessMessage();
    json loginFailMessage();
    json followingSuccessMessage(string user_name);
    json followingFailMessage(string user_name);

    json notificationMessage(string message);

    list<CLIENT_ADDRESS> getActiveSessions();

    bool connected();

    json asJson();

    void setConsumerThread(pthread_t consumer_thread);


private:
    bool ping(CLIENT_ADDRESS cli_addr, int sockfd);
};


#endif //INF01151_TF_USER_H
