#ifndef FlowUpdatingPeer_hpp
#define FlowUpdatingPeer_hpp

#include "../Common/Peer.hpp"
#include "../Common/Simulation.hpp"
#include <iostream>
#include <unordered_map>

namespace quantas {

using std::ostream;
using std::string;

//
// FlowUpdating of a message body type
//
struct FlowUpdatingMessage {
    long senderId;
    double flow;
    double estimate;
};

//
// FlowUpdating Peer used for network testing
//
class FlowUpdatingPeer : public Peer<FlowUpdatingMessage> {
  public:
    // methods that must be defined when deriving from Peer
    FlowUpdatingPeer(long);
    FlowUpdatingPeer(const FlowUpdatingPeer &rhs);
    ~FlowUpdatingPeer();

    // perform one step of the Algorithm with the messages in inStream
    void performComputation();
    // perform any calculations needed at the end of a round such as determine
    // throughput (only ran once, not for every peer)
    void endOfRound(const vector<Peer<FlowUpdatingMessage> *> &_peers);

    void handleNewMessages();

    void stateTransition();


    ostream &printTo(ostream &) const;
    friend ostream &operator<<(ostream &, const FlowUpdatingPeer &);

  private:
    double value;
    std::unordered_map<long, FlowUpdatingMessage> rcvdMsgs;
    std::unordered_map<long, double> initialFlows;

    FlowUpdatingMessage generateMessage(long idx);
    double estimate(std::unordered_map<long, double> &);
};

Simulation<quantas::FlowUpdatingMessage, quantas::FlowUpdatingPeer> *
generateSim();
} // namespace quantas
#endif /* FlowUpdatingPeer_hpp */
