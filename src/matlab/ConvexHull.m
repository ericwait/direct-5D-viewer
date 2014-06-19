function chIdx = ConvexHull(x,y)
    chIdx = [];
    
    try
        chIdx = convhull(x,y);
    catch me
        if ( strcmp(me.identifier,'MATLAB:convhull:NotEnoughPtsConvhullErrId') )
            chIdx = 1:length(x);
        elseif ( strcmp(me.identifier,'MATLAB:convhull:EmptyConvhull2DErrId') )
            chIdx = 1:length(x);
        else
            rethrow(me);
        end
    end
end

