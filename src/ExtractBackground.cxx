#include "ExtractBackground.h"

ImagePointer ExtractSegmentatedImage(ImagePointer img, ImagePointer img_seg) {
	ImagePointer m = BinaryMathOperation(MULTIPLY, img, img_seg);
	return m;
}

ImagePointer ExtractBackgroung(ImagePointer img, ImagePointer img_seg) {
	ImagePointer m = ExtractSegmentatedImage(img, img_seg);
	ImagePointer bg = Thresh(m,-vnl_huge_val(0.0),-0.001, 1, 0);
	return bg;
}

