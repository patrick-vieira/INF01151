//
// Created by vieir on 08/03/2022.
//

#ifndef INF01151_TF_USER_H
#define INF01151_TF_USER_H


#include <vector>
#include <string>

using namespace std;

class User {

    string name;
    vector<User> followers;

public:
    User(std::string name, std::vector<User> followers);
    void addFollower(User newFollower);

    void sendMessage(std::string message);
};


#endif //INF01151_TF_USER_H
