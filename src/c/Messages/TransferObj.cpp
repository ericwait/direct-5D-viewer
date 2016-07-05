#include "..\Global\Globals.h"
#include "mex.h"
#include "..\D3d\MessageProcessor.h"
#include "..\Mex\MexFunctions.h"
#include "TransferObj.h"

TransferObj::TransferObj(Vec<float> transferFunction, Vec<float> ranges, Vec<float> color, float alphaMod, int chan, char* b, size_t numElem){
	this->chan = chan;
	this->transferFunction = transferFunction;
	this->ranges = ranges;
	this->color = color;
	this->alphaMod = alphaMod;
	char buff[255];
	strncpy_s(buff, b, strlen(b));
	this->numElem = numElem;
}
TransferObj::~TransferObj(){

}