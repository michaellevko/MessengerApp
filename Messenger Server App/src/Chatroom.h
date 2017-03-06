/*
 * Chatroom.h
 *
 *  Created on: Mar 5, 2017
 *      Author: user
 */

#ifndef SRC_CHATROOM_H_
#define SRC_CHATROOM_H_

class Chatroom : public MThread{
private:
public:
	Chatroom();
	~Chatroom();
	string getRoomName();
	void printAllPeersInRoom();
};



#endif /* SRC_CHATROOM_H_ */
