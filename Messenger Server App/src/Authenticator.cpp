/*
 * Authenticator.cpp
 *
 *  Created on: Mar 4, 2017
 *      Author: user
 */

#include "Authenticator.h"
#include <iostream>
#include <fstream>
#include <algorithm>

#define USERS_FILE "UsersFile"

using namespace std;

Authenticator::Authenticator() {
	this->dispatcher = new Dispatcher(this);
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
			vector<string> data;
			data = TCPMessengerProtocol::readMsg(conn);

			// Check if client disconnected
			if (data.size() == 0){
				this->removeConn(conn);
			} else {
				switch (atoi(data[0].c_str())) {

				case GET_ALL_USERS:
				{
					TCPMessengerProtocol::sendMsg(conn, SUCCESS, this->getAllRegisteredUsers());
					break;
				}
				case CREATE_USER:
				{
					if (this->Register(data[1], data[2])) {
						TCPMessengerProtocol::sendMsg(conn, SUCCESS);
					} else { TCPMessengerProtocol::sendMsg(conn, FAILURE); }
					break;
				}
				case LOGIN_USER:
				{
					if (this->Login(data[1], data[2])) {
						Peer * newPeer = new Peer(conn, data[1]);
						this->getDispatcher()->addPeer(newPeer);
						this->removeConn(conn);
						TCPMessengerProtocol::sendMsg(conn, SUCCESS);
					} else {
						TCPMessengerProtocol::sendMsg(conn, FAILURE);
					}
					break;
				}
				case GET_ALL_CONNECTED_USERS:
				{
					TCPMessengerProtocol::sendMsg(conn, SUCCESS, this->getDispatcher()->getAllConnectedPeers());
					break;
				}
				case EXIT:
				{
					this->removeConn(conn);
					break;
				}
				default:
				{
					TCPMessengerProtocol::sendMsg(conn, FAILURE);
					break;
				}}
			}
		}
	}
}

// Attempts to login user
// returns false if user isnt registered, true if logged in successfully
bool Authenticator::Login(string userName, string password){
	bool isUserOk = false;
	if(this->isUserLegit(userName, password)){
		isUserOk = true;
		cout << "User " << userName << " logged in." << endl;
	}
	else {
		cout << "ERROR: " + userName + " isnt a registered user." << endl;
	}

	return isUserOk;
}

// Attempts to remove a connection from server
void Authenticator::removeConn(TCPSocket* conn){
	vector<TCPSocket*>::iterator it = this->findConnInVector(conn);
		if (it != this->conns.end())
			this->conns.erase(it);
		else
			cout << "ERROR: Can't remove connection (not found)." << endl;
}

// Find connection location in connection vector
vector<TCPSocket*>::iterator Authenticator::findConnInVector(TCPSocket* conn) {
	if (this->conns.size() > 0) {
		vector<TCPSocket*>::iterator it;
		for (it = this->conns.begin(); it != this->conns.end(); it++) {
			if (*it == conn) {
				return it;
			}
		}
	}
	return this->conns.end();
}


// Attempts to register user in UsersFile
// returns false if user already exists, true if registered successfully
bool Authenticator::Register(string userName, string password){
	bool ret = false;
	if(!isUserRegistered(userName)){
		ofstream file(USERS_FILE, ios::out | ios::app);
		if (file.is_open()) {
			file << userName + ";" + password << endl;
			file.close();
			ret = true;
			cout << "Registered " << userName << ":" << password << endl;
		} else
			cout << "ERROR: Unable to open UserPass file" << endl;
	}

	return ret;
}

// Checks if user already registered
// returns false if new user, true if user already exists
bool Authenticator::isUserRegistered(string userName){
	bool doesExist = false;
	ifstream file(USERS_FILE, ios::in);
	if (file.is_open()) {
		string line, username, password;
		while(getline(file,line)){
			vector<std::string> userPass = split(line, DELIMITER);
			if(userName == userPass[0]){
				doesExist = true;
				break;
			}
		}

		file.close();
	}
	return doesExist;
}

// Checks if user is registered and also if password is correct
// returns false if username not found or password incorrect, true if user is legit
bool Authenticator::isUserLegit(string userName, string password){
	bool isCorrect = false;
	ifstream file(USERS_FILE, ios::in);
	if (file.is_open()) {
		string line;
		while(getline(file,line)){
			vector<std::string> userPass = split(line, DELIMITER);
			if(userName == userPass[0]){
				if(password == userPass[1]){
					isCorrect = true;
					break;
				}
			}
		}
		file.close();
	}
	return isCorrect;
}

// Returns all users in UsersFile
vector<string> Authenticator::getAllRegisteredUsers(){
	vector<string> users;
	ifstream file(USERS_FILE, ios::in);
		if (file.is_open()) {
			string line, username;
			while(getline(file,line)){
				vector<std::string> userPass = split(line, DELIMITER);
				users.push_back(userPass[0]);
			}
			file.close();
		}
	return users;
}

// Prints all registered users to server console
void Authenticator::printAllRegisteredUsers(){
	vector<string> users = this->getAllRegisteredUsers();
	for (int i=0; i<users.size(); i++){
		cout << users[i] << endl;
	}
}

void Authenticator::shutdown() {
	for (vector<TCPSocket*>::iterator it = this->conns.begin(); it != this->conns.end(); ++it) {
		TCPSocket* p = *it;
		p->close();
		delete p;
	}
	delete this->dispatcher;
}

Dispatcher* Authenticator::getDispatcher(){
	return this->dispatcher;
}

// Handler method to return user list
void Authenticator::onUsersList(TCPSocket* conn){
	TCPMessengerProtocol::sendMsg(conn, SUCCESS, this->getAllRegisteredUsers());
}

Authenticator::~Authenticator() {
	// TODO Auto-generated destructor stub
}

