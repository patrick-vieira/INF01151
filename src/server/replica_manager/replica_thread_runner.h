//
// Created by vieir on 06/05/2022.
//

#ifndef INF01151_REPLICA_THREAD_RUNNER_H
#define INF01151_REPLICA_THREAD_RUNNER_H

#include <pthread.h>
#include <thread>

class ReplicaThreadRunner
{

public:
    ReplicaThreadRunner() {
        pthread_mutex_init(&mutex, NULL);
    }
    virtual ~ReplicaThreadRunner() {/* empty */}

    pthread_t StartListenerThread() {
        pthread_t _thread_listener;

        bool success = (pthread_create(&_thread_listener, NULL, ListenerEntryFunc, this) == 0);
        if (success)
            return _thread_listener;
        else
            exit(99);
    }

    void WaitForListenerToExit(pthread_t _thread_listener) {
        (void) pthread_join(_thread_listener, NULL);
    }

protected:
    virtual void ListenerImplementation() = 0;
    pthread_mutex_t mutex{};

private:
    static void * ListenerEntryFunc(void * This) {((ReplicaThreadRunner *)This)->ListenerImplementation(); return NULL;}

};
#endif //INF01151_REPLICA_THREAD_RUNNER_H
