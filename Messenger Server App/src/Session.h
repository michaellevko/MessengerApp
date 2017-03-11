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
#include "./SocketUtils/MTCPListener.h"
#include "./SocketUtils/TCPMessengerProtocol.h"

using namespace std;

class Session : public MThread{
public:
	class Handler{
	public:
		virtual void onSessionClose(Session* brocker,Peer* connA,Peer* connB)=0;
		virtual void onConnectedUsersList(TCPSocket* peer)=0;
		virtual void onUsersList(TCPSocket* peer)=0;
		virtual void onPeerDisconnect(Peer* peer)=0;
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
	Session();
	Session(Handler* handler, Peer* connA, Peer* connB);
	~Session();
};

#endif /* SRC_SESSION_H_ */
