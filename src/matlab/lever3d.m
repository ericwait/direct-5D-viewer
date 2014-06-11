function lever3d()
global imageData

[arrowFaces, arrowVerts, arrowNorms] = MakeArrow(0.65,0.05,0.15,40);
[sphereFaces, sphereVerts, shereNorms] = MakeSphere(0.20,40);

[im, imageData] = tiffReader('uint8',[],[],[],'d:\Users\Eric.Bioimage29\Documents\Images\LEVer3d\Susan_overnight_new\Susan_overnight.txt');

lever_3d('init',arrowFaces, arrowVerts, arrowNorms,sphereFaces, sphereVerts, shereNorms);

lever_3d('loadTexture',im,[imageData.XPixelPhysicalSize,imageData.YPixelPhysicalSize,imageData.ZPixelPhysicalSize]);

ViewerControls();

end
