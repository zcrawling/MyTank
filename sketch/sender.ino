#include <RF24.h>
#include <RF24_config.h>
#include <nRF24L01.h>
#include <printf.h>

#include <SoftwareSerial.h>

#define WINDOW_SIZE 30
#define INPUT_RECOVERY_TIME 1000
#define Z_THRESHOLD_F 0.3    // Z-score Forward, Right
#define Z_THRESHOLD_B 0.3   // Z-score Back, Left
#define Z_THRESHOLD_R 0.5   // Z-score Back, Left
#define Z_THRESHOLD_L 0.07    // Z-score Back, Left
#define FORWARD 1
#define BACKWARD 2
#define LEFTWARD 3
#define RIGHTWARD 4
#define JOYSTIC_X A1
#define JOYSTIC_Y A0
#define BT_RXD 8
#define BT_TXD 7

SoftwareSerial bluetooth(BT_RXD, BT_TXD);


class aMath
{
  public:
    static float sumVec(int* vec, int size)
    {
      float ret = 0;
      for (int i = 0; i < size; i++) ret += vec[i];
      return ret;
    }

    static float avgVec(int* vec, int size)
    {
      return sumVec(vec, size) / size;
    }

    static float meanSquareVec(int* vec, int size)
    {
      float ret = 0;
      for (int i = 0; i < size; i++) ret += vec[i] * vec[i];
      return ret / size;
    }

    static float zScore(float val, float mean, float std)
    {
      return fabs((val - mean) / std);
    }
};
class Joystic
{
  private:
    int xVec[WINDOW_SIZE];
    int yVec[WINDOW_SIZE];
    int xInput;
    int yInput;
    float xMean;
    float yMean;
    float xMeanSquare;
    float yMeanSquare;
    float xSD = 100.0;
    float ySD = 100.0;
    short location;
  public:
    void read()
    {
      xInput = analogRead(JOYSTIC_X);
      yInput = analogRead(JOYSTIC_Y);
    }

    void setup()
    {
      for (int i = 0; i < WINDOW_SIZE; i++)
      {
        xVec[i] = analogRead(JOYSTIC_X);
        yVec[i] = analogRead(JOYSTIC_Y);
      }
      xMean = aMath::avgVec(xVec, WINDOW_SIZE);
      yMean = aMath::avgVec(yVec, WINDOW_SIZE);
      xMeanSquare = aMath::meanSquareVec(xVec, WINDOW_SIZE);
      yMeanSquare = aMath::meanSquareVec(yVec, WINDOW_SIZE);
      xSD = sqrt(fabs(xMeanSquare - xMean * xMean));
      ySD = sqrt(fabs(yMeanSquare - yMean * yMean));
      location = 0;
    }

    void update()
    {
      xMean = xMean + (xInput - xVec[location]) / WINDOW_SIZE;
      yMean = yMean + (yInput - yVec[location]) / WINDOW_SIZE;
      xMeanSquare = xMeanSquare + (xInput * xInput - xVec[location] * xVec[location]) / WINDOW_SIZE;
      yMeanSquare = yMeanSquare + (yInput * yInput - yVec[location] * yVec[location]) / WINDOW_SIZE;
      xVec[location] = xInput;
      yVec[location] = yInput;
      xSD = sqrt(fabs(xMeanSquare - xMean * xMean)); //분산 >=0이나, 부동소수점 연산으로 음수가 될 수 있으므로 fabs사용
      ySD = sqrt(fabs(yMeanSquare - yMean * yMean));
      location = (location+1) % WINDOW_SIZE;
    }

    int detect()
    {
      read();
      int direction = 0;
      if (aMath::zScore(xInput, xMean, xSD) > Z_THRESHOLD_F && (xInput < xMean))  // 휴리스틱.
        direction = FORWARD;
      else if (aMath::zScore(xInput, xMean, xSD) > Z_THRESHOLD_B && (xInput > xMean))
        direction = BACKWARD;
      else if (aMath::zScore(yInput, yMean, ySD) > Z_THRESHOLD_L && (yInput < yMean))
        direction = LEFTWARD;
      else if (aMath::zScore(yInput, yMean, ySD) > Z_THRESHOLD_R && (yInput > yMean))
        direction = RIGHTWARD;
      else
      {
        update();
        return direction;
      }
      delay(INPUT_RECOVERY_TIME);
      setup();
      return direction;
    }
};

Joystic joystic;

void setup() {
  bluetooth.begin(38400);
  Serial.begin(9600);
  joystic.setup();
}

void loop() {
if(bluetooth.available()) Serial.write(bluetooth.read());
if(Serial.available())bluetooth.write(Serial.read());
  // int i = joystic.detect();
  // if(i != 0)
  // {
  //   if(bluetooth.available())
  //   {
  //     bluetooth.write(i);
  //   }
  // }
}