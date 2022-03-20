//
// Created by vieir on 08/03/2022.
//

#include "serverMemory.h"


ServerPersistence::ServerPersistence(string users_path, string messages_path) {

    this->loadUsers(users_path);
}

pair<bool, User*> ServerPersistence::getUser(const string& user_name) {
    for (auto it = this->users.begin(); it != this->users.end(); ++it) {
        int index = std::distance(users.begin(), it);
        if(it.operator*()->getName() == user_name) {
            return {true, it.operator*()};
        }
    }

    logger.message(INFO, "User not found [%s]", user_name.c_str());

    return {false, NULL};
}

User* ServerPersistence::getOrCreateUser(const string& user_name) {

    pair<bool, User*> user_found = this->getUser(user_name);
    if(user_found.first)
        return user_found.second;

    User* user = new User(user_name);
    users.push_back(user);

    logger.message(INFO, "Creating New User [%s]", user->getName().c_str());

    return user;
}

list<User *> ServerPersistence::getUserFollowers(User *user) {
    list<User *> followers;
    list<string> followers_name = user->getFollowers();

    for (auto it = begin(followers_name); it != end(followers_name); ++it) {
        pair<bool, User*> follower = this->getUser(*it);

        if(follower.first) {
            followers.push_back(follower.second);
        }
    }
    return followers;
}

list<User*> ServerPersistence::getAllUsers() {
    return users;
}

void ServerPersistence::saveUsers() {
    json jsonfile = json::array();

    for (auto it = this->users.begin(); it != this->users.end(); ++it) {
        int index = std::distance(users.begin(), it);
        User* user = *it;
        jsonfile.push_back(user->asJson());
    }
    ofstream file("users.json");
    file << jsonfile;
}

void ServerPersistence::loadUsers(string user_path) {

    ifstream jsonConfigFileStream("users.json", ifstream::in);
    json jsonConfigRoot;

    if (jsonConfigFileStream.is_open()) {
        jsonConfigFileStream >> jsonConfigRoot;
    }

    logger.message(DEBUG, "Reading users.json");
    for (auto array_it : jsonConfigRoot) {
        logger.message(DEBUG, "Reading users.json -> %s", to_string(array_it).c_str());

        User* user = jsonParseUser(array_it);
        users.push_back(user);
    }
}

list<string> ServerPersistence::jsonParserFollowers(json obj){
    logger.message(DEBUG, "Reading users.json [jsonParserFollowers]-> %s", to_string(obj).c_str());
    list<string> followers;

    for (auto array_sessions_it : obj) {
        logger.message(DEBUG, "Reading users.json [array_sessions_it]-> %s", to_string(array_sessions_it).c_str());

        for (auto& [key, val] : array_sessions_it.items())
            if(key == "follower")
                followers.push_back(val.get<string>());
    }

    return followers;
}

list<USER_SESSION> ServerPersistence::jsonParserCA(json obj){

    list<USER_SESSION> active_sessions;

    for (auto array_sessions_it : obj) {
        logger.message(DEBUG, "Reading users.json [array_sessions_it]-> %s", to_string(array_sessions_it).c_str());

        char ip[INET_ADDRSTRLEN];
        string ips;
        int port;
        time_t last_ping_response;

        for (auto& [key, val] : array_sessions_it.items()) {
            logger.message(DEBUG, "Reading users.json connection key: %s value: %s", key.c_str(), to_string(val).c_str());
            if(key == "ip") {
                strncpy(ip, val.get<string>().c_str(), INET_ADDRSTRLEN);

                ips = val.get<string>();

                logger.message(DEBUG, "Parsed ip %s ipss %s", ip, ips.c_str());


            }
            if(key == "port")
                port = val;
            if(key == "last_ping_response")
                last_ping_response = val;
        }

        USER_SESSION clientAddress(ip, port, last_ping_response);
        active_sessions.push_front(clientAddress);

    }

    return active_sessions;
}

User* ServerPersistence::jsonParseUser(json obj) {

    string name;
    list<USER_SESSION> active_sessions;
    list<string> followers_name;

    for (auto& [key, val] : obj.items()) {
        logger.message(DEBUG, "User obj -> key: %s -> value: %s", key.c_str(), to_string(val).c_str());
        if(key == "name") {
            name = val.get<string>();
        }
        if(key == "followers") {
            followers_name = jsonParserFollowers(val);
        }
        if(key == "active_sessions") {
            active_sessions = jsonParserCA(val);
        }
    }
    User* user = new User(name, followers_name, active_sessions);
    return user;
}

