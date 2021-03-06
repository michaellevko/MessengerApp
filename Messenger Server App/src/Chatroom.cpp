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
	this->chatActive = true;
	this->handler = handler;
}

Chatroom::~Chatroom(){
	cout << "Chatroom dtor." << endl;
}

// Returns a roomName
string Chatroom::getRoomName() {
	return this->chatRoomName;
}

// Returns vector of peers in room by roomName
vector<Peer*> Chatroom::getRoomPeers(){
	return this->chatRoomPeers;
}

Peer* Chatroom::getRoomOwner(){
	return this->chatRoomOwner;
}

// Adds peer to chatroom peers vector
void Chatroom::addPeer(Peer* peer){
	pthread_mutex_lock(&roomLock);
	TCPMessengerProtocol::printMutexLockMsg("Chatroom::addPeer", "lock");
	this->chatRoomPeers.push_back(peer);
	pthread_mutex_unlock(&roomLock);
	TCPMessengerProtocol::printMutexLockMsg("Chatroom::addPeer", "unlock");
	if (this->chatRoomPeers.size() == 1){
		start();
	}
}


// Removes a peer from chatRoomPeers vector
void Chatroom::removePeer(Peer* peerToRemove){
	vector<Peer*>::iterator it;
	pthread_mutex_lock(&roomLock);
	TCPMessengerProtocol::printMutexLockMsg("Chatroom::removePeer", "lock");
	for (it = this->chatRoomPeers.begin(); it != this->chatRoomPeers.end();it++) {
		Peer* peer = *it;
		if (peer == peerToRemove){
			this->chatRoomPeers.erase(it);
			break;
		}
	}
	pthread_mutex_unlock(&roomLock);
	TCPMessengerProtocol::printMutexLockMsg("Chatroom::removePeer", "unlock");
}

void Chatroom::run(){
	while (this->chatActive) {
		MTCPListener listener;
		listener.add(this->getPeersSockets());
		TCPSocket * peer = listener.listen(LISTEN_TIMEOUT);
		if (peer != NULL) {
			vector<string> data;
			data = TCPMessengerProtocol::readMsg(peer);
			Peer* sender = this->FindPeer(peer);

			// Check if client disconnected
			if (data.size() == 0){
				this->removePeer(sender);
				this->handler->onPeerDisconnect(sender);
			} else {
				switch (atoi(data[0].c_str())) {

				case GET_ALL_USERS_IN_ROOM:
				{
					TCPMessengerProtocol::sendMsg(peer, SUCCESS, this->getRoomPeersNames(this->getRoomName()));
					break;
				}
				case GET_ALL_USERS:
				{
					this->handler->onListUsers(peer);
					break;
				}
				case GET_ALL_ROOMS:
				{
					this->handler->onListRooms(peer);
					break;
				}
				case EXIT_CHAT_ROOM:
				{
					this->removePeer(sender);
					this->handler->onChatRoomExit(sender);
					TCPMessengerProtocol::sendMsg(peer,SUCCESS);
					cout << sender->getPeerName() << " left chatroom " << this->getRoomName() << endl;
					break;
				}
				case GET_ALL_CONNECTED_USERS:
				{
					handler->onListConnectedUsers(peer);
					break;
				}
				default:
				{
					TCPMessengerProtocol::sendMsg(peer, FAILURE);
				}}
			}
		}
	}
	//delete this;
}

// Marks room as closed and calls handler to delete from chatroom vector
void Chatroom::closeRoom(){
	this->chatActive = false;
	this->handler->onChatRoomClose(this);
}

// Returns the sockets of the peers in peers vector
vector<TCPSocket*> Chatroom::getPeersSockets() {
	vector<TCPSocket*> peersSockets;
	pthread_mutex_lock(&roomLock);
	TCPMessengerProtocol::printMutexLockMsg("Chatroom::getPeersSockets", "lock");
	for (int i = 0; i < this->chatRoomPeers.size(); i++) {
		peersSockets.push_back(this->chatRoomPeers[i]->getPeerSock());
	}
	pthread_mutex_unlock(&roomLock);
	TCPMessengerProtocol::printMutexLockMsg("Chatroom::getPeersSockets", "unlock");
	return peersSockets;
}

// Returns a peer from peers vector by userName
Peer*  Chatroom::FindPeer(string userName) {
	vector<Peer*>::iterator it;
	Peer* peer = NULL;
	pthread_mutex_lock(&roomLock);
	TCPMessengerProtocol::printMutexLockMsg("Chatroom::FindPeer", "lock");
	cout << "Chatroom::FindPeer lock" << std::hash<std::thread::id>()(std::this_thread::get_id()) << endl;
	for (it = chatRoomPeers.begin(); it != chatRoomPeers.end();it++) {
		peer = *it;
		if (peer->getPeerName() == userName){
			break;
		}
	}
	pthread_mutex_unlock(&roomLock);
	TCPMessengerProtocol::printMutexLockMsg("Chatroom::FindPeer", "unlock");
	return peer;
}

// Returns a peer from peers vector by socket
Peer* Chatroom::FindPeer(TCPSocket* conn) {
	vector<Peer*>::iterator it;
	Peer* peer = NULL;
	pthread_mutex_lock(&roomLock);
	TCPMessengerProtocol::printMutexLockMsg("Chatroom::FindPeer", "lock");
	for (it = chatRoomPeers.begin(); it != chatRoomPeers.end();it++) {
		peer = *it;
		if (peer->getPeerSock() == conn){
			break;
		}
	}
	pthread_mutex_unlock(&roomLock);
	TCPMessengerProtocol::printMutexLockMsg("Chatroom::FindPeer", "unlock");
	return peer;
}

// Returns room peers names by roomName
vector<string> Chatroom::getRoomPeersNames(string roomName){
	vector<string> peerNames;
	pthread_mutex_lock(&roomLock);
	TCPMessengerProtocol::printMutexLockMsg("Chatroom::getRoomPeersNames", "lock");
	for (int i=0; i < this->chatRoomPeers.size(); i++) {
		peerNames.push_back(this->chatRoomPeers[i]->getPeerName());
	}
	pthread_mutex_unlock(&roomLock);
	TCPMessengerProtocol::printMutexLockMsg("Chatroom::getRoomPeersNames", "unlock");
	return peerNames;
}

// Prints all chatroom's peers to console
void Chatroom::printAllPeersInRoom(){
	cout<<"Users In Room"<<this->chatRoomName<<" :"<<endl;
	vector<Peer*>::iterator it;
	pthread_mutex_lock(&roomLock);
	TCPMessengerProtocol::printMutexLockMsg("Chatroom::printAllPeersInRoom", "lock");
	for (it = this->chatRoomPeers.begin(); it != chatRoomPeers.end(); it++) {
		Peer* peer=*it;
		cout<<peer->getPeerName()<<endl;
	}
	pthread_mutex_unlock(&roomLock);
	TCPMessengerProtocol::printMutexLockMsg("Chatroom::printAllPeersInRoom", "unlock");
}
