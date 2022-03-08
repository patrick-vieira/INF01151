//
// Created by vieir on 07/03/2022.
//

#ifndef INF01151_TF_CLIENT_H
#define INF01151_TF_CLIENT_H


#include "../aux_shared/logger.h"

class Client
{
    Logger logger;

    int port;
    char * user;

    public:
        Client(char * user, int port);
        int start();

};


#endif //INF01151_TF_CLIENT_H
