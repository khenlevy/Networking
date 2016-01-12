#ifndef SERVERMANAGER_H_
#define SERVERMANAGER_H_

#include "MThread.h"
#include "TCPSocket.h"
#include <map>
#include "MultipleTCPSocketsListener.h"
#include "User.h"
#include "TCPMessengerProtocol.h"

class User;

class ServerManager: public MThread {

	const char* _loginFile = "connections.txt"; // Registered users list

	MultipleTCPSocketsListener *multiUserListener;
	map<string, User*> usersMap; // mMap of logged in users
	map<string, ChatRoom*> chatRoomsMap; // Map of chat rooms

	bool running;

	// Create a session between two users
	bool createSession(string userToChatWith);

	// Create a session between two users
	void createSession(User* initiatingUser, User* targetUser);

	map<string,User*>::iterator getBeginIter();
	map<string,User*>::iterator getEndIter();

	public:

		ServerManager();

		// Server main loop - handles the users requests
		void run();

		// Add user to users map
		bool addUser(TCPSocket*  userSocket, string LoginUserName);

		// User exit from the server
		void exitServer(User* clientName);

		// Print/send all the users that are registered (in the file)
		void readFromFile(User *clientName);

		// Print/send all the rooms
		void readfromChatRoom(User *clientName);

		// Send notification
		void printToSreen(string msgToScreen);

		// Returns the number of registered users
		int  numOfUsersFromFile();

		// Print List of the connected users
		int  getListConnectedUsers();

		// Send list of the connected users to the asking client
		int  getListConnectedUsers(User *client);

		// Check if user connected
		bool isConnected(string username);

		// Get list of all open sessions
		void getListSessions();

		// Print list of all chat rooms
		void getListRooms();

		// Send  list of chat rooms to the asking client
		void getListRooms(User *clientName);

		// Print list of users in a specific room
		int getListChatUsers(string ChatRoomName);

		// Send list of users in a specific room to the asking client
		int getListChatUsers(User *clientName);

		// Print List of users from login file
		void getListUsers();

		// Send List of users from login file to asking client
		void getListUsers(User *clientName);

		// Create new chat room
		void createChatRoom(User* creator);

		// delete chat room
		void deleteChatRoom(User* creator);

		// login to chat room
		void loginChatRoom(User* creator);

		// Destructor
		virtual ~ServerManager();

};

#endif /* SERVERMANAGER_H_ */
