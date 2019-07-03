classdef (Abstract,Sealed) Viewer
methods (Static)
    commandInfo = Info()
    Help(command)
    AddPolygons(polygonsStruct)
    CaptureSpinMovie()
    ImageOut = CaptureWindow()
    ClearAllTextures(BufferType)
    ClearTextureFrame(Frame,BufferType)
    Close()
    DeleteAllPolygons()
    Init(pathStr)
    InitVolume(ImageDims,PhysicalUnits)
    LoadTexture(Image,BufferType)
    LoadTextureFrame(Image,Frame,BufferType)
    MoveCamera(deltas)
    Play(playOn)
    MessageArray = Poll()
    ReleaseControl()
    RemovePolygon(index)
    ResetView()
    PolygonLighting(lightOn)
    SetBackgroundColor(color)
    SetBorderColor(color)
    SetCapturePath(filePath,filePrefix)
    SetCaptureSize(width,height)
    SetDpiScale(scalePct)
    SetFrame(frame)
    SetFrontClip(FrontClipDistance)
    SetViewOrigin(viewOrigin)
    SetViewRotation(rotationVector_xyz,deltaAngle)
    SetWindowSize(width,height)
    SetWorldRotation(rotationVector_xyz,deltaAngle)
    Spin(on)
    ShowAllPolygons(on)
    ShowFrameNumber(on)
    ShowLabels(on)
    ShowPolygonList(polygonIndices)
    ShowTexture(bufferType)
    ShowScaleBar(on)
    ShowWidget(on)
    TakeControl()
    TextureAttenuation(on)
    TextureLighting(lightOn)
    TransferFunction(TransferFunctionStruct,BufferType)
    ToggleWireframe(wireFrameOn)
    UpdateRender()
end
methods (Static, Access = private)
    varargout = Mex(command, varargin)
end
end
