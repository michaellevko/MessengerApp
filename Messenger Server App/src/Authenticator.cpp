/*
 * Authenticator.cpp
 *
 *  Created on: Mar 4, 2017
 *      Author: user
 */

#include "Authenticator.h"
#include "Parser.cpp"
#include <algorithm>

#define USERS_FILE "UsersFile"

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

			switch (data[0]) {

			case GET_ALL_USERS:
				TCPMessengerProtocol::sendMsg(conn, SUCCESS, this->getAllRegisteredUsers());
				break;

			case CREATE_USER:
				if (this->Register(data[1], data[2])) {
					TCPMessengerProtocol::sendMsg(conn, SUCCESS);
				} else { TCPMessengerProtocol::sendMsg(conn, FAILURE); }
				break;

			case LOGIN_USER:
				if (this->Login(data[0], data[1])) {
					Peer * newPeer = new Peer(conn, data[1]);
					this->getDispatcher()->addPeer(newPeer);
					this->removeConn(conn);
					TCPMessengerProtocol::sendMsg(conn, SUCCESS);
				} else {
					TCPMessengerProtocol::sendMsg(conn, FAILURE);
				}
				break;

			case GET_ALL_CONNECTED_USERS:
				TCPMessengerProtocol::sendMsg(conn, SUCCESS, this->getDispatcher()->getAllConnectedPeers());
				break;
			case EXIT:
				this->removeConn(conn);
				break;
			default:
				TCPMessengerProtocol::sendMsg(conn, FAILURE);
				break;
			}
		}
	}
}

// Attempts to login user
// returns false if user isnt registered, true if logged in successfully
bool Authenticator::Login(string userName, string password){
	bool isUserOk = false;
	if(this->isUserLegit(userName, password)){
		isUserLegit = true;
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
	cout << userName << ":" << password << endl;
	bool ret = false;

	if(!isUserRegistered(userName)){
		ofstream file(USERS_FILE, ios::out | ios::trunc);
		if (file.is_open()) {
			file << userName + ";" + password << endl;
			file.close();
			ret = true;
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
		string line, username, password;
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


Dispatcher* Authenticator::getDispatcher(){
	return this->dispatcher;
}

Authenticator::~Authenticator() {
	// TODO Auto-generated destructor stub
}

