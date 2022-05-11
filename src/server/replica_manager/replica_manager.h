//
// Created by vieir on 04/05/2022.
//

#ifndef INF01151_REPLICA_MANAGER_H
#define INF01151_REPLICA_MANAGER_H


#include <arpa/inet.h>
#include <list>
#include "../../aux_shared/logger.h"
#include "../../aux_shared/session/session.h"
#include "../../aux_shared/message_types.h"

#include "nlohmann/json.hpp"
#include "replica_thread_runner.h"

typedef struct {
    int elected;
    int priority;

    ADDRESS address;

} REPLICA;


struct thread_info_election_listener {
    pthread_t id;
    int num;
};


class ReplicaManager : ReplicaThreadRunner {
    Logger logger;

private:
    string host;
    int port;
    int elected;

    int participating_election = 0;

    int priority = 0;
    int election_in_progress = 0;

    int election_messages_sent = 0;

    list<REPLICA> replicas;

    time_t last_ping_send_time = time(0);
    time_t last_ping_response_time = time(0);

    Session session;
    pthread_t pthread_thread_election_listener;

public:
    ReplicaManager(string host, int port) {
        this->host = host;
        this->port = port;
        this->session = Session(host, port, 1);
        this->init();
    }

    int is_primary();
    REPLICA create_replica(sockaddr_in replica_address);
    json get_replicas_as_json();

    void close();

private:

    void ListenerImplementation() override;

    void init();

    void send_ping_to_primary();
    void update_known_replicas(json replicas_json);

    int primary_timeout();
    void start_election();
    void win_election();
    void reply_election(json json);
    void reset();

    static void *thread_election_listener(void *arg);
};


#endif //INF01151_REPLICA_MANAGER_H
