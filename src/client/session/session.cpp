//
// Created by vieir on 09/03/2022.
//


#include "session.h"


bool Session::openConnection() {

    server = gethostbyname(this->host.c_str());
    if (server == NULL) {
        logger.message(ERROR, "ERROR, no such host\n");
        exit(0);
    }

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
        logger.message(ERROR, "ERROR opening socket");

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(this->port);
    serv_addr.sin_addr = *((struct in_addr *)server->h_addr);
    bzero(&(serv_addr.sin_zero), 8);

    return true;
}

string Session::sendMessage(string message){

    n = sendto(sockfd, message.c_str(), strlen(message.c_str()), 0, (const struct sockaddr *) &serv_addr, sizeof(struct sockaddr_in));
    if (n < 0)
        logger.message(ERROR, "ERROR sendto");


    char buffer[256];
    length = sizeof(struct sockaddr_in);
    n = recvfrom(sockfd, buffer, 256, 0, (struct sockaddr *) &from, &length);
    if (n < 0)
        logger.message(ERROR, "ERROR recvfrom");

    string response = string(buffer);
    logger.message(INFO, "Got an ack: %s\n", response.c_str());

    return response;
}

void Session::closeConnection() {
    close(sockfd);
}

string Session::getHost() {
    return this->host;
}

int Session::getPort() {
    return this->port;
}

