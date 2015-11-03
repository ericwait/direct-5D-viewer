function StartTimer()
global d3dtimer

if (isempty(d3dtimer) || ~isvalid(d3dtimer))
    d3dtimer = timer('TimerFcn',@D3d.Messaging.Check,'ExecutionMode','fixedSpacing','Period',0.1,'Name','d3dtimer');
end

if (~strcmpi(get(d3dtimer,'Running'),'on'))
    start(d3dtimer);
end
end

