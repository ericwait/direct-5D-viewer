#include "DataQueue.h"
#include <string>
#include <iostream>

DataQueue::DataQueue()
{

}

DataQueue::~DataQueue()
{

}

Message DataQueue::getNextMessage()
{
	if (messages.size() > 0){
		Message message = (Message)messages.front();
		messages.pop();
		return message;
	}
}

size_t DataQueue::getNumMessages(){
	return messages.size();

}

void DataQueue::addMessage(Message msg)
{
	// Add the message to the end of the queue
	messages.push(msg);
}

void DataQueue::writeMessage(std::string cmd, void* data)
{
	Message constructedMessage;
	constructedMessage.command = cmd;
	constructedMessage.data = data;

	// Add it to the queue
	addMessage(constructedMessage);
}