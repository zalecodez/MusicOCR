/* edges.cpp
 *
 * compute intensity gradients and Canny edges
 */

#include "imageLib.h"
#include <math.h>

#define ABS(a) ((a) < 0 ? (-(a)) : (a))

// compute partial derivatives Ix and Iy, as well as gradient
// magnitude mag and gradient direction theta
void gradients(CFloatImage img, CFloatImage &Ix, CFloatImage &Iy,
	       CFloatImage &mag, CFloatImage &theta)
{
    // use masks [-0.5  0  0.5] to compute gradients
    // don't worry about the pixels on the borders

    CShape sh = img.Shape();
    if (sh.nBands != 1)
	throw CError("only 1-band float images supported");
    Ix.ReAllocate(sh);
    Iy.ReAllocate(sh);
    mag.ReAllocate(sh);
    theta.ReAllocate(sh);
    int w = sh.width, h = sh.height;
  
    for (int y = 1; y < h-1; y++) {
	for (int x = 1; x < w-1; x++) {
	    float xx = (img.Pixel(x+1, y, 0) - img.Pixel(x-1, y, 0)) / 2.0;
	    float yy = (img.Pixel(x, y+1, 0) - img.Pixel(x, y-1, 0)) / 2.0;
	    Ix.Pixel(x, y, 0) = xx;
	    Iy.Pixel(x, y, 0) = yy;
	    mag.Pixel(x, y, 0) = sqrt(xx*xx + yy*yy);
	    theta.Pixel(x, y, 0) = atan2(yy, xx);
	}
    }
}


/* Canny edge detector *****************************************/


// do non-maximum supression to find all pixel that are on "ridges" of
// the gradient magnitude
void NMS(CFloatImage Ix, CFloatImage Iy, CFloatImage mag, CByteImage &edges)
{
    // At each pixel, compute direction of the gradient and determine
    // which of the four cases of neighboring pixels to consider:  -  \  |  /
    // Then, if the current pixel is greater or equal than those two
    // neighboring pixels, set the pixel in "edges" to 0 (edge), otherwise
    // to 255 (no edge).

    CShape sh = Ix.Shape();
    int w = sh.width, h = sh.height;
    int x, y;
    edges.ReAllocate(sh);
    float a1 = sqrt(2)-1;  // arctan(45/2) 
    float a2 = sqrt(2)+1;  // arctan(90+45/2) 
  
    for (y = 1; y < h-1; y++) {
	for (x = 1; x < w-1; x++) {
	    float gx = Ix.Pixel(x, y, 0);
	    float gy = Iy.Pixel(x, y, 0);
	    float m = mag.Pixel(x, y, 0);
	    float a = (gx == 0.0 ? 100.0 : gy / gx);
	    int px, py;
	    if (1) { // 4 directions
		if (ABS(a) > a2)  { px=0; py=1;  }  // dir: '|'  
		else if (a > a1)  { px=1; py=1;  }  // dir: '/'  
		else if (a < -a1) { px=1; py=-1; }  // dir: '\'  
		else              { px=1; py=0;  }  // dir: '-'  
	    } else { // 2 directions
		if (ABS(gy) > ABS(gx))  { px=0; py=1;  }  // dir: '|'  
		else                    { px=1; py=0;  }  // dir: '-'  
	    }
	    edges.Pixel(x, y, 0) = 
		(   mag.Pixel(x+px, y+py, 0) < m
		 && mag.Pixel(x-px, y-py, 0) <= m) ? 0 : 255;
	}
    }
    
    // set all pixels on the borders to "255":
    for (x = 0; x < w; x++)
	edges.Pixel(x, 0, 0) = edges.Pixel(x, h-1, 0) = 255;
    for (y = 0; y < h; y++)
	edges.Pixel(0, y, 0) = edges.Pixel(w-1, y, 0) = 255;
}

// recursive helper function to follow ridges while changing
// weak edges (1) to strong edges (0)
void follow(CByteImage edges, int x, int y) {
    if (edges.Pixel(x, y, 0) != 1)
	return;
    edges.Pixel(x, y, 0) = 0;
    follow(edges, x+1, y);
    follow(edges, x-1, y);
    follow(edges, x, y+1);
    follow(edges, x, y-1);
    follow(edges, x+1, y+1);
    follow(edges, x+1, y-1);
    follow(edges, x-1, y+1);
    follow(edges, x-1, y-1);
}


// do thresholding with hysteresis:  only keep those pixels in "edges"
// for which "mag" is greater than "hi", and also those whose whose
// "mag" is greater than "lo" and that have a neighboring edge pixel.
void threshold(CByteImage edges, CFloatImage mag, float lo, float hi)
{
    CShape sh = edges.Shape();
    int w = sh.width, h = sh.height;
    int x, y;

    // in the first pass, we'll label pixels "0" if they are greater
    // than hi, "1", if they are between hi and lo, and "255" if they
    // are less than lo.
    
    for (y = 1; y < h-1; y++) {
	for (x = 1; x < w-1; x++) {
	    float m = mag.Pixel(x, y, 0);
	    if (edges.Pixel(x, y, 0) == 0) // edge pixel
		edges.Pixel(x, y, 0) = 
		    (m >= hi ? 0 : (m >= lo ? 1 : 255));
	}
    }

    // now, we'll do a pass where we recursively set all "1" (weak)
    // edge pixels to "0" (strong) if they have a "0" pixel in their
    // 8-neighborhood.  This is very similar to connected component
    // finding, but easier, since we're not labeling the components.
    // Also, assuming that all pixels on the borders are "255", we
    // don't have to do any boundary checks in our line-following
    // code.

    for (y = 1; y < h-1; y++) {
	for (x = 1; x < w-1; x++) {
	    if (edges.Pixel(x, y, 0) == 0) {
		edges.Pixel(x, y, 0) = 1; // pretend it's weak and follow it
		follow(edges, x, y);
	    }
	}
    }
  
    // Finally, we'll delete all "1" pixels that are left (set them to 255)
    for (y = 1; y < h-1; y++) {
	for (x = 1; x < w-1; x++) {
	    if (edges.Pixel(x, y, 0) == 1)
		edges.Pixel(x, y, 0) = 255;
	}
    }
}
