//
// Created by vieir on 09/03/2022.
//

#include "server_communication_manager.h"
#include "../../aux_shared/message_types.h"
#include <nlohmann/json.hpp>
#include <fstream>

using json = nlohmann::json;

void ServerCommunicationManager::openSocket(int port) {

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
        logger.message(ERROR, "ERROR opening socket");

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    bzero(&(serv_addr.sin_zero), 8);

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(struct sockaddr)) < 0) {
        logger.message(ERROR, "ERROR on binding");
        exit(911);
    }

    struct timeval tv; // set recvfrom timeout
    tv.tv_sec = 5;
    tv.tv_usec = 500000;
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv)) < 0) {
        logger.message(ERROR, "ERROR on setting opt timeout");
        exit(912);
    }

    clilen = sizeof(struct sockaddr_in);

}

void ServerCommunicationManager::closeSocket() {

    close(sockfd);
}

list<MESSAGE> ServerCommunicationManager::messageReceiver() {
    char* message_buffer;
    message_buffer = (char*) std::calloc(255, sizeof(char));

    struct sockaddr_in sender_cli_addr;

    list<MESSAGE> messages;

    /* receive from socket */
    n = recvfrom(sockfd, message_buffer, 256, 0, (struct sockaddr *) &sender_cli_addr, &clilen);
    if (n < 0) {
        logger.message(ERROR, "[Message Receiver]: Awaitng for message.");
        return messages;
    }
    json message_json = json::parse(message_buffer);

    logger.message(INFO, "[Message Receiver]: New message received: %s\n", to_string(message_json).c_str());


    switch(message_json["type"].get<int>()) {
        case LOGIN_REQUEST: {
            User* user;
            user = getOrCreateUser(message_json["user"]);
            if(newSession(user, sender_cli_addr)) {
                json login_response = user->loginSuccess(sender_cli_addr, sockfd);

                MESSAGE message;
                message.payload = login_response;
                message.direct_address = sender_cli_addr;
                message.direct_response = true;

                messages.push_front(message);
            } else {
                json login_response = user->loginFail(sender_cli_addr, sockfd);

                MESSAGE message;
                message.payload = login_response;
                message.direct_address = sender_cli_addr;
                message.direct_response = true;

                messages.push_front(message);
            }
            break;
        }
        case NEW_MESSAGE: {
            // TODO register message, user, timestamp, size, followers who will receive
            // TODO send message to followers
            // TODO check message mentions (not required)

            json response_message_json;
            response_message_json["type"] = NOTIFICATION;
            response_message_json["message"] = "Message sent:" + message_json["message"].get<string>();
            string flat_response_message = to_string(response_message_json);

            n = sendto(sockfd, flat_response_message.c_str(), strlen(flat_response_message.c_str()), 0,(struct sockaddr *) &sender_cli_addr, sizeof(struct sockaddr));
            if (n  < 0)
                logger.message(ERROR, "ERROR on sendto");

            break;
        }
        case FOLLOW_REQUEST: {
            // TODO register new follower in user

            json response_message_json;
            response_message_json["type"] = NOTIFICATION;
            response_message_json["message"] = "You are now following the user: " + message_json["user_name"].get<string>();
            string flat_response_message = to_string(response_message_json);

            n = sendto(sockfd, flat_response_message.c_str(), strlen(flat_response_message.c_str()), 0,(struct sockaddr *) &sender_cli_addr, sizeof(struct sockaddr));
            if (n  < 0)
                logger.message(ERROR, "ERROR on sendto");

            break;
        }
        default:{
            logger.message(ERROR, "Message type not recognized %s", to_string(message_json).c_str());
            break;
        }
    }




    return messages;

}

User* ServerCommunicationManager::getOrCreateUser(string user_name) {
//    list<User*>::iterator it;

    for (auto it = this->users.begin(); it != this->users.end(); ++it) {
        int index = std::distance(users.begin(), it);
        if(it.operator*()->getName() == user_name) {
            logger.message(INFO, "User found [%s] active sessions [%d]", it.operator*()->getName().c_str(), it.operator*()->active_sessions_count());
//            User user = it.operator*();
            User* user = *it;
            return user;
        }
    }
//for (it = begin (this->users); it != end (this->users); ++it) {
//        int index = std::distance(users.begin(), it);
//        if(it->getName() == user_name) {
//            logger.message(INFO, "User found [%s] active sessions [%d]", it->getName().c_str(), it->active_sessions_count());
//            User user = it.operator*();
//            return user;
//        }
//    }

    logger.message(INFO, "User found [%s]", user_name.c_str());
//    User user(user_name);
    User* user = new User(user_name);
    users.push_back(user);

    saveUsers();

    return user;
}

void ServerCommunicationManager::saveUsers() {
    json jsonfile = json::array();

    for (auto it = this->users.begin(); it != this->users.end(); ++it) {
        int index = std::distance(users.begin(), it);
        User* user = *it;
        jsonfile.push_back(user->asJson());
    }
//    for (auto it = begin (this->users); it != end (this->users); ++it) {
//        int index = std::distance(users.begin(), it);
//        User user = *it;
//        jsonfile.push_back(user.asJson());
//    }

    ofstream file("users.json");
    file << jsonfile;
}
//
//bool ServerCommunicationManager::updateUser(User user) {
//    list<User>::iterator it;
//
//    for (it = begin (this->users); it != end (this->users); ++it) {
//        int index = std::distance(users.begin(), it);
//        if(it->getName() == user.getName()) {
//            users.erase(it);
//            users.push_back(user);
//            logger.message(INFO, "User updaated [%s] active sessions [%d -> %d]", it->getName().c_str(), it->active_sessions_count(), user.active_sessions_count());
//            return true;
//        }
//    }
//    return false;
//}

bool ServerCommunicationManager::newSession(User* user, sockaddr_in cli_addr) {
    list<struct sockaddr_in> *user_sessions = user->getSessions();

    // parse
    char string_ip[INET_ADDRSTRLEN];
    uint16_t  port;

    // client ip to string
    inet_ntop(AF_INET, &(cli_addr.sin_addr), string_ip, INET_ADDRSTRLEN);
    // client port to int
    port = htons(cli_addr.sin_port);

    logger.message(INFO, "string_ip as str: %s:%d", string_ip, port);

    // restore

    struct sockaddr_in temp_address;
    // recreate socket address
    inet_pton(AF_INET, string_ip, &(temp_address.sin_addr));
    temp_address.sin_port = htons(port);
    temp_address.sin_family = AF_INET;

    bool result = ping(temp_address);

    for (auto iter = user_sessions->begin(), end = user_sessions->end(); iter != end; ++iter) {
        if(!ping(*iter))
            iter = user_sessions->erase(iter);
    }

    bool success = user->newSession(cli_addr);
//    updateUser(user); //TODO change user to pointer and can remove this function?
    return success;
}

bool ServerCommunicationManager::ping(sockaddr_in cli_addr){
    json response_message_json;
    response_message_json["type"] = PING;
    response_message_json["message"] = "[SERVER]: Ping";
    string flat_response_message = to_string(response_message_json);

    n = sendto(sockfd, flat_response_message.c_str(), strlen(flat_response_message.c_str()), MSG_CONFIRM ,(struct sockaddr *) &cli_addr, sizeof(struct sockaddr));
    if (n  < 0) {
        logger.message(ERROR, "ERROR ping not received.");
        return false;
    }

    char* server_response_buffer = (char*) calloc(256, sizeof(char));
    n = recvfrom(sockfd, server_response_buffer, 256, 0, (struct sockaddr *) &cli_addr, &clilen);
    if (n < 0)
        return false;

    return true;
}