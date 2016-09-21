% CaptureWindow - This will capture the current view to either the current directory or the directory specified in 'SetCapturePath'.
%    ImageOut = Viewer.CaptureWindow()
%    	ImageOut -- if an output is specified, the capture will be returned instead of written to the file system.
function ImageOut = CaptureWindow()
    [ImageOut] = D3d.Viewer.Mex('CaptureWindow');
end
