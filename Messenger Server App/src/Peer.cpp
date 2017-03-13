/*
 * Peer.cpp
 *
 *  Created on: Mar 4, 2017
 *      Author: user
 */

#include "Peer.h"

Peer::Peer(TCPSocket* sock, string name, string udpPort){
	this->peerSock = sock;
	this->peerName = name;
	this->udpSrcPort = udpPort;
}

string Peer::getPeerName(){
	return this->peerName;
}

TCPSocket* Peer::getPeerSock(){
	return this->peerSock;
}

string Peer::getPeerUdpSrcPort(){
	return this->udpSrcPort;
}

Peer::~Peer(){

}
