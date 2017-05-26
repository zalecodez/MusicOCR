/* smooth.cpp
 *
 * Gaussian smoothing and box filtering
 *
 */

#include "imageLib.h"
#include <math.h>
#include <vector>

#define VERBOSE 0

// Macros for "mirroring" out-of-bound indices
#define MirL(x)   ((x)<0    ? -(x)            : (x))
#define MirR(x,w) ((x)>=(w) ? ((w)+(w)-(x)-1) : (x))

// Macros for absolute value and rounding
#define ABS(a) ((a) < 0 ? (-(a)) : (a))
#define ROUND(a) ((int) ((a) > 0 ? ((a) + 0.5) : ((a) - 0.5)))


// x-convolve im1 with mask into im2.  mask is assumed to be symmetric
void convolveX(CFloatImage im1, CFloatImage &im2, float *mask, int masksize)
{
    CShape sh = im1.Shape();
    if (sh.nBands != 1)
	throw CError("only gray images supported");
    im2.ReAllocate(sh);
    int w = sh.width, h = sh.height;
  
    for (int y = 0; y < h; y++) {
	for (int x = 0; x < w; x++) {
	    float sum = mask[0] * im1.Pixel(x, y, 0);
	    for (int k = 1; k <= masksize; k++)
		sum += mask[k] * (im1.Pixel(MirL(x-k), y, 0) +
				  im1.Pixel(MirR(x+k, w), y, 0));
	    im2.Pixel(x, y, 0) = sum;
	}
    }
}

// y-convolve im1 with mask into im2.  mask is assumed to be symmetric
void convolveY(CFloatImage im1, CFloatImage &im2, float *mask, int masksize)
{
    CShape sh = im1.Shape();
    if (sh.nBands != 1)
	throw CError("only gray images supported");
    im2.ReAllocate(sh);
    int w = sh.width, h = sh.height;
  
    for (int y = 0; y < h; y++) {
	for (int x = 0; x < w; x++) {
	    float sum = mask[0] * im1.Pixel(x, y, 0);
	    for (int k = 1; k <= masksize; k++)
		sum += mask[k] * (im1.Pixel(x, MirL(y-k), 0) +
				  im1.Pixel(x, MirR(y+k, h), 0));
	    im2.Pixel(x, y, 0) = sum;
	}
    }
}

// smooth im1, using Gaussian if box==0, boxfilter if box==1
void smooth(CFloatImage im1, float sigma, int box)
{
    // allocate mask (positive half only)
    int masksize = ROUND(box ? (sigma-1)/2 : 4*sigma);
    std::vector<float> mask;
    mask.resize(masksize+1);
    
    if (masksize == 0)
	mask[0] = 1.0;
    else {
	for (int x = 0; x <= masksize; x++)
	    mask[x] = (box ? 1.0 / (2*masksize + 1)
		       : exp(- x*x/(2*sigma*sigma)) / (sqrt(2*M_PI)*sigma));
    }
  
    if (VERBOSE) {
	float sum = 0.0;
	for (int x=-masksize; x<=masksize; x++)
	    sum += mask[ABS(x)];
	fprintf(stderr, "%s: sigma = %g, masksize = %d\n",
		box? "Box filter" : "Gaussian Smoothing", sigma, 2*masksize+1);
	fprintf(stderr, "sum of mask values = %g\n", sum);
    }
	
    CFloatImage im2;

    convolveX(im1, im2, &mask[0], masksize);
    convolveY(im2, im1, &mask[0], masksize);
}
