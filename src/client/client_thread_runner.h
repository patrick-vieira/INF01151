//
// Created by vieir on 10/03/2022.
//

#ifndef INF01151_TF_CLIENT_THREAD_RUNNER_H
#define INF01151_TF_CLIENT_THREAD_RUNNER_H

#include <pthread.h>

class ClientThreadRunner
{
public:
    ClientThreadRunner() {/* empty */}
    virtual ~ClientThreadRunner() {/* empty */}

    /** Returns true if the thread was successfully started, false if there was an error starting the thread */
    bool StartMessageSenderThread()
    {
        return (pthread_create(&_thread_message_sender, NULL, MessageSenderEntryFunc, this) == 0);
    }
    /** Returns true if the thread was successfully started, false if there was an error starting the thread */
    bool StartMessageListennerThread()
    {
        return (pthread_create(&_thread_message_listenner, NULL, MessageListennerEntryFunc, this) == 0);
    }

    /** Will not return until the internal thread has exited. */
    void WaitForMessageSenderToExit()
    {
        (void) pthread_join(_thread_message_sender, NULL);
    }
    /** Will not return until the internal thread has exited. */
    void WaitForMessageListennerToExit()
    {
        (void) pthread_join(_thread_message_listenner, NULL);
    }

protected:
    /** Implement this method in your subclass with the code you want your thread to run. */
    virtual void MessageSenderImplementation() = 0;
    virtual void MessageListennerImplementation() = 0;

    bool login_success = false;
    bool running = true;

private:
    static void * MessageSenderEntryFunc(void * This) {((ClientThreadRunner *)This)->MessageSenderImplementation(); return NULL;}
    static void * MessageListennerEntryFunc(void * This) {((ClientThreadRunner *)This)->MessageListennerImplementation(); return NULL;}

    pthread_t _thread_message_sender;
    pthread_t _thread_message_listenner;
};


#endif //INF01151_TF_CLIENT_THREAD_RUNNER_H
