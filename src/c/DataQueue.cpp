#include "DataQueue.h"
#include <string>
#include <iostream>
#include "mex.h"
#include "comdef.h"

DataQueue::DataQueue()
{
	mutex = CreateMutex(NULL, FALSE, NULL);
}

DataQueue::~DataQueue()
{
	if (mutex != NULL)
		CloseHandle(mutex);

	mutex = NULL;
}

Message DataQueue::getNextMessage()
{
	if (!messages.empty()){
		DWORD waitTerm = WaitForSingleObject(mutex, INFINITE);

		Message message = messages.front();
		messages.pop();

		ReleaseMutex(mutex);

		return message;
	}
}

size_t DataQueue::getNumMessages(){
	return messages.size();

}

void DataQueue::addMessage(Message msg)
{
	// Add the message to the end of the queue
	const char* c = (msg.command).c_str();
  	//mexPrintf(c);

	DWORD waitTerm = WaitForSingleObject(mutex, INFINITE);
	messages.push(msg);
	ReleaseMutex(mutex);
}

void DataQueue::writeMessage(std::string cmd, void* data)
{
	Message constructedMessage;
	constructedMessage.command = cmd;
	constructedMessage.data = data;

	// Add it to the queue
	addMessage(constructedMessage);
}