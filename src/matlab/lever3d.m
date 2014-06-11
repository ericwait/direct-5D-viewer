function lever3d()
global imageData orgImage processedImage

[arrowFaces, arrowVerts, arrowNorms] = MakeArrow(0.65,0.05,0.15,40);
[sphereFaces, sphereVerts, shereNorms] = MakeSphere(0.20,40);

[orgImage, imageData] = tiffReader();

w = whos('orgImage');
if (~strcmpi('uint8',w.class))
    processedImage = uint8(orgImage);
else
    processedImage = orgImage;
end

lever_3d('init',arrowFaces, arrowVerts, arrowNorms,sphereFaces, sphereVerts, shereNorms);

lever_3d('loadTexture',processedImage,[imageData.XPixelPhysicalSize,imageData.YPixelPhysicalSize,imageData.ZPixelPhysicalSize]);

ViewerControls();

end
