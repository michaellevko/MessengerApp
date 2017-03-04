/*
 * TCPMessengerProtocol.h
 *
 *  Created on: Feb 13, 2013
 *      Author: efi
 */

#include "./SocketUtils/TCPSocket.h"

#ifndef TCPMESSENGERPROTOCOL_H_
#define TCPMESSENGERPROTOCOL_H_

/**
 * TCP Messenger protocol:
 * all messages are of the format [Command 4 byte int]
 * and optionally data follows in the format [Data length 4 byte int][ Data ]
 */
#define MSNGR_PORT 3346
#define DELIMITER ;

#define CLOSE_SESSION_WITH_PEER 	1
#define OPEN_SESSION_WITH_PEER 		2	//command:ip:port
#define EXIT						3
#define SEND_MSG_TO_PEER			4	//[length - int][msg - string]
#define SESSION_REFUSED				5
#define SESSION_ESTABLISHED			6
#define CREATE_USER					7	//username:password
#define LOGIN_USER					8	//username:password
#define GET_ALL_USERS				9
#define GET_ALL_CONNECTED_USERS		10
#define GET_ALL_SESSIONS			11
#define GET_ALL_ROOMS				12
#define GET_ALL_USERS_IN_ROOM		13	//roomName
#define CONNECT_TO_PEER_INIT		14	//userName
#define CONNECT_TO_PEER_RUN			15	//ip
#define CONNECTION_PERMITTED		16	//ip OR ip list seperated by delimiter
#define CONNECTION_REFUSED			17
#define ENTER_CHAT_ROOM				18	//roomName
#define ENTER_CHAT_ROOM_PEER		19


#define TEST_PEER_NAME "test"
#define SESSION_REFUSED_MSG "Connection to peer refused, peer might be busy or disconnected, try again later"

using namespace std;
using namespace npl;

class TCPMessengerProtocol{
public:
	void static readMsg(TCPSocket* sock){

	}
};

#endif /* TCPMESSENGERPROTOCOL_H_ */
