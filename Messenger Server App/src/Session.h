/*
 * Session.h
 *
 *  Created on: Mar 5, 2017
 *      Author: user
 */

#ifndef SRC_SESSION_H_
#define SRC_SESSION_H_

#include "Peer.h"
#include "./SocketUtils/MThread.h"
#include "./SocketUtils/TCPSocket.h"
#include "./SocketUtils/MTCPListener.h"
#include <vector>
#include "./SocketUtils/TCPMessengerProtocol.h"

using namespace std;
using namespace npl;

class Session : public MThread{
public:
	class Handler{
	public:
		virtual void onSessionClose(Session* brocker,Peer* connA,Peer* connB)=0;
		virtual void onConnectedUserList(TCPSocket* peer, vector<string> data)=0;
	};
private:
	Peer* peerA;
	Peer* peerB;
	bool isActive;
	Handler* handler;
public:
	Peer* getPeerA();
	Peer* getPeerB();
	void run();
	void close();
	Peer* FindPeer(TCPSocket* peer);
	Session(Handler* handler, Peer* connA, Peer* connB);
	~Session();
};



#endif /* SRC_SESSION_H_ */
