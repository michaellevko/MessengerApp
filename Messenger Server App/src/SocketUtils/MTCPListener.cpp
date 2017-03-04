/*
 * MTCPListener.cpp
 *
 *  Created on: Mar 20, 2016
 *      Author: user
 */

#include "MTCPListener.h"
#include <algorithm>

using namespace std;

namespace npl {
void MTCPListener::add(TCPSocket* socket){
	sockets.push_back(socket);

}

void MTCPListener::remove(TCPSocket socket)
{
	tSockets::iterator iter = sockets.begin();

	for(;iter!= sockets.end();iter++)
	{
			TCPSocket* sock = *iter;

			int fd = sock->socket();
			if(socket.socket() == fd)
			{
				sockets.erase(iter);
			}
		}
}

void MTCPListener::add(vector<TCPSocket*> sockets){
	this->sockets = sockets;
}

TCPSocket* MTCPListener::listen(){
	fd_set set;
	int nfd = 0;
	FD_ZERO(&set);
	tSockets::iterator iter = sockets.begin();
	for(;iter!= sockets.end();iter++){
		TCPSocket* sock = *iter;
		int fd = sock->socket();
		FD_SET(fd,&set);
		if (fd >= nfd){
			nfd = fd+1;
		}
	}

	int rc = select(nfd, &set,NULL,NULL,NULL);
	if (rc<1){
		FD_ZERO(&set);
		return NULL;
	}

	iter = sockets.begin();
	for(;iter!= sockets.end();iter++){
		TCPSocket* sock = *iter;
		int fd = sock->socket();
		if(FD_ISSET(fd,&set)){
			FD_ZERO(&set);
			return sock;
		}
	}
	FD_ZERO(&set);
	return NULL;
}


} /* namespace npl */
