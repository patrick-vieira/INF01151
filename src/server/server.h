//
// Created by vieir on 07/03/2022.
//

#ifndef INF01151_TF_SERVER_H
#define INF01151_TF_SERVER_H


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
    void ProducerImplementation() override;
    void ConsumerImplementation() override;

    MESSAGE buffer[MAX_ITEMS];
        /* nova conexão de usuario, se não existe cria,
         * se já existe conecta, se já tem 2 conectados retorna mensagem de erro para o client
         * */
    bool newUserConnection(User user);


};


#endif //INF01151_TF_SERVER_H

/*
 * Maximo de duas conecções por usuario;
 * cuidar para ao desconectar uma não desconectar as duas
 */