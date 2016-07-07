% SetCapturePath - This will set the path and file name prefix for all captured frames.
%    Viewer.SetCapturePath(filePath,filePrefix)
%    	FilePath - This is the path were the bmp frame captures will be saved.
%    	FilePrefix - This is the prefix to the files that will be saved to the FilePath.
%    		Files will be saved with the name prefix%04d.bmp, where the numbering starts at one and increases until the program is closed.
function SetCapturePath(filePath,filePrefix)
    D3d.Viewer.Mex('SetCapturePath',filePath,filePrefix);
end
