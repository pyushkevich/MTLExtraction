# Welcome!
This is an algorithm to help extracting the medial temporal lobe (MTL) of an hemisphere.

- [Presentation](#mtlextraction)
- [Install](#install)
- [How to use the algorithm](#how-to-use-the-algorithm)
  * [Automatic version](#automatic-version)
  * [Interactive version](#interactive-version)

## MTLExtraction
This algorithm creates models of 3D molds with slits guiding the cutting to extract the MTL. The first 3D printed mold is designed to hold the whole hemisphere, guiding cuts orthogonal to the midsagittal plane. The second mold is designed to hold the extracted tissue block, guiding subsequent longitudinal cuts.
Cuts can be manually positioned or automatically using Powell's method. In the latter case, the algorithm finds 6 cuts to trim the tissue to fit into a 50mm cylindrical holder.

## Install
With cmake on **Unix**:
```sh
$ git clone https://github.com/JLasserv/MTLExtraction.git
$ cd MTLExtraction
$ mkdir Build
$ cd Build
$ cmake ..
$ make
```

With Visual Studio on **Windows**:
```sh
$ git clone https://github.com/JLasserv/MTLExtraction.git
```
* Launch the CMake GUI
* Next to "Where is the source code:", click "Browse Source..." and navigate to where you cloned the repository with Git.
* Next to "Where to build the binaries:", select "Browse Build..." and select a place to build.
* Click "Configure", and then specify "Visual Studio" as the generator and "x64" as the platform for this project.
* Click "Generate".
* Click "Open Project". This will start a new Visual Studio instance.
* From the Build menu, choose "Build Solution" (or "Build ALL_BUILD")


## How to use the algorithm

### Automatic version
*__INPUT:__* 
Path to a folder containing:
 * 7T MRI scan of a formalin-fixed hemisphere named **hemisphere.nii.gz**
 * segmentation of the hemisphere named **hemisphere_seg.nii.gz**
 * segmentation of the MTL ROI named **mtl_seg.nii.gz**
 * (optional) segmentation of the second ROI to be spared during cutting (e.g. frontal lobe) named **roi_seg.nii.gz**

```sh
$ MTLExtraction path/to/files
```
*__OUTPUTS:__* 
Folder containing:
 * Two files for each mold, depending on whether it is a right or left hemisphere **slitmold_hem*i*.nii.gz**, **slitmold_mtl*i*.nii.gz** 
 * files of the cuts **cut*i*.nii.gz**
 * files of the reoriented segmentations **Oriented*Part*.nii.gz**
 * resulting tissue after using the first mold **MTL.nii.gz**
 * resulting tissue after using the two molds **finalMTL.nii.gz**
 
### Interactive version
*__INPUT:__* 
Path to a folder containing:
 * 7T MRI scan of a formalin-fixed hemisphere named **hemisphere.nii.gz**
 * segmentation of the hemisphere named **hemisphere_seg.nii.gz**
 * transformation matrices to orient the plan named **mold*i*_*k*.mat** or **mold*i*_*k*.txt** *(i take the value (1 or 2) corresponding to the mold on which they should appear; k can take any value)*

To determine the orientation of the cuts with ITK-Snap, run a first time the program:
```sh
$ MTLExtraction path/to/files manual
```
This step will create a new folder and an image **plan.nii.gz** representing the cut to orient. After saving the transformation matrices in the original folder (containing files of the hemisphere), run the program a second time to obtain the molds:
```sh
$ MTLExtraction path/to/files manual
```

*__OUTPUTS:__*
Folder containing:
 * Two files for each mold, depending on whether it is a right or left hemisphere **slitmold_hem*i*.nii.gz**, **slitmold_mtl*i*.nii.gz** 
 * files of the cuts **cut*i*.nii.gz**
 * resulting tissue after using the first mold **MTL.nii.gz**
 * resulting tissue after using the two molds **finalMTL.nii.gz**
