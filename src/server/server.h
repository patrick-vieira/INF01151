//
// Created by vieir on 07/03/2022.
//

#ifndef INF01151_TF_SERVER_H
#define INF01151_TF_SERVER_H


#include "../aux_shared/logger.h"

class Server
{
    int port;
    Logger logger;

    public:
        Server(int port);

    [[noreturn]] int start();

};


#endif //INF01151_TF_SERVER_H
