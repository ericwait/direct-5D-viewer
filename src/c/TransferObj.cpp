#include <concrt.h>
#include "Globals.h"
#include "mex.h"
#include "MessageProcessor.h"
#include "MexFunctions.h"
#include "TransferObj.h"

TransferObj::TransferObj(Vec<float> transferFunction, Vec<float> ranges, Vec<float> color, float alphaMod, int chan, char* b, size_t numElem){
	this->chan = chan;
	this->transferFunction = transferFunction;
	this->ranges = ranges;
	this->color = color;
	this->alphaMod = alphaMod;
	strncpy_s(buff, b, strlen(b));
	this->numElem = numElem;
}
TransferObj::~TransferObj(){

}