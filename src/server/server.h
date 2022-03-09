//
// Created by vieir on 07/03/2022.
//

#ifndef INF01151_TF_SERVER_H
#define INF01151_TF_SERVER_H


#include "../aux_shared/logger.h"
#include "user/user.h"

class Server
{
    int port;
    Logger logger;

    public:
        Server(int port);


    public:
        int start();


    private:

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