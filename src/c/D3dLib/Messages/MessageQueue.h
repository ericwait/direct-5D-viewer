#pragma once

#include "Message.h"

#include <windows.h>
#include <queue>

class MessageQueue
{
public:
	MessageQueue(bool waitAllowed);
	~MessageQueue();

	void open();
	void close();

	bool pushMessage(Message* message, bool forceWait = false);
	bool processNext();

	size_t getNumMessages();

	void clear();

private:
	HANDLE mutex;

	volatile bool queueOpen;
	volatile bool waitAllowed;
	volatile bool failed;

	std::queue<Message*> messages;
};