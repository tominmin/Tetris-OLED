/*
 * Tetris game with OLED
 * refered from http://www.nicovideo.jp/watch/sm8517855
 * OLED:  https://www.sparkfun.com/products/13003
 * D3-left button
 * D4-right button
 * D5-rotate button
 * D6-down button
 * 
 */
#include<Wire.h>
#include<SPI.h>
#include<SFE_MicroOLED.h>
#include"types.h"

#define PIN_RESET 9
#define PIN_DC 8
#define PIN_CS 10
#define DC_JUMPER 0
MicroOLED oled(PIN_RESET,PIN_DC,PIN_CS);
bool ret=false;
int w=0;
bool gameover_flag=false;
int board[12][22];
int score=0;
int button_state;
int button_state2;
int button_state3;
int button_state4;
int swCount=0;
int swCount2=0;
int swCount3=0;
int swCount4=0;
STATUS current;
bool left_check=false;
bool right_check=false;
bool rotate_check=false;
bool down_check=false;
bool delete_check=false;

BLOCK block[8]={
  {1,{{0,0},{0,0},{0,0}}},//null
  {2,{{0,-1},{0,1},{0,2}}},//tetris
  {4,{{0,-1},{0,1},{1,1}}},//L1
  {4,{{-1,-1},{1,1},{2,2}}},//difficult parts
  {2,{{0,-1},{1,0},{1,1}}},//key1
  {2,{{0,-1},{-1,0},{-1,1}}},//key2
  {1,{{0,1},{1,0},{1,1}}},//square
  {4,{{0,-1},{1,0},{-1,0}}},//T
};

int random(int max){
  return (int)((rand()/(RAND_MAX+1.0))*max);
}

void setup(){
  oled.begin();
  oled.clear(ALL);
  oled.setFontType(1);
  oled.setCursor(0,0);
  oled.display();
  oled.clear(PAGE);
  current.x=2;
  current.y=1;
  current.type=random(7)+1;
  current.rotate=random(4);
  //initialize
  for(int x=0;x<12;x++){
    for(int y=0;y<22;y++){
      if(x==0||x==11||y==20){
        board[x][y]=true;
      }else{
        board[x][y]=false;
      }
    }
  }
  pinMode(3,INPUT_PULLUP);
  pinMode(4,INPUT_PULLUP);
  pinMode(5,INPUT_PULLUP);
  pinMode(6,INPUT_PULLUP);
}

bool deleteBlock(STATUS s){
  board[s.x][s.y]=0;
  for(int i=0;i<3;i++){
    int dx=block[s.type].p[i].x;
    int dy=block[s.type].p[i].y;
    int r=s.rotate%block[s.type].rotate;
    for(int j=0;j<r;j++){
      int nx=dx,ny=dy;
      dx=ny;dy=-nx;
    }
    board[s.x+dx][s.y+dy]=0;
  }
  return true;
}


bool putBlock(STATUS s,bool action=false){
  if(board[s.x][s.y]!=0){
    return false;
  }
  if(action){
    board[s.x][s.y]=true;
  }
  for(int i=0;i<3;i++){
    int dx=block[s.type].p[i].x;
    int dy=block[s.type].p[i].y;
    int r=s.rotate%block[s.type].rotate;
    for(int j=0;j<r;j++){
      int nx=dx,ny=dy;
      dx=ny;dy=-nx;
    }
    if(board[s.x+dx][s.y+dy]!=0){
      return false;
    }
    if(action){
      board[s.x+dx][s.y+dy]=true;
    }
  }
  if(!action){
    putBlock(s,true);
  }
  return true;
}
void deleteLine(){
  for(int y=0;y<20;y++){
    bool flag=true;
    for(int x=1;x<=10;x++){
      if(board[x][y]==0){
        flag=false;
      }
    }
    if(flag){
      score+=10;
      delete_check=true;
      for(int j=y;j>0;j--){
        for(int i=1;i<=10;i++){
          board[i][j]=board[i][j-1];
        }
      }
      y++;
    }
   
  }
}

void blockDown(){
  deleteBlock(current);
  delay(30);
  current.y++;
  if(!putBlock(current)){
    current.y--;
    putBlock(current);
    
    deleteLine();
    current.x=3;
    current.y=1;
    current.type=random(7)+1;
    current.rotate=random(4);
    if(!putBlock(current)){
      gameover_flag=true;
    }
  }
}

void pixelset(int x,int y){
  for(int i=0;i<2;i++){
    for(int j=0;j<2;j++){
      oled.pixel(x*2+i,y*2+j);
    }
  }
}

void pixel_draw(){//set 2x2 pixel
  oled.clear(PAGE);
  oled.rect(1,0,22,41);
  for(int i=1;i<=10;i++){
    for(int j=0;j<20;j++){
        if(board[i][j]){
        pixelset(i,j);
        }
    }
  }
  oled.setFontType(0);
  oled.setCursor(25,10);
  oled.print("tetris");
  oled.setCursor(25,20);
  if(left_check){
    oled.print("left");
    left_check=false;
  }else if(right_check){
    oled.print("right");
    right_check=false;
  }else if(rotate_check){
    oled.print("rotate");
    rotate_check=false;
  }else if(down_check){
    oled.print("down");
    down_check=false;
  }else if(delete_check){
    oled.print("delete");
    delete_check=false;
  }
  oled.setCursor(25,30);
  oled.print("score:");
  oled.setCursor(25,40);
  oled.print(score);

  oled.display();
  delay(30);
}

void initialize(){
  oled.clear(PAGE);
  current.x=2;
  current.y=1;
  current.type=random(7)+1;
  current.rotate=random(4);
  //initialize
  for(int x=0;x<12;x++){
    for(int y=0;y<22;y++){
      if(x==0||x==11||y==20){
        board[x][y]=true;
      }else{
        board[x][y]=false;
      }
    }
  }
}



void loop(){//drawing tetris game display
button_state=digitalRead(3);
button_state2=digitalRead(4);
button_state3=digitalRead(5);
button_state4=digitalRead(6);


if(gameover_flag){
  oled.clear(PAGE);
  oled.setCursor(10,10);
  oled.print("gameover");
  oled.display();
  delay(1000);
  score=0;
  initialize();
  gameover_flag=false;
}
STATUS n=current;
if(button_state==LOW){
swCount++;
}else{
 swCount=0; 
}
if(button_state2==LOW){
swCount2++;
}else{
 swCount2=0; 
}
if(button_state3==LOW){
  swCount3++;
}else{
 swCount3=0; 
}
if(button_state4==LOW){
swCount4++;
}else{
 swCount4=0; 
}
if(swCount==1){
  n.x--;ret=true;
  left_check=true;
}
if(swCount2==1){
  n.x++;ret=true;
  right_check=true;
}
if(swCount3==1){
  n.rotate++;ret=true;
  rotate_check=true;
}
if(swCount4==1){
  n.y+=3;
  ret=true;
  down_check=true;
}

if(ret){
deleteBlock(current);
if(putBlock(n)){
  current=n;
}else{
  putBlock(current);
}
ret=false;
}
if(w%4==0){
  blockDown();
}
w++;
pixel_draw();
delay(50);
}


