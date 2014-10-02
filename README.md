*******************************************************************************
*  Copyright 2014 Andrew Cohen, Eric Wait, and Mark Winter
*  This file is part of LEVER 3-D - the tool for 5-D stem cell segmentation,
*  tracking, and lineaging. See http://bioimage.coe.drexel.edu 'software'
*  section for details. LEVER 3-D is free software: you can redistribute it
*  and/or modify it under the terms of the GNU General Public License as
*  published by the Free Software Foundation, either version 3 of the License,
*  or (at your option) any later version.
*  LEVER 3-D is distributed in the hope that it will be useful, but WITHOUT ANY
*  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
*  FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
*  details.
*  You should have received a copy of the GNU General Public License along with
*  LEVer in file "gnu gpl v3.txt".  If not, see 
*  <http://www.gnu.org/licenses/>.
*******************************************************************************

NVIDIA CUDA toolkit EULA can be found at [CUDA EULA](http://docs.nvidia.com/cuda/eula/index.html#axzz3EoF66FNY)

To run LEVER 3-D executable you will need the following:

MATLAB's runtime called MCR [version 8.3 (R2014a)](http://www.mathworks.com/products/compiler/mcr/index.html)

Visual C++ Redistributable for [Visual Studio 2013](http://www.microsoft.com/en-us/download/details.aspx?id=40784)

DirectX 11 runtime (for Windows 7, no need on Windows 8) and capable device [DirectX 11](http://www.microsoft.com/en-
us/download/details.aspx?id=35)

To process images you will need:
A CUDA compute 3.5 capable device or better

Once all of the above prerequisites have been met, run the LEVER3D.exe from the same directory as:
`DefaultMeshShaders.fx`
`ViewAlignedVertexShader.fx`

The program will open and ask for a metadata file.  Please see example dataset [here](http://bioimage.coe.drexel.edu/info/?page_id=620)

The metadata needs to be in the same directory as the tiff files.  The tiff
files need to be named such that the following sprintf line will find each file
`sprintf('%s_c%02d_t%04d_z%04d.tif',datasetName,channel,frame,zSlice)`

Please see 'Additional Files 3' of BMC paper for a video tutorial of the user
interface.
