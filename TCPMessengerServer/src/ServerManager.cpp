#include "ServerManager.h"


ServerManager::ServerManager() {

	multiUserListener = NULL;
	running = true;

	start();
	cout<<"Server manager started"<<endl;
}

/*
 * server main loop - handles the users requests
 */
void ServerManager::run()
{
	running = true;
	User* currUser = NULL;
	multiUserListener = NULL;
	long command = 0;
	string msg;
	while (running)
	{
		// Create listener
		if (multiUserListener != NULL)
		{
			delete (multiUserListener);
		}
		multiUserListener = new MultipleTCPSocketsListener();

		// Convert User to Socket before adding to list
		map<string, TCPSocket*> toSocket;
		for (map<string,User*>::iterator biter = usersMap.begin(); biter != usersMap.end(); biter++)
		{
			toSocket[biter->first] = (biter->second->getSocket());
		}

		// Add socket
		multiUserListener->addSockets(toSocket);

		// Listen to socket
		TCPSocket* readyPeer = multiUserListener->listenToSocket(2);
		if (!readyPeer){
			continue;
		}

		// Found user from tcpsocket list
		for (map<string,User*>::iterator iter = usersMap.begin(); iter != usersMap.end(); iter++)
		{
			if (readyPeer == iter->second->getSocket())
			{
				currUser = iter->second;
				break;
			}
		}

		// READ command from the user
		command = currUser->readCommand();

		switch(command)
		{
			case 0:
				exitServer(currUser);
				break;
			case SESSION_CREATE:
				msg = currUser->readMsg(); // Partner name
				if (!usersMap[msg])
				{
					currUser->writeCommand(SESSION_CREATE_REFUSED);
					currUser->writeMsg(string("No such user"));
					break;
				}
				else if (usersMap[msg]->inChat())
				{
					currUser->writeCommand(SESSION_CREATE_REFUSED);
					currUser->writeMsg(string("The selected user is already in chat"));
					break;
				}
				createSession(currUser,usersMap[msg]);
				cout<<"Session was created between: "<<currUser->getusername()<<" AND "<<msg<<endl;
				break;
			case EXIT:
				exitServer(currUser);
				break;
			case SESSION_CLOSE:
				currUser->closeSession(true);
				break;
			case CHAT_ROOM_CREATE:
				createChatRoom(currUser);
				break;
			case CHAT_ROOM_CLOSE:
				deleteChatRoom(currUser);
				break;
			case CHAT_ROOM_LOGIN:
				loginChatRoom(currUser);
				break;
			case CHAT_ROOM_EXIT:
				currUser->disconnectFromChatRom(false);
				break;
			case LIST_CONNECTED_USERS:
				getListConnectedUsers(currUser);
				break;
			case LIST_ROOMS:
				getListRooms(currUser);
			break;
			case LIST_CONNECTED_USERS_IN_ROOM:
				getListChatUsers(currUser);
				break;
			case LIST_USERS:
				getListUsers(currUser);
				break;
		}
	}
}

// Add user to users map
bool ServerManager::addUser(TCPSocket* userSocket, string LoginUserName)
{
	bool added = true;
	User* userToAdd = new User(LoginUserName,userSocket);

	usersMap.insert(pair<string,User*>(LoginUserName,userToAdd));
	cout <<  LoginUserName << " has logged in" << endl;

	return added;
}

// Print List of the connected users
int ServerManager::getListConnectedUsers()
{
	if (usersMap.begin() == usersMap.end())
	{
		cout << "There aren't any users connected" << endl;
		return 0;
	}

	cout<<"The Connected users are:"<<endl;

	map<string,User*>::iterator iter;

	string name;
	int count = 0;
	for (iter = usersMap.begin(); iter != usersMap.end(); iter++)
	{
		name = iter->first;
		printToSreen(name);
		count++;
	}
	return count;
}

// Send list of the connected users to the asking client
int ServerManager::getListConnectedUsers(User *client)
{
	client->writeCommand(LIST_CONNECTED_USERS);
	client->writeCommand(usersMap.size());

	map<string,User*>::iterator iter ;

	string name;
	int count = 0;
	for (iter = usersMap.begin() ; iter != usersMap.end();iter++)
	{
		name = iter->first;
		client->writeMsg( name);
		count++;
	}

	return count;
}

bool ServerManager::isConnected(string username)
{
	if (usersMap.find(username) == usersMap.end())
		return false; // not found
	return true;
}

// get list of all open sessions
void ServerManager::getListSessions()
{
	cout<<"All the connected users that in Session:"<<endl;
	std::map<string,User*>::iterator iter;

	string name;
	for (iter = usersMap.begin() ; iter != usersMap.end();iter++)
	{
		if(iter->second->inSession())
		{
			name = iter->first;
			printToSreen(name);
		}
	}
}

// Print list of all chat rooms
void ServerManager::getListRooms()
{
	if (chatRoomsMap.begin() != chatRoomsMap.end())
	{
		cout<<"the rooms list:"<<endl;
		this->readfromChatRoom(NULL);
	}
	else
		cout<<"There are no rooms yet"<<endl;
}

// Send  list of chat rooms to the asking client
void ServerManager::getListRooms(User *client)
{
	client->writeCommand(LIST_ROOMS);
	client->writeCommand(chatRoomsMap.size());

	this->readfromChatRoom(client);
}

// Print list of users in a specific room
int ServerManager::getListChatUsers(string ChatRoomName)
{
	int numOfUsers;
	if (chatRoomsMap.find(ChatRoomName) == chatRoomsMap.end())
	{
		cout << "No such room: " << ChatRoomName << endl;
		return 0;
	}
	cout<<"Users list in Room:"<<endl;
	for (map<string,ChatRoom*>::iterator iter = chatRoomsMap.begin(); iter !=  chatRoomsMap.end();iter++)
	{
		if (ChatRoomName == iter->first){
			numOfUsers = iter->second->printUsers();
		}
	}

	if (numOfUsers == 0)
		cout << "There are no users in this room"<< endl;

	return numOfUsers;
}

// Send list of users in a specific room to the asking client
int ServerManager::getListChatUsers(User *client)
{
	int numOfUsers = 0;
	string ChatRoomName = client->readMsg();
	for (map<string,ChatRoom*>::iterator iter = chatRoomsMap.begin(); iter !=  chatRoomsMap.end();iter++)
	{
		if (ChatRoomName == iter->first)
		{
			client->writeCommand(LIST_CONNECTED_USERS_IN_ROOM);
			(iter)->second->sendUserList(client);
			break;
		}
	}
	return numOfUsers;
}

// Send List of users from login file to asking client
void ServerManager::getListUsers(User *client)
{
	int numOfusers = 0;
	numOfusers = numOfUsersFromFile();

	client->writeCommand(LIST_USERS);
	client->writeCommand(numOfusers -1);

	if (client != NULL)
		readFromFile(client);
	else
		readFromFile(NULL);

}

// Print List of users from login file
void ServerManager::getListUsers()
{
	cout << "the users:"<< endl;
	readFromFile(NULL); // To server
}

// User exit from the server
void ServerManager::exitServer(User* clientName)
{
	cout<<"the User: "<< clientName->getusername() << " was  disconnect" <<endl;
	usersMap.erase(clientName->getusername());
}

// Create a session between two users
void ServerManager::createSession(User* fromUser, User* toUser) {
	// login the two users
	toUser->loginUsertoSession(fromUser);
	fromUser->loginUsertoSession(toUser);

	// Send communication details
	toUser->writeCommand(SESSION_ESTABLISHED);
	toUser->writeMsg(fromUser->getusername());
	toUser->writeMsg(fromUser->getIP());
	toUser->writeCommand(fromUser->getport());
	toUser->writeCommand(toUser->getport());

	fromUser->writeCommand(SESSION_ESTABLISHED);
	fromUser->writeMsg(toUser->getusername());
	fromUser->writeMsg(toUser->getIP());
	fromUser->writeCommand(toUser->getport());
	fromUser->writeCommand(fromUser->getport());

}


// Create new chat room
void ServerManager::createChatRoom(User* creator)
{
	string msg;
	bool exist = false;
	msg = creator->readMsg();

	// Checking that the name does not exist
	for (map<string,ChatRoom*>::iterator iter = chatRoomsMap.begin(); iter !=  chatRoomsMap.end();iter++)
	{
		if (iter->first == msg)
		{
			exist = true;
		}
	}


	if (exist)
	{
		creator->writeCommand(CHAT_ROOM_CREATE_DENIED);
		creator->writeMsg(string("Chat Room name already exists"));
		return;
	}

	//Add new chat room
	chatRoomsMap[msg] = new ChatRoom(creator, msg);
	creator->writeCommand(CHAT_ROOM_CREATED);
	cout << "Room : "<< msg <<" was created" << endl;
}


// Remove chat room
void ServerManager::deleteChatRoom(User* creator)
{
	string msg;
	msg = creator->readMsg();

	bool exist = false;

	for (map<string,ChatRoom*>::iterator iter = chatRoomsMap.begin(); iter !=  chatRoomsMap.end();iter++)
	{
		if (iter->first == msg)
		{
			exist = true;
		}
	}


	if(!exist){
		creator->writeCommand(CHAT_ROOM_UNCLOSED);
	}

	if(chatRoomsMap[msg]->getOwner()->getusername() == creator->getusername())
	{
		delete(chatRoomsMap[msg]);
		creator->writeCommand(CHAT_ROOM_CLOSED);
		chatRoomsMap.erase(msg);
	}
	else
	{
		creator->writeCommand(CHAT_ROOM_UNCLOSED);
	}

}

// Login to chat room
void ServerManager::loginChatRoom(User* loginUser)
{
	string roomName = loginUser->readMsg();
	bool exist = false;
	for (map<string,ChatRoom*>::iterator iter = chatRoomsMap.begin(); iter !=  chatRoomsMap.end();iter++)
	{
		if (iter->first == roomName)
		{
			exist = true;
		}
	}


	if (!exist)
	{
		loginUser->writeCommand(CHAT_ROOM_LOGED_IN_DENIED);
		loginUser->writeMsg(string("Room does not exist"));
		return;
	}

	loginUser->loginUserToChatRoom(chatRoomsMap[roomName]);
	if (chatRoomsMap[roomName]->addUser(loginUser))
	{
		loginUser->writeCommand(CHAT_ROOM_LOGED_IN);
		loginUser->writeCommand(loginUser->getport());
	}
	else
	{
		loginUser->writeCommand(CHAT_ROOM_LOGED_IN_DENIED);
		loginUser->writeMsg(string("you are already logged in"));
	}
}

// Count registered users
int ServerManager::numOfUsersFromFile(){
	string line;
	fstream loginFile;
	string userFromFile;
	int counter = 0;

	loginFile.open(_loginFile,ios::in|ios::out|ios::binary);

	while ( !loginFile.eof() )
	{
	  getline (loginFile,line);
	  counter++;
	}
	loginFile.close();
	return counter;
}

// Show all registered users (from connections file)
void ServerManager::readFromFile(User *clientName)
{
	string line;
	fstream loginFile;
	string userFromFile;
	loginFile.open(_loginFile,ios::in|ios::out|ios::binary);
	if (loginFile.is_open())
	{
		while ( !loginFile.eof() )
		{
			getline (loginFile,line);
			istringstream liness( line );
			getline( liness, userFromFile, '-' );
			if (clientName != NULL)
			{
			  clientName->writeMsg(userFromFile); // Send to client who requested
			}
			else
			{
			  printToSreen(userFromFile); // Print on server
			}

		}
	loginFile.close();
	}
	else
	{
		 printToSreen("Error - could not open the file");
	}

}

void ServerManager::printToSreen(string msgToScreen){
    cout << msgToScreen << endl;
}

// Show all chat rooms
void ServerManager::readfromChatRoom(User *clientName)
{
	string name;
	for (map<string,ChatRoom*>::iterator iter = chatRoomsMap.begin(); iter !=  chatRoomsMap.end();iter++)
	{
		name = iter->first;
		if (clientName != NULL)
		{
			clientName->writeMsg(name); // send to client who requested
		}
		else
		{
			printToSreen(name); // Print on server
		}

	}
}

ServerManager::~ServerManager() {
	running = false;
	waitForThread();
}
