/*
 * Dispatcher.cpp
 *
 *  Created on: Mar 4, 2017
 *      Author: user
 */

#include "Dispatcher.h"

namespace npl{

Dispatcher::Dispatcher() {

	start();

}

Dispatcher::~Dispatcher() {
	// TODO Auto-generated destructor stub
}

void Dispatcher::run(){
	while (peers.size() > 0) {
			MTCPListener listener;
			listener.add(this->peers);
			TCPSocket * peer = listener.listen(30);

			if (peer != NULL) {
				Command command;
				map<string, string> data;
				MessengerProtocol::readMSG(peer, command, data);

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

}

vector<string> Dispatcher::getAllConnectedPeers(){
	return NULL;
}
