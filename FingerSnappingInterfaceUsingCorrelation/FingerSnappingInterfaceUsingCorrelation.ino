//--------------------------------------------------------------------
//Finger Snapping Interface using Correlation
//Programmed by N.Tomi(tomi.nori+github at gmail,com)
//--------------------------------------------------------------------
#include <MsTimer2.h>

//--------------------------------------------------------------------
//RingBuffer
//--------------------------------------------------------------------
#define QUEUE_SIZE  40
#define COUNT_RESET (QUEUE_SIZE*128)
int ringBuffer[QUEUE_SIZE] = {0};
volatile int ringCount = 0;

void AddRingBuffer(int val)
{
  //reset index
  if( ringCount==COUNT_RESET )
  {
    ringCount = 0;
  }
  //add ringbuffer
  int ringIndex = ringCount % QUEUE_SIZE;
  ringBuffer[ringIndex] = val;
  ringCount++;
}

//--------------------------------------------------------------------
//Classfy using Correlation
//--------------------------------------------------------------------
//correct signal 1
int correctY1SubAve[] = {
  -13,-7,-9,-6,-16,-10,-5,-4,-16,-6,-3,-4,-8,6,-8,39,-19,-41,-75,505,-152,-224,245,26,-101,9,-35,-8,3,7,-83,3,6,14,-21,20,9,-16,-7};
int correctY1_2_sqrt  = 648;

//correct signal 2
int correctY2SubAve[] = {
  -3,1,0,1,2,1,1,2,-2,2,3,2,6,8,24,61,-32,-57,-36,-510,282,49,-92,111,64,30,-34,9,-45,26,53,27,-7,-10,-3,-2,-2,17,18};
int correctY2_2_sqrt  = 621;

int ave = 0;
void CalcAve()
{
  ave = 0;
  for( int i=0;i<QUEUE_SIZE;i++ )
  {
    ave += ringBuffer[i];
  }
  ave /= QUEUE_SIZE; //imaiti...
}

boolean Classfy1()
{
  int bunsiX1     = 0.0;
  int bunboTempX1 = 0.0;
  int bunsiX2     = 0.0;
  int bunboTempX2 = 0.0;
  for( int i=0;i<QUEUE_SIZE;i++ )
  {
    int temp  = ringBuffer[i] - ave;
    int temp2 = temp*temp;
    
    bunsiX1 += temp*correctY1SubAve[i];
    bunboTempX1 += temp2;
    
    bunsiX2 += temp*correctY2SubAve[i];
    bunboTempX2 += temp2;
  }
  float rX1 = abs( bunsiX1/(sqrt(bunboTempX1)*correctY1_2_sqrt));
  float rX2 = abs( bunsiX2/(sqrt(bunboTempX2)*correctY2_2_sqrt));
  
  //Serial.println(rX1);
  //Serial.println(rX2);
  
  if( rX1 > 0.7 && rX2 > 0.7  )
  {
    return true;
  }
  else
  {
    return false;
  }
}

//--------------------------------------------------------------------
//ArduinoSetup
//--------------------------------------------------------------------
#define SKIPOCOUNT 200
const int Led = 13;
const int N = 39;
int skipCount = 0;
volatile boolean isON = false;

//ContinuousADConvert
void ContinuousADConvert()
{
  //read adc0
  int sensorValue = analogRead(A0);
  //add ringbuffer
  AddRingBuffer(sensorValue);

  //debug
  //Serial.println(sensorValue);

  //skip 1回ONになったらすこし無視する  
  if( isON==true )
  {
    skipCount++;
    if( skipCount>SKIPOCOUNT )
    {
      skipCount=0;
      isON = false;
      digitalWrite(8, LOW);
      digitalWrite(13, LOW);      
    }
    return;
  }

  //Classfy
  CalcAve();
  if( Classfy1()==true )
  {
    digitalWrite(8, HIGH);
    digitalWrite(13, HIGH);
    isON = true;
  }
}

//Setup
void setup() {
  //Serial.begin(57600);
  pinMode(13, OUTPUT);
  pinMode(8,OUTPUT);

  //Hello world L tika!
  for( int i=0;i<2;i++ )
  {
    digitalWrite(8, HIGH);
    digitalWrite(13, HIGH);
    delay(300);
    digitalWrite(8, LOW);
    digitalWrite(13, LOW);
    delay(300);
  }

  //Timer2 interrupt 1ms
  MsTimer2::set(1, ContinuousADConvert);
  MsTimer2::start();
}

//Loop
void loop() {
  //nop
}
