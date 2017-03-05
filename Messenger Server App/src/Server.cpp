/*
 * Server.cpp
 *
 *  Created on: Mar 4, 2017
 *      Author: user
 */

#include "Server.h"

namespace npl{

Server::Server() {
	socket = new TCPSocket(MSNGR_PORT);
	auth = new Authenticator();
	start();
}


Server::~Server() {

}


void Server::listAllUsers() {
	//this->getDispatcher()->printAllUsers();
}

void Server::ListAllConnectedUsers() {
	//this->getDispatcher()->printAllConnectedUsers();
}

void Server::ListAllSessions() {
	//this->getDispatcher()->printAllSessions();
}

void Server::ListAllRooms() {
	//this->getDispatcher()->printAllRooms();
}

void Server::ListAllUsersInThisRoom(string roomName) {
	//this->getDispatcher()->printAllUsersInThisRoom(roomName);
}

void Server::run() {
	while(true){
		TCPSocket * conn = socket->listenAndAccept();
		if(conn == NULL){
			break;
		}
		this->getAuthenticator()->addConn(conn);
	}
}

void Server::shutdown() {
	socket->close();
	waitForThread();
	delete this->socket;
	delete this->auth;
}

Authenticator* Server::getAuthenticator(){
	return this->auth;
}

}
