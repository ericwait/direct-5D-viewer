% TransferFunction - these are the values for the transfer function.  The transfer function is the quadratic mapping of input intensities to output (viewed) intensities per channel and buffer.
%    Viewer.TransferFunction(TransferFunctionStruct,BufferType)
%    	TransferFunctionStruct -- this is a channel number sized array of structures with the fields: color, a, b, c, minVal, maxVal, aphlaMod, visible.
%    		Color is a array of three values (r,g,b) between [0,1] that represent the color of a channel.
%    		A, B, & C represent the quadratic equation that the transfer function will apply (a*x^2 + b*x + c).
%    		Min and Max values are the clipping thresholds that are used to ensure that the quadratic equation is monotonically increasing between them.
%    		AlphaMod is the alpha multiplier to change the transparency of the channel.
%    		Visible is a logical flag that represents whether or not a channel is visible.
%    	BufferType is the buffer that you would like to apply these setting to.  The possible values are 'original' and 'processed'. The correspond to the first and second texture buffers.
function TransferFunction(TransferFunctionStruct,BufferType)
    D3d.Viewer.Mex('TransferFunction',TransferFunctionStruct,BufferType);
end
