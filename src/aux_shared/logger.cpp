//
// Created by vieir on 07/03/2022.
//

#include <cstdio>
#include <cstdarg>
#include <cstring>
#include <cstdlib>
#include "logger.h"

int Logger::current_level = INFO;

void Logger::setMode(int level) {
    current_level = level;
}


void Logger::message(int level, const char *message, ...) {

    if(current_level >= level) {
        char* message_type;

        switch (level) {
            case DEBUG: message_type = (char*)"\n DEBUG: "; break;
            case INFO:  message_type = (char*)"\n INFO: "; break;
            case ERROR: message_type = (char*)"\n ERROR: "; break;
            default:
                message_type = (char*)"\n ????: ";
        }

        char* full_message;
        full_message = (char*) malloc(strlen(message_type) + strlen(message)+ 1);
        strcpy(full_message, message_type );
        strcat(full_message, message);

        va_list argptr;
        va_start(argptr, full_message); // warning ok, ele reclama que full_message não é o ultimo argumento da função
        vfprintf(stderr, full_message, argptr);
        va_end(argptr);
    }


}
