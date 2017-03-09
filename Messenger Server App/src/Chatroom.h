/*
 * Chatroom.h
 *
 *  Created on: Mar 5, 2017
 *      Author: user
 */

#ifndef SRC_CHATROOM_H_
#define SRC_CHATROOM_H_

#include "Peer.h"
#include "./SocketUtils/MThread.h"
#include "./SocketUtils/MTCPListener.h"
#include "./SocketUtils/TCPMessengerProtocol.h"

class Chatroom : public MThread{
public:
	class Handler{
	public:
		virtual void onChatRoomClose(Chatroom* brocker)=0;
		virtual void onChatRoomExit(Peer* chatRoomPeer)=0;
		virtual void onUsersList(TCPSocket* peer)=0;
		virtual void onConnectedUsersList(TCPSocket* peer)=0;
	};
private:
	vector<Peer*> chatRoomPeers;
	string chatRoomName;
	Peer* chatRoomOwner;
	bool chatActive;
	Handler* handler;
	pthread_mutex_t lock;

public:
	Chatroom();
	Chatroom(Handler* handler,Peer* peer, string Chatroom);
	~Chatroom();
	void run();
	void closeRoom();
	void addPeer(Peer* peer);
	void removePeer(Peer* peer);
	Peer* FindPeer(string userName);
	Peer* FindPeer(TCPSocket*  peer);
	string getRoomName();
	vector<Peer*>getRoomPeers(string roomName);
	vector<string>getRoomPeersNames(string roomName);
	string getRoomOwner();
	vector<TCPSocket*> getPeersSockets();
	void printAllPeersInRoom();
};



#endif /* SRC_CHATROOM_H_ */
