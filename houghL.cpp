/* houghL.cpp
 *
 * Hough transform for finding lines
 */

#include "imageLib.h"
#include <math.h>
#include "liness.h"
// find lines using Hough transform



int getslope(CByteImage edges, CFloatImage theta)
{
    int slope[720];
    for (int i=0; i<720; i++){
        slope[i]=0;
    }
    CShape sh = edges.Shape();
    int w = sh.width, h = sh.height;
    int nD = 2*w, nT = 360;  // number of bins



    int x, y;
    for (y = 1; y < h-1; y++) {
	for (x = 1; x < w-1; x++) {
	    if (edges.Pixel(x, y, 0) == 0) { // edge
		float th = theta.Pixel(x, y, 0); // gradient direction, between -PI and PI

    int conversion = (int) ((th+3.14159) * 114.592);
    slope[conversion] += 1;



	     }
	    }
    }
    int modeslope=0;
    int slopecount=0;
    for (int i=0; i<720; i++){
        //printf("i: %d slope: %d \n", i, slope[i]);
        if (slope[i] >= slopecount){
          modeslope=i;
          slopecount=slope[i];
        }
    }
    printf("modeslope: %d \n", modeslope);
    return modeslope;
    /*
    int findlines[h];
    for (y = 1; y < h-1; y++) {
	     for (x = 1; x < w-1; x++) {
         findlines[y] +=
    */
}


void houghL(CByteImage edges, CFloatImage theta, CFloatImage mag, CFloatImage &accum)
{
    int slope[720];
    for (int i=0; i<720; i++){
        slope[i]=0;
    }
    CShape sh = edges.Shape();
    int w = sh.width, h = sh.height;
    int nD = 2*w, nT = 360;  // number of bins
    CShape aSh(nD, nT, 1);
    accum.ReAllocate(aSh);
    accum.ClearPixels();

    float maxdist = sqrt(w*w + h*h);

    int x, y;
    for (y = 1; y < h-1; y++) {
	for (x = 1; x < w-1; x++) {
	    if (edges.Pixel(x, y, 0) == 0) { // edge
		float th = theta.Pixel(x, y, 0); // gradient direction, between -PI and PI

    int conversion = (int) ((th+3.14159) * 114.592);
    slope[conversion] += 1;


		float magn = mag.Pixel(x, y, 0);  // gradient magnitude
		if (th < 0) th += M_PI;          // only need 0..PI
		float di = (x * cos(th) + y * sin(th));  // distance from origin
		int t = (int)(0.5 + nT * th / (M_PI));
		t = t % nT; // possible that th == M.Pi
		int d = (int)(0.5 + nD * (di / maxdist / 2 +.5));
		if (d < 0 || d >= nD) throw CError("d out of bounds %d\n", d);
		if (t < 0 || t >= nT) throw CError("t out of bounds %d\n", t);
		accum.Pixel(d, t, 0) += magn; // vote proportional to gradient magnitude
	    }
	}
    }
    int modeslope=0;
    int slopecount=0;
    for (int i=0; i<720; i++){
        //printf("i: %d slope: %d \n", i, slope[i]);
        if (slope[i] >= slopecount){
          modeslope=i;
          slopecount=slope[i];
        }
    }
    //printf("modeslope: %d \n", modeslope);
    //return modeslope;
    /*
    int findlines[h];
    for (y = 1; y < h-1; y++) {
	     for (x = 1; x < w-1; x++) {
         findlines[y] +=
    */


}

// arrays with neighbor indices:
static int nx[8] = {1, 0, -1,  0, 1, -1, -1,  1};
static int ny[8] = {0, 1,  0, -1, 1,  1, -1, -1};

// determine whether the values at (x, y) is greater or equal than its 8 neighbors
int local_max(CFloatImage img, int x, int y)
{
    CShape sh = img.Shape();
    int w = sh.width, h = sh.height;
    float val = img.Pixel(x, y, 0);
    for (int i = 0; i < 8; i++) {
	int xx = x + nx[i];
	int yy = y + ny[i];
	yy = (yy + h) % h; // vertical axis (angles) wraps around
	if (xx >= 0 && xx < w) {
	    if (img.Pixel(xx, yy, 0) > val)
		return 0;
	    // only return one local max if same neighboring value
	    //if (img.Pixel(xx, yy, 0) == val && (xx > x || (xx == x && yy > y)))
	    //return 0;
	}
    }
    return 1;
}

// draw line represented by (theta, dist) in img using color col
void drawLine( CByteImage img, float theta, float dist, int col, Line lines[])
{

  CShape sh = img.Shape();
  int w = sh.width, h = sh.height;
  float fx = cos(theta);
  float fy = sin(theta);
  int x, y;
  int begun = 0;
  Line l;
  l.length = 0;




  if (fabs(fx) < fabs(fy)) {
    // line is roughly horizontal (slope < 1)
    for (x = 0; x < w; x++) {
      int pi = 0;

      y = (int)((dist - x * fx) / fy);
      if (y >= 0 && y < h){

        for(int why = y-4; why <= y+4; why++){
          if(why >= 0 && why < h){
            for(int ex = x; ex <= x+3; ex++){
              if(ex >= 0 && ex < w){
                if(img.Pixel(ex,why,0) < 230){
                  pi = 1;
                }
              }
            }
          }
        }
        //if(img.Pixel(x, y, 0) < 230)

        if(pi == 1){
          l.length += 1;
          img.Pixel(x, y, 0) = col;
          if(begun == 0){
            l.startx = x;
            l.starty = y;
            begun = 1;
          }
          else{
            l.endx = x;
            l.endy = y;

            if(begun > 1){
              for(int i = x-1; i >= x-begun+1; i--){
                l.length += 1;
                img.Pixel(i, (int)((dist - i * fx) / fy), 0) = col;
              }
              begun = 1;
            }
          }
        }
        else{
          if(begun != 0){//has begun drawing line
            begun++;
          }
        }
        if(begun == 50){
          break;
        }
      }
    }
  }
  else {
    // line is roughly vertical (slope >= 1)
    for (y = 0; y < h; y++) {
      x = (int)((dist - y * fy) / fx);
      if (x >= 0 && x < w){
        //	img.Pixel(x, y, 0) = col;
      }
    }
  }

  for(int i = 0; i < LSIZE; i ++){
    if(lines[i].length == 0){
      l.slope = (float)(l.endy - l.starty) / (float)(l.endx - l.startx);
      lines[i] = l;
      lines[i].type = staff;
      lines[i].staffNum[0] = -1;
      lines[i].staffNum[1] = -1;
      if(i < LSIZE-1){
        lines[i+1].length = 0;
      }
      break;
    }
  }
}

// For all local maxima in accum that are >= mincnt,
// draw the corresponding line in outimg
// return number of lines found
int findMaxL(CFloatImage accum, CByteImage outimg, int mincnt, Line lines[])
{
    CShape sh = accum.Shape();
    int nD = sh.width, nT = sh.height;

    // recompute maxdist
    sh = outimg.Shape();
    int w = sh.width, h = sh.height;
    float maxdist = sqrt(w*w + h*h);

    mincnt = __max(1, mincnt);
    int found = 0;
    for (int d = 0; d < nD; d++) {
	for (int t = 0; t < nT; t++) {
	    float val = accum.Pixel(d, t, 0);
	    if (val >= mincnt && local_max(accum, d, t)) {
		//fprintf(stderr, "found local max = %d\n", val);
		float theta = (float)(t * M_PI / nT);
		float dist = (float)(d * 2.0 * maxdist / nD - maxdist);
		drawLine(outimg, theta, dist, 0, lines);
		found++;

      }
	}
    }
    return found;
}
