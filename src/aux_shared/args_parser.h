//
// Created by vieir on 07/03/2022.
//

#ifndef INF01151_TF_ARGS_PARSER_H
#define INF01151_TF_ARGS_PARSER_H


#include <vector>
#include <string>
#include "logger.h"

class ArgsParser {

    Logger logger;

    std::vector<std::string> args;

    char* getArgsOptionValue(const char *option);
    bool getArgsOptionPresent(const std::string &option);

    public:
        ArgsParser(int argc, char * argv[]);
        int getPort();
        bool isDebug();
        char* getMemoryFile();

    void show();

    bool isServer();

    bool isClient();

    char* getClientID();
};


#endif //INF01151_TF_ARGS_PARSER_H
