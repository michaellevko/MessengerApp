/*
 * Dispatcher.h
 *
 *  Created on: Mar 4, 2017
 *      Author: user
 */

#ifndef SRC_DISPATCHER_H_
#define SRC_DISPATCHER_H_

#include <string>
#include "./SocketUtils/TCPMessengerProtocol.h"
#include "./SocketUtils/MThread.h"
#include "Peer.h"
#include "./SocketUtils/MTCPListener.h"
#include "Session.h"
#include "Chatroom.h"


using namespace std;
namespace npl{
class Dispatcher : public MThread{
private:
	vector<Peer*> peers;
	vector<Session*> sessions;
	vector<Chatroom*> chatRooms;
	Peer* FindPeer(string userName);
	Peer* FindPeer(TCPSocket* sock);
	bool isPeerAvailable(Peer* peer);
	Chatroom* findChatRoom(string usrname);

public:
	Dispatcher();
	vector<TCPSocket*> getPeersSockets();
	void run();
	vector<string> getAllConnectedPeers();
	void OpenSession(Peer* peerA, Peer* peerB);
	void addPeer(Peer* peer);
	void removePeer(Peer* peer);
	void printAllConnectedUsers();
	void printAllSessions();
	void printAllRooms();
	void printAllUsersInThisRoom(string roomName);
	virtual ~Dispatcher();
};
}
#endif /* SRC_DISPATCHER_H_ */
