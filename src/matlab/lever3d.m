function lever3d()
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here

[arrowFaces, arrowVerts, arrowNorms] = MakeArrow(0.65,0.05,0.15,40);
[sphereFaces, sphereVerts, shereNorms] = MakeSphere(0.20,40);

[im, imageData] = tiffReader('uint8');

lever_3d('init',arrowFaces, arrowVerts, arrowNorms,sphereFaces, sphereVerts, shereNorms);

lever_3d('loadTexture',im);

%clear mex

end

