//
// Created by vieir on 07/03/2022.
//


#include <thread>
#include <sstream>
#include "server.h"


using namespace std;

Server::Server(int port): ServerThreadRunner() {
    this->port = port;
}

int Server::start() {
    logger.message(INFO,"---> Server starting on port: %d  <---", this->port);

//
//
//    CONSUMER_ARGS consumerArgs2;
//    consumerArgs2.stop_lt = 40;
//    pthread_t consumer2 = this->StartConsumerThread(consumerArgs2);
//
//    CONSUMER_ARGS consumerArgs3;
//    consumerArgs3.stop_lt = 80;
//    pthread_t consumer3 = this->StartConsumerThread(consumerArgs3);
//
//

//    this->WaitForConsumerToExit(consumer3);
//    this->WaitForConsumerToExit(consumer2);
//



    communicationManager.openSocket(port);

    pthread_t producer = this->StartProducerThread();


    CONSUMER_ARGS consumerArgs1;
    consumerArgs1.stop_lt = 123;
    pthread_t consumer1 = this->StartConsumerThread(consumerArgs1);

    WaitForConsumerToExit(consumer1);
    WaitForProducerToExit(producer);

    communicationManager.closeSocket();

    return 0;
}

bool Server::newUserConnection(User user) {
    return false;
}

void Server::ProducerImplementation() {
    while (TRUE) {
//        sleep(rand()%5);

        list<MESSAGE> messages = communicationManager.messageReceiver();
        if(messages.size() <= 0)
            continue;

        pthread_mutex_lock(&mutex);
        while (counter == MAX_ITEMS)
            pthread_cond_wait(&cond_empty, &mutex);


        for (auto it = begin (messages); it != end (messages); ++it) {
            int index = distance(messages.begin(), it);
            buffer[in] = *it;
            counter++;
            in = (in + 1) % MAX_ITEMS;
        }


//        MESSAGE message;
//        message.sender = "Sender-" + to_string(r);
//        message.body = "body-"+ to_string(r);
//        buffer[in] = message;
//        counter++;
//        logger.message(INFO, "------------------------------------Produzindo buffer[%d]: sender %s body %s", in, buffer[in].sender.c_str(), buffer[in].body.c_str());
//        in = (in + 1) % MAX_ITEMS;


        pthread_cond_signal(&cond_full);
        pthread_mutex_unlock(&mutex);
    }
}

void Server::ConsumerImplementation() {

    std::stringstream ss;
    ss << std::this_thread::get_id();
    string ts = pthreadAsString(this_thread::get_id());
    string ts2 = ss.str();

    CONSUMER_ARGS not_working_par1 = getThreadArgs(ts);
    CONSUMER_ARGS not_working_par2=  getThreadArgs(ts2);
    CONSUMER_ARGS not_working_par3 = pthread_args_dict.find(ss.str())->second;


    pthread_mutex_lock(&mutex);
    const CONSUMER_ARGS t_parameters = pthread_args_vec.back();
    pthread_args_vec.pop_back();
    pthread_mutex_unlock(&mutex);

    while (t_parameters.args_load) {
//        sleep(rand()%5);
        pthread_mutex_lock(&mutex);
        while (counter == 0)
            pthread_cond_wait(&cond_full, &mutex);


        MESSAGE my_task = buffer[out]; //get item to consume
        counter--;

        logger.message(INFO, "--ids[%s] param[%d] - Consumindo buffer[%d]: is_direct: %d -> payload %s",
                       ts.c_str(), t_parameters.stop_lt, out, my_task.direct_response, to_string(my_task.payload).c_str());

        out = (out + 1) % MAX_ITEMS;

        pthread_cond_signal(&cond_empty);
        pthread_mutex_unlock(&mutex);
    }

    logger.message(INFO, "--ids[%s] saindo [%d] buffer[%d]",
                   ts.c_str(), t_parameters.stop_lt, out);

}
