//
// Created by vieir on 08/03/2022.
//

#ifndef INF01151_TF_SERVERMEMORY_H
#define INF01151_TF_SERVERMEMORY_H


//#include "../user/user.h"

#include <string>
#include <fstream>
#include <nlohmann/json.hpp>
#include "../user/user.h"

using namespace std;
using json = nlohmann::json;

class ServerPersistence {

    Logger logger;

    list<User*> users;

public:
    ServerPersistence(string users_path, string messages_path);

    pair<bool, User*> getUser(const string& user_name);
    User* getOrCreateUser(const string& user_name);

    void saveUsers();
    void saveUser(User* user){  //TODO or not ;)
        logger.message(INFO, "[PERSISTENCE] USER: %s SAVED\n", user->getName().c_str());
        this->saveUsers();
    }

    list<User*> getAllUsers();

    list<User *> getUserFollowers(User *pUser);

private:
    void loadUsers(string user_path);

    User* jsonParseUser(json obj);
    list<USER_SESSION> jsonParserCA(json obj);
    list<string> jsonParserFollowers(json obj);
};


#endif //INF01151_TF_SERVERMEMORY_H
