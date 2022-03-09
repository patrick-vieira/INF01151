//
// Created by vieir on 07/03/2022.
//
#include <algorithm>
#include "args_parser.h"

// privados

char *ArgsParser::getArgsOptionValue(const char *option) {

    auto it = find(args.begin(), args.end(), option);
    if (it != args.end()) {
        // int index = std::distance(args.begin(), it) + 1;
        char* value = std::next(it)->data();
        logger.message(DEBUG, "argv[%s] = %s", it->data(), value);
        return value;
    }

    logger.message(INFO, "ARGS [%s] not found.", option);

    return 0;
}

bool ArgsParser::getArgsOptionPresent(const std::string &option) {
    auto it = find(args.begin(), args.end(), option);
    if (it != args.end())
        return true;
    return false;
}

// construtor

ArgsParser::ArgsParser(int argc, char *argv[]) {
    std::vector<std::string> args( argv, argv + argc );
    this->args = args;
}

// publicos

void ArgsParser::show() {
    logger.message(INFO, "------------ ARGS ----------");

    for (auto it = begin (this->args); it != end (this->args); ++it) {
        int index = std::distance(args.begin(), it);
        logger.message(INFO, "argv[%d] = %s", index, it->data());
    }

    logger.message(INFO, "----------------------------\n");
}

bool ArgsParser::isDebug() {
    if(this->getArgsOptionPresent("-d")) {
        logger.message(INFO, "Executando em modo debug");
        return true;
    }
    return false;
}

int ArgsParser::getPort() {
    char * port_string = this->getArgsOptionValue("-p");

    if (!port_string)
        port_string = (char*)"9090";

    logger.message(DEBUG, "Usando a porta: %s", port_string);

    return std::stoi(port_string);

}

char* ArgsParser::getMemoryFile() {
    char * filename = this->getArgsOptionValue("-f");
    if (filename)
    {
        logger.message(DEBUG, "Usando o arquivo de memoria: %s", filename);
        return filename;
    }
    return 0;
}

bool ArgsParser::isServer() {
    if(this->getArgsOptionPresent("-s")) {
        return true;
    }
    return false;
}

bool ArgsParser::isClient() {
    if(this->getArgsOptionPresent("-c")) {
        return true;
    }
    return false;
}

char * ArgsParser::getClientID() {
    char * client_id = this->getArgsOptionValue("-u");

    if (client_id)
        return client_id;

    return 0;
}

