function lever3d()
global imageData orgImage processedImage

[arrowFaces, arrowVerts, arrowNorms] = MakeArrow(0.65,0.05,0.15,40);
[sphereFaces, sphereVerts, shereNorms] = MakeSphere(0.20,40);

[orgImage, imageData] = tiffReader([],[],[],[],'d:\Users\Eric.Bioimage29\Documents\Images\LEVer3d\Susan_overnight_new');

processedImage = orgImage;

lever_3d('init',arrowFaces, arrowVerts, arrowNorms,sphereFaces, sphereVerts, shereNorms);

lever_3d('loadTexture',uint8(orgImage),[imageData.XPixelPhysicalSize,imageData.YPixelPhysicalSize,imageData.ZPixelPhysicalSize]);

ViewerControls();

end
