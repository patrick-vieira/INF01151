//
// Created by vieir on 07/03/2022.
//

#ifndef INF01151_TF_SERVER_H
#define INF01151_TF_SERVER_H

#include <sys/syscall.h>
#include "../aux_shared/logger.h"
#include "user/user.h"
#include "server_thread_runner.h"
#include "server_communication_manager/server_communication_manager.h"

#define     TRUE        1
#define		MAX_ITEMS	200


class Server : ServerThreadRunner {

    int port;
    Logger logger;

    ServerCommunicationManager communicationManager;

public:
    Server(int port);


public:
    int start();


private:
    [[noreturn]] void ProducerImplementation() override;
    void ConsumerImplementation() override;

    MESSAGE direct_messages_buffer[MAX_ITEMS];
    map<User*, list<MESSAGE>> user_messages_buffer;


};


#endif //INF01151_TF_SERVER_H

