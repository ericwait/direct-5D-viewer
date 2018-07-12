function AutoTransferFunction(im)
    imC = ImUtils.ConvertType(im(:,:,:,:,1),'uint8',true);
    lb = zeros(1,size(im,4));
    ub = zeros(1,size(im,4));
    
    for c=1:size(im,4)
        curIm = imC(:,:,:,c,:);
        if (nnz(curIm(:))==0)
            lb(c) = 0;
            ub(c) = 255;
            continue
        end
        N = histcounts(curIm(curIm>0),255);
        
        forwardSum = cumsum(N);
        lowerBound = forwardSum > sum(N)*0.1;
        l = find(lowerBound,1,'first');
        if (isempty(l))
            lb(c) = 0;
        else
            lb(c) = l;
        end
        
        reverseSum = cumsum(N,'reverse');
        upperBound = reverseSum < sum(N)*0.005;
        u = find(upperBound,1,'first');
        if (isempty(u))
            ub(c) = 255;
        else
            ub(c) = u;
        end
    end

    [imageData, ucolors, channelData] = D3d.UI.Ctrl.GetUserData();
    
    for c=1:size(im,4)
        channelData(c).minVal = lb(c)/255;
        channelData(c).maxVal = ub(c)/255;
        channelData(c).alphaMod = 0.5;
    end
    D3d.UI.Ctrl.SetUserData(imageData,ucolors,channelData);
    D3d.UI.Ctrl.UpdateCurrentState();
end
