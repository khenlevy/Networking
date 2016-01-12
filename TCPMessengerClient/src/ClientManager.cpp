#include "ClientManager.h"

ClientManager::ClientManager() {
	login = false;
	sessionOn = false;
	roomOn = false;
	connectedToServer = false;
	serverSocket = NULL;
	_username.clear();
}

// Client receive loop, ran once a login to the server is created
void ClientManager::run()
{
	string parameter1;
	int partnerPort;
	int command;
	running = true;

	while (running)
	{
		command = serverSocket->readCommand();
		if (!command)
			continue;
		switch(command)
		{
			case SESSION_ESTABLISHED:
				udpChatSideB._sideB_name = serverSocket->readMsg();
				udpChatSideB._IP = serverSocket->readMsg();
				udpChatSideB._port = serverSocket->readCommand();

				partnerPort = serverSocket->readCommand();
				clientLinker = new ClientLinker(partnerPort);
				sessionOn = true;

				cout<<"You are connected to "<<udpChatSideB._sideB_name<<endl;
				break;
			case SESSION_CREATE_REFUSED:
				parameter1 = serverSocket->readMsg();
				cout<<"Can't create session because "<<parameter1 <<endl;
				break;
			case SESSION_CLOSED:
				udpChatSideB.Clean();
				sessionOn = false;
				delete(clientLinker);
				cout<<"the session is now terminated"<<endl;
				break;
			case CHAT_ROOM_CREATE_DENIED:
				parameter1 = serverSocket->readMsg();
				cout << "Room cannot be opened because " << parameter1 << endl;
				break;
			case CHAT_ROOM_CREATED:
				cout << "Chat room has been created" << endl;
				break;
			case CHAT_ROOM_LEAVED:
				chatRoomLeaved();
				cout<<"You left the chat room"<<endl;
				break;
			case CHAT_ROOM_LOGED_IN:
				partnerPort = serverSocket->readCommand(); // UDP listen port
				clientLinker = new ClientLinker(partnerPort);
				roomOn = true;
				cout << "You have joined the chat room" << endl;
				break;
			case CHAT_ROOM_LOGED_IN_DENIED:
				cout << serverSocket->readMsg() << endl;
				break;
			case CHAT_ROOM_UPDATED:
				chatRoomUpdate();
				break;
			case CHAT_ROOM_CLOSED:
				cout<<"The chat room was closed"<<endl;
				break;
			case CHAT_ROOM_UNCLOSED:
				cout<<"You cannot delete this chat room"<<endl;
				break;
			case LIST_CONNECTED_USERS:
				printConnectedUsers();
				break;
			case LIST_ROOMS:
				printRoomsList();
				break;
			case LIST_CONNECTED_USERS_IN_ROOM:
				printConnectedUsers();
				break;
			case LIST_USERS:
				printListUsers();
		}
	} // End command interface

}

// Connect to the given server ip and port
bool ClientManager::connectToServer(string ip, int port)
{
	if(serverSocket != NULL)
		return false;
	serverSocket = new TCPSocket(ip,port);
	connectedToServer = true;
	return true;
}

// Register a new user to server with a given username and password
void ClientManager::sign(string username,string password,int cmd)
{
	if (login)
	{
		cout << "You cannot register while you logged in" << endl;
		return;
	}

	int response;
	if(serverSocket)
	{
		serverSocket->writeCommand(cmd);
		serverSocket->writeMsg(username);
		serverSocket->writeMsg(password);
		response = serverSocket->readCommand();

		if (response == SIGNUP_REQUEST_APPROVED)
		{
			cout<<"signed up was successful with the user: "<<username<<endl;
		}
		else if(cmd==SIGNUP_REQUEST_DENIED)
		{
			cout<<"you failed to sign up"<<endl;
		}
	}
	else
	{
		cout<<"the server is not connected"<<endl;
	}
}

// Login to server with a given username and password
void ClientManager::log(string username,string password,int cmd)
{
	if (login)
	{
		cout<<"login failed - you are already logged in"<<endl;
		return;
	}

	int response;
	if(serverSocket)
	{
		serverSocket->writeCommand(cmd);
		serverSocket->writeMsg(username);
		serverSocket->writeMsg(password);
		response = serverSocket->readCommand();

		if(response == LOGIN_REQUEST_APPROVED)
		{
			_username = username;
			login = true;
			start();
			cout << "You were logged in as " + username << endl;
		}
		else if(response == LOGIN_REQUEST_WRONG_DETAILS)
		{
			cout<<"Incorrect login credentials"<<endl;
		}
		else if (response == LOGIN_REQUEST_ALREADY_LOGGED)
		{
			cout<< username + " already logged in"<<endl;
		}
	}
	else
	{
		cout<<"You are not conneted to a server"<<endl;
	}
}

// Open session with the given peer name
bool ClientManager::openSession(string partnerName)
{
	if (!serverSocket || !login)
	{
		cout<<"You are not connected/logged in"<<endl;
		return false;
	}
	if (isInChat())
	{
		cout << "You are already in a session" << endl;
		return false;
	}
	if (partnerName == _username)
	{
		cout<<"You can't connect to your self"<<endl;
		return false;
	}
	serverSocket->writeCommand(SESSION_CREATE);
	serverSocket->writeMsg(partnerName);

	return true;
}

// Print the current status of the client
void ClientManager::printCurrentInfo()
{
	if (serverSocket)
		cout<< "Connected to server "<<endl;
	else
		cout<< "Not Connected to server "<<endl;
	if (login)
		cout<< "Logged in as  "<< _username<<endl;
	else
		cout<< "Not logged in "<<endl;
	if (sessionOn)
		cout<< "In session "<<endl;
	else
		cout<< "Not in session "<<endl;
	if (roomOn)
		cout<< "Connected to chat room "<<chatRoomName <<endl;
	else
		cout<< "Not In chat room "<<endl;
}

// Sending UDP message to specific user or to all the users in a room.
bool ClientManager::sendMsg(string msg)
{
	if(sessionOn)
	{
		clientLinker->send( string(">[")+_username+string("]") + msg,udpChatSideB._IP,udpChatSideB._port);
		return true;
	}
	else if (roomOn)
	{
		std::vector<ChatSideB*>::iterator iter = chatUsers.begin();
		std::vector<ChatSideB*>::iterator enditer = chatUsers.end();

		while (iter != enditer)
		{
			clientLinker->send(string(">[")+_username+string("] ") + msg,(*iter)->_IP,(*iter)->_port);
			iter++;
		}
		return true;
	}
	return false;
}

// Open session in the given room name
bool ClientManager::createChatRoom(string name)
{
	if(!serverSocket || !login || isInChat())
		return false;
	serverSocket->writeCommand(CHAT_ROOM_CREATE);
	serverSocket->writeMsg(name);
	return true;
}

bool ClientManager::loginToChatRoom(string roomName)
{
	if(isInChat() || !serverSocket || !login)
		return false;
	serverSocket->writeCommand(CHAT_ROOM_LOGIN);
	serverSocket->writeMsg(roomName);
	return true;
}

// Print all of the connected users after received from the server
void ClientManager::printConnectedUsers()
{
	int i;
	int numOfUsers;
	numOfUsers = serverSocket->readCommand();
	for(i=0 ; i<numOfUsers ; i++)
	{
		cout << " User: " << serverSocket->readMsg() << endl;
	}
	if(i == 0)
	{
		cout << "No one is connected" << endl;
	}
}

// Print the list of the users from the file
void ClientManager::printListUsers()
{
	int i;
	int numOfUsers;
	numOfUsers = serverSocket->readCommand();
	for(i=0 ; i<numOfUsers ; i++)
	{
		cout << " User: " << serverSocket->readMsg() << endl;
	}
	if(i == 0)
	{
		cout << "No one is connected"<< endl;
	}
}

// send to the server request of connected users
void ClientManager::printConnectedUsersRequest()
{
	if (!serverSocket || !login)
	{
		cout << "You are not connected or not logged in"<<endl;
		return;
	}
	serverSocket->writeCommand(LIST_CONNECTED_USERS);
}

// Send to the server request of list of the users from the file
void ClientManager::listUsers()
{
	if (!serverSocket || !login)
	{
		cout << "You are not connected or not logged in"<<endl;
		return;
	}
	serverSocket->writeCommand(LIST_USERS);
}

// Send to the server request of room list
void ClientManager::RoomsList()
{
	if (!serverSocket || !login)
	{
		cout<<"You are not connected or not logged in"<<endl;
		return;
	}
	serverSocket->writeCommand(LIST_ROOMS);
}

// Send to the server request of all the users in a room (given by it's name)
void ClientManager::listConnectedUsersInRoom(string roomName)
{
	if (!serverSocket || !login)
	{
		cout<<"You are not connected or not logged in"<<endl;
		return;
	}
	serverSocket->writeCommand(LIST_CONNECTED_USERS_IN_ROOM);
	serverSocket->writeMsg(roomName);
}

// Delete the room by it's name (only the owner of the room can delete it)
bool ClientManager::deleteChatRoom(string name)
{
	if (!serverSocket || !login)
	{
		cout<<"You are not connected or logged in"<<endl;
		return false;
	}
	serverSocket->writeCommand(CHAT_ROOM_CLOSE);
	serverSocket->writeMsg(name);
	return true;
}

// Disconnect the open session / exit from a room
bool ClientManager::exitRoomOrCloseSession()
{
	if (roomOn)
	{
		serverSocket->writeCommand(CHAT_ROOM_EXIT);
	}
	else if (sessionOn)
	{
		serverSocket->writeCommand(SESSION_CLOSE);
	}
	else
	{
		return false;
	}

	return true;

}

// Adds room mate vector of the users
void ClientManager::addRoomUser(string roomate, string IP, int port)
{
	ChatSideB *temp= new ChatSideB();
	temp->_sideB_name =roomate;
	temp->_IP = IP;
	temp->_port = port;
	chatUsers.push_back(temp);
}

// Update the user details after leaving a room
void ClientManager::chatRoomLeaved()
{
	roomOn = false;
	clearRoomUsers();
	delete(clientLinker);
}

// Clear the vector of the users - (after exiting a room)
void ClientManager::clearRoomUsers()
{
	std::vector<ChatSideB*>::iterator iter = chatUsers.begin();
	std::vector<ChatSideB*>::iterator enditer = chatUsers.end();

	while (iter != enditer)
	{
		delete(*iter);
		iter++;
	}
	chatUsers.clear();
}

// Return true if the user is in a session or in a chat. false otherwise
bool ClientManager::isInChat()
{
	return roomOn || sessionOn;
}

bool ClientManager::isConnectedToServer() const {
	return connectedToServer;
}

// Disconnect from the server and exit
void ClientManager::exitAll()
{
	exitRoomOrCloseSession();
	serverSocket->writeCommand(EXIT);
	_username.clear();
	running = false;
	login = false;
	connectedToServer = false;
	serverSocket = NULL;
}

// Print the list rooms
void ClientManager::printRoomsList()
{
	int numOfRooms;
	numOfRooms = serverSocket->readCommand();

	if(numOfRooms > 0)
	{
		cout << "Rooms: "<<endl;
		for(int i = 0 ; i<numOfRooms;i++)
		{
			cout << "Room name: " << serverSocket->readMsg() << endl;
		}
	}
	else
		cout << "There are no rooms" << endl;
}

// Update the rooms info
void ClientManager::chatRoomUpdate()
{
	roomOn = true;
	clearRoomUsers();
	chatRoomName = serverSocket->readMsg();
	int countofmemebers = serverSocket->readCommand();
	for (int i = 0 ; i< countofmemebers;i++)
	{
		string user = serverSocket->readMsg();
		string ip = serverSocket->readMsg();
		int port = serverSocket->readCommand();
		addRoomUser(user,ip,port);
	}
	cout<<"Chat room "<< chatRoomName<<" updated"<<endl;
}

ClientManager::~ClientManager() {
	running = false;
}
