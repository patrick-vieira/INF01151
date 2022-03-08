//
// Created by vieir on 07/03/2022.
//

#include <iostream>
#include "client.h"

Client::Client(char * user, int port) {
    this->user = user;
    this->port = port;
}

int Client::start() {
    logger.message(INFO, "---> Client user:@%s starting on port: %d  <---", this->user, this->port);
    return 0;
}