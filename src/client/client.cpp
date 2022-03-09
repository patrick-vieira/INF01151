//
// Created by vieir on 07/03/2022.
//

#include <iostream>
#include <netinet/in.h>
#include <netdb.h>
#include <cstring>
#include <unistd.h>
#include "client.h"
#include "user_menu/user_menu.h"

Client::Client(string user, string host, int port) {
    this->user = user;
    this->host = host;
    this->port = port;
}

int Client::start() {
    logger.message(INFO, "---> Client user:@%s starting on host: [%s] port: [%d]  <---", this->user.c_str(), this->host.c_str(), this->port);

    UserMenu menu;

    menu.start(user, host, port);

    return 0;
}

bool Client::validateUser() {
    return false;
}

bool Client::initSession() {

    int sockfd, n;
    unsigned int length;
    struct sockaddr_in serv_addr, from;
    struct hostent *server;

    char buffer[256];

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

    printf("Enter the message: ");
    bzero(buffer, 256);
    fgets(buffer, 256, stdin);

    n = sendto(sockfd, buffer, strlen(buffer), 0, (const struct sockaddr *) &serv_addr, sizeof(struct sockaddr_in));
    if (n < 0)
        logger.message(ERROR, "ERROR sendto");

    length = sizeof(struct sockaddr_in);
    n = recvfrom(sockfd, buffer, 256, 0, (struct sockaddr *) &from, &length);
    if (n < 0)
        logger.message(ERROR, "ERROR recvfrom");

    logger.message(INFO, "Got an ack: %s\n", buffer);

    close(sockfd);

    return false;
}

bool Client::messageSender() {
    return false;
}

bool Client::follow(string user_name) {
    return false;
}

bool Client::messageListener() {
    return false;
}
