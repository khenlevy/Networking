#include "ChatRoom.h"

ChatRoom::ChatRoom(User* owner, string chatRoom) {
	_roomOwner = owner;
	_chatRoomName = chatRoom;
}

// Show room updates after new user added/removed
void ChatRoom::updateusers()
{
	for(vector<User*>::iterator iter = chatRoomUsers.begin(); iter != chatRoomUsers.end(); iter++)
	{
		(*iter)->writeCommand(CHAT_ROOM_UPDATED);
		(*iter)->writeMsg(_chatRoomName);
		(*iter)->writeCommand(chatRoomUsers.size());

		for(vector<User*>::iterator iter2 = chatRoomUsers.begin(); iter2 != chatRoomUsers.end(); iter2++)
		{
			(*iter)->writeMsg((*iter2)->getusername());
			(*iter)->writeMsg((*iter2)->getIP());
			(*iter)->writeCommand((*iter2)->getport());
		}
	}
}

// Show all users and return count
int ChatRoom::printUsers()
{
	int count = 0;
	for(vector<User*>::iterator iter = chatRoomUsers.begin(); iter != chatRoomUsers.end(); iter++)
	{
		cout<<(*iter)->getusername()<<endl;
		count++;
	}
	return count;
}

// Send list of users to specific user
int ChatRoom::sendUserList(User *sendto)
{
	int numOfUsers = 0;

	sendto->writeCommand(chatRoomUsers.size());
	for(vector<User*>::iterator iter = chatRoomUsers.begin(); iter != chatRoomUsers.end(); iter++)
	{
		sendto->writeMsg((*iter)->getusername());
		numOfUsers++;
	}

	return numOfUsers;
}

// Add user to room
bool ChatRoom::addUser(User* userToAdd)
{
	bool exist = false;

	// Check if exist
	for(vector<User*>::iterator iter = chatRoomUsers.begin(); iter != chatRoomUsers.end(); iter++)
	{
		if ((*iter) == userToAdd)
		{
			exist = true;
		}

	}

	if(exist)
	{
		cout << "User already in room" <<endl;
		return false;
	}

	chatRoomUsers.push_back(userToAdd);
	updateusers();

	return true;
}

// Get room owner
User* ChatRoom::getOwner()
{
	return _roomOwner;
}

// Remove user from chat room
bool ChatRoom::logOffUser(User *usertologof)
{
	bool exist = false;

	// Check if exist
	for(vector<User*>::iterator iter = chatRoomUsers.begin(); iter != chatRoomUsers.end(); iter++)
	{
		if ((*iter) == usertologof)
		{
			exist = true;
		}

	}

	if(!exist){
		return false;
	}

	for(vector<User*>::iterator iter = chatRoomUsers.begin(); iter != chatRoomUsers.end(); iter++)
		{
		if ((*iter) == usertologof)
		{
			chatRoomUsers.erase(iter);
			break;
		}
	}
	updateusers();
	return true;
}

ChatRoom::~ChatRoom() {

	for(vector<User*>::iterator iter = chatRoomUsers.begin(); iter != chatRoomUsers.end(); iter++)
	{
		(*iter)->disconnectFromChatRom(true);
	}
}

