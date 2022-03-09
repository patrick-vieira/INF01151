//
// Created by vieir on 07/03/2022.
//

#ifndef INF01151_TF_CLIENT_H
#define INF01151_TF_CLIENT_H


#include "../aux_shared/logger.h"
#include "session/session.h"
#include "user_gui/user_gui.h"

using namespace std;

class Session;
class UserGUI;

class Client
{
    Logger logger;


    string menu_choices[3] = {"Write message", "Follow user", "Exit"};

private:
    string user;
    Session session;
    UserGUI gui;

public:
    Client(string user, Session session);
    int start();

    string getUser() {
        return user;
    }

private:

    /** validateUser()
      valida o numero de caracteres no nome do user
      min 4 max 20    **/
    bool validateUser();

    /** openConnection()
      se conecta com o servidor **/
    string initSession();

    /** messageSender()
      envia uma mensagem para o servidor **/
    bool messageSender();

    /** messageSender()
      envia uma mensagem para o servidor **/
    bool follow(string user_name);

    /** messageListener()
      canal para receber as mensagens do servidor **/
    bool messageListener();

};


#endif //INF01151_TF_CLIENT_H

/*
 * user de 4 a 20 caracteres
 * ao conectar como cliente deve ser possivel
 * - enviar mensagem
 * - se inscrever para receber mensagem de um usuario (seguir)
 * - receber mensagem
 *      ao mesmo tempo, aqui vai duas threads, abrir uma ao começar a escrever a mensagem.
 *          ideia: fazer um menu estilo digite 1 para escrever uma mensagem dai lança
 *          a thread enquanto a outra fica escutando as mensagens que recebe.



 */