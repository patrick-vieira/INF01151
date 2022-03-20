//
// Created by vieir on 09/03/2022.
//

#include "server_communication_manager.h"

using json = nlohmann::json;


ServerCommunicationManager::ServerCommunicationManager(ServerPersistence* persistence) {
    this->persistence = persistence;
}

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
    tv.tv_sec = 1;
    tv.tv_usec = 20000;
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv)) < 0) {
        logger.message(ERROR, "ERROR on setting opt timeout");
        exit(912);
    }

    clilen = sizeof(struct sockaddr_in);

}

void ServerCommunicationManager::closeSocket() {
    close(sockfd);
}

bool ServerCommunicationManager::messageSender(MESSAGE message) {
    if(message.direct_response)
        return sendDirectMessage(message);
    else
        return sendMessage(message);
}

bool ServerCommunicationManager::sendMessage(const MESSAGE &message) {
    list<USER_SESSION> active_sessions = message.receiver->getActiveSessions();

    for (auto iter = active_sessions.begin(), end = active_sessions.end(); iter != end; ++iter) {
        struct sockaddr_in _address = iter->sockaddrIn();
        string message_body = to_string(message.payload);

        int n = sendto(sockfd, message_body.c_str(), strlen(message_body.c_str()), 0,(struct sockaddr *) &_address, sizeof(struct sockaddr));
        if (n  < 0){
            logger.message(ERROR, "ERROR on sendto");
            return false;
        }
    }
    return true;
}

bool ServerCommunicationManager::sendDirectMessage(const MESSAGE &message) {
    struct sockaddr_in _address = message.direct_response_address;
    string message_body = to_string(message.payload);

    int n = sendto(sockfd, message_body.c_str(), strlen(message_body.c_str()), 0, (struct sockaddr *) &_address, sizeof(struct sockaddr));
    if (n  < 0) {
        logger.message(ERROR, "ERROR on sendto");
        return false;
    }
    return true;
}

list<MESSAGE> ServerCommunicationManager::messageReceiver() {
    char* message_buffer;
    message_buffer = (char*) std::calloc(255, sizeof(char));

    struct sockaddr_in sender_cli_addr;

    list<MESSAGE> messages;

    /* receive from socket */
    int n = recvfrom(sockfd, message_buffer, 256, 0, (struct sockaddr *) &sender_cli_addr, &clilen);
    if (n < 0) {
//        logger.message(DEBUG, "[Message Receiver]: Awaitng for message.");
        return messages;
    }
    json message_json = json::parse(message_buffer);

    if(message_json["type"].get<int>() != PING_RESPONSE) // supress ping response log spam
        logger.message(INFO, "[Message Receiver]: New message received: %s\n", to_string(message_json).c_str());

    User* user;
    user = persistence->getOrCreateUser(message_json["user"]);

    switch(message_json["type"].get<int>()) {
        case LOGIN_REQUEST: {
            if(user->login(sender_cli_addr)) {
                MESSAGE message;

                message.sender = user;
                message.receiver = user;
                message.payload = user->loginSuccessMessage();

                message.direct_response_address = sender_cli_addr;
                message.direct_response = true;

                messages.push_front(message);
            } else {
                MESSAGE message;

                message.sender = user;
                message.receiver = user;
                message.payload = user->loginFailMessage();

                message.direct_response_address = sender_cli_addr;
                message.direct_response = true;

                messages.push_front(message);
            }
            break;
        }
        case LOGOUT_REQUEST: {
            user->logout(sender_cli_addr);
            break;
        }
        case NEW_MESSAGE: {

            list<User*> followers = persistence->getUserFollowers(user);

            for (auto follower_it = followers.begin(); follower_it != followers.end(); ++follower_it) {
                MESSAGE message;

                message.sender = user;
                message.receiver = *follower_it;
                message.payload = user->notificationMessage(message_json["message"].get<string>());

                messages.push_front(message);
            }

            break;
        }
        case FOLLOW_REQUEST: {
            pair<bool, User*> user_to_follow = persistence->getUser(message_json["user_name"]);

            MESSAGE message;

            message.sender = user;
            message.receiver = user;

            if(user_to_follow.first){
                user_to_follow.second->addFollower(user);
                message.payload = user->followingSuccessMessage(user_to_follow.second->getName());
            } else {
                message.payload = user->followingFailMessage(message_json["user_name"]);
                message.direct_response_address = sender_cli_addr;
                message.direct_response = true;
            }

            messages.push_front(message);
            break;
        }
        case PING_RESPONSE:{
            user->pingResponse(sender_cli_addr);
            break;
        }
        default:{
            logger.message(ERROR, "Message type not recognized %s", to_string(message_json).c_str());
            break;
        }
    }

    if(message_json["type"].get<int>() != PING_RESPONSE) // supress ping response log spam
        persistence->saveUsers();
    return messages;

}

void ServerCommunicationManager::pingAll() {

    list<User*> users = persistence->getAllUsers();

    for (auto user_iter = users.begin(), user_iter_end = users.end(); user_iter != user_iter_end; ++user_iter) {
        User* user = *user_iter;
        list<USER_SESSION> active_sessions = user->getActiveSessions();

        for (auto active_sessions_iter = active_sessions.begin(), active_sessions_iter_end = active_sessions.end(); active_sessions_iter != active_sessions_iter_end; ++active_sessions_iter) {
            if(!ping(*active_sessions_iter)){
                logger.message(INFO, "Ping on user [%s] ip: %s:%d without response.", user->getName().c_str(), active_sessions_iter->ip, active_sessions_iter->port);
                user->logout(active_sessions_iter->sockaddrIn());
                persistence->saveUser(user);
            }
        }
    }
}

bool ServerCommunicationManager::ping(USER_SESSION cli_addr){
    json response_message_json;
    response_message_json["type"] = PING;
    response_message_json["message"] = "[SERVER]: Ping";
    string flat_response_message = to_string(response_message_json);

    sockaddr_in sockaddrIn = cli_addr.sockaddrIn();

    int n = sendto(sockfd, flat_response_message.c_str(), strlen(flat_response_message.c_str()), MSG_CONFIRM ,(struct sockaddr *) &sockaddrIn, sizeof(struct sockaddr));
    if (n  < 0) {
        logger.message(ERROR, "ERROR ping not received.");
        return false;
    }

    time_t now = time(0);
    double diff = difftime(now,cli_addr.last_ping_response);
    bool time_out = diff > 5; // 5 seconds timeout
    logger.message(DEBUG, "PING DIFF %0.2lf", diff);
    if(time_out)
        return false;


    return true;
}

