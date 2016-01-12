#include <iostream>
#include "ClientManager.h"
using namespace std;

void printInstructions()
{
	cout << "Use one of the following commands: " << endl;
	cout << "c <server address>              -  Connect to server" << endl;
	cout << "-----------------------------------------------------------" << endl;
	cout << "register <username> <password>  -  Register" <<  endl;
	cout << "login <username> <password>     -  Login" << endl;
	cout << "-----------------------------------------------------------" << endl;
	cout << "lu                              -  Show all registered users" << endl;
	cout << "lcu                             -  Show all connected users" << endl;
	cout << "cr <room name>                  -  Create a new chat room" << endl;
	cout << "or <room name>                  -  Enter a chat room" << endl;
	cout << "dr <room name>                  -  Delete a chat room" << endl;
	cout << "lr                              -  Show all chat rooms" << endl;
	cout << "lru <room name>                 -  Show all users in a specific chat room" << endl;
	cout << "-----------------------------------------------------------" << endl;
	cout << "o <username>                    -  Connect to user" << endl;
	cout << "s <message>                     -  Send a message" << endl;
	cout << "cs                              -  Close opened session or exit from a room" << endl;
	cout << "-----------------------------------------------------------" << endl;
	cout << "l                               -  Print the current status of the client" << endl;
	cout << "d                               -  Disconnect from server" << endl;
	cout << "x                               -  Close the application" << endl;
	cout << "p                               -  Print instructions" << endl;
	cout << "-----------------------------------------------------------" << endl;
}

int main() {

	cout<<"Starting messenger.."<<endl;

	ClientManager clientManager;
	bool running = true;

	while(running)
	{
		string command, parameter1, parameter2, answer;
		cin >> command;

		if(command == "c")
		{
			cin >> parameter1; // Server ip
			if(clientManager.connectToServer(parameter1,MSNGR_PORT))
				cout << "connected to: " << parameter1 << endl;
			else
				cout<<"connection failed"<<endl; // Already connected
		} else if(command=="login") // Login
		{
			cin >> parameter1; // Username
			cin >> parameter2; // Password
			clientManager.log(parameter1,parameter2,LOGIN_REQUEST);
		} else if(command=="register") // Register
		{
			cin >> parameter1; // Username
			cin >> parameter2; // Password
			clientManager.sign(parameter1,parameter2,SIGNUP_REQUEST);
		} else if(command == "o") { //Open a session
			cin >> parameter1; // Username to open session with
			clientManager.openSession(parameter1);
		} else if(command == "l") //Current status
		{
			clientManager.printCurrentInfo();
		} else if(command == "s") // Send message
		{
			getline(std::cin,parameter1); // Message to sent with UDP
			if(!clientManager.sendMsg(parameter1))
				cout<<"You must connect to a user or a chat room first"<<endl;
		} else if(command == "cr") // Create new chat room
		{
			cin >> parameter1; // Room name
			if (!clientManager.createChatRoom(parameter1))
				cout<<"You cannot create a room"<<endl;
		}
		else if(command == "or") // Connect to a chat room
		{
			cin >> parameter1; // Room name
			if (!clientManager.loginToChatRoom(parameter1))
				cout<<"Cannot connect to the room, check if you already logged in."<<endl;
		} else if(command == "lu") // Show all registered users
		{
			clientManager.listUsers();
		} else if(command == "lcu") // Show all connected users
		{
			clientManager.printConnectedUsersRequest();
		} else if(command == "lr") // Show all chat rooms
		{
			clientManager.RoomsList();
		} else if(command == "lru") // Show all users in specific chat room
		{
			cin >> parameter1; // Room name
			clientManager.listConnectedUsersInRoom(parameter1);
		} else if(command == "cs") { // Close open session or exit room
			if(!clientManager.exitRoomOrCloseSession())
				cout << "There is no connection to close" << endl;
		} else if(command == "dr") // Delete a chat room
		{
			cin >> parameter1; // Room name
			if (!clientManager.deleteChatRoom(parameter1))
				cout << "You cannot delete this room" << endl;
		} else if(command == "d") //Disconnect from server
		{
			if (clientManager.isConnectedToServer())
			{
				clientManager.exitAll();
				cout << "You have logged out" << endl;
			} else {
				cout << "You are already disconnected" << endl;
			}

		} else if(command == "x") // Exit
		{
			if (clientManager.isConnectedToServer())
			{
				clientManager.exitAll();
			}
			cout<<"Shutting down..."<<endl;
			running = false;

		} else if(command == "p") { // Print instructions
			printInstructions();
		} else
		{
			cout << "Wrong input - type p for a list of commands" << endl;
		}
	} // End of interface

}
