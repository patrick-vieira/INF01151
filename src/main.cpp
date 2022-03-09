//
// Created by vieir on 07/03/2022.
//

#include "server/server.h"
#include "aux_shared/args_parser.h"
#include "client/client.h"
#include "aux_shared/logger.h"

using namespace std;

int main(int argc, char * argv[])
{
    Logger logger;

    ArgsParser argsParser(argc, argv);

    if (!argsParser.isServer() && !argsParser.isClient() ) {
        logger.message(ERROR, "Argumentos insuficientes!!\n Execute com uma das flags: \n\t-s para rodar um servidor. \n\t-c para rodar um cliente.\n");
        exit(2);
    } else if (argsParser.isClient() && argsParser.getClientID() == 0){
        logger.message(ERROR, "Argumentos insuficientes!!\n Para executar como cliente: \n\t-u <string> informar um usuario \n");
        exit(3);
    }

    if (argsParser.isDebug()){
        argsParser.show();
        Logger::setMode(DEBUG);
    }

    if ( argsParser.isServer() ) {

        Server server(argsParser.getPort());
        server.start();

    } else if ( argsParser.isClient() ) {

        Session session("localhost", argsParser.getPort());
        Client client(argsParser.getClientID(), session);
        client.start();

    }

    return 0;
}
