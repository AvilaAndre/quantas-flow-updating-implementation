//
//  ExamplePeer.cpp
//  BlockGuard
//
//  Created by Kendric Hood on 3/15/19.
//  Copyright © 2019 Kent State University. All rights reserved.
//

#include <iostream>
#include "ExamplePeer.hpp"

namespace blockguard {

	//
	// Example Channel definitions
	//
	ExamplePeer::~ExamplePeer() {

	}

	ExamplePeer::ExamplePeer(const ExamplePeer& rhs) : Peer<ExampleMessage>(rhs) {
		
	}

	ExamplePeer::ExamplePeer(long id) : Peer(id) {
		
	}

	void ExamplePeer::performComputation() {
		cout << "Peer:" << id() << " performing computation" << endl;
		// Send message to self
		ExampleMessage msg;
		msg.message = "Message: it's me " + std::to_string(id()) + "!";
		msg.aPeerId = std::to_string(id());
		Packet<ExampleMessage> newMsg(getRound(), id(), id());
		newMsg.setMessage(msg);
		pushToOutSteam(newMsg);

		// Send hello to everyone else
		msg.message = "Message: Hello From " + std::to_string(id()) + ". Sent on round: " + std::to_string(getRound());
		msg.aPeerId = std::to_string(id());
		broadcast(msg);

		while (!inStreamEmpty()) {
			Packet<ExampleMessage> newMsg = popInStream();
			cout << endl << std::to_string(id()) << " has receved a message from " << newMsg.getMessage().aPeerId << endl;
			cout << newMsg.getMessage().message << endl;
		}
		cout << endl;
	}

	void ExamplePeer::endOfRound(const vector<Peer<ExampleMessage>*>& _peers) {
		cout << "End of round " << getRound() << endl;
	}

	ostream& ExamplePeer::printTo(ostream& out)const {
		Peer<ExampleMessage>::printTo(out);

		out << id() << endl;
		out << "counter:" << getRound() << endl;

		return out;
	}

	ostream& operator<< (ostream& out, const ExamplePeer& peer) {
		peer.printTo(out);
		return out;
	}
}