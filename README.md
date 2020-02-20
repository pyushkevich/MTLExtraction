# Welcome!

- [Presentation](#mtlextraction)
- [Install](#install)
- [How to use the algorithm](#how-to-use-the-algorithm)
  * [Automatic version](#automatic-version)
  * [Interactive version](#interactive-version)

## MTLExtraction
This algorithm creates models of 3D molds with slits guiding the cutting to extract the medial temporal lobe of an hemisphere. The first 3D printed mold is designed to hold the whole hemisphere, guiding cuts orthogonal to the midsagittal plane. The second mold is designed to hold the extracted tissue block, guiding subsequent longitudinal cuts.
Cuts can be manually positioned or automatically using Powell's method. In the latter case, the algorithm finds 6 cuts to trim the tissue to fit into a 50mm cylindrical holder.

## Install
```sh
$ git clone https://github.com/JLasserv/MTLExtraction.git
$ make
```

## How to use the algorithm

### Automatic version
*INPUTS:* 
Path to a folder countaining:
 * 7T MRI scan of a formalin-fixed hemisphere named **hemisphere.nii.gz**
 * segmentation of the hemisphere named **hemisphere_seg.nii.gz**
 * segmentation of the MTL ROI named **mtl_seg.nii.gz**
 * (optional) segmentation of the second ROI to be spared during cutting (e.g. frontal lobe) named **roi_seg.nii.gz**
  
### Interactive version

