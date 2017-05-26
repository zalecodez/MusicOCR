#define LSIZE 1000

typedef enum LineType{notstaff, staff}LineType;

typedef struct liness{
    int length;
    int startx;
    int starty;
    int endy;
    int endx;
    float slope;
    LineType type;
    int staffNum[2];
}Line; 

