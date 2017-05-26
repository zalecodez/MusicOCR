// find circles using hough transform
void houghC(CByteImage edges, CFloatImage theta, CFloatImage mag, CFloatImage &accum, int rad);

// For all local maxima in accum that are >= mincnt,
// draw the corresponding circle in outimg
// return number of circles found
int findMaxC(CFloatImage accum, CByteImage outimg, int mincnt, int rad);
