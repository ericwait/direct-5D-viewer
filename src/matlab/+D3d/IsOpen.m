function isOpen = IsOpen()
    global D3dIsOpen

    isOpen = false;
    if (~isempty(D3dIsOpen))
        isOpen = D3dIsOpen;
    end
end
