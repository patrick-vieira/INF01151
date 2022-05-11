//
// Created by vieir on 07/03/2022.
//

#include "server.h"


using namespace std;

int Server::start() {
    logger.message(INFO,"---> Server starting on port: %d  <---", this->port);

    communicationManager.openSocket(port);

    pthread_t producer = this->StartProducerThread();
    pthread_t ping = this->StartPingThread();

    WaitForProducerToExit(producer);

    communicationManager.closeSocket();

    return 0;
}

[[noreturn]] void Server::PingImplementation() {
    while (TRUE) {
        communicationManager.pingAll();
        sleep(2);
    }
}

[[noreturn]] void Server::ProducerImplementation() {

    logger.message(INFO, "--PRODUCER STARTED");

    while (TRUE) {

        list<MESSAGE> new_messages = communicationManager.messageReceiver();

        for (auto received_message_it = begin (new_messages); received_message_it != end (new_messages); ++received_message_it) {
            startUserThreadConsumer(received_message_it->sender);

            map<User*, list<MESSAGE>>::iterator user_messages_buffer_it;
            user_messages_buffer_it = user_messages_buffer.find(received_message_it->receiver);

            if (user_messages_buffer_it != user_messages_buffer.end()) {
                if(received_message_it->direct_response)
                    user_messages_buffer_it->second.push_front(*received_message_it); // direct message in fron to consume first;
                else
                    user_messages_buffer_it->second.push_back(*received_message_it);

                string payload = to_string(received_message_it->payload);
                logger.message(INFO, "[NEW MESSAGE ON DICT]: SENDER: %s  RECEIVER: %s PAYLOAD: %s", received_message_it->sender->getName().c_str(), received_message_it->receiver->getName().c_str(), payload.c_str());
            } else {
                list<MESSAGE> user_messages;
                user_messages.push_back(*received_message_it);
                user_messages_buffer.insert({received_message_it->receiver, user_messages});
                logger.message(INFO, "[NEW USER ADD TO DICT]: USER %s", received_message_it->sender->getName().c_str());
            }

            // send signal to receiver consumer
            pthread_cond_signal(&received_message_it->receiver->cond_message_avaliable);

        }
    }
}

void Server::ConsumerImplementation() {

    pthread_mutex_lock(&mutex_consumer_starting);

    const CONSUMER_ARGS* t_parameters;
    t_parameters = getThreadArgs(pthread_self());

    pthread_mutex_unlock(&mutex_consumer_starting);

    logger.message(INFO, "--NEW CONSUMMER THREAD STARTED - User consumer [%s]", t_parameters->user->getName().c_str());


    while (t_parameters->user->isConnected()) {
        pthread_mutex_lock(&mutex);

        map<User*, list<MESSAGE>>::iterator user_messages_buffer_it;
        user_messages_buffer_it = user_messages_buffer.find(t_parameters->user);

        if (user_messages_buffer_it != user_messages_buffer.end()) { //if user present in dictionary


            while(user_messages_buffer_it->second.empty()) {  //while user has no messages
                if(!t_parameters->user->isConnected()){
                    logger.message(INFO, "--CONSUMER THREAD EXIT - user [%s] ", t_parameters->user->getName().c_str());
                    t_parameters->user->consumerThreadExit();
                    pthread_mutex_unlock(&mutex);
                    return; // exit thread if user disconected
                }
                pthread_cond_wait(&user_messages_buffer_it->first->cond_message_avaliable, &mutex);
                logger.message(INFO, "--SIGNAL MESSAGES AVALIABLE [%d] - User consumer [%s] ", user_messages_buffer_it->second.size(), t_parameters->user->getName().c_str());
            }


            MESSAGE my_task = user_messages_buffer_it->second.front(); // get user message

            if (communicationManager.messageSender(my_task)) {  // consume user message
                user_messages_buffer_it->second.pop_front();             // remove user message from list if consumed
                logger.message(INFO, "--User consumer [%s] consumed message from sender [%s] - payload %s", t_parameters->user->getName().c_str(), my_task.sender->getName().c_str(), to_string(my_task.payload).c_str());
            } else {
                logger.message(INFO, "--FAIL - User consumer [%s] fail to consume message from sender [%s] - payload %s", t_parameters->user->getName().c_str(), my_task.sender->getName().c_str(), to_string(my_task.payload).c_str());
            }
        }
        pthread_mutex_unlock(&mutex);
    }

    t_parameters->user->consumerThreadExit();
    logger.message(INFO, "--CONSUMMER STOPPED - User consumer [%s]", t_parameters->user->getName().c_str());

}

void Server::ReplicationManagerImplementation() {

}

void Server::startUserThreadConsumer(User* user) {
    if(!user->isConsumerThreadRunning() && user->isConnected()) {
        // if user who send message has no consumer, create one.

        auto* consumerArgs = new CONSUMER_ARGS();
        consumerArgs->user = user;

        logger.message(INFO, "[STARTING NEW THREAD CONSUMER LAUNCH]:[%s]", user->getName().c_str());
        user->setConsumerThread(this->StartConsumerThread(consumerArgs));
    }
}

