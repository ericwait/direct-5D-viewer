#pragma once
#include <queue>
#include <vector>
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
	std::vector<Message> flushQueue();

private:
	bool validQueue;
	HANDLE queueMutex;
	std::queue<Message> messages;
};