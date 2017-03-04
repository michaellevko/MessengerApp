/*
 * Authenticator.cpp
 *
 *  Created on: Mar 4, 2017
 *      Author: user
 */

#include "Authenticator.h"

using namespace npl;

Authenticator::Authenticator() {
	this->dispatcher = new Dispatcher();
}

// Adds new client connection to authentication list
void Authenticator::addConn(TCPSocket* conn){
	this->conns.push_back(conn);
	if (this->conns.size() == 1){
		start();
	}
}

void Authenticator::run() {
	while (conns.size() > 0) {
		MTCPListener listener;
		listener.add(this->conns);
		TCPSocket* conn = listener.listen();

		if (conn != NULL) {
			string* data;
			data = TCPMessengerProtocol::readMsg(conn);

			switch (command) {

			case LIST_USERS:

				MessengerProtocol::SendMSG(peer, LIST_USERS,
						getUserPassMap());
				break;

			case REGISTER:
				if (this->Register(data)) {
					PeerName * thisPeer = new PeerName(
							data.find(USER_NAME_KEY)->second, peer);
					dispatcher->add(thisPeer);
					this->remove(peer);
				} else {
					MessengerProtocol::SendMSG(peer, ERROR_REGISTER);

				}
				break;

			case LOGIN:
				if (this->Login(data)) {
					PeerName * thisPeer = new PeerName(
							data.find(USER_NAME_KEY)->second, peer);
					dispatcher->add(thisPeer);
					this->remove(peer);
					cout << "login successful" << endl;
				} else {
					MessengerProtocol::SendMSG(peer, ERROR_LOGIN);
				}
				break;

			case LIST_CONNECTED_USERS:

				MessengerProtocol::SendMSG(peer, LIST_CONNECTED_USERS,
						this->dispatcher->getMapOfAllConnectPeers());

				break;
			case DISCONNECT:
				this->remove(peer);
				break;
			case NONE:
				this->remove(peer);
				break;
			default:
				MessengerProtocol::SendMSG(peer, COMMAND_NOT_FOUND);
				break;
			}
		}
	}
}


Dispatcher* Authenticator::getDispatcher(){
	return this->dispatcher;
}

Authenticator::~Authenticator() {
	// TODO Auto-generated destructor stub
}

