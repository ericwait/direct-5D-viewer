function SpinSingleFrame( capturePath, filePrefix )
%SPINSINGLEFRAME Summary of this function goes here
%   Detailed explanation goes here

texTime = tic;
fprintf('Capturing %s...',filePrefix)
% Texture capture
%D3d.Viewer.SetBackgroundColor([0,0,0]);
D3d.Viewer.SetCapturePath(capturePath,filePrefix);
if (exist(capturePath,'dir'))
    rmdir(capturePath,'s');
end
mkdir(capturePath);
for t=1:720
    D3d.Viewer.CaptureWindow();
    D3d.Viewer.SetRotation(0,0.5,0);
end
dList = dir(fullfile(capturePath,[filePrefix,'*.bmp']));
while (length(dList)<720)
    pause(5);
    dList = dir(fullfile(capturePath,[filePrefix,'*.bmp']));
end
fprintf('%s\n',Utils.PrintTime(toc(texTime)))
end
