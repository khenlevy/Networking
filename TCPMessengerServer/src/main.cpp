#include "Login.h"
#include "ServerManager.h"

void printInstructions()
{
	cout << "Use one of the following commands: " << endl;
	cout << "lu              - Show all registered users"<<endl;
	cout << "lcu             - Show all connected users"<<endl;
	cout << "ls              - Show all open sessions"<<endl;
	cout << "lr              - Show all rooms"<<endl;
	cout << "lru <room name> - Show all users in <room name>"<<endl;
	cout << "x               - Shutdown server"<<endl;
	cout << "p               - Show available commands" << endl;
}

int main()
{
	cout<<"Starting messenger server.."<<endl;

	ServerManager serverManager; // Listens to all of the sockets who have been connected (in the usersMap)
	Login registration(&serverManager); // Handles the requests of all peer in the peer map (non-users)
	Connection connection(&registration); // Listen to all of the new connections and adds the peers to the peer map

	bool loop = true;
	while (loop)
	{
		string chatRoom;
		string msg;
		string command;
		cin >> command;

		if(command == string("lcu")) { //List Connected Users
			serverManager.getListConnectedUsers();
		} else if(command == string("ls")) { // List of all open sessions
			serverManager.getListSessions();
		} else if(command == string("lu")) {// List of all Users from the file
			serverManager.getListUsers();
		} else if(command==string("lr")) { // List of all the rooms
			serverManager.getListRooms();
		} else if(command == string("lru")) { // List of users in a specific room
			cin>>chatRoom;
			serverManager.getListChatUsers(chatRoom);
		} else if(command == string("p")) { // Print instructions
			printInstructions();
		} else if(command == string("x")) { // Exit
			cout<<"Shutting down.."<<endl;
			loop = false; //Brake loop
		} else { //Wrong input
			cout<<"Wrong input - type p for a list of commands"<<endl;
		}
	}

	cout<<"BYE BYE"<<endl;

	return 0;
}
