#include<Arduino.h>

struct POSITION{
  int x; 
  int y;
};

struct BLOCK{
  int rotate;
  POSITION p[3];
};

struct STATUS{
  int x;
  int y;
  int type;
  int rotate;
};

