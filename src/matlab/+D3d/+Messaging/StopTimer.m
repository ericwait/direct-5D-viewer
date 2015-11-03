function StopTimer()
global d3dtimer

if (~isempty(d3dtimer) && isvalid(d3dtimer))
    stop(d3dtimer);
else
    d3dtimer = fimerfindall('Name','d3dtimer');
    if (~isempty(d3dtimer) && isvalid(d3dtimer))
        stop(d3dtimer);
    else
        error('Cannot find d3dtimer!');
    end
end
end

