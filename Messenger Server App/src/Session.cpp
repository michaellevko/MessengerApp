/*
 * Session.cpp
 *
 *  Created on: Mar 5, 2017
 *      Author: user
 */

#include "Session.h"

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
		listener.add(this->peerB->getPeerSock());
		TCPSocket * peer = listener.listen(LISTEN_TIMEOUT);
		if (peer != NULL) {
			vector<string> data;
			data = TCPMessengerProtocol::readMsg(peer);
			Peer* sender = FindPeer(peer);

			// Check if client disconnected
			if (data.size() == 0){
				this->close();
				this->handler->onPeerDisconnect(sender);
			} else {
				switch (atoi(data[0].c_str())) {

				case GET_ALL_CONNECTED_USERS:
				{
					this->handler->onConnectedUsersList(peer);
					break;
				}
				case GET_ALL_USERS:
				{
					this->handler->onUsersList(peer);
					break;
				}
				case CLOSE_SESSION_WITH_PEER:
				{
					close();
					break;
				}
				default:
				{
					TCPMessengerProtocol::sendMsg(peer, FAILURE);
				}}
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
