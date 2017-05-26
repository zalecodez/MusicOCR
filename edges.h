// compute partial derivatives Ix and Iy, as well as gradient
// magnitude mag and gradient direction theta
void gradients(CFloatImage img, CFloatImage &Ix, CFloatImage &Iy,
	       CFloatImage &mag, CFloatImage &theta);

    
// do non-maximum supression to find all pixel that are on "ridges" of
// the gradient magnitude
void NMS(CFloatImage Ix, CFloatImage Iy, CFloatImage mag, CByteImage &edges);


// do thresholding with hysteresis:  only keep those pixels in "edges"
// for which "mag" is greater than "hi", and also those whose whose
// "mag" is greater than "lo" and that have a neighboring edge pixel.
void threshold(CByteImage edges, CFloatImage mag, float lo, float hi);
