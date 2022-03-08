//
// Created by vieir on 07/03/2022.
//

#ifndef INF01151_TF_LOGGER_H
#define INF01151_TF_LOGGER_H


#define ERROR     0
#define INFO      1
#define DEBUG     2

class Logger {
    static int current_level;
    public :
        static void setMode(int level) ;
        static void message(int level, char const * message, ...) ;

};


#endif //INF01151_TF_LOGGER_H
