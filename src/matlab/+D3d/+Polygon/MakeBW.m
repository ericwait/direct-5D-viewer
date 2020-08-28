function polygons = MakeBW(imBW, colors, minVol, verbose)
    if (~exist('colors','var') || isempty(colors))
        colors = hsv(size(imBW,4));
    end
    if (~exist('minVol','var') || isempty(minVol))
        minVol = 3;
    end
    if (~exist('verbose','var') || isempty(verbose))
        verbose = true;
    end

    polygons = D3d.Polygon.MakeEmptyStruct();
    frameSize = size(imBW(:,:,:,1,1));
    
    if (verbose)
        prgs = Utils.CmdlnProgress(size(imBW,5),true,'Making Polygons',false);
    end
    
    for t=1:size(imBW,5)
        for c=1:size(imBW,4)
            cc = bwconncomp(imBW(:,:,:,c,t));
            
            for ccind = 1:length(cc.PixelIdxList)
                if (length(cc.PixelIdxList{ccind})<minVol)
                    continue;
                end
                
                pixList_xyz = Utils.SwapXY_RC(Utils.IndToCoord(frameSize,cc.PixelIdxList{ccind}));
                ind = length(polygons) +1;
                ply = D3d.Polygon.Make(pixList_xyz,ind,num2str(ind),t,colors(c,:));
                
                if (isempty(ply))
                    continue
                end
                
                polygons = vertcat(polygons,ply);
            end
        end
        
        if (verbose)
            prgs.PrintProgress(t);
        end
    end
    
    if (verbose)
        prgs.ClearProgress(true);
    end
end
