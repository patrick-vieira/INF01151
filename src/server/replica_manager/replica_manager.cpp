//
// Created by vieir on 04/05/2022.
//

#include "replica_manager.h"
#include "../user/user.h"


int ReplicaManager::is_primary() {

    send_ping_to_primary();

    if(elected) {
        WaitForListenerToExit(pthread_thread_election_listener);
        return 1;
    }

    return 0;
}

void ReplicaManager::send_ping_to_primary() {
    json message;
    message["type"] = PING_RM;

    session.sendMessage(message);

    last_ping_send_time = time(0);
}

 void ReplicaManager::ListenerImplementation() {

    while(!elected) {
        json message = session.receiveMessage();

        json replicas_json;
        switch (message["type"].get<int>()) {
            case NO_MESSAGE:
                logger.message(INFO, "NO_MESSAGE");

                if(participating_election) {
                    election_messages_sent--; // wait for n timeouts;
                    if(election_messages_sent <= 0)
                        win_election();
                    break;
                }

                if(primary_timeout() && !participating_election)
                    start_election();

                break;

            case PING_RM_RESPONSE:
                logger.message(INFO, "[Primary Server Response]: %s", to_string(message).c_str());

                priority = message.at("priority").get<int>();

                replicas_json = message.at("replicas");
                update_known_replicas(replicas_json);

                last_ping_response_time = time(0);
                break;

            case ELECTION:

                logger.message(INFO, "[ELECTION]: priority received: %d, my priority: %d", message["priority"].get<int>(), priority);
                election_in_progress = message["priority"].get<int>();

                if(election_in_progress > priority) {
                    logger.message(INFO, "[ELECTION]: ignored");
                    participating_election = 0;
                }

                if(!participating_election && election_in_progress < priority) {
                    logger.message(INFO, "[ELECTION]: Joined");
                    reply_election(message);
                    start_election();
                }
                break;

            case ELECTION_RESPONSE:
                election_in_progress = message["priority"].get<int>();

                if(election_in_progress > priority) {
                    logger.message(INFO, "[ELECTION]: dropped out from election");
                    participating_election = 0;
                }
                break;

            case ELECTION_RESULT:
                logger.message(INFO, "[ELECTION]: finished new priority: %d", message["priority"].get<int>());
                reset();
                break;

            default:
                logger.message(INFO, "[ReplicaManager::connect_with_primary :::: DEFAULT] - type %d", message["type"].get<int>());
                break;
        }

        sleep(2);
    }
}

void ReplicaManager::reset() {
    elected = 0;
    participating_election = 0;
    priority = 0;
    election_in_progress = 0;

    replicas.clear();

}
REPLICA ReplicaManager::create_replica(sockaddr_in replica_address) {
    if(elected) {

        ADDRESS address(replica_address);

        for (auto replica_it = begin (replicas); replica_it != end (replicas); ++replica_it) {
            if(strcmp(replica_it->address.ip, address.ip) == 0 && replica_it->address.port == address.port)
                return replica_it.operator*();
        }

        REPLICA replica;
        replica.address = address;
        replica.elected = 0;
        replica.priority = replicas.size() + 1;
        replicas.push_back(replica);

        return replica;
    } else
        throw ERROR_NOT_ALLOWED;
}

void ReplicaManager::start_election() {

    if(election_in_progress == 0) {
        logger.message(INFO, "[ELECTION]: Start");
        election_in_progress = priority;
    }

    if(election_in_progress > priority) {
        logger.message(INFO, "[ELECTION]: in progress not participating: my priority %d, other priority: %d", priority, election_in_progress);
        return;
    }

    participating_election = 1;

    json election_message;
    election_message["type"] = ELECTION;
    election_message["priority"] = priority;

    for (auto replica_it = begin (replicas); replica_it != end (replicas); ++replica_it) {
        if(replica_it->priority > priority) {
            Session new_session(replica_it->address.ip, replica_it->address.port, 1);
            new_session.openConnection();
            new_session.sendMessage(election_message);
            new_session.closeConnection();
            election_messages_sent++;

            logger.message(INFO, "[ELECTION]: Send ELECTION [%d] START Message to %s:%d", priority, replica_it->address.ip, replica_it->address.port);
            break;
        }
    }

}

json ReplicaManager::get_replicas_as_json() {

    json response = json::array({});

    for (auto replica_it = begin (replicas); replica_it != end (replicas); ++replica_it) {
        json replica;
        replica["ip"] = replica_it->address.ip;
        replica["port"] = replica_it->address.port;
        replica["priority"] = replica_it->priority;
        replica["elected"] = replica_it->elected;
        response.insert(response.end(), replica);
    }

    return response;
}

void ReplicaManager::update_known_replicas(json replicas_json) {

    replicas.clear();

    for (auto& [key, val] : replicas_json.items()) {
        logger.message(INFO, "key: %s, ip: %s, port: %d", key.c_str(), val["ip"].get<string>().c_str(), val["port"].get<int>());

        REPLICA replica;
        replica.elected = val["elected"].get<int>();
        replica.priority = val["priority"].get<int>();
        replica.address = ADDRESS(val["ip"].get<string>().c_str(), val["port"].get<int>());

        if(priority != replica.priority)
            replicas.push_back(replica);

    }

}

int ReplicaManager::primary_timeout() {
    double diff = difftime(last_ping_response_time,last_ping_send_time);
    bool time_out = diff > 50 || diff < 0;

    if(time_out)
        return 1;

    return 0;
}

void ReplicaManager::init() {

    pthread_thread_election_listener = this->StartListenerThread();

    session.openConnection();
}

void ReplicaManager::close() {
    session.closeConnection();
}

void ReplicaManager::reply_election(json message) {

    Session new_session(message["received_from_host"].get<string>(), message["received_from_port"].get<int>(), 1);
    new_session.openConnection();

    json answer;
    answer["priority"] = priority;
    answer["type"] = ELECTION_RESPONSE;

    new_session.sendMessage(answer);
    new_session.closeConnection();
}

void ReplicaManager::win_election() {

    if(election_in_progress > priority)
        return;

    elected = 1;
    participating_election = 0;

    logger.message(INFO, "[ELECTION]: Win");

    for (auto replica_it = begin(replicas); replica_it != end(replicas); ++replica_it) {
        if (replica_it->priority > priority) {
            json election_message;
            election_message["type"] = ELECTION_RESULT;
            election_message["priority"] = priority;

            Session new_session(replica_it->address.ip, replica_it->address.port, 1);
            new_session.openConnection();
            new_session.sendMessage(election_message);
            new_session.closeConnection();

            logger.message(INFO, "[ELECTION]: Send WIN Message to %s:%d", replica_it->address.ip, replica_it->address.port);

            break;
        }
    }
}

