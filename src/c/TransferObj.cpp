#include <concrt.h>
#include "Globals.h"
#include "mex.h"
#include "MessageProcessor.h"
#include "MexFunctions.h"
#include "TransferObj.h"

TransferObj::TransferObj(int fvtIdx, Vec<float> transferFunction, Vec<float> ranges, Vec<float> color, float alphaMod, int chan){
	this->chan = chan;
	this->fvtIdx = fvtIdx;
	this->transferFunction = transferFunction;
	this->ranges = ranges;
	this->color = color;
	this->alphaMod = alphaMod;
}
TransferObj::~TransferObj(){

}