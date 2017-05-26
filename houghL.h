#include "liness.h"

// find lines using hough transform
void houghL(CByteImage edges, CFloatImage theta, CFloatImage mag, CFloatImage &accum);

// For all local maxima in accum that are >= mincnt,
// draw the corresponding line in outimg
// return number of lines found
int findMaxL(CFloatImage accum, CByteImage outimg, int mincnt, Line[]);

int getslope(CByteImage edges, CFloatImage theta);
