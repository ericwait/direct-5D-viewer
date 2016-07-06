% ShowTexture - This selects which of the two buffers to display.
%    Viewer.ShowTexture(bufferType)
%    	BufferType - there are two buffers available to be displayed, 'original' and 'processed'.
%    		Original - corresponds to the first buffer and can be displayed if an image has been loaded.
%    		Processed - corresponds to the second buffer and can be displayed if an image has been loaded.
function ShowTexture(bufferType)
    D3d.Viewer.Mex('ShowTexture',bufferType);
end
