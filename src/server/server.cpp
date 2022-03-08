//
// Created by vieir on 07/03/2022.
//

#include "server.h"


Server::Server(int port) {
    this->port = port;
}

int Server::start() {
    logger.message(INFO,"---> Server starting on port: %d  <---", this->port);


    return 0;
}