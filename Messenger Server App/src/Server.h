/*
 * Server.h
 *
 *  Created on: Mar 4, 2017
 *      Author: user
 */

#ifndef SRC_SERVER_H_
#define SRC_SERVER_H_

#include "Authenticator.h"

using namespace std;

class Server : public MThread {

private:
	TCPSocket* socket;
	Authenticator * auth;

public:
	Server();
	void listAllUsers();
	void run();
	void shutdown();
	void ListAllUsersInThisRoom(string roomName);
	void ListAllConnectedUsers();
	void ListAllSessions();
	void ListAllRooms();
	Authenticator* getAuthenticator();
	virtual ~Server();
};

#endif /* SRC_SERVER_H_ */
