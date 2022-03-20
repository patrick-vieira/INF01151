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
        pthread_cond_init(&cond_consumer_args_available, NULL); // this condintion
        pthread_mutex_init(&mutex, NULL);
        pthread_mutex_init(&mutex_args, NULL);
    }
    virtual ~ServerThreadRunner() {/* empty */}

    pthread_t StartProducerThread()
    {
        pthread_t _thread_producer;

        bool success = (pthread_create(&_thread_producer, NULL, ProducerEntryFunc, this) == 0);
        if (success)
            return _thread_producer;
        else
            exit(99);
    }

    pthread_t StartConsumerThread(CONSUMER_ARGS* consumerArgs)
    {
        pthread_t _thread_consumer;

        bool success = (pthread_create(&_thread_consumer, NULL, ConsumerEntryFunc, this) == 0);
        pthread_args_dict.insert(pair<pthread_t, CONSUMER_ARGS*>(_thread_consumer, consumerArgs));
        pthread_cond_signal(&cond_consumer_args_available);

        if (success)
            return _thread_consumer;
        else
            exit(98);
    }

    /** Will not return until the internal thread has exited. */
    void WaitForProducerToExit(pthread_t _thread_producer)
    {
        (void) pthread_join(_thread_producer, NULL);
    }
    /** Will not return until the internal thread has exited. */
    void WaitForConsumerToExit(pthread_t _thread_consumer)
    {
        (void) pthread_join(_thread_consumer, NULL);
    }


    CONSUMER_ARGS* getThreadArgs(unsigned long sid){
        CONSUMER_ARGS* args = pthread_args_dict.find(sid)->second;
        return args;
    }


protected:
    /** Implement this method in your subclass with the code you want your thread to run. */
    virtual void ProducerImplementation() = 0;
    virtual void ConsumerImplementation() = 0;

    pthread_cond_t 	cond_consumer_args_available{};
    pthread_mutex_t mutex{}, mutex_args{};

    map<unsigned long , CONSUMER_ARGS*> pthread_args_dict;


private:
    static void * ProducerEntryFunc(void * This) {((ServerThreadRunner *)This)->ProducerImplementation(); return NULL;}
    static void * ConsumerEntryFunc(void * This) {((ServerThreadRunner *)This)->ConsumerImplementation(); return NULL;}


};


#endif //INF01151_TF_SERVER_THREAD_RUNNER_H
