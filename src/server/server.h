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
#include "replica_manager/replica_manager.h"

#define     TRUE        1
#define		MAX_ITEMS	200


class Server : ServerThreadRunner {

    int port;
    Logger logger;

    ServerPersistence* persistence = new ServerPersistence("users.json", "messages.json");
    ServerCommunicationManager communicationManager;

public:
    Server(int port, ReplicaManager* replicaManager) : communicationManager(persistence, replicaManager) {
        this->port = port;
        this->initMessageBuffer();
        replicaManager->close();
    }

    int start();


private:
    MESSAGE direct_messages_buffer[MAX_ITEMS];
    map<User*, list<MESSAGE>> user_messages_buffer;

    [[noreturn]] void ProducerImplementation() override;
    void ConsumerImplementation() override;
    [[noreturn]] void PingImplementation() override;
    [[noreturn]] void ReplicationManagerImplementation() override;

    void initMessageBuffer() {
        list < User * > users = persistence->getAllUsers();

        for (auto user_iter = users.begin(), user_iter_end = users.end(); user_iter != user_iter_end; ++user_iter) {
            User* user = *user_iter;
            startUserThreadConsumer(user);

            list<MESSAGE> user_messages;
            user_messages_buffer.insert({user, user_messages});
        }
    }

    void startUserThreadConsumer(User* user);
};


#endif //INF01151_TF_SERVER_H

