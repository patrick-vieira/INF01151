//
// Created by vieir on 09/03/2022.
//

#ifndef INF01151_TF_CLIENT_COMMUNICATION_MANAGER_H
#define INF01151_TF_CLIENT_COMMUNICATION_MANAGER_H

#include <string>
#include <nlohmann/json.hpp>

#include "../session/session.h"
#include "../../aux_shared/message_types.h"


using json = nlohmann::json;

using namespace std;

class Session;
class ClientCommunicationManager {

private:
    Session session;

public:
    ClientCommunicationManager(Session session): session(session){

    };

    bool login(string user_name);
    void logout();
    bool pingReply();

    bool sendMessage(string user_name);
    bool followUser(string basicString);
    json notificationAvailable();



private:
    bool away_timeout(int time);

};


#endif //INF01151_TF_CLIENT_COMMUNICATION_MANAGER_H
