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
#include <arpa/inet.h>

#include "../logger.h"

#include "nlohmann/json.hpp"

using json = nlohmann::json;

typedef struct client_address {

    char ip[INET_ADDRSTRLEN];
    int port;

    client_address(){};

    client_address(const char *ip, int port) {
        strncpy(this->ip, ip,INET_ADDRSTRLEN);
        this->port = port;
    }

    client_address(struct sockaddr_in cli_addr){
        inet_ntop(AF_INET, &(cli_addr.sin_addr), this->ip, INET_ADDRSTRLEN);
        this->port = htons(cli_addr.sin_port);
    };

    struct sockaddr_in sockaddrIn(){
        struct sockaddr_in temp_address;
        // recreate socket address
        inet_pton(AF_INET, ip, &(temp_address.sin_addr));
        temp_address.sin_port = htons(port);
        temp_address.sin_family = AF_INET;
        return temp_address;
    }


} ADDRESS;

class Session {
    Logger logger;

    int port;
    string host;
    int timeout = 0;

    int sockfd, n;
    unsigned int length;
    struct sockaddr_in serv_addr, from;
    struct hostent *server;



public:
    Session() {}

    Session(string host, int port) {
        this->host = host;
        this->port = port;
    }

    Session(string host, int port, int timeout) {
        this->host = host;
        this->port = port;
        this->timeout = timeout;
    }

    bool openConnection();
    int sendMessage(json message);
    json receiveMessage();
    void closeConnection();

    string getHost();
    int getPort();

};


#endif //INF01151_TF_SESSION_H
