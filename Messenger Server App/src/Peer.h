/*
 * Peer.h
 *
 *  Created on: Mar 4, 2017
 *      Author: user
 */

#ifndef SRC_PEER_H_
#define SRC_PEER_H_

#include <string.h>
#include "./SocketUtils/TCPSocket.h"

using namespace std;

class Peer{
private:
	string peerName;
	TCPSocket* peerSock;
	string udpSrcPort;
public:
	Peer(TCPSocket* sock, string name, string udpSrcPort);
	string getPeerName();
	TCPSocket* getPeerSock();
	string getPeerUdpSrcPort();
	~Peer();
};

#endif /* SRC_PEER_H_ */
