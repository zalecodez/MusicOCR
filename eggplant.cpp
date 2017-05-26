void checkNotes(Line lines[], Note notes[][256], int staffCount, CByteImage img){
  CShape sh = img.Shape();
  int w = sh.width, h = sh.height;
  int gap;
  Note dummy;
  dummy.xposition = -999;
  dummy.yposition = -999;
  dummy.pitch = -999;

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
  for(i = 1; i <= staffCount; i++){
    while(lines[index].staffNum[0] != i && lines[index].staffNum[1] != 1){ //1st line of i-th staff, ???????
      index++;
    }
    int numNotes = 0; // ????????
    for(y = lines[index].starty; x <= lines[index].endx; y = (int)((float)y+lines[index].slope)){
      for(x = lines[index].startx; x <= lines[index].endx; x++){
        for(j = -2; j <= 10; j++){
          int cy = y+(j*(gap/2));
          if(cy>=0 && x>=0 && cy<h && x<w){
            //for(by = y; by)
            int sum = 0;
            int num = 0;
            int avg;
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
            if(avg < 140){
              Note n;
              n.xposition = x;
              n.yposition = cy;
              n.pitch = j;
              notes[i][numNotes++] = n;
              printf("%i, %i, %i  \n" , n.xposition, n.yposition, n.pitch);
            }
          }
        }
      }
    }
    notes[i][numNotes].xposition = 0;
    notes[i][numNotes].yposition = 0;
    notes[i][numNotes].pitch = 0;
  }
}
