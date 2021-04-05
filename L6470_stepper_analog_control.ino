//Motor Stepper for L6470 and SM-42BYG011-25
//for Arduino Uno and Nano (Every)
//
//SPI
//A5(Speed Input)

//Reference
//http://spinelify.blog.fc2.com/blog-entry-41.html


#include <SPI.h>

//define Pins
#define PIN_SPI_MOSI 11 //L6470 MOSI
#define PIN_SPI_MISO 12 //L6470 MISO
#define PIN_SPI_SCK 13 //L6470 Clock
#define PIN_SPI_SS 10 //L6470 Chip Select

//Motor Parameters
#define Motor_Spd_In A5     // the number of the pushbutton pin
#define MaxSpd 120

int Spd = 0;
SPISettings Settings = SPISettings(4000000, MSBFIRST, SPI_MODE3);

void setup()
{
  //Set pins
  pinMode(PIN_SPI_MOSI, OUTPUT);
  pinMode(PIN_SPI_MISO, INPUT);
  pinMode(PIN_SPI_SCK, OUTPUT);
  pinMode(PIN_SPI_SS, OUTPUT);
  digitalWrite(PIN_SPI_SS, HIGH);

  //5V out
  pinMode(A0, OUTPUT);
  digitalWrite(A0, HIGH);

  //SPI
  SPI.begin();

  //Serial
  Serial.begin(115200);
  SPI.setDataMode(SPI_MODE3);
  SPI.setBitOrder(MSBFIRST);

  //Command Clear
  L6470_send(0x00);
  L6470_send(0x00);
  L6470_send(0x00);
  L6470_send(0x00);

  //Reset Command
  L6470_send(0xc0);//ResetRevice
  L6470_setup();//L6470を設定
}

void loop() {
  Spd = map(analogRead(Motor_Spd_In), 0, 1023, -MaxSpd, MaxSpd);
  if (abs(Spd) > 3) {
    L6470_move(Spd);
  } else {
    L6470_stop();
    Serial.print("stop!");
  }
  Serial.println(Spd);
}

void L6470_move(int ispeed) {
  if (ispeed > 0) {
    L6470_send(0x51);//Run(DIR,SPD),0x51:CW,0x50:CCW
  } else {
    L6470_send(0x50);//Run(DIR,SPD),0x51:CW,0x50:CCW
  }
  L6470_send(0x00);//SPD(20bit)
  L6470_send(abs(ispeed));
  L6470_send(0x00);
}

void L6470_stop() {
  L6470_send(0xB0);//SoftStop
}

void L6470_send(unsigned char add_or_val) {
  SPI.beginTransaction(Settings);
  digitalWrite(PIN_SPI_SS, LOW);
  SPI.transfer(add_or_val); // transfer address or data
  digitalWrite(PIN_SPI_SS, HIGH);
  SPI.endTransaction();
}

void L6470_setup() {
  //最大回転スピード このあとにスピードデータを送信してもこの値がリミットとなる。
  L6470_send(0x07);//レジスタアドレス
  L6470_send(0x41);//値(10bit),デフォルト0x41

  //L6470_send(0x30);

  //モータ停止中の電圧設定
  L6470_send(0x09);//レジスタアドレス
  L6470_send(0x29);//値(8bit),デフォルト0x29

  //モータ定速回転時の電圧設定A
  L6470_send(0x0a);//レジスタアドレス
  L6470_send(0xC0);//値(8bit),デフォルト0x29

  //加速中の電圧設定
  L6470_send(0x0b);//レジスタアドレス
  L6470_send(0xC0);//値(8bit),デフォルト0x29

  //減速中の電圧設定
  L6470_send(0x0c);//レジスタアドレス
  L6470_send(0xC0);//値(8bit),デフォルト0x29

  //加速係数
  L6470_send(0x05);//レジスタアドレス
  L6470_send(0x00);//値(12bit),デフォルト0x08A
  L6470_send(0x80);//値(12bit),デフォルト0x08A

  //減速係数
  L6470_send(0x06);//レジスタアドレス
  L6470_send(0x00);//値(12bit),デフォルト0x08A
  L6470_send(0x80);//値(12bit),デフォルト0x08A

  //フ ル ス テ ッ プ,ハ ー フ ス テ ッ プ,1/4, 1/8,…,1/128 ステップの設定
  L6470_send(0x16);//レジスタアドレス
  L6470_send(0x07);//値(8bit),デフォルト0x07
}
