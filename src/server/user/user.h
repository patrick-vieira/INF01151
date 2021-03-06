//
// Created by vieir on 08/03/2022.
//

#ifndef INF01151_TF_USER_H
#define INF01151_TF_USER_H


#include <list>
#include <string>
#include <netinet/in.h>
#include <nlohmann/json.hpp>
#include "../../aux_shared/message_types.h"
#include "../../aux_shared/logger.h"
#include "../../aux_shared/session/session.h"

using namespace std;

using json = nlohmann::json;


typedef struct user_session : client_address {

    time_t last_ping_response = time(0);

    user_session(){};

    user_session(const char *ip, int port, long last_ping_response) {
        strncpy(this->ip, ip,INET_ADDRSTRLEN);
        this->port = port;
        this->last_ping_response = last_ping_response;
    }

    user_session(struct sockaddr_in cli_addr){
        inet_ntop(AF_INET, &(cli_addr.sin_addr), this->ip, INET_ADDRSTRLEN);
        this->port = htons(cli_addr.sin_port);
    };

    void setLastPingResponse(){
        last_ping_response = time(0);
    }

} USER_SESSION;


class User {
    Logger logger;

    string name;
    list<string> followers_name;

    list<USER_SESSION> active_sessions;

    pthread_t consumer_thread = 0;
    bool consumer_thread_running = false;

public:
    pthread_cond_t 	cond_message_avaliable;

    User(string name);
    User(string name, list<string> followers, list<USER_SESSION> active_sessions);

    void sentMessage(string message);       //TODO registry
    void receivedMessage(string message);   //TODO registry

    string getName();

    bool login(struct sockaddr_in address);
    void logout(struct sockaddr_in address);
    void pingResponse(struct sockaddr_in cli_addr);
    bool isConnected();

    void addFollower(User* newFollower);
    list<string> getFollowers();

    list<USER_SESSION> getActiveSessions();
    int active_sessions_count();

    void setConsumerThread(pthread_t consumer_thread);

    bool isConsumerThreadRunning() {
        return consumer_thread_running;
    }
    void consumerThreadExit() {
        consumer_thread_running = false;
    }

    json loginSuccessMessage();
    json loginFailMessage();
    json followingSuccessMessage(string user_name);
    json followingFailMessage(string user_name);
    json notificationMessage(string message);

    json asJson();


};


#endif //INF01151_TF_USER_H
