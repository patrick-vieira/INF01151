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


void Logger::message(int level, string message, ...) {

    if(current_level >= level) {

        switch (level) {
            case DEBUG: message = "\nDEBUG: " + message; break;
            case INFO:  message = "\nINFO: " + message; break;
            case ERROR: message = "\nERROR: " + message; break;
            default: message = "\n????: " + message; break;
        }

        va_list argptr;
        va_start(argptr, message); // warning ok, ele reclama que full_message não é o ultimo argumento da função
        vfprintf(stderr, message.c_str(), argptr);
        va_end(argptr);
    }


}
