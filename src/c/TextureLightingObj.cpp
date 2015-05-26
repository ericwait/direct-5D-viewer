#include <concrt.h>
#include "Globals.h"
#include "mex.h"
#include "MessageProcessor.h"
#include "MexFunctions.h"
#include "TextureLightingObj.h"

TextureLightingObj::TextureLightingObj(bool value, int index){
	this->value = value;
	this->index = index;
}

TextureLightingObj::~TextureLightingObj(){

}