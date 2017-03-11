/*
 * Authenticator.h
 *
 *  Created on: Mar 4, 2017
 *      Author: user
 */

#ifndef SRC_AUTHENTICATOR_H_
#define SRC_AUTHENTICATOR_H_

#include "Dispatcher.h"

using namespace std;

class Authenticator : public MThread, public Dispatcher::Handler{
private:
	Dispatcher* dispatcher;
	vector<TCPSocket*> conns;
	pthread_mutex_t lock;
	bool isUserRegistered(string userName);
	bool isUserLegit(string userName, string password);
	vector<string> getAllRegisteredUsers();
	vector<TCPSocket*>::iterator findConnInVector(TCPSocket* peer);
public:
	void printAllRegisteredUsers();
	Dispatcher * getDispatcher();
	void addConn(TCPSocket* conn);
	void shutdown();
	void run();
	bool Login(string userName, string password);
	bool Register(string userName, string password);
	void removeConn(TCPSocket* conn);
	void onUsersList(TCPSocket* conn);
	Authenticator();
	virtual ~Authenticator();
};

#endif /* SRC_AUTHENTICATOR_H_ */
