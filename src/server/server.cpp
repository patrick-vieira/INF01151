//
// Created by vieir on 07/03/2022.
//


#include "server.h"
#include "communication_manager/communication_manager.h"


Server::Server(int port) {
    this->port = port;
}

int Server::start() {
    logger.message(INFO,"---> Server starting on port: %d  <---", this->port);

    CommunicationManager communicationManager;

    communicationManager.openSocket(port);

    communicationManager.messageReceiver();

    communicationManager.closeSocket();

    return 0;
}

bool Server::newUserConnection(User user) {
    return false;
}
