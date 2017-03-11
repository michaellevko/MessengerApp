/*
 * Dispatcher.h
 *
 *  Created on: Mar 4, 2017
 *      Author: user
 */

#ifndef SRC_DISPATCHER_H_
#define SRC_DISPATCHER_H_

#include "Session.h"
#include "Chatroom.h"

using namespace std;

class Dispatcher : public MThread, public Session::Handler, public Chatroom::Handler{
public:
	class Handler{
	public:
		virtual void onUsersList(TCPSocket* peer)=0;
	};
private:
	vector<Peer*> allConnectedPeers;
	vector<Peer*> peers;
	vector<Session*> sessions;
	vector<Chatroom*> chatRooms;
	Handler* handler;
	Peer* FindPeer(string userName);
	Peer* FindPeer(TCPSocket* sock);
	Peer* FindPeerByIP(string address);
	bool isPeerAvailable(Peer* peer);
	Chatroom* findChatRoom(string roomName);
	pthread_mutex_t connectedpeerslistlock;
	pthread_mutex_t peerlistlock;
	pthread_mutex_t sessionslock;
	pthread_mutex_t chatRoomslock;

public:
	Dispatcher(Handler* handler);
	virtual ~Dispatcher();
	void run();
	vector<TCPSocket*> getPeersSockets();
	vector<string> getAllConnectedPeers();
	vector<string> getAllRoomNames();
	void addPeer(Peer* peer);
	void removePeer(Peer* peer);
	void printAllConnectedUsers();
	void printAllSessions();
	void printAllRooms();
	void printAllUsersInThisRoom(string roomName);
	void openSession(Peer* peerA, Peer* peerB);
	void openChatRoom(Peer* roomOwner, string roomName);
	vector<string> enterChatRoom(Peer* peer, Chatroom* room);

	void onSessionClose(Session* brocker,Peer* connA,Peer* connB);
	void onConnectedUsersList(TCPSocket* peer);
	void onUsersList(TCPSocket* peer);
	void onChatRoomExit(Peer* chatRoomPeer);
	void onChatRoomClose(Chatroom* brocker);
	void onPeerDisconnect(Peer* peer);
};

#endif /* SRC_DISPATCHER_H_ */
