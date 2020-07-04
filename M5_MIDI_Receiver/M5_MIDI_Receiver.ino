#include <M5Stack.h>
#include "ScrollTextWindow.h"

#define TOP_FIXED_HEIGHT        24  //上部の固定エリア高さ
#define BOTTOM_FIXED_HEIGHT     16  //下部の固定エリア高さ
#define TEXE_WIDTH  6 //フォント高さ
#define TEXT_HEIGHT  9 //フォント幅

ScrollTextWindow *stw;              //スクロールクラス

#include "BluetoothSerial.h"
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;
// キューの大きさ
#define QUEUE_LENGTH 256

// 通常のキュー
QueueHandle_t xQueue;

uint8_t data ;
int ret;

void task0(void* param) { //タスク0 MIDI担当
  while (1) {
    if (SerialBT.available()) {
      data = SerialBT.read();
      xQueueSend(xQueue, &data, 0);
      Serial2.write(data);
    }
  }
}

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
  M5.Lcd.drawString("M5Stack MIDI_Receiver", SCREEN_WIDTH / 2, TOP_FIXED_HEIGHT / 2);

  //下部固定エリア
  M5.Lcd.fillRect(0, SCREEN_HEIGHT - BOTTOM_FIXED_HEIGHT, SCREEN_WIDTH, BOTTOM_FIXED_HEIGHT, TFT_BLUE);
  M5.Lcd.drawString("@YoutechA320U", SCREEN_WIDTH / 2, SCREEN_HEIGHT - BOTTOM_FIXED_HEIGHT / 2);

  M5.Lcd.setTextDatum(TL_DATUM);
  M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);

  //スクロールクラスの初期化
  stw = new ScrollTextWindow(TOP_FIXED_HEIGHT, BOTTOM_FIXED_HEIGHT, TFT_BLACK, TEXE_WIDTH, TEXT_HEIGHT);

  //MIDIポートとBluetoothシリアル通信の設定
  Serial2.begin(31250);//シリアルポート2 ボーレート31250bps ボーレートを変えればMIDI以外のシリアル通信も送信できます
  SerialBT.begin("MIDI_Receiver"); //Bluetooth スレーブデバイス名

  xQueue = xQueueCreate( QUEUE_LENGTH, sizeof( uint8_t ) ); // キュー作成

  xTaskCreatePinnedToCore(task0, "Task0", 8192, NULL, 1, NULL, 0); //コア0で関数task0をstackサイズ8192,優先順位1で起動
  xTaskCreatePinnedToCore(task1, "Task1", 8192, NULL, 0, NULL, 1);  //コア1で関数task1をstackサイズ8192,優先順位0で起動
}

void loop() {
  delay(1); //タスク1も動けるようにする用
}
