#pragma once
#include <queue>
#include <vector>
#include "windows.h"

struct Message
{
	std::string command;
	std::string message;
	double val;
};

class MessageQueue
{
public:
	MessageQueue();
	~MessageQueue();

	Message getNextMessage();
	void addMessage(std::string command, double val);
	void addMessage(std::string command, std::string message);
	void addMessage(std::string command, std::string message, double val);
	void addErrorMessage(HRESULT hr);
	void addErrorMessage(std::string message);
	void clear();
	std::vector<Message> flushQueue();

private:
	void addMessage(Message message);
	bool validQueue;
	HANDLE queueMutex;
	std::queue<Message> messages;
};