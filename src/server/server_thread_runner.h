//
// Created by vieir on 10/03/2022.
//

#ifndef INF01151_TF_SERVER_THREAD_RUNNER_H
#define INF01151_TF_SERVER_THREAD_RUNNER_H

#include <pthread.h>

#define     TRUE        1
#define		MAX_ITEMS	3



class ServerThreadRunner
{

public:
    ServerThreadRunner() {
        pthread_cond_init(&cond_empty, NULL);
        pthread_cond_init(&cond_full, NULL);
        pthread_mutex_init(&mutex, NULL);
    }
    virtual ~ServerThreadRunner() {/* empty */}

    /** Returns true if the thread was successfully started, false if there was an error starting the thread */
    bool StartProducerThread()
    {
        return (pthread_create(&_thread_producer, NULL, ProducerEntryFunc, this) == 0);
    }
    /** Returns true if the thread was successfully started, false if there was an error starting the thread */
    bool StartConsumerThread()
    {
        return (pthread_create(&_thread_consumer, NULL, ConsumerEntryFunc, this) == 0);
    }

    /** Will not return until the internal thread has exited. */
    void WaitForProducerToExit()
    {
        (void) pthread_join(_thread_producer, NULL);
    }
    /** Will not return until the internal thread has exited. */
    void WaitForConsumerToExit()
    {
        (void) pthread_join(_thread_consumer, NULL);
    }

protected:
    /** Implement this method in your subclass with the code you want your thread to run. */
    virtual void ProducerImplementation() = 0;
    virtual void ConsumerImplementation() = 0;

    pthread_cond_t 	cond_empty, cond_full;
    pthread_mutex_t mutex;
    int counter = 0, in = 0, out = 0;

    typedef struct {
        string sender;
        string body;

    } MESSAGE;

    MESSAGE buffer[MAX_ITEMS];

private:
    static void * ProducerEntryFunc(void * This) {((ServerThreadRunner *)This)->ProducerImplementation(); return NULL;}
    static void * ConsumerEntryFunc(void * This) {((ServerThreadRunner *)This)->ConsumerImplementation(); return NULL;}

    pthread_t _thread_producer;
    pthread_t _thread_consumer;



};


#endif //INF01151_TF_SERVER_THREAD_RUNNER_H
