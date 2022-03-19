//
// Created by vieir on 09/03/2022.
//

#ifndef INF01151_TF_SERVER_COMMUNICATION_MANAGER_H
#define INF01151_TF_SERVER_COMMUNICATION_MANAGER_H

#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <list>
#include <nlohmann/json.hpp>
#include "../../aux_shared/message_types.h"
#include "../../aux_shared/logger.h"
#include "../user/user.h"

using namespace std;

using json = nlohmann::json;

typedef struct {
    User* sender;
    User* receiver;

    json payload;
    time_t now = time(0);

    bool direct_response = false; // if true send to direct_response_address else to all user connections
    struct sockaddr_in direct_response_address;
} MESSAGE;


class ServerCommunicationManager {

private:
    Logger logger;

    int sockfd;

    socklen_t clilen;
    struct sockaddr_in serv_addr;

    list<User*> users;


public:
    void openSocket(int port);
    void closeSocket();

    bool messageSender(MESSAGE message);
    list<MESSAGE> messageReceiver();

    User* getOrCreateUser(const string& user_name);
    void saveUsers();

    bool newSession(User* user, sockaddr_in cli_addr);
    bool ping(CLIENT_ADDRESS cli_addr);

    pair<bool, User*> getUser(const string& user_name);

    void pingAll();
};


#endif //INF01151_TF_SERVER_COMMUNICATION_MANAGER_H
