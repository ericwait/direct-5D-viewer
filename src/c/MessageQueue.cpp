#include "MessageQueue.h"


MessageQueue::MessageQueue()
{
	queueMutex = CreateMutex(NULL,FALSE,NULL);
	validQueue = true;
}

MessageQueue::~MessageQueue()
{
	if (queueMutex!=NULL)
		CloseHandle(queueMutex);

	queueMutex = NULL;
}

Message MessageQueue::getNextMessage()
{
	DWORD waitTime = INFINITE;
	Message msgOut;

#ifdef _DEBUG
	waitTime = 36000;
#endif // _DEBUG
	
	DWORD waitTerm = WaitForSingleObject(queueMutex,waitTime);
	if (waitTerm==WAIT_TIMEOUT)
	{
		throw std::runtime_error("Could not acquire mutex for message queue!");
	}

	if (messages.empty())
		msgOut.str = "null";
	else
	{
		msgOut = messages.front();
		messages.pop();
	}

	ReleaseMutex(queueMutex);

	return msgOut;
}

void MessageQueue::addMessage(std::string message, double val)
{
	Message msgIn;
	msgIn.str = message;
	msgIn.val = val;

	addMessage(msgIn);
}

void MessageQueue::addMessage(Message message)
{
	if (!validQueue) return;

	DWORD waitTime = INFINITE;

#ifdef _DEBUG
	waitTime = 36000;
#endif // _DEBUG

	DWORD waitTerm = WaitForSingleObject(queueMutex,waitTime);
	if (waitTerm==WAIT_TIMEOUT)
	{
		throw std::runtime_error("Could not acquire mutex for message queue!");
	}

	messages.push(message);

	ReleaseMutex(queueMutex);
}

void MessageQueue::clear()
{
	DWORD waitTime = INFINITE;

#ifdef _DEBUG
	waitTime = 36000;
#endif // _DEBUG

	DWORD waitTerm = WaitForSingleObject(queueMutex,waitTime);
	if (waitTerm==WAIT_TIMEOUT)
	{
		throw std::runtime_error("Could not acquire mutex for message queue!");
	}

	while (!messages.empty())
	{
		messages.pop();
	}

	validQueue = false;

	ReleaseMutex(queueMutex);
}

std::vector<Message> MessageQueue::flushQueue()
{
	DWORD waitTime = INFINITE;
	Message msgOut;

#ifdef _DEBUG
	waitTime = 36000;
#endif // _DEBUG

	DWORD waitTerm = WaitForSingleObject(queueMutex,waitTime);
	if (waitTerm==WAIT_TIMEOUT)
	{
		throw std::runtime_error("Could not acquire mutex for message queue!");
	}

	std::vector<Message> queueOut;
	if (messages.empty())
	{
		Message none;
		none.str = "null";
		none.val = -1;

		queueOut.push_back(none);
	}
	else
	{
		while (!messages.empty())
		{
			queueOut.push_back(messages.front());
			messages.pop();
		}
	}

	ReleaseMutex(queueMutex);

	return queueOut;
}

