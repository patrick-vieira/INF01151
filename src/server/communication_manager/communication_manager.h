//
// Created by vieir on 09/03/2022.
//

#ifndef INF01151_TF_COMMUNICATION_MANAGER_H
#define INF01151_TF_COMMUNICATION_MANAGER_H

#include <unistd.h>
#include <netinet/in.h>
#include <cstring>
#include <cstdlib>
#include "../../aux_shared/logger.h"

using namespace std;

class CommunicationManager {

private:
    Logger logger;

    int sockfd, n;

    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;

public:
    void openSocket(int port);
    void closeSocket();

    void messageReceiver();
    void messageSender();
};


#endif //INF01151_TF_COMMUNICATION_MANAGER_H
