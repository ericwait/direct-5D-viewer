#include "MessageQueue.h"
#include "ErrorMsg.h"

#include "comdef.h"

#include <string>
#include <iostream>
#include <thread>


MessageQueue::MessageQueue(bool waitAllowed)
	: failed(false), waitAllowed(waitAllowed), queueOpen(false)
{
	mutex = CreateMutex(NULL, FALSE, NULL);
}

MessageQueue::~MessageQueue()
{
	if (mutex != NULL)
		CloseHandle(mutex);

	mutex = NULL;
}

void MessageQueue::open()
{
	queueOpen = true;
    failed = false;
}

void MessageQueue::close()
{
	queueOpen = false;
	clear();
}

bool MessageQueue::pushMessage(Message* newMessage, bool forceWait)
{
	DWORD waitTerm = WaitForSingleObject(mutex, INFINITE);

    failed = false;
	messages.push(newMessage);

	ReleaseMutex(mutex);

	if ( !queueOpen )
		return false;

	if ( waitAllowed && forceWait )
	{
		while ( !failed && !messages.empty() )
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	return true;
}

bool MessageQueue::processNext()
{
	Message* nextMessage = NULL;
	if ( !messages.empty() )
	{
		// NOTE: Front isn't threadsafe unfortunately, this is the only
		// thread that pops though so only single conditional required
		DWORD waitTerm = WaitForSingleObject(mutex, INFINITE);
		nextMessage = messages.front();
		ReleaseMutex(mutex);
	}

	// Process message before it's removed from the queue
	bool success = true;
	if ( nextMessage )
		success = nextMessage->process();

	// Acquire mutex to pop message, pop needs to happen last so that blocking push code works
	DWORD waitTerm = WaitForSingleObject(mutex, INFINITE);
	messages.pop();
	ReleaseMutex(mutex);

	SAFE_DELETE(nextMessage);

	// TODO: Have a better way of failing
	if ( !success )
		failed = true;

	return success;
}

size_t MessageQueue::getNumMessages()
{
	return messages.size();
}

void MessageQueue::clear()
{
	DWORD waitTerm = WaitForSingleObject(mutex, INFINITE);

	while ( !messages.empty() )
	{
		Message* nextMessage = messages.front();
		messages.pop();

		delete nextMessage;
	}

	ReleaseMutex(mutex);
}

