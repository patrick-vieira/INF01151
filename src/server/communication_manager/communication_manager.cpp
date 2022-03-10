//
// Created by vieir on 09/03/2022.
//

#include "communication_manager.h"
#include "../../aux_shared/message_types.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

void CommunicationManager::openSocket(int port) {

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
        logger.message(ERROR, "ERROR opening socket");

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    bzero(&(serv_addr.sin_zero), 8);

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(struct sockaddr)) < 0)
        logger.message(ERROR, "ERROR on binding");

    clilen = sizeof(struct sockaddr_in);

}

void CommunicationManager::closeSocket() {

    close(sockfd);
}

void CommunicationManager::messageReceiver() {

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
