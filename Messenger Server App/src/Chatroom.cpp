/*
 * Chatroom.cpp
 *
 *  Created on: Mar 5, 2017
 *      Author: user
 */

#include "Chatroom.h"

using namespace std;

// Constructor for a new chatroom instance
Chatroom::Chatroom(Handler* handler,Peer* peer, string chatRoomName) {
	this->chatRoomOwner = peer;
	this->chatRoomName = chatRoomName;
	this->chatRoomPeers.push_back(peer);
	this->chatActive = true;
	this->handler = handler;
}

Chatroom::~Chatroom(){

}

// Returns a roomName
string Chatroom::getRoomName() {
	return this->chatRoomName;
}

// Returns vector of peers in room by roomName
vector<Peer*> Chatroom::getRoomPeers(string roomName){
	return this->chatRoomPeers;
}

string Chatroom::getRoomOwner(){
	return this->chatRoomOwner->getPeerName();
}

// Adds peer to chatroom peers vector
void Chatroom::addPeer(Peer* peer){
	pthread_mutex_lock(&lock);
	this->chatRoomPeers.push_back(peer);
	pthread_mutex_unlock(&lock);
	if (this->chatRoomPeers.size() == 1){
		start();
	}
}


// Removes a peer from chatRoomPeers vector
void Chatroom::removePeer(Peer* peerToRemove){
	vector<Peer*>::iterator it;
	for (it = this->chatRoomPeers.begin(); it != this->chatRoomPeers.end();it++) {
		Peer* peer = *it;
		if (peer == peerToRemove){
			this->chatRoomPeers.erase(it);
			break;
		}
	}
}

void Chatroom::run(){
	while (this->chatActive) {
		MTCPListener listener;
		listener.add(this->getPeersSockets());
		TCPSocket * peer = listener.listen();
		if (peer != NULL) {
			vector<string> data;
			data = TCPMessengerProtocol::readMsg(peer);
			Peer* sender = this->FindPeer(peer);
			switch (atoi(data[0].c_str())) {

			case DESTROY_CHAT_ROOM:
				if((sender == this->chatRoomOwner) && (this->chatRoomPeers.size() == 0)){
					this->closeRoom();
					TCPMessengerProtocol::sendMsg(peer, SUCCESS);
				} else {
					TCPMessengerProtocol::sendMsg(peer, FAILURE);
				}
				break;

			case GET_ALL_USERS_IN_ROOM:
					TCPMessengerProtocol::sendMsg(peer, SUCCESS, this->getRoomPeersNames(this->getRoomName()));
				break;

			case GET_ALL_USERS:
					this->handler->onUsersList(peer);
				break;

			case EXIT_CHAT_ROOM:
					this->removePeer(sender);
					this->handler->onChatRoomExit(sender);
					TCPMessengerProtocol::sendMsg(peer,SUCCESS);
				break;

			case EXIT:
					TCPMessengerProtocol::sendMsg(peer,FAILURE);
				break;

			case GET_ALL_CONNECTED_USERS:
					handler->onConnectedUsersList(peer);
				break;

			default:
				TCPMessengerProtocol::sendMsg(peer, FAILURE);
			}
		}
	}
	delete this;
}

// Marks room as closed and calls handler to delete from chatroom vector
void Chatroom::closeRoom(){
	this->chatActive = false;
	this->handler->onChatRoomClose(this);
}

// Returns the sockets of the peers in peers vector
vector<TCPSocket*> Chatroom::getPeersSockets() {
	vector<TCPSocket*> peersSockets;
	for (int i = 0; i < this->chatRoomPeers.size(); i++) {
		peersSockets.push_back(this->chatRoomPeers[i]->getPeerSock());
	}

	return peersSockets;
}

// Returns a peer from peers vector by userName
Peer*  Chatroom::FindPeer(string userName) {
	vector<Peer*>::iterator it;
	for (it = chatRoomPeers.begin(); it != chatRoomPeers.end();it++) {
		Peer* peer = *it;
		if (peer->getPeerName() == userName){
			return *it;
		}
	}
	return NULL;
}

// Returns a peer from peers vector by socket
Peer* Chatroom::FindPeer(TCPSocket* conn) {
	vector<Peer*>::iterator it;
	for (it = chatRoomPeers.begin(); it != chatRoomPeers.end();it++) {
		Peer* peer = *it;
		if (peer->getPeerSock() == conn){
			return *it;
		}
	}
	return NULL;
}

// Returns room peers names by roomName
vector<string> Chatroom::getRoomPeersNames(string roomName){
	vector<string> peerNames;
	for (int i=0; i < this->chatRoomPeers.size(); i++) {
		peerNames.push_back(this->chatRoomPeers[i]->getPeerName());
	}
	return peerNames;
}

// Prints all chatroom's peers to console
void Chatroom::printAllPeersInRoom(){
	cout<<"Users In Room"<<this->chatRoomName<<" :"<<endl;
	vector<Peer*>::iterator it;
	for (it = this->chatRoomPeers.begin(); it != chatRoomPeers.end(); it++) {
		Peer* peer=*it;
		cout<<peer->getPeerName()<<endl;
	}
}
