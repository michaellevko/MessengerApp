/*
 * Session.cpp
 *
 *  Created on: Mar 5, 2017
 *      Author: user
 */

#include "Session.h"

using namespace npl;

Session::Session(Handler* handler, Peer* connA, Peer* connB){
	this->peerA = connA;
	this->peerB = connB;
	this->handler = handler;
	this->isActive = true;
	start();
}

void Session::run(){
	while (isActive) {
			MTCPListener listener;
			listener.add(this->peerA->getPeerSock());
			listener.add(peerB->getPeerSock());
			TCPSocket * peer = listener.listen();
			if (peer != NULL) {
				vector<string> data;
				data = TCPMessengerProtocol::readMsg(peer);

				Peer* sender = FindPeer(peer);
				switch (atoi(data[0].c_str())) {

				default:
				}
			}
		}
		delete this;
}

Peer* Session::FindPeer(TCPSocket* peer) {
	if (peer == this->peerA->getPeerSock()){
		return peerA;
	}
	return peerB;
}

Session::~Session(){

}

Peer* Session::getPeerA(){
	return this->peerA;
}

Peer* Session::getPeerB(){
	return this->peerB;
}

void Session::close(){
	this->isActive = false;
	handler->onSessionClose(this, this->peerA, this->peerB);
}
