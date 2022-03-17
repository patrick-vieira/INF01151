//
// Created by vieir on 09/03/2022.
//

#include "server_communication_manager.h"

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
    tv.tv_sec = 1;
    tv.tv_usec = 100000;
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

    if(message.direct_response){
        struct sockaddr_in _address = message.direct_address;
        string message_body = to_string(message.payload);

        int n = sendto(sockfd, message_body.c_str(), strlen(message_body.c_str()), 0,(struct sockaddr *) &_address, sizeof(struct sockaddr));
        if (n  < 0)
            logger.message(ERROR, "ERROR on sendto");

        return true;

    } else {
        list<CLIENT_ADDRESS> *active_sessions = message.user_destination.getActiveSessions(sockfd);
        for (auto iter = active_sessions->begin(), end = active_sessions->end(); iter != end; ++iter) {
            struct sockaddr_in _address = iter->sockaddrIn();
            string message_body = to_string(message.payload);

            int n = sendto(sockfd, message_body.c_str(), strlen(message_body.c_str()), 0,(struct sockaddr *) &_address, sizeof(struct sockaddr));
            if (n  < 0)
                logger.message(ERROR, "ERROR on sendto");
        }

        return true;
    }
}

list<MESSAGE> ServerCommunicationManager::messageReceiver() {
    char* message_buffer;
    message_buffer = (char*) std::calloc(255, sizeof(char));

    struct sockaddr_in sender_cli_addr;

    list<MESSAGE> messages;

    /* receive from socket */
    int n = recvfrom(sockfd, message_buffer, 256, 0, (struct sockaddr *) &sender_cli_addr, &clilen);
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
                MESSAGE message;
                message.payload = user->loginSuccessMessage();
                message.direct_address = sender_cli_addr;
                message.direct_response = true;
                messages.push_front(message);
            } else {
                MESSAGE message;
                message.payload = user->loginFail();
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

    for (auto it = this->users.begin(); it != this->users.end(); ++it) {
        int index = std::distance(users.begin(), it);
        if(it.operator*()->getName() == user_name) {
            logger.message(INFO, "User found [%s] active sessions [%d]", it.operator*()->getName().c_str(), it.operator*()->active_sessions_count());
//            User user = it.operator*();
            User* user = *it;
            return user;
        }
    }


    User* user = new User(user_name);
    users.push_back(user);
    saveUsers();

    logger.message(INFO, "New User [%s]", user->getName().c_str());

    return user;
}

void ServerCommunicationManager::saveUsers() {
    json jsonfile = json::array();

    for (auto it = this->users.begin(); it != this->users.end(); ++it) {
        int index = std::distance(users.begin(), it);
        User* user = *it;
        jsonfile.push_back(user->asJson());
    }
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
    bool success = user->newSession(cli_addr, sockfd);
    saveUsers();
    return success;
}

bool ServerCommunicationManager::ping(CLIENT_ADDRESS cli_addr){
    json response_message_json;
    response_message_json["type"] = PING;
    response_message_json["message"] = "[SERVER]: Ping";
    string flat_response_message = to_string(response_message_json);

    sockaddr_in cli_addr2 = cli_addr.sockaddrIn();

    int n = sendto(sockfd, flat_response_message.c_str(), strlen(flat_response_message.c_str()), MSG_CONFIRM ,(struct sockaddr *) &cli_addr2, sizeof(struct sockaddr));
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

