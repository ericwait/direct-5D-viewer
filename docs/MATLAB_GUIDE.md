# Matlab Usage Guide for Direct 5D Viewer

This guide provides an overview of how to control the Direct 5D Viewer using the Matlab interface (`+D3d` package).

## 1. Getting Started

Ensure that the `src/matlab` directory is in your Matlab path.

```matlab
addpath('path/to/direct-5D-viewer/src/matlab');
```

## 2. Opening the Viewer

The primary entry point is the `D3d.Open` function. It initializes the viewer window and loads your data.

### Basic Usage (Image Only)
If you have a 3D, 4D, or 5D image array loaded in Matlab, you can open it directly. The viewer will assume default metadata (isotropic voxels).

```matlab
% Create a 5D random noise image (Y, X, Z, Channel, Time)
im = rand(256, 256, 50, 2, 10, 'uint8') * 255;

% Open the viewer
D3d.Open(im);
```

### Advanced Usage (Image + Metadata)
For correct physical scaling (e.g., microns/pixel) and channel names, you should provide a metadata structure. This is often compatible with the format returned by `MicroscopeData` utilities.

```matlab
% Define metadata
imData.Dimensions = [256, 256, 50];       % [X, Y, Z]
imData.NumberOfChannels = 2;
imData.NumberOfFrames = 10;
imData.PixelPhysicalSize = [0.5, 0.5, 1.0]; % [X, Y, Z] (microns)
imData.DatasetName = 'MySample';

% Load image
im = ...; % load your 5D image here

% Open with specific metadata
D3d.Open(im, imData);
```

## 3. Controlling the Viewer via Scripts

Once the viewer is open, you can control it programmatically.

### Updating the Image
You can replace the current image or update specific frames using `D3d.LoadImage`.

```matlab
newFrame = rand(256, 256, 50, 2, 'uint8') * 255;

% Load into buffer 1, for time point 5
D3d.LoadImage(newFrame, 1, 5);

% Trigger a re-render
D3d.Update();
```

### Checking Status
To avoid errors, check if the viewer is open before running commands:

```matlab
if D3d.IsOpen()
    disp('Viewer is running!');
else
    D3d.Open(im);
end
```

## 4. Transfer Functions (Color & Opacity)

The viewer uses Transfer Functions to map pixel intensities to colors and opacity.

-   **Save**: `D3d.SaveTransferFunction('MySettings.json')`
-   **Load**: `D3d.LoadTransferFunction('MySettings.json')`

This allows you to save your visualization settings (red channel brightness, blue channel opacity, etc.) and apply them to other datasets.

## 5. Closing the Viewer

To properly release resources (GPU handles, timers), use the close command:

```matlab
D3d.Close();
```

## 6. Full API Reference

Most functionality is exposed through the static methods in the `+D3d` folder.
-   `D3d.Open`: Initialize and load.
-   `D3d.Close`: Shutdown.
-   `D3d.Update`: Force a frame redraw.
-   `D3d.LoadImage`: Send new pixel data to the GPU.
-   `D3d.Viewer.*`: Direct access to the low-level Mex commands (e.g., `D3d.Viewer.Init`, `D3d.Viewer.SetWindowSize`).
