function isOpen = IsOpen()
%IsOpen Checks if the D3d Viewer is currently open.
%
%   isOpen = D3d.IsOpen()
%
%   Returns:
%       isOpen: Boolean true if the viewer is initialized and open, false otherwise.
    global D3dIsOpen

    isOpen = false;
    if (~isempty(D3dIsOpen))
        isOpen = D3dIsOpen;
    end
end
