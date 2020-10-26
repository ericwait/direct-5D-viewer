function polygons = MakeBW(imBW, colors, minVol, maxVol, verbose)
    if (~exist('colors','var') || isempty(colors))
        colors = hsv(size(imBW,4));
    end
    if (~exist('minVol','var') || isempty(minVol))
        minVol = 3;
    end
    if (~exist('maxVol','var') || isempty(maxVol))
        maxVol = inf;
    end
    if (~exist('verbose','var') || isempty(verbose))
        verbose = true;
    end

    polycell = cell(size(imBW,5),1);
    frameSize = size(imBW(:,:,:,1,1));
    
    if (verbose)
        prgs = Utils.CmdlnProgress(size(imBW,5),true,'Making Polygons',false);
    end
    
    parfor t=1:size(imBW,5)
        curPolys = D3d.Polygon.MakeEmptyStruct();
        for c=1:size(imBW,4)
            cc = bwconncomp(imBW(:,:,:,c,t));
            
            for ccind = 1:length(cc.PixelIdxList)
                if (length(cc.PixelIdxList{ccind})<minVol || length(cc.PixelIdxList{ccind})>maxVol)
                    continue;
                end
                
                pixList_xyz = Utils.SwapXY_RC(Utils.IndToCoord(frameSize,cc.PixelIdxList{ccind}));
                ind = length(curPolys) +1;
                ply = D3d.Polygon.Make(pixList_xyz,ind,num2str(ind),t,colors(c,:));
                
                if (isempty(ply))
                    continue
                end
                
                curPolys = vertcat(curPolys,ply);
            end
        end
        
        polycell{t} = curPolys;
        
        if (verbose)
            prgs.PrintProgress(t);
        end
    end
    
    polygons = D3d.Polygon.MakeEmptyStruct();
    
    for t=1:size(imBW,5)
        curPolys = polycell{t};
        for i=1:length(curPolys)
            cp = curPolys(i);
            cp.index = length(polygons)+1;
            cp.label = num2str(cp.index);
            polygons(cp.index) = cp;
        end
    end
    
    if (verbose)
        prgs.ClearProgress(true);
    end
end
