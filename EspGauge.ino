#include <TFT_eSPI.h> 
#include "fonts.h"
TFT_eSPI tft = TFT_eSPI(); 
TFT_eSprite img = TFT_eSprite(&tft);

#define ENC_A 15
#define ENC_B 17

#define color1 TFT_WHITE
#define color2  0x8410
#define color3 TFT_ORANGE
#define color4 0x15B3
#define color5 0x00A3

volatile int counter = 0;
float VALUE;
float lastValue=0;

double rad=0.01745;
float x[360];
float y[360];

float px[360];
float py[360];

float lx[360];
float ly[360];


int r=100;
int sx=110;
int sy=116;

String cc[10]={"0","10","20","30","40","50","60","70","80","90"};
int start[10];
int startP[60];

const int pwmFreq = 5000;
const int pwmResolution = 8;
const int pwmLedChannelTFT = 0;

const int ledPin2 = 27;
const int freq2 = 5000;
const int ledChannel2 = 2;
const int resolution2 = 8;
int dutyCycle2 = 128;

bool onOff=0;
bool debounce=0;

void setup() {

     pinMode(ENC_A, INPUT_PULLUP);
     pinMode(ENC_B, INPUT_PULLUP);
     attachInterrupt(digitalPinToInterrupt(ENC_A), read_encoder, CHANGE);
     attachInterrupt(digitalPinToInterrupt(ENC_B), read_encoder, CHANGE);
    
    pinMode(2,OUTPUT);
    pinMode(0,INPUT_PULLUP);
    pinMode(35,INPUT_PULLUP);
    pinMode(13,INPUT_PULLUP);

    digitalWrite(2,0);

    ledcSetup(pwmLedChannelTFT, pwmFreq, pwmResolution);
    ledcAttachPin(TFT_BL, pwmLedChannelTFT);
    ledcWrite(pwmLedChannelTFT, 90);

    ledcSetup(ledChannel2,freq2,resolution2);
    ledcAttachPin(ledPin2,ledChannel2);

    tft.init();
    tft.setRotation(1);
    tft.setSwapBytes(true);
    tft.fillScreen(TFT_BLACK);
    tft.setTextDatum(4);
    tft.drawString("ON/OFF",215,4);
     tft.drawString("CONTROL",25,20);
    tft.drawString("MOTOR",25,6,2);
    
    
    
    tft.setTextColor(color4,TFT_BLACK);
    tft.drawString("REVOLUTIONS PER MINUTE",120,4);
   
    

    tft.setTextColor(color3,TFT_BLACK);
    tft.setFreeFont(&Slackey_Regular_16);
    tft.drawString("32465",120,18);

    drawOnOFF();
    
    
    img.setSwapBytes(true);
    img.createSprite(220, 105);
    img.setTextDatum(4);

int b=0;
int b2=0;


    for(int i=0;i<360;i++)
    {
       x[i]=(r*cos(rad*i))+sx;
       y[i]=(r*sin(rad*i))+sy;
       px[i]=((r-16)*cos(rad*i))+sx;
       py[i]=((r-16)*sin(rad*i))+sy;

       lx[i]=((r-24)*cos(rad*i))+sx;
       ly[i]=((r-24)*sin(rad*i))+sy;
       
       if(i%36==0){
       start[b]=i;
       b++;
       }

       if(i%6==0){
       startP[b2]=i;
       b2++;
       }
      }

}
int angle=270;
int lastAngle=0;

void loop() {

 static int lastCounter = 0;
 read_encoder();

  if(angle != lastCounter){
  lastCounter = angle; }

 if(digitalRead(0)==0)
 {angle=angle+6;
 delay(20);
 }
 
 if(digitalRead(13)==0)
 {
  if(debounce==0)
  {
    debounce=1;
    onOff=!onOff;
    drawOnOFF();
    
  }
  }else debounce=0;
  
 if(digitalRead(35)==0)
 {angle=angle-6;
 delay(20);
 }



  if(angle!=lastAngle)
  {
     lastAngle=angle;      

     VALUE=((angle-270)/3.60)*-1;
     if(VALUE<0)
     VALUE=VALUE+100;
 
     
     
 img.fillSprite(TFT_BLACK);
 img.fillCircle(sx,sy,124,color5);
 
 img.setTextColor(TFT_WHITE,color5);
 

img.setFreeFont(&FreeSans9pt7b);
 
 for(int i=0;i<10;i++)
 if(start[i]+angle<360){
 img.drawString(cc[i],x[start[i]+angle],y[start[i]+angle]);
 img.drawLine(px[start[i]+angle],py[start[i]+angle],lx[start[i]+angle],ly[start[i]+angle],color1);
 }
 else
 {
 img.drawString(cc[i],x[(start[i]+angle)-360],y[(start[i]+angle)-360]);
 img.drawLine(px[(start[i]+angle)-360],py[(start[i]+angle)-360],lx[(start[i]+angle)-360],ly[(start[i]+angle)-360],color1);
 }


 img.setFreeFont(&DSEG7_Modern_Bold_20);
 img.drawString(String(VALUE),sx,sy-22);
 img.setTextFont(0);
 img.drawString("POWER",sx,sy-42);
 img.drawString("%",sx+44,sy-16,2);
  for(int i=0;i<60;i++)
  if(startP[i]+angle<360)
 img.fillCircle(px[startP[i]+angle],py[startP[i]+angle],1,color2);
 else
 img.fillCircle(px[(startP[i]+angle)-360],py[(startP[i]+angle)-360],1,color2);
 
 img.fillTriangle(sx-1,sy-70,sx-5,sy-56,sx+4,sy-56,TFT_ORANGE);
 img.pushSprite(10, 30);

 if(VALUE==0 || onOff==1)
 {
 digitalWrite(27,0);}else{
 dutyCycle2=map((int)VALUE,0,100,0,255);
 ledcWrite(ledChannel2,dutyCycle2);}
  }
}

void read_encoder() {

  static uint8_t old_AB = 3;  // Lookup table index
  static int8_t encval = 0;   // Encoder value  
  static const int8_t enc_states[]  = {0,-1,1,0,1,0,0,-1,-1,0,0,1,0,1,-1,0}; // Lookup table

  old_AB <<=2;  // Remember previous state

  if (digitalRead(ENC_A)) old_AB |= 0x02; // Add current state of pin A
  if (digitalRead(ENC_B)) old_AB |= 0x01; // Add current state of pin B
  
  encval += enc_states[( old_AB & 0x0f )];

  // Update counter if encoder has rotated a full indent, that is at least 4 steps
  if( encval > 1 ) {        // Four steps forward
    angle=angle+6;              // Increase counter
    encval = 0;
  }
  else if( encval < -1 ) {  // Four steps backwards
   angle=angle-6;               // Decrease counter
   encval = 0;
  }

  if(angle>=360)
  angle=0;

  if(angle<0)
  angle=354;

   

    
}

void drawOnOFF()
{
    tft.fillRect(192,11,50,16,TFT_BLACK);
    tft.drawCircle(202,18,7,TFT_WHITE);
    tft.drawCircle(222,18,7,TFT_WHITE);
    tft.drawLine(202,11,222,11,TFT_WHITE);
    tft.drawLine(202,25,222,25,TFT_WHITE);
    tft.fillRect(203,12,18,13,TFT_BLACK);
    if(onOff==1)
    tft.fillCircle(222,18,4,TFT_ORANGE); 
    else
    tft.fillCircle(202,18,4,TFT_ORANGE); 
  
}
