#include "FlowUpdatingPeer.hpp"
#include <cstdlib>
#include <unordered_map>

namespace quantas {

//
// FlowUpdating Channel definitions
//
FlowUpdatingPeer::~FlowUpdatingPeer() {}

FlowUpdatingPeer::FlowUpdatingPeer(const FlowUpdatingPeer &rhs)
    : Peer<FlowUpdatingMessage>(rhs) {}

FlowUpdatingPeer::FlowUpdatingPeer(long id) : Peer(id) {
    this->value = (double)(rand() % 40 + 10);
}

FlowUpdatingMessage FlowUpdatingPeer::generateMessage(long idx) {
    double flow = 0.0;
    if (this->initialFlows.find(idx) != this->initialFlows.end()) {
        flow = this->initialFlows.at(idx);
    }

    return FlowUpdatingMessage{
        .senderId = this->id(),
        .flow = flow,
        .estimate = this->estimate(this->initialFlows),
    };
}

double FlowUpdatingPeer::estimate(std::unordered_map<long, double> &flows) {
    double flowsSum = 0.0;
    for (const auto &[_, value] : flows) {
        flowsSum += value;
    }
    return this->value - flowsSum;
}

void FlowUpdatingPeer::handleNewMessages() {
    while (!inStreamEmpty()) {
        Packet<FlowUpdatingMessage> newMsg = popInStream();
        /* DEBUG
        cout << std::to_string(id()) << " has receved a message from "
             << newMsg.getMessage().senderId << " with flow "
             << newMsg.getMessage().flow << " and estimate "
             << newMsg.getMessage().estimate << endl;
             */

        rcvdMsgs.insert_or_assign(newMsg.sourceId(), newMsg.getMessage());
    }
}

void FlowUpdatingPeer::stateTransition() {
    auto neighs = neighbors();

    std::unordered_map<long, double> stateF;

    for (auto it = neighs.begin(); it != neighs.end(); it++) {
        if (this->rcvdMsgs.find(*it) != this->rcvdMsgs.end()) {
            FlowUpdatingMessage msg = this->rcvdMsgs.at(*it);
            stateF.insert_or_assign(*it, msg.flow * -1);
        } else if (this->initialFlows.find(*it) != this->initialFlows.end()) {
            stateF.insert_or_assign(*it, this->initialFlows.at(*it));
        }
    }

    std::unordered_map<long, double> stateE;

    stateE.insert_or_assign(id(), this->estimate(stateF));

    for (auto it = neighs.begin(); it != neighs.end(); it++) {
        if (this->rcvdMsgs.find(*it) != this->rcvdMsgs.end()) {
            FlowUpdatingMessage msg = this->rcvdMsgs.at(*it);
            stateE.insert_or_assign(*it, msg.estimate);
        } else {
            stateE.insert_or_assign(*it, this->estimate(this->initialFlows));
        }
    }

    double avg = 0.0;
    if (stateE.size() > 0) {
        double tot = 0.0;
        for (const auto &[_, val] : stateE) {
            tot += val;
        }
        avg = tot / stateE.size();
    }

    std::unordered_map<long, double> fPrime;

    for (const auto &[key, val] : stateF) {
        fPrime.insert_or_assign(key, val + avg - stateE.at(key));
    }

    // Set flows for next round
    this->initialFlows = fPrime;
}

void FlowUpdatingPeer::performComputation() {
    cout << "FlowUpdatingPeer: " << id() << " with value: " << value
         << " and estimate: " << this->estimate(this->initialFlows)
         << " performing computation" << endl;

    vector<long> neighborIds = neighbors();

    rcvdMsgs.clear();

    auto neighs = neighbors();

    for (auto it = neighs.begin(); it != neighs.end(); it++) {
        // DEBUG
        // cout << "> " << id() << " unicasting to: " << *it << endl;
        unicastTo(generateMessage(*it), *it);
    }

    handleNewMessages();

    stateTransition();

    /* DEBUG
    cout << id() << " initialFlows = [ ";
    for (const auto &[key, val] : initialFlows) {
        cout << key << ": " << val << "; ";
    }
    cout << "]" << endl;
    */
}

void FlowUpdatingPeer::endOfRound(
    const vector<Peer<FlowUpdatingMessage> *> &_peers
) {
    cout << endl << "End of round " << getRound() << endl << endl;

    const vector<FlowUpdatingPeer *> peers =
        reinterpret_cast<vector<FlowUpdatingPeer *> const &>(_peers);

    if (lastRound()) {
        for (const auto &peer : peers) {
            LogWriter::getTestLog()["initialFlowsS"][std::to_string(peer->id())] =
                peer->estimate(peer->initialFlows);
        }
    }
}

Simulation<quantas::FlowUpdatingMessage, quantas::FlowUpdatingPeer> *
generateSim() {

    Simulation<quantas::FlowUpdatingMessage, quantas::FlowUpdatingPeer> *sim =
        new Simulation<quantas::FlowUpdatingMessage, quantas::FlowUpdatingPeer>;
    return sim;
}
} // namespace quantas
