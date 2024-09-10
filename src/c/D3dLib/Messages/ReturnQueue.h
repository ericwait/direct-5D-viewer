////////////////////////////////////////////////////////////////////////////////
//Copyright 2014 Andrew Cohen, Eric Wait, and Mark Winter
//This file is part of LEVER 3-D - the tool for 5-D stem cell segmentation,
//tracking, and lineaging. See http://bioimage.coe.drexel.edu 'software' section
//for details. LEVER 3-D is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by the Free
//Software Foundation, either version 3 of the License, or (at your option) any
//later version.
//LEVER 3-D is distributed in the hope that it will be useful, but WITHOUT ANY
//WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
//A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//You should have received a copy of the GNU General Public License along with
//LEVer in file "gnu gpl v3.txt".  If not, see  <http://www.gnu.org/licenses/>.
////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <queue>
#include <vector>
#include <string>

#include "windows.h"

class RtnMessage
{
public:
	RtnMessage()
	{
		command = "";
		message = "";
		val1 = 0;
		val2 = 0;
		aray = NULL;
	}


	std::string command;
	std::string message;
	double val1;
	double val2;
	void* aray;
};

class ReturnQueue
{
public:
	ReturnQueue();
	~ReturnQueue();

	RtnMessage getNextMessage();
	void addMessage(std::string command, double val);
	void addMessage(std::string command, std::string message);
	void addMessage(std::string command, std::string message, double val);
	void addMessage(std::string command,std::string message,double val1,double val2,unsigned char* aray);
	void addErrorMessage(HRESULT hr);
	void addErrorMessage(std::string message);
	size_t size() { return messages.size(); }
	bool isempty() { return messages.empty(); }
	bool hasError() { return errorExist; }
	bool doneLoading() { return loadDone; }
	void clearLoadFlag() { loadDone = false; }
	void clear();
	std::vector<RtnMessage> flushQueue();

private:
	void addMessage(RtnMessage message);
	bool validQueue;
	bool errorExist;
	bool loadDone;
	HANDLE queueMutex;
	std::queue<RtnMessage> messages;
};
