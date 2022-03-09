//
// Created by vieir on 07/03/2022.
//

#include <unistd.h>
#include <netinet/in.h>
#include <cstring>
#include <cstdlib>
#include "server.h"


Server::Server(int port) {
    this->port = port;
}

int Server::start() {
    logger.message(INFO,"---> Server starting on port: %d  <---", this->port);

    int sockfd, n;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;
    // char buf[256];


    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
        logger.message(ERROR, "ERROR opening socket");

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(this->port);
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    bzero(&(serv_addr.sin_zero), 8);

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(struct sockaddr)) < 0)
        logger.message(ERROR, "ERROR on binding");

    clilen = sizeof(struct sockaddr_in);

    bool stop = false;

    while (!stop) {

        char* message;
        message = (char*) std::calloc(255, sizeof(char));

        /* receive from socket */
        n = recvfrom(sockfd, message, 256, 0, (struct sockaddr *) &cli_addr, &clilen);
        if (n < 0)
            logger.message(ERROR, "ERROR on recvfrom");

        logger.message(INFO, "Received a datagram: %s\n", message);

        if(message == "stop")
            stop = true;

        /* send to socket */
        n = sendto(sockfd, "Got your message", 17, 0,(struct sockaddr *) &cli_addr, sizeof(struct sockaddr));
        if (n  < 0)
            logger.message(ERROR, "ERROR on sendto");
    }

    close(sockfd);

    return 0;
}
