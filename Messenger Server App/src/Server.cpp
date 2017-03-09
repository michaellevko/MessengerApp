/*
 * Server.cpp
 *
 *  Created on: Mar 4, 2017
 *      Author: user
 */

#include "Server.h"

Server::Server() {
	socket = new TCPSocket(MSNGR_PORT);
	auth = new Authenticator();
	start();
}


Server::~Server() {

}


void Server::listAllUsers() {
	this->getAuthenticator()->printAllRegisteredUsers();
}

void Server::ListAllConnectedUsers() {
	this->getAuthenticator()->getDispatcher()->printAllConnectedUsers();
}

void Server::ListAllSessions() {
	this->getAuthenticator()->getDispatcher()->printAllSessions();
}

void Server::ListAllRooms() {
	this->getAuthenticator()->getDispatcher()->printAllRooms();
}

void Server::ListAllUsersInThisRoom(string roomName) {
	this->getAuthenticator()->getDispatcher()->printAllUsersInThisRoom(roomName);
}

void Server::run() {
	while(true){
		TCPSocket* conn = this->socket->listenAndAccept();
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
	this->getAuthenticator()->shutdown();
	delete this->auth;
}

Authenticator* Server::getAuthenticator(){
	return this->auth;
}
