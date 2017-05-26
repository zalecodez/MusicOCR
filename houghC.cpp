/* houghC.cpp
 *
 * Hough transform for finding circles
 */

#include "imageLib.h"
#include <math.h>

#define ROUND(a) ((int) ((a) > 0 ? ((a) + 0.5) : ((a) - 0.5)))

// find circles of given radius using Hough transform
void houghC(CByteImage edges, CFloatImage theta, CFloatImage mag, CFloatImage &accum, int rad)
{
    CShape sh = edges.Shape();
    int w = sh.width, h = sh.height;
    int scaledown = 1;
    int w2 = w/scaledown, h2 = h/scaledown;
    CShape aSh(w2, h2, 1);
    accum.ReAllocate(aSh);
    accum.ClearPixels();

    for (int y = 1; y < h-1; y++) {
	for (int x = 1; x < w-1; x++) {
	    if (edges.Pixel(x, y, 0) == 0) { // edge
		float th = theta.Pixel(x, y, 0); // gradient direction, between -PI and PI
		float magn = mag.Pixel(x, y, 0);  // gradient magnitude
		if (th < 0) th += M_PI;          // only need 0..PI
		float gx = cos(th);
		float gy = sin(th);
		for (int s = -1; s <= 1; s += 2) { // both signs -1, +1
		    int xx = ROUND(x + gx * s * rad) / scaledown;
		    int yy = ROUND(y + gy * s * rad) / scaledown;
		    if (xx >= 0 && xx < w2 && yy >= 0 && yy < h2) {
			accum.Pixel(xx, yy, 0) += magn; // vote proportional to gradient magnitude
		    }
		}
	    }
	}
    }
}

// arrays with neighbor indices:
static int nx[8] = {1, 0, -1,  0, 1, -1, -1,  1};
static int ny[8] = {0, 1,  0, -1, 1,  1, -1, -1};

// determine whether the values at (x, y) is greater or equal than its 8 neighbors
int local_max2(CFloatImage img, int x, int y)
{
    float val = img.Pixel(x, y, 0);
    for (int i = 0; i < 8; i++) {
	int xx = x + nx[i];
	int yy = y + ny[i];
	if (img.Pixel(xx, yy, 0) > val)
	    return 0;
	    // only return one local max if same neighboring value
	    //if (img.Pixel(xx, yy, 0) == val && (xx > x || (xx == x && yy > y)))
	    //return 0;
    }
    return 1;
}

// draw circle represented by (x0, y0, rad) in img using color col
void drawCircle(CByteImage img, int x0, int y0, int rad, int col)
{
    CShape sh = img.Shape();
    int w = sh.width, h = sh.height;

    int npoints = (int)(8 * rad);
    for (int k = 0; k < npoints; k++) {
	float theta = 2.0 * M_PI * (float)k / (float)npoints;
	int x = ROUND(x0 + rad * cos(theta));
	int y = ROUND(y0 + rad * sin(theta));

  x -= x0;
  y -= y0;
  x += 

  x += x0;
  y += y0;
	if (x >= 0 && x < w && y >= 0 && y < h)
	    img.Pixel(x, y, 0) = col;
    }
}


// For all local maxima in accum that are >= mincnt,
// draw the corresponding circle in outimg
// return number of circles found
int findMaxC(CFloatImage accum, CByteImage outimg, int mincnt, int rad)
{
    CShape sh = accum.Shape();
    int w = sh.width, h = sh.height;
    int scaleup = outimg.Shape().width / w;
    int found = 0;
    mincnt = __max(1, mincnt);
    for (int y = 1; y < h-1; y++) {
	for (int x = 1; x < w-1; x++) {
	    float val = accum.Pixel(x, y, 0);
	    if (val >= mincnt && local_max2(accum, x, y)) {
		//fprintf(stderr, "found local max = %d\n", val);
		drawCircle(outimg, x*scaleup, y*scaleup, rad, 0);
		found++;
	    }
	}
    }
    return found;
}
