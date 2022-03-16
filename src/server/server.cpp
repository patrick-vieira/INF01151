//
// Created by vieir on 07/03/2022.
//


#include "server.h"
#include "communication_manager/communication_manager.h"

using namespace std;

Server::Server(int port): ServerThreadRunner() {
    this->port = port;
}

int Server::start() {
    logger.message(INFO,"---> Server starting on port: %d  <---", this->port);

    this->StartConsumerThread();

    this->StartProducerThread();

    this->WaitForConsumerToExit();

//    CommunicationManager communicationManager;
//
//    communicationManager.openSocket(port);
//
//    communicationManager.messageReceiver();
//
//    communicationManager.closeSocket();

    return 0;
}

bool Server::newUserConnection(User user) {
    return false;
}

void Server::ProducerImplementation() {
    while (TRUE) {
        sleep(rand()%5);
        pthread_mutex_lock(&mutex);
        while (counter == MAX_ITEMS)
            pthread_cond_wait(&cond_empty, &mutex);


//        buffer[in] = rand()%100;
        int r = rand()%100;
        MESSAGE message;
        message.sender = "Sender-" + to_string(r);
        message.body = "body-"+ to_string(r);
        buffer[in] = message;
        counter++;
        logger.message(INFO, "-----------Produzindo buffer[%d]: sender %s body %s", in, buffer[in].sender.c_str(), buffer[in].body.c_str());
        in = (in + 1) % MAX_ITEMS;


        pthread_cond_signal(&cond_full);
        pthread_mutex_unlock(&mutex);
    }
}

void Server::ConsumerImplementation() {

    MESSAGE my_task;

    int consumer_id = rand() %100;

    while (TRUE) {
        sleep(rand()%5);
        pthread_mutex_lock(&mutex);
        while (counter == 0)
            pthread_cond_wait(&cond_full, &mutex);


        my_task = buffer[out]; //get item to consume
        counter--;
        logger.message(INFO, "--Consumindo id[%d] buffer[%d]: sender: %s -> body %s", consumer_id, out, my_task.sender.c_str(), my_task.body.c_str());
        out = (out + 1) % MAX_ITEMS;


        pthread_cond_signal(&cond_empty);
        pthread_mutex_unlock(&mutex);
    }

}
