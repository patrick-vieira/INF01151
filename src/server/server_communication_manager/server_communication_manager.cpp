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

    if(message.direct_response){
        struct sockaddr_in _address = message.direct_response_address;
        string message_body = to_string(message.payload);

        int n = sendto(sockfd, message_body.c_str(), strlen(message_body.c_str()), 0,(struct sockaddr *) &_address, sizeof(struct sockaddr));
        if (n  < 0)
            logger.message(ERROR, "ERROR on sendto");

        return true;

    } else {
        list<CLIENT_ADDRESS> active_sessions = message.receiver->getActiveSessions();
        bool consumed = false;
        for (auto iter = active_sessions.begin(), end = active_sessions.end(); iter != end; ++iter) {
            struct sockaddr_in _address = iter->sockaddrIn();
            string message_body = to_string(message.payload);

            int n = sendto(sockfd, message_body.c_str(), strlen(message_body.c_str()), 0,(struct sockaddr *) &_address, sizeof(struct sockaddr));
            if (n  < 0)
                logger.message(ERROR, "ERROR on sendto");
            consumed = true;
        }

        return consumed;
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
//        logger.message(DEBUG, "[Message Receiver]: Awaitng for message.");
        return messages;
    }
    json message_json = json::parse(message_buffer);

    logger.message(INFO, "[Message Receiver]: New message received: %s\n", to_string(message_json).c_str());

    User* user;
    user = getOrCreateUser(message_json["user"]);

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
//
//                message.payload = user->loginSuccessMessage();
//                message.direct_response_address = sender_cli_addr;
//                message.direct_response = true;
//                messages.push_front(message);
            } else {
                MESSAGE message;

                message.sender = user;
                message.receiver = user;
                message.payload = user->loginFailMessage();

                message.direct_response_address = sender_cli_addr;
                message.direct_response = true;

                messages.push_front(message);
//
//                message.payload = user->loginFailMessage();
//                message.direct_response_address = sender_cli_addr;
//                message.direct_response = true;
//                messages.push_front(message);
            }
            break;
        }
        case LOGOUT_REQUEST: {
            user->logout(sender_cli_addr);
            break;
        }
        case NEW_MESSAGE: {

            list<User*> followers = user->getFollowers();

            for (auto follower_it = followers.begin(); follower_it != followers.end(); ++follower_it) {
//                User* follower = *follower_it;

                MESSAGE message;

                message.sender = user;
                message.receiver = *follower_it;
                message.payload = user->notificationMessage(message_json["message"].get<string>());

                messages.push_front(message);
            }

            break;
        }
        case FOLLOW_REQUEST: {
            // TODO register new follower in user

            pair<bool, User*> user_to_follow = getUser(message_json["user_name"]);

            if(user_to_follow.first){
                user_to_follow.second->addFollower(user);

                MESSAGE message;

                message.sender = user;
                message.receiver = user;
                message.payload = user->followingSuccessMessage(user_to_follow.second->getName());

                messages.push_front(message);
            } else {
                MESSAGE message;
                message.sender = user;
                message.receiver = user;
                message.payload = user->followingFailMessage(message_json["user_name"]);

                message.direct_response_address = sender_cli_addr;
                message.direct_response = true;
                messages.push_front(message);
            }
            break;
        }
        default:{
            logger.message(ERROR, "Message type not recognized %s", to_string(message_json).c_str());
            break;
        }
    }

    saveUsers();
    return messages;

}

pair<bool, User*> ServerCommunicationManager::getUser(const string& user_name) {
    for (auto it = this->users.begin(); it != this->users.end(); ++it) {
        int index = std::distance(users.begin(), it);
        if(it.operator*()->getName() == user_name) {
            logger.message(INFO, "User found [%s] active sessions [%d]", it.operator*()->getName().c_str(), it.operator*()->active_sessions_count());

            return {true, it.operator*()};
        }
    }
    User* no_user = new User();
    return {false, no_user};
}

User* ServerCommunicationManager::getOrCreateUser(const string& user_name) {

    for (auto it = this->users.begin(); it != this->users.end(); ++it) {
        if(it.operator*()->getName() == user_name) {
            logger.message(INFO, "User found [%s] active sessions [%d]", it.operator*()->getName().c_str(), it.operator*()->active_sessions_count());
            return it.operator*();
        }
    }

    User* user = new User(user_name);
    users.push_back(user);

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

bool ServerCommunicationManager::newSession(User* user, sockaddr_in cli_addr) {
//    for (auto iter = user->active_sessions.begin(), end = user->active_sessions.end(); iter != end; ++iter) {
//        if(!ping(*iter))
//            iter = user->active_sessions.erase(iter);
//    }

    bool success = user->login(cli_addr);
    saveUsers();
    return success;
}


void ServerCommunicationManager::pingAll() {
    for (auto user_iter = users.begin(), user_iter_end = users.end(); user_iter != user_iter_end; ++user_iter) {
        User* user = *user_iter;
        for (auto active_sessions_iter = user->active_sessions.begin(), active_sessions_iter_end = user->active_sessions.end(); active_sessions_iter != active_sessions_iter_end; ++active_sessions_iter) {
            if(!ping(*active_sessions_iter)){
                logger.message(INFO, "Ping on user [%s] without response.", user->getName().c_str());
//                active_sessions_iter = user->active_sessions.erase(active_sessions_iter);
                user->logout(active_sessions_iter->sockaddrIn());
                break;
            }
        }
    }
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
    if (n < 0) {
        return false;
    }

    return true;
}

