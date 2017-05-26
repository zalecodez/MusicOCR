/* main.cpp
 *
 * main program for Hough transform for finding lines / circles
 *
 * Input: gray image
 * Output: gray image overlaid with found lines
 *
 * CS 453 HW 4
 */

static const char *usage = "usage: %s sigma lo hi [minrad maxrad] nBlur mincnt avgthresh in.png out.png\n";

#include "imageLib.h"

// these "header" files declare the functions defined in the corresponding cpp files
#include "smooth.h"
#include "edges.h"
#include "houghL.h"
#include "houghC.h"
#include "note.h"
#include <math.h>
#define VERBOSE 1
#define SAVE_ACCUM 1
#define ABS(x) x<0?-x:x


// blur accumulator array with .25 * [1 2 1] kernel both in x and in y
// wrap around in x and y
void blur(CFloatImage im1)
{
    CShape sh = im1.Shape();
    int w = sh.width, h = sh.height;
    CFloatImage im2(sh);

    int x, y;
    for (y = 0; y < h; y++) {
	for (x = 0; x < w; x++) {
	    int x1 = (x - 1 + w) % w;
	    int x2 = (x + 1) % w;
	    im2.Pixel(x, y, 0) = 0.25 * (  im1.Pixel(x1, y, 0) +
					 2*im1.Pixel(x,  y, 0) +
					   im1.Pixel(x2, y, 0));
	}
    }
    for (y = 0; y < h; y++) {
	for (x = 0; x < w; x++) {
	    int y1 = (y - 1 + h) % h;
	    int y2 = (y + 1) % h;
	    im1.Pixel(x, y, 0) = 0.25 * (  im2.Pixel(x, y1, 0) +
					 2*im2.Pixel(x, y,  0) +
					   im2.Pixel(x, y2, 0));
	}
    }
}

// find max value
float maxval(CFloatImage im)
{
    CShape sh = im.Shape();
    int w = sh.width, h = sh.height;

    float m = im.Pixel(0, 0, 0);
    for (int y = 0; y < h; y++) {
	for (int x = 0; x < w; x++) {
	    float v = im.Pixel(x, y, 0);
	    m = __max(m, v);
	}
    }
    return m;
}

void rotateimage(CByteImage &img, float angle){

  CShape sh = img.Shape();
  int w= sh.width, h=sh.height;
  CByteImage outimg;
  outimg.ReAllocate(sh);

  for (int y=0; y<h; y++){
    for (int x=0; x<w; x++){
      int ax  = x- w/2;
      int ay  = y- h/2;
      int rotx = ax*cos(angle)-ay*sin(angle);
      int roty = ax*sin(angle)+ay*cos(angle);
      rotx += w/2;
      roty += h/2;
      if (rotx >= 0 && rotx < w && roty>= 0 && roty < h){
        outimg.Pixel(rotx,roty,0) =img.Pixel(x,y,0);
        //printf("y: %d \n", y);
        }
    }
  }
  for (int y=0; y<h; y++){
    for (int x=0; x<w; x++){
      img.Pixel(x,y,0)=outimg.Pixel(x,y,0);
    }
  }
}


void printnote(Note notes[][256], int staffCount){
  for (int m=0; m<staffCount; m++){
    int n=0;
    while (notes[m][n].pitch != -999){
      switch(notes[m][n].pitch){
        case -2: printf("A ");
          break;
        case -1: printf("G ");
          break;
        case 0: printf("F ");
          break;
        case 1: printf("E ");
          break;
        case 2: printf("D ");
          break;
        case 3: printf("C ");
          break;
        case 4: printf("B ");
          break;
        case 5: printf("A ");
          break;
        case 6: printf("G ");
          break;
        case 7: printf("F ");
          break;
        case 8: printf("E ");
          break;
        case 9: printf("D ");
          break;
        case 10: printf("C ");
          break;
        default:
          break;
      }
      n++;
    }
    printf("\n");
  }
}



int makeStaff(Line lines[]){
    int i, j;
    int prevgap = 0, gap, count = 0;
    int maxlen = 0;
    int staffCount = 0;
    float maxslope = 0.0;
    float maxslopefreq = 0.0;
    float slopefreq[LSIZE/2][2];
    int numslopes = 0;
    int lineGroup[5];
    //int gapGroup[4];

    i = 0;
    while(lines[i].length != 0 && i < LSIZE){
      //printf("length: %d ", lines[i].length);
      if(lines[i].length > maxlen){
          maxlen = lines[i].length;
          //printf("maxlen: %d \n", maxlen);
      }
      i++;
    }

    i = 0;
    while(lines[i].length != 0 && i < LSIZE){
      lines[i].type = staff;
      if(lines[i].length < (int)(maxlen*0.8)){
        lines[i].type = notstaff;
      }
      i++;
    }

    i=0;
    int found;
    while(lines[i].length != 0 && i < LSIZE){
      if(lines[i].length == staff){
        found = 0;
        for(j = 0; j < numslopes; j++){
          if(slopefreq[j][0] == lines[i].slope){
            found = 1;
            slopefreq[j][1] += 1.0;

            if(slopefreq[j][1] > maxslopefreq){
              maxslopefreq = slopefreq[j][1];
              maxslope = slopefreq[j][0];
            }
            break;
          }
        }
        if(found == 0){
          slopefreq[numslopes][0] = lines[i].slope;
          slopefreq[numslopes][1] = 1.0;
          numslopes++;
        }
      }
      i++;
    }

    i = 0;
    while(lines[i].length != 0 && i < LSIZE){
      if(lines[i].slope != maxslope){
        lines[i].type = notstaff;
      }
      i++;
    }

    i = 0;
    int next = 1;

    gap = 0; prevgap = 0;
    count = 0;

    while(lines[i].length != 0 && i < LSIZE){
      if(lines[i].type == staff){
        //next = i+1;
        while(lines[next].type != staff){
          next++;
          if(next >= LSIZE)
            break;
        }
        gap = lines[next].starty - lines[i].starty;

        if(prevgap == 0){
          prevgap = gap;
          lineGroup[count] = i;
          lineGroup[++count] = next;
          i = next;
          next++;
          continue;
        }

        lineGroup[++count] = next;

        if(ABS(gap - prevgap) < 5){
          if(count == 4){
            staffCount++;
            for(j = 0; j < 5; j++){
              //printf("%i %i ",j, lines[lineGroup[j]].starty);
              lines[lineGroup[j]].staffNum[0] = staffCount;
              lines[lineGroup[j]].staffNum[1] = j+1;
            }
            count = 0;
            gap = 0;
            next++;
          }
          prevgap = gap;
        }
        else{
          lineGroup[0] = lineGroup[count-1];
          lineGroup[1] = lineGroup[count];
          count = 1;
        }
      }
      i = next;
      next++;
    }

    printf("\n\n");
    for(int a = 0; a < LSIZE; a++){
        if(lines[a].type == staff){

            if(lines[a].length != 0){
            printf("%i\t %i\t %i\t %i\t %i\t %.2f\t %i\t %i\n", lines[a].length, lines[a].startx, lines[a].starty, lines[a].endx, lines[a].endy, lines[a].slope, lines[a].staffNum[0], lines[a].staffNum[1]);
            }
            else
                break;
        }
    }
    return staffCount;
}

void checkNotes(Line lines[], Note notes[][256], int staffCount, CByteImage img, int avgthresh){
  CShape sh = img.Shape();
  int w = sh.width, h = sh.height;
  int gap;
  Note dummy;
  dummy.xposition = -999;
  dummy.yposition = -999;
  dummy.pitch = -999;

  notes[0][0] = dummy;
  int i = 0, j = 1;
  int x, y;

  while(lines[i].type != staff){
    i++;
    j++;
  }
  while(lines[j].type != staff){
    j++;
  }

  gap = lines[j].starty - lines[i].starty; //both lines are in staff

  int index = 0;
  fprintf(stderr,"\n\n\nSTAFFCOUNT = %i\n\n\n", staffCount);

  for(i = 1; i <= staffCount; i++){
    notes[i-1][0] = dummy;
    while(lines[index].staffNum[0] != i /*|| lines[index].staffNum[1] != 1*/){ //1st line of i-th staff, ???????
      index++;
    }
    fprintf(stderr,"\n\n\nINDEX = %i\n\n\n i = %i\n\n\nstaffnum = %i\n\n\nx=%i, y=%i\n\n\n\n", index, i, lines[index].staffNum[0], lines[index].startx, lines[index].starty);
    int numNotes = 0; // ????????

    x = lines[index].startx;
    for(y = lines[index].starty; x <= lines[index].endx; y = (int)((float)y+lines[index].slope)){

      for(j = -2; j <= 10; j++){

        int cy = y+(j*(gap/2));
        if(cy>=0 && x>=0 && cy<h && x<w){

          //for(by = y; by)
          int sum = 0;
          int num = 0;
          int avg = 0;
          for(int why = cy-gap/2; why <= cy+gap/2; why++){
            for(int ex = x-gap/2; ex <= x+gap/2; ex++){
              if(why>=0 && ex>=0 && why<h && ex<w){
                num++;
                sum += img.Pixel(ex, why, 0);
                //printf("%i in",gap/2);
              }
            }
          }
          avg = (int)(sum/num);

          if(avg < avgthresh){
            Note n;
            n.xposition = x;
            n.yposition = cy;
            n.pitch = j;
            notes[i-1][numNotes++] = n;
            notes[i-1][numNotes+1] = dummy;
            notes[i][0] = dummy;
            x+=gap-1;


            fprintf(stderr,"%i, %i, %i  \n" , n.xposition, n.yposition, n.pitch);
          }
        }
      }
      x++;
    }
    notes[i][numNotes].xposition = 0;
    notes[i][numNotes].yposition = 0;
    notes[i][numNotes].pitch = 0;
  }

  printnote(notes, staffCount);
}


int main(int argc, char **argv)
{
    try {
	int argnum = 1;
  Line lines[LSIZE];
  lines[0].length = 0;
  int staffCount;

  Note notes[LSIZE/5][256];

	if (argc != 9 && argc != 11)
	    throw CError(usage, argv[0]);

	int findlines = (argc == 9);

	float sigma = atof(argv[argnum++]);
	float lo = atof(argv[argnum++]);
	float hi = atof(argv[argnum++]);
	int minrad = (findlines ? 0 : atoi(argv[argnum++]));
	int maxrad = (findlines ? 0 : atoi(argv[argnum++]));
	int nBlur = atoi(argv[argnum++]);
	int mincnt = atof(argv[argnum++]);
  int avgthresh = atoi(argv[argnum++]);
	char *inname = argv[argnum++];
	char *outname = argv[argnum++];

	CByteImage img, edges, outim, acim;
	CFloatImage imf, Ix, Iy, mag, theta, accum;

	ReadImageVerb(img, inname, VERBOSE);
	if (VERBOSE)
	    fprintf(stderr, "Smoothing (sigma=%g), Canny (lo=%g, hi=%g)\n", sigma, lo, hi);




  //float angle= (getslope(edges,theta)/2) * 3.14159/180;
  //rotateimage(img,angle);
	CopyPixels(img, imf);
	smooth(imf, sigma, 0);


  //CShape sh = imf.Shape();
  //int w = sh.width, h = sh.height;
  /*
  int rowval[h];
  for (int y=0; y < h; y++){
    for (int x=0; x<w; x++){
      if (imf.Pixel(x,y,0) < 50){
        rowval[y] = rowval[y]+1;
      }
    }
    if (rowval[y] > 800){
    printf("y: %d rowval: %d \n", y, rowval[y] );
    }
  }
  */


	gradients(imf, Ix, Iy, mag, theta);
	NMS(Ix, Iy, mag, edges);
	threshold(edges, mag, lo, hi);
	ScaleAndOffset(edges, outim, 0.25, 191); // make edges faint in output image

  //printf("angle: %.2f \n", angle );
	if (findlines) {
	    if (VERBOSE)
		fprintf(stderr, "finding lines, nBlur=%d, mincnt=%d\n", nBlur, mincnt);
	    houghL(edges, theta, mag, accum);
	    for (int k=0; k<nBlur; k++)
		blur(accum);
	    float maxcnt = maxval(accum);
	    if (SAVE_ACCUM) {
		ScaleAndOffset(accum, acim, 2*255.0/maxcnt, 0);
		WriteImageVerb(acim, "accum.png", 1);
	    }
	    int found = findMaxL(accum, outim, mincnt, lines);
	    printf("found %d lines (maxcnt = %.0f)\n", found, maxcnt);
	} else { // find circles
	    if (VERBOSE)
		fprintf(stderr, "finding circles (rad=%d..%d), nBlur=%d, mincnt=%d\n",
			minrad, maxrad, nBlur, mincnt);
	    float totmaxcnt = 0;
	    int totfound = 0;
	    for (int rad = minrad; rad <= maxrad; rad++) {
		houghC(edges, theta, mag, accum, rad);
		for (int k=0; k<nBlur; k++)
		    blur(accum);
		float maxcnt = maxval(accum);
		totmaxcnt = __max(totmaxcnt, maxcnt);
		if (SAVE_ACCUM) {
		    ScaleAndOffset(accum, acim, 2*255.0/maxcnt, 0);
		    char fname[1000];
		    sprintf(fname, "accum%03d.png", rad);
		    WriteImageVerb(acim, fname, 1);
		}
		int found = findMaxC(accum, outim, mincnt, rad);
		printf("found %d circles (maxcnt = %.0f)\n", found, maxcnt);
		totfound += found;
	    }
	    printf("total found = %d, overall maxcnt = %.0f\n", totfound, totmaxcnt);
	}
	WriteImageVerb(outim, outname, VERBOSE);

        for(int a = 0; a < LSIZE; a++){
            if(lines[a].length != 0){
            fprintf(stderr,"%i\t %i\t %i\t %i\t %i\t %.9f\n", lines[a].length, lines[a].startx, lines[a].starty, lines[a].endx, lines[a].endy, lines[a].slope);
            }
            else
                break;
        }

        staffCount = makeStaff(lines);

        checkNotes(lines, notes, staffCount, img, avgthresh);
    }

    catch (CError &err) {
	fprintf(stderr, err.message);
	fprintf(stderr, "\n");
	return -1;
    }


    return 0;
}
