#pragma once
#include <queue>
#include "windows.h"

struct Message
{
	std::string str;
	double val;
};

class MessageQueue
{
public:
	MessageQueue();
	~MessageQueue();

	Message getNextMessage();
	void addMessage(std::string message, double val);
	void addMessage(Message message);
	void clear();

private:
	bool validQueue;
	HANDLE queueMutex;
	std::queue<Message> messages;
};