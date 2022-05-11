//
// Created by vieir on 09/03/2022.
//

#ifndef INF01151_TF_SERVER_COMMUNICATION_MANAGER_H
#define INF01151_TF_SERVER_COMMUNICATION_MANAGER_H

#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <thread>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <list>
#include <nlohmann/json.hpp>
#include "../../aux_shared/message_types.h"
#include "../../aux_shared/logger.h"
#include "../user/user.h"
#include "../persistence/serverMemory.h"
#include "../replica_manager/replica_manager.h"

using namespace std;

using json = nlohmann::json;

typedef struct {
    User* sender;
    User* receiver;

    json payload;
    time_t now = time(0);

    bool direct_response = false; // if true send to direct_response_address with priority else send to all user sessions
    struct sockaddr_in direct_response_address;
} MESSAGE;


class ServerCommunicationManager {

private:
    Logger logger;

    int sockfd;

    socklen_t clilen;
    struct sockaddr_in serv_addr;

    ServerPersistence* persistence;
    ReplicaManager* replicaManager;

public:
    ServerCommunicationManager(ServerPersistence* persistence, ReplicaManager* replicaManager);
    void openSocket(int port);
    void closeSocket();

    bool messageSender(MESSAGE message);
    list<MESSAGE> messageReceiver();

    void pingAll();

private:
    bool sendMessage(const MESSAGE &message);
    bool sendDirectMessage(const MESSAGE &message);

    bool ping(USER_SESSION cli_addr);
};


#endif //INF01151_TF_SERVER_COMMUNICATION_MANAGER_H
