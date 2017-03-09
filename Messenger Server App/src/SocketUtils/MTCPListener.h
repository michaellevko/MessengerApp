/*
 * MTCPListener.h
 *
 *  Created on: Mar 20, 2016
 *      Author: user
 */

#ifndef MTCPLISTENER_H_
#define MTCPLISTENER_H_

#include "TCPSocket.h"
#include <vector>

using namespace std;

class MTCPListener {

protected:
	typedef vector<TCPSocket*> tSockets;
	vector<TCPSocket*> sockets;

public:
	/**
	 * add socket to listen to
	 */
	void add(TCPSocket* socket);
	void remove(TCPSocket socket);
	void add(vector<TCPSocket*> sockets);
	TCPSocket* listen();

};

#endif /* MTCPLISTENER_H_ */
