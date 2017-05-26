//assume we have notes[staff][notes in staff] = pitch
void printnote(Note notes[][256]){

  for (m=0; m<staffCount; m++){
    n=0;
    while notes[m][n].pitch != 999{
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
      }
    }
  }
}
