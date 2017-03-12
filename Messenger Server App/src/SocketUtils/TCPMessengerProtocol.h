/*
 * TCPMessengerProtocol.h
 *
 *  Created on: Feb 13, 2013
 *      Author: efi
 */

#include <cstdlib>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <thread>
#include <iostream>
#include "TCPSocket.h"
#include "../Parser.h"

#ifndef TCPMESSENGERPROTOCOL_H_
#define TCPMESSENGERPROTOCOL_H_

/**
 * TCP Messenger protocol:
 * all messages are of the format [Command 4 byte int]
 * and optionally data follows in the format [Data length 4 byte int][ Data ]
 */
#define MSNGR_PORT 3346
#define DELIMITER ((const char)';')
#define SEND_MSG_TO_PEER			1	//[length - int][msg - string]
#define CONNECT_TO_PEER_RUN			2	//ip
#define CONNECT_TO_PEER_INIT		3	//userName
#define CLOSE_SESSION_WITH_PEER 	4
#define GET_ALL_CONNECTED_USERS		5
#define GET_ALL_ROOMS				6
#define LOGIN_USER					7	//username:password
#define SUCCESS						8
#define FAILURE						9
#define CREATE_USER					10	//username:password
#define EXIT						11
#define ENTER_CHAT_ROOM				12	//roomName
#define CREATE_CHAT_ROOM			13
#define GET_ALL_USERS				14
#define EXIT_CHAT_ROOM				15
#define DESTROY_CHAT_ROOM			16  //roomName
#define GET_ALL_USERS_IN_ROOM		17	//roomName

const int MAX_MSG_LEN = 256;
const int LISTEN_TIMEOUT =10;

using namespace std;

class TCPMessengerProtocol{
public:
	static vector<string> readMsg(TCPSocket* sock){
		char buffer[512];
		int rc = sock->recv(buffer, MAX_MSG_LEN);
		if (rc <= 0){
			vector<string> empty;
			return empty;
		}
		return split(buffer, DELIMITER);
	}

	void static sendMsg(TCPSocket* sock, int command, vector<string> data){
		string msg = numberToString(command) + DELIMITER;
		for (int i=0; i<data.size(); i++){
			msg += data[i] + DELIMITER;
		}
		sock->send(msg);
	}

	void static sendMsg(TCPSocket* sock, int command){
		string msg = numberToString(command) + DELIMITER;
		sock->send(msg);
	}

	void static printMutexLockMsg(string methodName, string lockState){
		cout<< methodName << "	" << lockState << "	" << std::hash<std::thread::id>()(std::this_thread::get_id()) << endl;
	}
};

#endif /* TCPMESSENGERPROTOCOL_H_ */
