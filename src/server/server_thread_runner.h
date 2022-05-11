//
// Created by vieir on 10/03/2022.
//

#ifndef INF01151_TF_SERVER_THREAD_RUNNER_H
#define INF01151_TF_SERVER_THREAD_RUNNER_H

#include <pthread.h>
#include <map>
#include <sstream>
#include <thread>

using namespace std;

class ServerThreadRunner
{

public:
    typedef struct {
        User* user;
    } CONSUMER_ARGS;

    ServerThreadRunner() {
        pthread_mutex_init(&mutex, NULL);
        pthread_mutex_init(&mutex_consumer_starting, NULL);
    }
    virtual ~ServerThreadRunner() {/* empty */}



    pthread_t StartProducerThread() {
        pthread_t _thread_producer;

        bool success = (pthread_create(&_thread_producer, NULL, ProducerEntryFunc, this) == 0);
        if (success)
            return _thread_producer;
        else
            exit(99);
    }

    void WaitForProducerToExit(pthread_t _thread_producer) {
        (void) pthread_join(_thread_producer, NULL);
    }



    pthread_t StartConsumerThread(CONSUMER_ARGS* consumerArgs) {
        pthread_t _thread_consumer;


        pthread_mutex_lock(&mutex_consumer_starting);
        bool success = (pthread_create(&_thread_consumer, NULL, ConsumerEntryFunc, this) == 0);
        pthread_args_dict.insert(pair<pthread_t, CONSUMER_ARGS*>(_thread_consumer, consumerArgs));//
        pthread_mutex_unlock(&mutex_consumer_starting);

        if (success)
            return _thread_consumer;
        else
            exit(98);
    }

    void WaitForConsumerToExit(pthread_t _thread_consumer) {
        (void) pthread_join(_thread_consumer, NULL);
    }



    pthread_t StartPingThread() {
        pthread_t _thread_ping;

        bool success = (pthread_create(&_thread_ping, NULL, PingEntryFunc, this) == 0);
        if (success)
            return _thread_ping;
        else
            exit(99);
    }

    void WaitForPingToExit(pthread_t _thread_ping) {
        (void) pthread_join(_thread_ping, NULL);
    }

    pthread_t ReplicationManagerThread() {
        pthread_t _thread_rm;

        bool success = (pthread_create(&_thread_rm, NULL, ReplicationManagerEntryFunc, this) == 0);
        if (success)
            return _thread_rm;
        else
            exit(99);
    }

    void ReplicationManagerToExit(pthread_t _thread_rm) {
        (void) pthread_join(_thread_rm, NULL);
    }


    CONSUMER_ARGS* getThreadArgs(unsigned long sid) {
        CONSUMER_ARGS* args = pthread_args_dict.find(sid)->second;
        return args;
    }


protected:
    /** Implement this method in your subclass with the code you want your thread to run. */
    virtual void ProducerImplementation() = 0;
    virtual void ConsumerImplementation() = 0;
    virtual void PingImplementation() = 0;
    virtual void ReplicationManagerImplementation() = 0;

    pthread_mutex_t mutex{}, mutex_consumer_starting{};

    map<unsigned long , CONSUMER_ARGS*> pthread_args_dict;


private:
    static void * ProducerEntryFunc(void * This) {((ServerThreadRunner *)This)->ProducerImplementation(); return NULL;}
    static void * ConsumerEntryFunc(void * This) {((ServerThreadRunner *)This)->ConsumerImplementation(); return NULL;}
    static void * PingEntryFunc(void * This) {((ServerThreadRunner *)This)->PingImplementation(); return NULL;}
    static void * ReplicationManagerEntryFunc(void * This) {((ServerThreadRunner *)This)->ReplicationManagerImplementation(); return NULL;}


};


#endif //INF01151_TF_SERVER_THREAD_RUNNER_H
