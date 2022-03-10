//
// Created by vieir on 09/03/2022.
//

#ifndef INF01151_TF_SESSION_MANAGER_H
#define INF01151_TF_SESSION_MANAGER_H


#include "../user/user.h"
#include "../persistence/serverMemory.h"

using namespace std;

class SessionManager {

private:
    vector<User> active_sessions;

public:
    SessionManager(ServerPersistence memory);
    void start();
    User userRequestConnection(string user_name);
};


#endif //INF01151_TF_SESSION_MANAGER_H
