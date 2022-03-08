//
// Created by vieir on 07/03/2022.
//

#include <iostream>
#include "client.h"

Client::Client(int id, int port) {
    this->id = id;
    this->port = port;
}

int Client::start() {
    logger.message(INFO, "---> Client id:[%d] starting on port: %d  <---", this->id, this->port);
    return 0;
}