/////////////////////////////////////////////////////////////////////////////////////////////
//
// - copy-paste header (.h) file of tune you want to play it in sketch folder (look in folders of internal version)
// - all checks are off, check if tune is playable in internal version first
//
//////////////////////////////////////////////////////////////////////////////////////////////

#include"Monty_on_the_Run.h"  // MUSICIANS\H\Hubbard_Rob\Monty_on_the_Run.sid

#define GENEROUS  0; // set amount of uS per IRQ that generously will be given to your sketch (greater the number, your sketch will run faster, but sound quality will degrade )

volatile uint8_t Tune ; // in this global variable will be tune number currently playing when you sketch start. Use it to change tune number playing.

//////////////////////////////////////////////////////////////////////////////////////////////
//
// Original sketch can be found as library example (only Roger's core) : https://github.com/cbm80amiga/Arduino_ST7735_Fast
//
// Special thanks for cbm80amiga, his source code was very inspirational. 
//
//////////////////////////////////////////////////////////////////////////////////////////////

// ST7735 library example
// Amiga Multi Boing Ball Demo
// (c) 2019 Pawel A. Hernik
// YT videos:
// https://youtu.be/rtnI4TEeBpA
// https://youtu.be/KwtkfmglT-c

/*
 ST7735 128x160 LCD pinout (header at the top for better viewwing angles, from left):
 #1 LED   -> 3.3V
 #2 SCK   -> SCL/D13/PA5
 #3 SDA   -> MOSI/D11/PA7
 #4 A0/DC -> D8/PA1  or any digital
 #5 RESET -> D9/PA0  or any digital
 #6 CS    -> D10/PA2 or any digital
 #7 GND   -> GND
 #8 VCC   -> 3.3V
*/

#define SCR_WD 128
#define SCR_HT 160
#include <SPI.h>
#include <Adafruit_GFX.h>

#define TFT_DC PA3 // 
#define TFT_CS PA4
#define TFT_RST PA2
// TFT_CLK          PA5
// TFT_MISO         PA6
// TFT_MOSI         PA7
#include <Arduino_ST7735_STM.h>


Arduino_ST7735 lcd = Arduino_ST7735(TFT_DC, TFT_RST, TFT_CS);

#include "ball64.h"
#include "ball48.h"
#include "ball32.h"

uint16_t line[SCR_WD];
uint16_t bgCol    = RGBto565(160,160,160);
uint16_t bgColS   = RGBto565(90,90,90);
uint16_t lineCol  = RGBto565(150,40,150);
uint16_t lineColS = RGBto565(80,10,80);
uint16_t ballCol1 = RGBto565(255,255,255);
uint16_t ballCol2 = RGBto565(255,0,0);
uint16_t ballCol1S = RGBto565(128,128,128);
uint16_t ballCol2S = RGBto565(128,0,0);


uint8_t colsRGB[7*3] = {
  255,0,0,
  0,155,0,
  255,0,255,
  185,125,0,
  0,185,185,
  0,0,255,
  55,55,55,
};

uint16_t cols[7], colsS[7];

#define LINE_YS  10
#define LINE_XS1 19
#define LINE_XS2 2

#define BALL_SWD 128
#define BALL_SHT 145

int shadow=20;

struct BallData {
  int anim,animd;
  int x,xd;
  int y,yd;
  int wd = 64;
  int ht = 64;
  uint8_t *img;
  uint16_t palette[16];
  uint16_t ballCol1 = RGBto565(255,255,255);
  uint16_t ballCol2 = RGBto565(255,0,0);
  uint16_t ballCol1S = RGBto565(128,128,128);
  uint16_t ballCol2S = RGBto565(128,0,0);
};

#define NUM_BALLS 10
int numBalls=7;
struct BallData ball[NUM_BALLS];


void drawShadow(int ii, int c)
{
  if(ii<0 || ii>=SCR_WD) return;
  if(line[ii]==bgCol) line[ii]=bgColS; else
  if(line[ii]==lineCol) line[ii]=lineColS; 
  if(c<0) return;
  if(line[ii]==ballCol1) line[ii]=ballCol1S; else
  for(int i=0;i<c;i++) if(line[ii]==cols[i]) { line[ii]=colsS[i]; break; }
}

void renderFrame()
{
  for(int y=0;y<SCR_HT;y++) {
    drawBg(y);
    for(int b=0;b<numBalls;b++) {
      if(y>=ball[b].y && y<ball[b].y+ball[b].ht) {
        if(shadow<0) {
          uint8_t v,*img = ball[b].img+16*2+6+(y-ball[b].y)*ball[b].wd/2+ball[b].wd/2-1;
          for(int i=ball[b].wd-1;i>=0;i--) {
            v = pgm_read_byte(img);
            v = (i&1) ? v&0xf : v>>4;
            if((i&1)==0) img--;
            if(v) {
              line[ball[b].x+i] = ball[b].palette[v];
              if(shadow) drawShadow(ball[b].x+i+shadow,b-1);
            }
          }
        } else {
          uint8_t v,*img = ball[b].img+16*2+6+(y-ball[b].y)*ball[b].wd/2;
          for(int i=0;i<ball[b].wd;i++) {
            v = pgm_read_byte(img);
            v = (i&1) ? v&0xf : v>>4;
            if(i&1) img++;
            if(v) {
              line[ball[b].x+i] = ball[b].palette[v];
              if(shadow) drawShadow(ball[b].x+i+shadow,b-1);
            }
          }
        }
      }
    }
    lcd.drawImage(0,y,SCR_WD,1,line);
  }
}

void drawBg(int yy)
{
  int i;
  // horizontal lines
  if(yy==LINE_YS      || yy==LINE_YS+1*10 || yy==LINE_YS+2*10 || yy==LINE_YS+3*10 || yy==LINE_YS+4*10 || yy==LINE_YS+5*10 || yy==LINE_YS+6*10 ||
     yy==LINE_YS+7*10 || yy==LINE_YS+8*10 || yy==LINE_YS+9*10 || yy==LINE_YS+10*10 || yy==LINE_YS+11*10 || yy==LINE_YS+12*10) {
    for(i=0;i<LINE_XS1;i++) line[i]=line[SCR_WD-1-i]=bgCol;
    for(i=0;i<=SCR_WD-LINE_XS1*2;i++) line[i+LINE_XS1]=lineCol;
  // vertical lines, top/bottom empty area and shadow indicator at the bottom
  } else if(yy<LINE_YS+12*10 || yy>SCR_HT-LINE_YS) {
    for(i=0;i<SCR_WD;i++) line[i]=bgCol;
    if(yy>LINE_YS && yy<LINE_YS+12*10) for(i=0;i<10;i++) line[LINE_XS1+i*10]=lineCol;
    if(yy>=SCR_HT-LINE_YS+3 && yy<SCR_HT-LINE_YS+3+5) {
      int xs = -shadow+30;
      int yi = yy-(SCR_HT-LINE_YS+3);
      for(i=0;i<yi*2+1;i++) line[xs*2+i-yi+4]=RED;
    }
  // horizontal lines at the bottom
  } else if(yy==SCR_HT-LINE_YS || yy==SCR_HT-LINE_YS-7 || yy==SCR_HT-LINE_YS-7-6 || yy==SCR_HT-LINE_YS-7-6-4) {
    int dy=SCR_HT-LINE_YS-(LINE_YS+10*12);
    int dx=LINE_XS1-LINE_XS2;
    int o=(SCR_HT-LINE_YS-yy)*dx/dy;
    for(i=0;i<LINE_XS2+o;i++) line[i]=line[SCR_WD-1-i]=bgCol;
    for(i=0;i<=SCR_WD-LINE_XS2*2-o*2;i++) line[LINE_XS2+o+i]=lineCol;
  // vertical lines at the bottom
  } else {
    for(i=0;i<SCR_WD;i++) line[i]=bgCol;
    int dy=SCR_HT-LINE_YS-(LINE_YS+10*12);
    int dx=LINE_XS1-LINE_XS2;
    int o=(SCR_HT-LINE_YS-yy)*dx/dy;
    int x1=o;
    int x2=SCR_WD-o-LINE_XS2*2;
    for(i=0;i<10;i++) line[LINE_XS2+o+i*(x2-x1)/9]=lineCol;
  }
}

void MySetup() 
{

   Serial.begin(9600);
  lcd.init();
  //lcd.fillScreen(bgCol);
  for(int i=0;i<SCR_HT;i++) { drawBg(i); lcd.drawImage(0,i,SCR_WD,1,line); }
  //delay(1000);
  for(int i=0;i<NUM_BALLS;i++) {
    ball[i].x = random(0,64);
    ball[i].y = random(0,64);
    ball[i].xd = random(1,3);
    ball[i].yd = random(1,3);
    ball[i].anim = 0;
    ball[i].animd = 1;
    ball[i].img = (uint8_t*)ball64;
    cols[i] = RGBto565(colsRGB[i*3+0],colsRGB[i*3+1],colsRGB[i*3+2]);
    colsS[i] = RGBto565(colsRGB[i*3+0]/2,colsRGB[i*3+1]/2,colsRGB[i*3+2]/2);
    ball[i].ballCol2 = cols[i];
  }
  ball[1].img=(uint8_t*)ball48; ball[1].wd=ball[1].ht=48;
  ball[2].img=(uint8_t*)ball48; ball[2].wd=ball[2].ht=48;
  ball[3].img=(uint8_t*)ball32; ball[3].wd=ball[3].ht=32;
  ball[4].img=(uint8_t*)ball32; ball[4].wd=ball[4].ht=32;
  ball[5].img=(uint8_t*)ball32; ball[5].wd=ball[5].ht=32;
  ball[6].img=(uint8_t*)ball48; ball[6].wd=ball[6].ht=48;
}

int anim=0, animd=1;
int x=0, y=0;
int xd=2, yd=1;
unsigned long ms;

void render()
{
  ms=millis();
  for(int b=0;b<numBalls;b++) {
    for(int i=0;i<14;i++)
      ball[b].palette[i+1] = ((i+ball[b].anim)%14)<7 ? ball[b].ballCol1 : ball[b].ballCol2;
    ball[b].anim+=ball[b].animd;
    if(ball[b].anim<0) ball[b].anim+=14;
    ball[b].x+=ball[b].xd;
    ball[b].y+=ball[b].yd;
    if(ball[b].x<0) { ball[b].x=0; ball[b].xd=-ball[b].xd; ball[b].animd=-ball[b].animd; }
    if(ball[b].x>=BALL_SWD-ball[b].wd) { ball[b].x=BALL_SWD-ball[b].wd; ball[b].xd=-ball[b].xd; ball[b].animd=-ball[b].animd; }
    if(ball[b].y<0) { ball[b].y=0; ball[b].yd=-ball[b].yd; }
    if(ball[b].y>=BALL_SHT-ball[b].ht) { ball[b].y=BALL_SHT-ball[b].ht; ball[b].yd=-ball[b].yd; }
  }
  renderFrame();
  while(millis()-ms<40); // limit to 25fps
  //ms=millis()-ms; Serial.println(ms);
}

void MyLoop()
{
  for(int i=30*5;i>-30*5;i--) { shadow=i/5; render(); }
  for(int i=0*5;i<20*5;i++) { shadow=-30; render(); }
  for(int i=-30*5;i<30*5;i++) { shadow=i/5; render(); }
  for(int i=0*5;i<20*5;i++) { shadow=30; render(); }
}
