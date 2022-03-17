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

    clilen = sizeof(struct sockaddr_in);

}

void ServerCommunicationManager::closeSocket() {

    close(sockfd);
}

list<MESSAGE> ServerCommunicationManager::messageReceiver2() {
    char* message_buffer;
    message_buffer = (char*) std::calloc(255, sizeof(char));

    struct sockaddr_in sender_cli_addr;

    /* receive from socket */
    n = recvfrom(sockfd, message_buffer, 256, 0, (struct sockaddr *) &sender_cli_addr, &clilen);
    if (n < 0)
        logger.message(ERROR, "ERROR on recvfrom");

    json message_json = json::parse(message_buffer);

    logger.message(INFO, "Received a datagram: %s\n", to_string(message_json).c_str());

    list<MESSAGE> messages;

    switch(message_json["type"].get<int>()) {
        case LOGIN_REQUEST: {
            User user;
            user = getOrCreateUser(message_json["user"]);
            if(newSession(user, sender_cli_addr)) {
                json login_response = user.loginSuccess(sender_cli_addr, sockfd);

                MESSAGE message;
                message.payload = login_response;
                message.direct_address = sender_cli_addr;
                message.direct_response = true;

                messages.push_front(message);
            } else {
                json login_response = user.loginFail(sender_cli_addr, sockfd);

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
void ServerCommunicationManager::messageReceiver() {

    bool stop = false;

    while (!stop) {

        char* message;
        message = (char*) std::calloc(255, sizeof(char));

        /* receive from socket */
        n = recvfrom(sockfd, message, 256, 0, (struct sockaddr *) &cli_addr, &clilen);
        if (n < 0)
            logger.message(ERROR, "ERROR on recvfrom");

        auto message_json = json::parse(message);
//        message_json["user_socket"] = cli_addr;
        logger.message(INFO, "Received a datagram: %s\n", to_string(message_json).c_str());

        switch(message_json["type"].get<int>()){
            case LOGIN_REQUEST: {

                // TODO create user, check connections and pending notifications;
                // TODO send message with login result
                // TODO await to send pending messages, client need a moment to setup up

                json response_message_json;
                response_message_json["type"] = LOGIN_RESPONSE_SUCCESS;
                response_message_json["message"] = "Login success: welcome" + message_json["user"].get<string>();
                string flat_response_message = to_string(response_message_json);

                n = sendto(sockfd, flat_response_message.c_str(), strlen(flat_response_message.c_str()), 0,(struct sockaddr *) &cli_addr, sizeof(struct sockaddr));
                if (n  < 0)
                    logger.message(ERROR, "ERROR on sendto");
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

                n = sendto(sockfd, flat_response_message.c_str(), strlen(flat_response_message.c_str()), 0,(struct sockaddr *) &cli_addr, sizeof(struct sockaddr));
                if (n  < 0)
                    logger.message(ERROR, "ERROR on sendto");
                n = sendto(sockfd, flat_response_message.c_str(), strlen(flat_response_message.c_str()), 0,(struct sockaddr *) &cli_addr, sizeof(struct sockaddr));
                if (n  < 0)
                    logger.message(ERROR, "ERROR on sendto");
                n = sendto(sockfd, flat_response_message.c_str(), strlen(flat_response_message.c_str()), 0,(struct sockaddr *) &cli_addr, sizeof(struct sockaddr));
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

                n = sendto(sockfd, flat_response_message.c_str(), strlen(flat_response_message.c_str()), 0,(struct sockaddr *) &cli_addr, sizeof(struct sockaddr));
                if (n  < 0)
                    logger.message(ERROR, "ERROR on sendto");

                break;
            }
            default:{
                logger.message(ERROR, "Message type not recognized %s", message);
                break;
            }

        }



        if(message == "stop")
            stop = true;

//        string response = to_string(message_json);
//        string response2 = message_json["message"].get<string>();

        /* send to socket */

    }

}

User ServerCommunicationManager::getOrCreateUser(string user_name) {
    list<User>::iterator it;

    for (it = begin (this->users); it != end (this->users); ++it) {
        int index = std::distance(users.begin(), it);
        if(it->getName() == user_name) {
            logger.message(INFO, "User found [%s] active sessions [%d]", it->getName().c_str(), it->active_sessions_count());
            User user = it.operator*();
            return user;
        }
    }

    logger.message(INFO, "User found [%s]", user_name.c_str());
    User user(user_name);
    users.push_back(user);

    saveUsers();

    return user;
}

void ServerCommunicationManager::saveUsers() {
    json jsonfile = json::array();

    for (auto it = begin (this->users); it != end (this->users); ++it) {
        int index = std::distance(users.begin(), it);
        User user = *it;
        jsonfile.push_back(user.asJson());
    }

    ofstream file("users.json");
    file << jsonfile;
}

bool ServerCommunicationManager::updateUser(User user) {
    list<User>::iterator it;

    for (it = begin (this->users); it != end (this->users); ++it) {
        int index = std::distance(users.begin(), it);
        if(it->getName() == user.getName()) {
            users.erase(it);
            users.push_back(user);
            logger.message(INFO, "User updaated [%s] active sessions [%d -> %d]", it->getName().c_str(), it->active_sessions_count(), user.active_sessions_count());
            return true;
        }
    }
    return false;
}

bool ServerCommunicationManager::newSession(User user, sockaddr_in cli_addr) {
//    user.newSession(cli_addr);
//    user.removeSession(cli_addr);
    //TODO ping on active sessions
    bool success = user.newSession(cli_addr);
    updateUser(user); //TODO change user to pointer and can remove this function?
    return success;
}
