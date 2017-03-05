/*
 * Authenticator.h
 *
 *  Created on: Mar 4, 2017
 *      Author: user
 */

#ifndef SRC_AUTHENTICATOR_H_
#define SRC_AUTHENTICATOR_H_

#include "./Dispatcher.h"
#include "./SocketUtils/MThread.h"
#include <iostream>
#include <fstream>
#include "Peer.h"

using namespace npl;

using namespace std;

class Authenticator : public MThread {
private:
	Dispatcher* dispatcher;
	vector<TCPSocket*> conns;
	bool isUserRegistered(string userName);
	bool isUserLegit(string userName, string password);
	vector<string> getAllRegisteredUsers();
	vector<TCPSocket*>::iterator Authenticator::findConnInVector(TCPSocket* peer);
public:
	Dispatcher * getDispatcher();
	void addConn(TCPSocket* conn);
	void run();
	bool Login(string userName, string password);
	bool Register(string userName, string password);
	void removeConn(TCPSocket* conn);
	Authenticator();
	virtual ~Authenticator();
};

#endif /* SRC_AUTHENTICATOR_H_ */
