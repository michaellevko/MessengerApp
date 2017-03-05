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

using namespace std;
namespace npl{
class Dispatcher : public MThread, public MTCPListener{
private:
	vector<Peer*> peerList;
//	vector<Session*> sessions;
//	vector<ChatRoomSession*> chatRooms;

public:
	Dispatcher();
	void run();
	vector<string> getAllConnectedPeers();
	void addPeer(Peer* peer);
	void printAllUsers();
	void printAllConnectedUsers();
	void printAllSessions();
	void printAllRooms();
	void printAllUsersInThisRoom(string roomName);
	virtual ~Dispatcher();
};
}
#endif /* SRC_DISPATCHER_H_ */
