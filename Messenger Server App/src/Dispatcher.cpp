/*
 * Dispatcher.cpp
 *
 *  Created on: Mar 4, 2017
 *      Author: user
 */

#include "Dispatcher.h"

using namespace std;

Dispatcher::Dispatcher(Handler* handler) {
	this->handler = handler;
}

Dispatcher::~Dispatcher() {
	// TODO Auto-generated destructor stub
}

// Prints all users in peers vector to server console
void Dispatcher::printAllConnectedUsers() {
	cout<<"Online users :"<<endl;
	vector<Peer*>::iterator it;
	for (it = this->peers.begin(); it != this->peers.end(); it++) {
		Peer* peer=*it;
		cout<<peer->getPeerName()<<endl;
	}
}

// Prints all sessions in sessions vector to server console
void Dispatcher::printAllSessions() {
	cout<<"Active sessions :"<<endl;
	vector<Session*>::iterator it;
	for (it = this->sessions.begin(); it != this->sessions.end(); it++) {
		Session* session=*it;
		cout<<"session with" <<session->getPeerA()->getPeerName()<<" and "<<session->getPeerB()->getPeerName()<<endl;
	}
}

// Prints all rooms in chatrooms vector to server console
void Dispatcher::printAllRooms() {
	cout<<"Active rooms :"<<endl;
	vector<Chatroom* >::iterator it;
	for (it = this->chatRooms.begin(); it != this->chatRooms.end(); it++) {
		Chatroom* chatroom=*it;
		cout<<chatroom->getRoomName()<<endl;
	}
}

// Prints all users in a specific room by name
void Dispatcher::printAllUsersInThisRoom(string roomName) {
	Chatroom* chatroom = this->findChatRoom(roomName);
	chatroom->printAllPeersInRoom();
}

// Returns a chatroom instance from chatrooms vector by name
Chatroom* Dispatcher::findChatRoom(string roomName) {
	vector<Chatroom*>::iterator it;
	Chatroom* ret = NULL;
	for (it = this->chatRooms.begin(); it != this->chatRooms.end(); it++) {
		Chatroom* room = *it;
		if (room->getRoomName() == roomName) {
			ret = room;
		}
	}
	return ret;
}

// Returns a peer from peers vector by socket
Peer* Dispatcher::FindPeer(TCPSocket* sock) {
	Peer* ret = NULL;
	vector<Peer*>::iterator it;
	for (it = this->peers.begin(); it != this->peers.end(); it++) {
		Peer* user = *it;

		if (user->getPeerSock() == sock) {
			ret = *it;
		}
	}
	return ret;
}

// Returns a peer from peers vector by userName
Peer* Dispatcher::FindPeer(string userName) {
	Peer* ret = NULL;
	vector<Peer*>::iterator it;
	for (it = this->peers.begin(); it != this->peers.end(); it++) {
		Peer* user = *it;

		if (user->getPeerName() == userName) {
			ret = *it;
		}
	}
	return ret;
}

// Adds a new peer from Authenticator to Dispatcher after the login process
void Dispatcher::addPeer(Peer* peer){
	pthread_mutex_lock(&peerlistlock);
	this->peers.push_back(peer);
	pthread_mutex_unlock(&peerlistlock);
	if (peers.size() == 1)
		start();
}

void Dispatcher::run(){
	while (this->peers.size() > 0) {
		MTCPListener listener;
		listener.add(this->getPeersSockets());
		TCPSocket * conn = listener.listen();

		if (conn != NULL) {
			vector<string> data;
			data = TCPMessengerProtocol::readMsg(conn);
			Peer* peer = FindPeer(conn);

			switch (atoi(data[0].c_str())) {

			case CONNECT_TO_PEER_INIT:
			{
				// Check if dest peer is available, if so send ip, if not send fail
				Peer* destPeer = this->FindPeer(data[1]);
				if (this->isPeerAvailable(destPeer)){
					vector<string> destPeerAddress;
					destPeerAddress.push_back(inet_ntoa(destPeer->getPeerSock()->getPeerAddr().sin_addr));
					TCPMessengerProtocol::sendMsg(conn, SUCCESS, destPeerAddress);
				} else {
					TCPMessengerProtocol::sendMsg(conn, FAILURE);
				}
				break;
			}
			case CONNECT_TO_PEER_RUN:
			{
				// Check if ip in data is available
				Peer* destPeer = this->FindPeer(data[1]);
				if (this->isPeerAvailable(destPeer)){
					vector<string> destPeerName;
					destPeerName.push_back(destPeer->getPeerName());
					TCPMessengerProtocol::sendMsg(conn, SUCCESS, destPeerName);

					// Opens a new session for peers
					this->openSession(peer, destPeer);
				} else {
					TCPMessengerProtocol::sendMsg(conn, FAILURE);
				}
				break;
			}
			case GET_ALL_CONNECTED_USERS:
			{
				TCPMessengerProtocol::sendMsg(conn, SUCCESS, this->getAllConnectedPeers());
				break;
			}
			case GET_ALL_USERS:
			{
				this->handler->onUsersList(conn);
				break;
			}
			case EXIT:
			{
				this->removePeer(peer);
				TCPMessengerProtocol::sendMsg(conn, SUCCESS);
				break;
			}
			default:
				TCPMessengerProtocol::sendMsg(conn, FAILURE);
				break;
			}
		}
	}
}

// Returns a vector of all room names
vector<string> Dispatcher::getAllRoomNames(){
	vector<string> roomNames;
	vector<Chatroom* >::iterator it;
	for (it = this->chatRooms.begin(); it != this->chatRooms.end(); it++) {
		Chatroom* chatroom=*it;
		roomNames.push_back(chatroom->getRoomName());
	}
	return roomNames;
}

// Creates a new chat room
void Dispatcher::openChatRoom(Peer* roomOwner, string roomName){
	Chatroom* chatRoom = findChatRoom(roomName);
	if (chatRoom == NULL) {
		Chatroom* newChatRoom = new Chatroom(this, roomOwner, roomName);
		this->removePeer(roomOwner);
		this->chatRooms.push_back(newChatRoom);
	}
}

// Upon session close, add both peers to peers vector and erase session from sessions vector
void Dispatcher::onSessionClose(Session* brocker, Peer* peerA,Peer* peerB){
	for (vector<Session*>::iterator it = this->sessions.begin(); it != this->sessions.end();it++) {
		Session* session = *it;
		if (session == brocker) {
			sessions.erase(it);
			break;
		}
	}
	// Add both peers to peers vector
	if(peerA != NULL) {this->addPeer(peerA);}
	if(peerB != NULL) {this->addPeer(peerB);}
}


// Opens a new session between two peers
void Dispatcher::openSession(Peer* peerA, Peer* peerB){
	// Remove both peers from peers vector and open a new session
	this->removePeer(peerA);
	this->removePeer(peerB);
	Session* session = new Session(this, peerA, peerB);
	this->sessions.push_back(session);
}

// Removes peer from peers vector
void Dispatcher::removePeer(Peer* peer){
	vector<Peer*>::iterator it;
	for (it = this->peers.begin(); it != this->peers.end(); it++) {
		Peer* user = *it;
		if (user->getPeerName() == peer->getPeerName())
			break;
	}
	if (it != this->peers.end()) {
		this->peers.erase(it);
	}
}

// Checks if peer socket is in peers vector and not in sessions or chatrooms vectors
// returns true if so, returns false otherwise
bool Dispatcher::isPeerAvailable(Peer* peer){
	bool isAvailable = false;
	if (peer == this->FindPeer(peer->getPeerName())){
		isAvailable = true;
	}
	return isAvailable;
}

vector<string> Dispatcher::getAllConnectedPeers(){
	vector<string> userNameList;
	for (int i=0; i < this->peers.size(); i++) {
		userNameList.push_back(this->peers[i]->getPeerName());
	}
	return userNameList;
}

// Returns the sockets of the peers in peers vector
vector<TCPSocket*> Dispatcher::getPeersSockets() {
	vector<TCPSocket*> peersSockets;
	for (int i = 0; i < this->peers.size(); i++) {
		peersSockets.push_back(this->peers[i]->getPeerSock());
	}

	return peersSockets;
}

// Sends all connected users to peer by getting all elements of peers vector
void Dispatcher::onConnectedUsersList(TCPSocket* peer){
	TCPMessengerProtocol::sendMsg(peer, SUCCESS, this->getAllConnectedPeers());
}

// Calls Authenticator to send registered users to conn
void Dispatcher::onUsersList(TCPSocket* peer){
	this->handler->onUsersList(peer);
}

// Adds chatRoomPeer to Dispatcher peers vector
void Dispatcher::onChatRoomExit(Peer* chatRoomPeer){
	this->addPeer(chatRoomPeer);
}

// Removes chatroom from vector
void Dispatcher::onChatRoomClose(Chatroom* broker){
	vector<Chatroom*>::iterator it;
	for (it = this->chatRooms.begin(); it != this->chatRooms.end(); it++){
		Chatroom* chatRoom = *it;
		if (chatRoom == broker){
			this->chatRooms.erase(it);
		}
	}
}
