#include "..\Global\Globals.h"
#include "mex.h"
#include "..\D3d\MessageProcessor.h"
#include "..\Mex\MexFunctions.h"
#include "Image.h"

void XloadTextureCommand(Message m);
void XresetViewCommand(Message m);
void XtransferUpdateCommand(Message m);
void XaddPolygonsCommand(Message m);
void XpeelUpdateCommand(Message m);
void XremovePolygonCommand(Message m);
void XplayCommand(Message m);
void XrotateCommand(Message m);
void XshowLabelsCommand(Message m);
void XcaptureSpinMovieCommand(Message m);
void XviewTextureCommand(Message m);
void XviewSegmentationCommand(Message m);
void XwireframeSegmentationCommand(Message m);
void XsetFrameCommand(Message m);
void XsetViewOriginCommand(Message m);
void XtextureLightingUpdateCommand(Message m);
void XtextureAttenUpdateCommand(Message m);
void XsegmentationLighting(Message m);
void XdisplayPolygonsCommand(Message m);
void XdeleteAllPolygonsCommand(Message m);
void XcaptureWindow();
void XsetBackgroundColor(Message m);