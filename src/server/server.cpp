//
// Created by vieir on 07/03/2022.
//

#include <sstream>
#include "server.h"


using namespace std;

Server::Server(int port): ServerThreadRunner() {
    this->port = port;
}

int Server::start() {
    logger.message(INFO,"---> Server starting on port: %d  <---", this->port);

    communicationManager.openSocket(port);

    pthread_t producer = this->StartProducerThread();

    WaitForProducerToExit(producer);

    communicationManager.closeSocket();

    return 0;
}

[[noreturn]] void Server::ProducerImplementation() {

    pid_t tid;

    tid = syscall(SYS_gettid);

    logger.message(INFO, "--PRODUCER STARTED - pid [%d]", tid);

    while (TRUE) {
//        sleep(rand()%5);

//         TODO ideia: a cada login criar uma thread, mensagens para aquele usuario vão para um dicionario <user, messagesList>
//         essa thread executa enquanto existir uma sessão do usuario
//         se a sessão cai continua recebendo no buffer
//         quando voltar consome. uma fila consome de um lado e produz no outro
//         se não tem usuario é mensagem direta, vai para o buffer normal

        list<MESSAGE> new_messages = communicationManager.messageReceiver();

        pthread_cond_signal(&cond_message_avaliable);

        if(new_messages.empty()){
            communicationManager.pingAll();
            continue;
        }

        for (auto received_message_it = begin (new_messages); received_message_it != end (new_messages); ++received_message_it) {
            if(!received_message_it->sender->connected()) {
                // if user who send message has no consumer, create one.
                CONSUMER_ARGS consumerArgs;
                consumerArgs.user = received_message_it->sender;

                logger.message(INFO, "[STARTING NEW THREAD CONSUMER LAUNCH]:[%s]", received_message_it->sender->getName().c_str());
                received_message_it->sender->setConsumerThread(this->StartConsumerThread(consumerArgs));
            }

            map<User*, list<MESSAGE>>::iterator user_messages_buffer_it;
            user_messages_buffer_it = user_messages_buffer.find(received_message_it->receiver);

            if (user_messages_buffer_it != user_messages_buffer.end()) {
                if(received_message_it->direct_response)
                    user_messages_buffer_it->second.push_front(*received_message_it); // direct message in fron to consume first;
                else
                    user_messages_buffer_it->second.push_back(*received_message_it);

                string payload = to_string(received_message_it->payload);
                logger.message(INFO, "[NEW MESSAGE ON DICT]: SENDER: %s  RECEIVER: %s PAYLOAD: %S", received_message_it->sender->getName().c_str(), received_message_it->receiver->getName().c_str(), payload.c_str());
            } else {
                list<MESSAGE> user_messages;
                user_messages.push_back(*received_message_it);
                user_messages_buffer.insert({received_message_it->receiver, user_messages});
                logger.message(INFO, "[NEW USER ADD TO DICT]: USER %s", received_message_it->sender->getName().c_str());
            }

        }
    }
}

void Server::ConsumerImplementation() {

    pthread_mutex_lock(&mutex_args);
    const CONSUMER_ARGS t_parameters = pthread_args_vec.back();
    pthread_args_vec.pop_back();
    pthread_mutex_unlock(&mutex_args);

    pid_t tid;

    tid = syscall(SYS_gettid);

    logger.message(INFO, "--NEW CONSUMMER STARTED - User consumer [%s] - pid [%d]", t_parameters.user->getName().c_str(), tid);

    while (t_parameters.user->connected()) {
        pthread_mutex_lock(&mutex);

        map<User*, list<MESSAGE>>::iterator user_messages_buffer_it;
        user_messages_buffer_it = user_messages_buffer.find(t_parameters.user);

        if (user_messages_buffer_it != user_messages_buffer.end()) { //if user present in dictionary

            logger.message(INFO, "--PRE-SIGNAL MESSAGES AVALIABLE [%d] - User consumer [%s] ", user_messages_buffer_it->second.size(), t_parameters.user->getName().c_str());
            while(user_messages_buffer_it->second.empty()) {  //while user has no messages
                logger.message(INFO, "--SIGNAL MESSAGES AVALIABLE [%d] - User consumer [%s] ", user_messages_buffer_it->second.size(), t_parameters.user->getName().c_str());
                if(!t_parameters.user->connected()){
                    pthread_mutex_unlock(&mutex);
                    return;
                }
                pthread_cond_wait(&cond_message_avaliable, &mutex);
            }
            MESSAGE my_task = user_messages_buffer_it->second.front(); // get user message

            if (communicationManager.messageSender(my_task)) {  // consume user message
                user_messages_buffer_it->second.pop_front();             // remove user message from list if consumed
                logger.message(INFO, "--SUCCESS - User consumer [%s] - message from sender [%s] - payload %s", t_parameters.user->getName().c_str(), my_task.sender->getName().c_str(), to_string(my_task.payload).c_str());
            } else {
                logger.message(INFO, "--FAIL - User consumer [%s] - message from sender [%s] - payload %s", t_parameters.user->getName().c_str(), my_task.sender->getName().c_str(), to_string(my_task.payload).c_str());
            }
        }
        pthread_mutex_unlock(&mutex);
    }

    logger.message(INFO, "--CONSUMMER STOPPED - User consumer [%s]", t_parameters.user->getName().c_str());

}
