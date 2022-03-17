//
// Created by vieir on 09/03/2022.
//

#ifndef INF01151_TF_SERVER_COMMUNICATION_MANAGER_H
#define INF01151_TF_SERVER_COMMUNICATION_MANAGER_H

#include <unistd.h>
#include <netinet/in.h>
#include <cstring>
#include <cstdlib>
#include <list>
#include <nlohmann/json.hpp>
#include "../../aux_shared/logger.h"
#include "../user/user.h"

using namespace std;

using json = nlohmann::json;

typedef struct {
    User destination;
    json payload;

    bool direct_response; // if true send to direct_address else to all user connections
    struct sockaddr_in direct_address;
} MESSAGE;


class ServerCommunicationManager {

private:
    Logger logger;

    int sockfd, n;

    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;

    list<User> users;


public:
    void openSocket(int port);
    void closeSocket();

    void messageReceiver();
    list<MESSAGE> messageReceiver2();
    void messageSender();

    User getOrCreateUser(string user_name);
    bool updateUser(User user);
    void saveUsers();

    bool newSession(User user, sockaddr_in cli_addr);
};


#endif //INF01151_TF_SERVER_COMMUNICATION_MANAGER_H
