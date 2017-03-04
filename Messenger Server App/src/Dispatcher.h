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
	void addToUserList(Peer peer);
	void removeFromUserList(Peer peer);
	vector<Peer> authenticationList;

public:
	Dispatcher();
	void run();
	void addToAuthenticationList(TCPSocket* conn);
	void removeFromAuthenticationList(TCPSocket* conn);
	void printAllUsers();
	void printAllConnectedUsers();
	void printAllSessions();
	void printAllRooms();
	void printAllUsersInThisRoom(string roomName);
	virtual ~Dispatcher();
};
}
#endif /* SRC_DISPATCHER_H_ */
