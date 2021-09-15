function movieFrames = Pause(numPauseFrames)    
    D3d.Update();
    movieFrames = D3d.Viewer.CaptureWindow();
    
    movieFrames = repmat(movieFrames,1,1,1,numPauseFrames);
    
end