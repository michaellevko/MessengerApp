/*
 * Peer.cpp
 *
 *  Created on: Mar 4, 2017
 *      Author: user
 */

#include "Peer.h"

Peer::Peer(TCPSocket* sock, string name){
	this->peerSock = sock;
	this->peerName = name;
}

string Peer::getPeerName(){
	return this->peerName;
}

TCPSocket* Peer::getPeerSock(){
	return this->peerSock;
}

Peer::~Peer(){

}
