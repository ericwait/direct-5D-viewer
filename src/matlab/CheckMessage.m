function CheckMessage()
global uiHandle
%CHECKMESSAGE Summary of this function goes here
%   Detailed explanation goes here
% persistent cnt;
% mlock
% 
% if (isempty(cnt))
%     cnt = 1;
% end
% 
% disp(cnt)
% cnt = cnt + 1;
% 
% if (cnt>100)
%     munlock
% end

[command, val] = lever_3d('poll');

switch command
    case 'null'
        return
    case 'close'
        close(uiHandle)
end

%disp(command);

%disp(val);

end

