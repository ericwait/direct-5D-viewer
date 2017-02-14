function AutoTransferFunction(im)
    imC = ImUtils.ConvertType(im(:,:,:,:,1),'uint8',true);
    lb = zeros(1,size(im,4));
    ub = zeros(1,size(im,4));
    
    for c=1:size(im,4)
        curIm = imC(:,:,:,c,:);
        N = histcounts(curIm(curIm>0),255);
        
        forwardSum = cumsum(N);
        lowerBound = forwardSum > sum(N)*0.05;
        lb(c) = find(lowerBound,1,'first');
        
        reverseSum = cumsum(N,'reverse');
        upperBound = reverseSum < sum(N)*0.005;
        ub(c) = find(upperBound,1,'first');
    end

    [imageData, ucolors, channelData] = D3d.UI.Ctrl.GetUserData();
    
    for c=1:size(im,4)
        channelData(c).minVal = lb(c)/255;
        channelData(c).maxVal = ub(c)/255;
        channelData(c).alphaMod = 0.1;
    end
    D3d.UI.Ctrl.SetUserData(imageData,ucolors,channelData);
    D3d.UI.Ctrl.UpdateCurrentState();
end
