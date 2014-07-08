function lever3d()
global imageData orgImage processedImage tmr uiControlFig

[arrowFaces, arrowVerts, arrowNorms] = MakeArrow(0.65,0.05,0.15,40);
[sphereFaces, sphereVerts, shereNorms] = MakeSphere(0.20,40);

[orgImage, imageData] = tiffReader([],[],[],[],'d:\Users\Eric.Bioimage29\Documents\Images\LEVer3d\Susan_overnight_new');
%[orgImage, imageData] = tiffReader();

processedImage = orgImage;

tmr = timer('ExecutionMode','fixedSpacing','Period',0.1,'TimerFcn','CheckMessage');
start(tmr);

lever_3d('init',arrowFaces, arrowVerts, arrowNorms,sphereFaces, sphereVerts, shereNorms);

lever_3d('loadTexture',imageConvert(orgImage,'uint8'),[imageData.XPixelPhysicalSize,imageData.YPixelPhysicalSize,imageData.ZPixelPhysicalSize]);

uiControlFig = ViewerControls();

end
