# M5_MIDI_Transceiver
M5Stackで動作するBluetoothシリアル通信のトランスミッターとレシーバーのプログラムです。

[M5StackMIDIモジュール](https://www.switch-science.com/catalog/6389/)を使ってMIDIを受送信するようになっています。binファイルは[SD-Updater](https://github.com/tobozo/M5Stack-SD-Updater/),[LovyanLauncher](https://github.com/lovyan03/M5Stack_LovyanLauncher)に対応しています

[espressif/arduino-esp32/libraries/BluetoothSerial/のサンプルスケッチ](https://github.com/espressif/arduino-esp32/tree/master/libraries/BluetoothSerial)がベースです。親機、子機のデバイス名を変更したり接続方法を変更する場合はこちらを参照してください。

## 備考
Bluetoothシリアル通信を経由する都合上通常のMIDIより動作が重くなりやすいです。一度にたくさんのセットアップメッセージを入出力すると取りこぼしが発生する可能性があります。

## 必要なライブラリ
動作には以下のライブラリが必要です。  
* https://github.com/totsucom/M5Stack_ScrollTextWindow