/*
 * main.cpp
 *
 *  Created on: Mar 1, 2017
 *      Author: user
 */

#include "Server.h"

#define LIST_USERS "lu"
#define LIST_CONNECTED_USERS "lcu"
#define LIST_SESSIONS "ls"
#define LIST_ROOMS "lr"
#define LIST_ROOM_USERS "lru"
#define SHUTDOWN "x"
#define	HELP "help"

using namespace std;
using namespace npl;

static void printInstructions(){
		cout<<"---------------MESSENGER SERVER---------------"<<endl;
		cout<<"lu ­             - List All Users"<<endl;
		cout<<"lcu             ­- List All Connected Users  "<<endl;
		cout<<"ls ­             - List All Sessions"<<endl;
		cout<<"lr ­             - List All Room"<<endl;
		cout<<"lru <room name> - ­List All Users In This Room"<<endl;
		cout<<"x               - Shutdown"<<endl;
		cout<<"help 		   - Print Menu"<<endl;
		cout << "-----------------------------------------" << endl;
}

int main() {
	cout << "Welcome to TCP Messenger Server" << endl;
	Server* server = new Server();
	printInstructions();
	while (true) {
		string msg;
		string command;
		cin >> command;

		string delimiter = ":";
		string beginCommand = command.substr(0, command.find(delimiter));
		string endCommand = command.substr(command.find(delimiter)+1);

		if (beginCommand == LIST_USERS)
		{
			server->listAllUsers();
		}
		else if (beginCommand == LIST_CONNECTED_USERS)
		{
			server->ListAllConnectedUsers();
		}
		else if (beginCommand == LIST_SESSIONS)
		{
			server->ListAllSessions();
		}
		else if (beginCommand == LIST_ROOMS)
		{
			server->ListAllRooms();
		}
		else if (beginCommand == LIST_ROOM_USERS)
		{
			server->ListAllUsersInThisRoom(endCommand);
		}
		else if (beginCommand == HELP)
		{
			printInstructions();
		}
		else if (beginCommand == SHUTDOWN)
		{
			server->shutdown();

			break;
		}
		else
		{
			cout << "Error: invalid command - " << beginCommand <<endl;
		}
	}
	return 0;
}

