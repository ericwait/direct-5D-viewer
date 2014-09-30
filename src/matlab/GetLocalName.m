function localName = GetLocalName(fullName, chkPath)
    if ( ~exist('chkPath', 'var') )
        chkPath = pwd();
    end

    localName = [];
    partialPath = fullName;
    while ( ~isempty(partialPath) && ~strcmpi(chkPath,partialPath) )
        [partialPath callPart] = fileparts(partialPath);
        
        if ( callPart(1) == '+' )
            localName = [callPart(2:end) '.' localName];
        else
            localName = fullfile(callPart, localName);
        end
    end
end