//
// Created by vieir on 09/03/2022.
//


#include "session.h"
#include "../message_types.h"


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

    if(timeout > 0) {
        struct timeval tv; // set recvfrom timeout
        tv.tv_sec = 1;
        tv.tv_usec = 20000;
        if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv)) < 0) {
            logger.message(ERROR, "ERROR on setting opt timeout");
            exit(912);
        }
    }
    return true;
}


int Session::sendMessage(json message){

    string plain_message = to_string(message);

    n = sendto(sockfd, plain_message.c_str(), strlen(plain_message.c_str()), 0, (const struct sockaddr *) &serv_addr, sizeof(struct sockaddr_in));
    if (n < 0)
        logger.message(ERROR, "ERROR sendto");

    return n >= 0;
}

json Session::receiveMessage(){
    char* server_response_buffer = (char*) calloc(2048, sizeof(char));
    n = recvfrom(sockfd, server_response_buffer, 2048, 0, (struct sockaddr *) &from, &length);
    if (n < 0) {
        json server_response;
        server_response["type"] = NO_MESSAGE;
        return server_response;
    }

    string response = string(server_response_buffer);

    json server_response = json::parse(response);

    ADDRESS rec_from(from);
    server_response["received_from_host"] = rec_from.ip;
    server_response["received_from_port"] = rec_from.port;

    return server_response;
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


