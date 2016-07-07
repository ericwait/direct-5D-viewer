% SetFrame - This will set the currently viewed frame to the one specified.
%    Viewer.SetFrame(frame)
%    	Frame - This is the frame that should be displayed. The first frame starts at 1.
function SetFrame(frame)
    D3d.Viewer.Mex('SetFrame',frame);
end
