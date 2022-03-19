//
// Created by vieir on 09/03/2022.
//

#ifndef INF01151_TF_SESSION_H
#define INF01151_TF_SESSION_H

#include <iostream>
#include <netinet/in.h>
#include <netdb.h>
#include <cstring>
#include <unistd.h>

#include "../../aux_shared/logger.h"

#include <nlohmann/json.hpp>

using json = nlohmann::json;

class Session {
    Logger logger;

    int port;
    string host;

    int sockfd, n;
    unsigned int length;
    struct sockaddr_in serv_addr, from;
    struct hostent *server;



public:
    Session(string host, int port) {
        this->host = host;
        this->port = port;
    }

    bool openConnection();
    void sendMessage(json message);
    json receiveMessage();
    void closeConnection();

    string getHost();
    int getPort();

};


#endif //INF01151_TF_SESSION_H
