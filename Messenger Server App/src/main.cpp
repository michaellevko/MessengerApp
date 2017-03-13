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
		char command[50];
		cin.getline(command, sizeof(command));

		char delimiter = ' ';
		vector<string> userInput = split(command, delimiter);

		if (userInput[0] == LIST_USERS)
		{
			server->listAllUsers();
		}
		else if (userInput[0] == LIST_CONNECTED_USERS)
		{
			server->ListAllConnectedUsers();
		}
		else if (userInput[0] == LIST_SESSIONS)
		{
			server->ListAllSessions();
		}
		else if (userInput[0] == LIST_ROOMS)
		{
			server->ListAllRooms();
		}
		else if (userInput[0] == LIST_ROOM_USERS)
		{
			server->ListAllUsersInThisRoom(userInput[1]);
		}
		else if (userInput[0] == HELP)
		{
			printInstructions();
		}
		else if (userInput[0] == SHUTDOWN)
		{
			server->shutdown();

			break;
		}
		else
		{
			cout << "Error: invalid command - " << userInput[0] <<endl;
		}
	}
	return 0;
}

