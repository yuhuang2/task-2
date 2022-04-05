#include <Arduino.h>
#include <math.h>
#include <M5Core2.h>

float accX = 0.0F; // Define variables for storing inertial sensor data
float accY = 0.0F; //定义存储惯性传感器相关数据的相关变量
float accZ = 0.0F;

volatile uint8_t interruptFlag; // prepare the timer
uint64_t totalInterruptCounter;
hw_timer_t *timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR onTimer()
{
  portENTER_CRITICAL_ISR(&timerMux);
  interruptFlag = 1;
  portEXIT_CRITICAL_ISR(&timerMux);
}

void setup()
{
  M5.begin(); 
  M5.IMU.Init();  // Initialize the M5
  M5.IMU.SetAccelFsr(M5.IMU.AFS_4G);
  M5.Lcd.fillScreen(BLACK);         // Set the screen background color to black.
  M5.Lcd.setTextColor(BLUE, BLACK); //  Set the foreground color BLUE and background color BLACK
  M5.Lcd.setTextSize(2);            //  Set the text size.

  Serial.begin(115200);

  /*	function mame：timerBegin()
  the use of function：Timer initial，there are 3 paratmeter
    function input：1. timer（0到3，对应全部4个硬件定时器）
         2. 预分频器数值（ESP32计数器基频为80M，80分频单位是微秒）
         3. 计数器向上（true）或向下（false）计数的标志
      函数返回：一个指向 hw_timer_t 结构类型的指针*/
  timer = timerBegin(0, 80, true);

  //	函数名称：timerAttachInterrupt()
  //	函数功能：绑定定时器的中断处理函数，分别有三个参数
  //	函数输入：1. 指向已初始化定时器的指针（本例子：timer）
  //			 2. 中断服务函数的函数指针
  //			 3. 表示中断触发类型是边沿（true）还是电平（false）的标志
  //	函数返回：无
  timerAttachInterrupt(timer, &onTimer, true);

  //	函数名称：timerAlarmWrite()
  //	函数功能：指定触发定时器中断的计数器值，分别有三个参数
  //	函数输入：1. 指向已初始化定时器的指针（本例子：timer）
  //			 2. 第二个参数是触发中断的计数器值（1000000 us -> 1s）
  //			 3. 定时器在产生中断时是否重新加载的标志
  //	函数返回：无
  timerAlarmWrite(timer, 1000000, true);
  timerAlarmEnable(timer); //	使能定时器
}

void loop()
{
  // Serial.println("Looping");

  if (interruptFlag == 1)
  {
    interruptFlag = 0;
    M5.IMU.getAccelData(&accX, &accY, &accZ); // Stores the triaxial accelerometer.  存储三轴加速度计数据

    M5.Lcd.setCursor(0, 30);
    M5.Lcd.printf("accX,   accY,  accZ");
    M5.Lcd.setCursor(5, 52);
    M5.Lcd.printf("%5.2f  %5.2f  %5.2f G", accX, accY, accZ);
    M5.Lcd.setCursor(0, 80);
    M5.Lcd.printf("acceleration range  +-4G");
    M5.Lcd.setCursor(0, 100);
    M5.Lcd.printf("delay 1 sec");
    Serial.printf("%5.2f  %5.2f  %5.2f G\n", accX, accY, accZ);
  }

  // Serial.printf("accX: %1.2f, accY: %1.2f  accZ: %1.2f\n", accX, accY, accZ);
  // Delay 1s.
}
