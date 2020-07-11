//This example code is in the Public Domain (or CC0 licensed, at your option.)
//By Victor Tchistiak - 2019
//
//This example demostrates master mode bluetooth connection and pin
//it creates a bridge between Serial and Classical Bluetooth (SPP)
//this is an extention of the SerialToSerialBT example by Evandro Copercini - 2018
//

#include <M5Stack.h>
#include "ScrollTextWindow.h"
#include "BluetoothSerial.h"

#define TOP_FIXED_HEIGHT        24  //上部の固定エリア高さ
#define BOTTOM_FIXED_HEIGHT     16  //下部の固定エリア高さ
#define TEXE_WIDTH  6 //フォント高さ
#define TEXT_HEIGHT  9 //フォント幅

ScrollTextWindow *stw;              //スクロールクラス

BluetoothSerial SerialBT;

//String MACadd = "84:0D:8E:1C:F7:7A";
//uint8_t address[6]  = {0x84, 0x0D, 0x8E, 0x1C, 0xF7, 0x7A};
//uint8_t address[6]  = {0x00, 0x1D, 0xA5, 0x02, 0xC3, 0x22};
String name = "MIDI_Receiver";
char *pin = "1234"; //<- standard pin would be provided by default
bool connected;

// キューの大きさ
#define QUEUE_LENGTH 256

// 通常のキュー
QueueHandle_t xQueue;

uint8_t data ;
int ret;

void task1(void* param) { //タスク1 描画担当
  while (1) {
    delay(1); //速すぎると描画できないので
    ret = xQueueReceive( xQueue, &data, 0 );
    if ( ret != 0 ) {
      stw->print(data);
      stw->print(" "); //空白 お好みで
      //stw->print("\r\n"); //改行 お好みで
    }
  }
}

void setup() {
  disableCore0WDT(); //コア0ウォッチドッグタイマー停止
  disableCore1WDT(); //コア1ウォッチドッグタイマー停止
  M5.begin();
  M5.Lcd.fillScreen(TFT_BLACK);

  //固定エリアの描画
  M5.Lcd.setTextDatum(MC_DATUM);
  M5.Lcd.setTextColor(TFT_WHITE, TFT_BLUE);

  //上部固定エリア
  M5.Lcd.fillRect(0, 0, SCREEN_WIDTH, TOP_FIXED_HEIGHT, TFT_BLUE);
  M5.Lcd.drawString("M5Stack MIDI_Transmitter", SCREEN_WIDTH / 2, TOP_FIXED_HEIGHT / 2);

  //下部固定エリア
  M5.Lcd.fillRect(0, SCREEN_HEIGHT - BOTTOM_FIXED_HEIGHT, SCREEN_WIDTH, BOTTOM_FIXED_HEIGHT, TFT_BLUE);
  M5.Lcd.drawString("@YoutechA320U", SCREEN_WIDTH / 2, SCREEN_HEIGHT - BOTTOM_FIXED_HEIGHT / 2);

  M5.Lcd.setTextDatum(TL_DATUM);
  M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);

  //スクロールクラスの初期化
  stw = new ScrollTextWindow(TOP_FIXED_HEIGHT, BOTTOM_FIXED_HEIGHT, TFT_BLACK, TEXE_WIDTH, TEXT_HEIGHT);
  Serial2.begin(31250);
  Serial2.setRxBufferSize(100000);
  SerialBT.begin("MIDI_Transmitter", true);

  // connect(address) is fast (upto 10 secs max), connect(name) is slow (upto 30 secs max) as it needs
  // to resolve name to address first, but it allows to connect to different devices with the same name.
  // Set CoreDebugLevel to Info to view devices bluetooth address and device names
  connected = SerialBT.connect(name);
  //connected = SerialBT.connect(address);

  if (connected) {
  } else {
    while (!SerialBT.connected(10000)) {
    }
  }
  // disconnect() may take upto 10 secs max
  if (SerialBT.disconnect()) {
  }
  // this would reconnect to the name(will use address, if resolved) or address used with connect(name/address).
  SerialBT.connect();

  xQueue = xQueueCreate( QUEUE_LENGTH, sizeof( uint8_t ) ); // キュー作成

  xTaskCreatePinnedToCore(task1, "Task1", 8192, NULL, 1, NULL, 1);  //コア1で関数task1をstackサイズ8192,優先順位0で起動
}

void loop() {
    if (Serial2.available()) {
      SerialBT.write(data = Serial2.read());
      xQueueSend(xQueue, &data, 0);
    }
}
