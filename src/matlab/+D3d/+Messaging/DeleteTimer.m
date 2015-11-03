function DeleteTimer()
global d3dtimer

if (~isempty(d3dtimer) && isvalid(d3dtimer))
    stop(d3dtimer);
    delete(d3dtimer);
else
    t = timerfindall('Name','d3dtimer');
    if (~isempty(t) && isvalid(t))
        delete(t);
    else
        warning('Cannot find d3dtimer!');
    end
end

d3dtimer = [];
end

