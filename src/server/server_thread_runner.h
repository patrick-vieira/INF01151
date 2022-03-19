//
// Created by vieir on 10/03/2022.
//

#ifndef INF01151_TF_SERVER_THREAD_RUNNER_H
#define INF01151_TF_SERVER_THREAD_RUNNER_H

#include <pthread.h>
#include <map>
#include <sstream>

using namespace std;

class ServerThreadRunner
{

public:
    typedef struct {
//        bool direct_response_consumer = false;
        User* user;
    } CONSUMER_ARGS;

    ServerThreadRunner() {
//        pthread_cond_init(&cond_message_avaliable, NULL);
        pthread_mutex_init(&mutex, NULL);
        pthread_mutex_init(&mutex_args, NULL);
    }
    virtual ~ServerThreadRunner() {/* empty */}

    /** Returns true if the thread was successfully started, false if there was an error starting the thread */
    pthread_t StartProducerThread()
    {
        pthread_t _thread_producer;

        bool success = (pthread_create(&_thread_producer, NULL, ProducerEntryFunc, this) == 0);
        if (success)
            return _thread_producer;
        else
            exit(99);
    }
    /** Returns true if the thread was successfully started, false if there was an error starting the thread */
    pthread_t StartConsumerThread(CONSUMER_ARGS stop_lt)
    {
        pthread_t _thread_consumer;
        pthread_args_vec.push_back(stop_lt);

        string key  = pthreadAsString(_thread_consumer);
        pthread_args_dict.insert(pair<string, CONSUMER_ARGS>(key, stop_lt));

        bool success = (pthread_create(&_thread_consumer, NULL, ConsumerEntryFunc, this) == 0);

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


    template <class T> inline //required to pass pthread pointer as parameter on diferent systems
    std::string pthreadAsString(const T& t) {
        std::stringstream ss;
        ss << t;
        return ss.str();
    }

    // it doesn't work but it should
    CONSUMER_ARGS getThreadArgs(string sid){
//        thread::id tid = this_thread::get_id();
//        string stid = pthreadAsString(tid);
//
//        std::stringstream ss;
//        ss << std::this_thread::get_id();

        CONSUMER_ARGS a2 = pthread_args_dict.find(sid)->second;
        return a2;
    }


protected:
    /** Implement this method in your subclass with the code you want your thread to run. */
    virtual void ProducerImplementation() = 0;
    virtual void ConsumerImplementation() = 0;

//    pthread_cond_t 	cond_message_avaliable;
    pthread_mutex_t mutex, mutex_args;
    int counter = 0, in = 0, out = 0;

    map<string , CONSUMER_ARGS> pthread_args_dict;
    std::vector<CONSUMER_ARGS> pthread_args_vec;


private:
    static void * ProducerEntryFunc(void * This) {((ServerThreadRunner *)This)->ProducerImplementation(); return NULL;}
    static void * ConsumerEntryFunc(void * This) {((ServerThreadRunner *)This)->ConsumerImplementation(); return NULL;}


//    pthread_t _thread_producer;
//    pthread_t _thread_consumer;




};


#endif //INF01151_TF_SERVER_THREAD_RUNNER_H
