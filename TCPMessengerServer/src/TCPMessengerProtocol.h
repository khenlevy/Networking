#ifndef TCPMESSENGERPROTOCOL_H_
#define TCPMESSENGERPROTOCOL_H_

#define MSNGR_PORT 4343

#define CLOSE_SESSION_WITH_PEER 	1
#define OPEN_SESSION_WITH_PEER 		2
#define EXIT						3
#define SEND_MSG_TO_PEER			4
#define SESSION_REFUSED				5
#define SESSION_ESTABLISHED			6
#define SESSION_CREATE				7
#define SESSION_CLOSE 				8
#define SESSION_CREATE_REFUSED		9
#define SESSION_CLOSED				10
#define CHAT_ROOM_CREATE			11
#define CHAT_ROOM_CLOSE				12
#define CHAT_ROOM_LOGIN				13
#define CHAT_ROOM_EXIT				14

#define LIST_CONNECTED_USERS		15
#define LIST_ROOMS					16
#define LIST_CONNECTED_USERS_IN_ROOM 17
#define SIGNUP_REQUEST_APPROVED		18
#define SIGNUP_REQUEST_DENIED		19
#define LIST_USERS 					20

// server
#define CHAT_ROOM_CREATED			21
#define CHAT_ROOM_CREATE_DENIED		22
#define CHAT_ROOM_CLOSED			23
#define CHAT_ROOM_LOGED_IN			24
#define CHAT_ROOM_LOGED_IN_DENIED	25
#define CHAT_ROOM_LEAVED			26
#define CHAT_ROOM_UPDATED			27
#define CHAT_ROOM_UNCLOSED			28


//LOGIN
#define LOGIN_REQUEST				30
#define SIGNUP_REQUEST				31
#define LOGIN_REQUEST_APPROVED		32
#define LOGIN_REQUEST_WRONG_DETAILS	33
#define LOGIN_REQUEST_ALREADY_LOGGED 34

#define TEST_PEER_NAME "test"
#define SESSION_REFUSED_MSG "Connection to peer failed"

#endif /* TCPMESSENGERPROTOCOL_H_ */
