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
	pthread_mutex_lock(&connectedpeerslistlock);
	for (it = this->allConnectedPeers.begin(); it != this->allConnectedPeers.end(); it++) {
		Peer* peer=*it;
		cout<<peer->getPeerName()<<endl;
	}
	pthread_mutex_unlock(&connectedpeerslistlock);
}

// Prints all sessions in sessions vector to server console
void Dispatcher::printAllSessions() {
	cout<<"Active sessions :"<<endl;
	vector<Session*>::iterator it;
	pthread_mutex_lock(&sessionslock);
	for (it = this->sessions.begin(); it != this->sessions.end(); it++) {
		Session* session=*it;
		cout<<"session with" <<session->getPeerA()->getPeerName()<<" and "<<session->getPeerB()->getPeerName()<<endl;
	}
	pthread_mutex_unlock(&sessionslock);
}

// Prints all rooms in chatrooms vector to server console
void Dispatcher::printAllRooms() {
	cout<<"Active rooms :"<<endl;
	vector<Chatroom* >::iterator it;
	pthread_mutex_lock(&chatRoomslock);
	for (it = this->chatRooms.begin(); it != this->chatRooms.end(); it++) {
		Chatroom* chatroom=*it;
		cout<<chatroom->getRoomName()<<endl;
	}
	pthread_mutex_unlock(&chatRoomslock);
}

// Prints all users in a specific room by name
void Dispatcher::printAllUsersInThisRoom(string roomName) {
	Chatroom* chatroom = this->findChatRoom(roomName);
	if (chatroom != NULL){
		chatroom->printAllPeersInRoom();
	}
}

// Returns a chatroom instance from chatrooms vector by name
Chatroom* Dispatcher::findChatRoom(string roomName) {
	vector<Chatroom*>::iterator it;
	Chatroom* ret = NULL;
	pthread_mutex_lock(&chatRoomslock);
	for (it = this->chatRooms.begin(); it != this->chatRooms.end(); it++) {
		Chatroom* room = *it;
		if (room->getRoomName() == roomName) {
			ret = room;
		}
	}
	pthread_mutex_unlock(&chatRoomslock);
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

// Returns a peer from peers vector by ip
Peer* Dispatcher::FindPeerByIP(string address) {
	Peer* ret = NULL;
	vector<Peer*>::iterator it;
	for (it = this->peers.begin(); it != this->peers.end(); it++) {
		Peer* user = *it;

		string ip = inet_ntoa(user->getPeerSock()->getPeerAddr().sin_addr);
		if (ip == address) {
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
	pthread_mutex_lock(&connectedpeerslistlock);
	this->allConnectedPeers.push_back(peer);
	pthread_mutex_unlock(&connectedpeerslistlock);
	if (peers.size() == 1)
		start();
}

void Dispatcher::run(){
	while (this->peers.size() > 0) {
		MTCPListener listener;
		listener.add(this->getPeersSockets());
		TCPSocket * conn = listener.listen(LISTEN_TIMEOUT);

		if (conn != NULL) {
			vector<string> dataIn;
			dataIn = TCPMessengerProtocol::readMsg(conn);
			Peer* peer = FindPeer(conn);

			// Check if client disconnected
			if (dataIn.size() == 0){
				this->removePeer(peer);
				this->onPeerDisconnect(peer);
			} else {
				switch (atoi(dataIn[0].c_str())) {

				case CONNECT_TO_PEER_INIT:
				{
					if (dataIn.size() > 1)
					{
						// Check if dest peer is available, if so send ip, if not send fail
						Peer* destPeer = this->FindPeer(dataIn[1]);
						if (this->isPeerAvailable(destPeer)){
							vector<string> destPeerAddress;
							destPeerAddress.push_back(inet_ntoa(destPeer->getPeerSock()->getPeerAddr().sin_addr));
							TCPMessengerProtocol::sendMsg(conn, SUCCESS, destPeerAddress);
						}
						else
						{
							TCPMessengerProtocol::sendMsg(conn, FAILURE);
						}
					}
					else
					{
						TCPMessengerProtocol::sendMsg(conn, FAILURE);
						cout << "connect to peer without peer data" << endl;
					}
					break;
				}
				case CONNECT_TO_PEER_RUN:
				{
					if (dataIn.size() > 1)
					{
						// Check if ip in data is available
						Peer* destPeer = this->FindPeerByIP(dataIn[1]);
						if (this->isPeerAvailable(destPeer)){
							vector<string> destPeerName;
							destPeerName.push_back(destPeer->getPeerName());
							TCPMessengerProtocol::sendMsg(conn, SUCCESS, destPeerName);

							// Opens a new session for peers
							this->openSession(peer, destPeer);
						}
						else
						{
							TCPMessengerProtocol::sendMsg(conn, FAILURE);
						}
					}
					else
					{
						TCPMessengerProtocol::sendMsg(conn, FAILURE);
						cout << "connect to peer without peer data" << endl;
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
				case GET_ALL_USERS_IN_ROOM:
				{
					if (dataIn.size() > 1)
					{
						string roomName = dataIn[1];
						Chatroom* room = this->findChatRoom(roomName);
						if (room != NULL){
							TCPMessengerProtocol::sendMsg(conn, SUCCESS, room->getRoomPeersNames(roomName));
						} else {
							TCPMessengerProtocol::sendMsg(conn, FAILURE);
						}
					}
					else
					{
						TCPMessengerProtocol::sendMsg(conn, FAILURE);
						cout << "get users in room without room name" << endl;
					}
					break;
				}
				case GET_ALL_ROOMS:
				{
					TCPMessengerProtocol::sendMsg(conn, SUCCESS, this->getAllRoomNames());
					break;
				}
				case CREATE_CHAT_ROOM:
				{
					if (dataIn.size() > 1)
					{
						string roomName = dataIn[1];
						Chatroom* room = this->findChatRoom(roomName);
						if(room == NULL){
							this->openChatRoom(peer, roomName);
							TCPMessengerProtocol::sendMsg(conn, SUCCESS);
							cout << peer->getPeerName() << " has created room " << roomName << endl;
						} else {
							TCPMessengerProtocol::sendMsg(conn, FAILURE);
						}
					}
					else
					{
						TCPMessengerProtocol::sendMsg(conn, FAILURE);
						cout << "create chat room without room name" << endl;
					}
					break;
				}
				case ENTER_CHAT_ROOM:
				{
					if (dataIn.size() > 1)
					{
						string roomName = dataIn[1];
						Chatroom* room = this->findChatRoom(roomName);
						if(room != NULL){
							vector<string> roomPeersDetails;
							roomPeersDetails = this->enterChatRoom(peer, room);
							TCPMessengerProtocol::sendMsg(conn, SUCCESS, roomPeersDetails);
							cout << peer->getPeerName() << " has entered room " << roomName << endl;
						} else {
							TCPMessengerProtocol::sendMsg(conn, FAILURE);
						}
					}
					else
					{
						TCPMessengerProtocol::sendMsg(conn, FAILURE);
						cout << "enter chat room without room name" << endl;
					}
					break;
				}
				case EXIT:
				{
					this->removePeer(peer);
					this->onPeerDisconnect(peer);
					TCPMessengerProtocol::sendMsg(conn, SUCCESS);
					break;
				}
				default:
					TCPMessengerProtocol::sendMsg(conn, FAILURE);
					break;
				}}
		}
	}
}

// Adds peer to existing chatroom by roomname
vector<string> Dispatcher::enterChatRoom(Peer* peer, Chatroom* room){
	vector<string> roomPeersDetails;
	vector<Peer*> roomPeers = room->getRoomPeers();
	vector<Peer*>::iterator it;
	for(it = roomPeers.begin(); it != roomPeers.end(); it++){
		Peer* peer = *it;
		string ip = inet_ntoa(peer->getPeerSock()->getPeerAddr().sin_addr);
		roomPeersDetails.push_back(peer->getPeerName());
		roomPeersDetails.push_back(ip);
	}
	this->removePeer(peer);
	room->addPeer(peer);

	return roomPeersDetails;
}

// Returns a vector of all room names
vector<string> Dispatcher::getAllRoomNames(){
	vector<string> roomNames;
	vector<Chatroom* >::iterator it;
	pthread_mutex_lock(&chatRoomslock);
	for (it = this->chatRooms.begin(); it != this->chatRooms.end(); it++) {
		Chatroom* chatroom=*it;
		roomNames.push_back(chatroom->getRoomName());
	}
	pthread_mutex_unlock(&chatRoomslock);
	return roomNames;
}

// Creates a new chat room
void Dispatcher::openChatRoom(Peer* roomOwner, string roomName){
	Chatroom* chatRoom = findChatRoom(roomName);
	if (chatRoom == NULL) {
		Chatroom* newChatRoom = new Chatroom(this, roomOwner, roomName);
		pthread_mutex_lock(&chatRoomslock);
		this->chatRooms.push_back(newChatRoom);
		pthread_mutex_unlock(&chatRoomslock);
	}
}

// Upon session close, add both peers to peers vector and erase session from sessions vector
void Dispatcher::onSessionClose(Session* brocker, Peer* peerA,Peer* peerB){
	pthread_mutex_lock(&sessionslock);
	for (vector<Session*>::iterator it = this->sessions.begin(); it != this->sessions.end();it++) {
		Session* session = *it;
		if (session == brocker) {
			sessions.erase(it);
			break;
		}
	}
	pthread_mutex_unlock(&sessionslock);
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
	pthread_mutex_lock(&sessionslock);
	this->sessions.push_back(session);
	pthread_mutex_unlock(&sessionslock);
}

// Removes peer from peers vector
void Dispatcher::removePeer(Peer* peer){
	vector<Peer*>::iterator it;
	pthread_mutex_lock(&peerlistlock);
	for (it = this->peers.begin(); it != this->peers.end(); it++) {
		Peer* user = *it;
		if (user->getPeerName() == peer->getPeerName())
			break;
	}
	if (it != this->peers.end()) {
		this->peers.erase(it);
	}
	pthread_mutex_unlock(&peerlistlock);
}

// Removes peer from allConnectedPeers vector
void Dispatcher::onPeerDisconnect(Peer* peer){
	vector<Peer*>::iterator it;
	pthread_mutex_lock(&connectedpeerslistlock);
	for (it = this->allConnectedPeers.begin(); it != this->allConnectedPeers.end(); it++) {
		Peer* user = *it;
		if (user->getPeerName() == peer->getPeerName())
			break;
	}
	if (it != this->allConnectedPeers.end()) {
		this->allConnectedPeers.erase(it);
	}
	pthread_mutex_unlock(&connectedpeerslistlock);
}

// Checks if peer socket is in peers vector and not in sessions or chatrooms vectors
// returns true if so, returns false otherwise
bool Dispatcher::isPeerAvailable(Peer* peer){
	bool isAvailable = false;
	if (peer !=NULL && peer == this->FindPeer(peer->getPeerName())){
		isAvailable = true;
	}
	return isAvailable;
}

vector<string> Dispatcher::getAllConnectedPeers(){
	vector<string> userNameList;
	pthread_mutex_lock(&connectedpeerslistlock);
	for (int i=0; i < this->allConnectedPeers.size(); i++) {
		userNameList.push_back(this->allConnectedPeers[i]->getPeerName());
	}
	pthread_mutex_unlock(&connectedpeerslistlock);
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
	pthread_mutex_lock(&chatRoomslock);
	for (it = this->chatRooms.begin(); it != this->chatRooms.end(); it++){
		Chatroom* chatRoom = *it;
		if (chatRoom == broker){
			this->chatRooms.erase(it);
		}
	}
	pthread_mutex_unlock(&chatRoomslock);
}

