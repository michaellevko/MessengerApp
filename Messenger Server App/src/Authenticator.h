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

using namespace npl;

class Authenticator : public MThread {
private:
	Dispatcher* dispatcher;
	vector<TCPSocket*> conns;
public:
	Dispatcher * getDispatcher();
	void addConn(TCPSocket* conn);
	void run();
	bool Login();
	bool Register();
	Authenticator();
	virtual ~Authenticator();
};

#endif /* SRC_AUTHENTICATOR_H_ */
