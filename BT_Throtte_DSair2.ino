//////////////////////////////////////////////////////////////////////////////////
// 
// BleKeyboard DCC Throttle for DSair2 (R1.00)
// DSair2 >>> https://desktopstation.net/wiki/doku.php/dsair2_ukeng
// 
// Copyright(C)'2022-2023 - H.Matsuo
// This software is released under the MIT License, see LICENSE.txt
// 
// https://github.com/HMX-1972/BT_Throtte_DSair2.git
// 
// I use Arudino IDE with M5Stack 2.0.3 B.M.
// 
//////////////////////////////////////////////////////////////////////////////////

#include <Adafruit_SSD1306.h>
#include <RotaryEncoder.h>
#include <Adafruit_NeoPixel.h>
#include <BleKeyboard.h>
#include <Keypad.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET    -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

// BT Keyboard //
BleKeyboard bleKeyboard;

// OLED Display //
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define LED_PIN 2
#define Enc_1A_PIN 4
#define Enc_1B_PIN 19
#define SW_1_PIN 20

static boolean ValueChange = true;

const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

char KEYX[3] = {'_', '_'};
char KEYZ[3] = {'_', '_'};
char KEYA[3] = {'0', '_'};
char KEYB[3] = {'1', '_'};
char KEYC[3] = {'2', '_'};
char KEYD[3] = {'3', '_'};
char *INFO;
int z = 0;

byte rowPins[ROWS] = {6, 5, 18, 21}; // M5Stamp3の場合は他の組み合わせでは動作しない可能性あり
byte colPins[COLS] = {0, 1, 10, 7}; // M5Stamp3の場合は他の組み合わせでは動作しない可能性あり

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// create a pixel strand with 1 pixel on PIN_NEOPIXEL
Adafruit_NeoPixel pixels(1, LED_PIN);

// Encoder //
RotaryEncoder* encoder1 = nullptr;

IRAM_ATTR void checkPosition() {
  encoder1->tick(); // just call tick() to check the state.
}

void setup() {

  // Serial //
  // Serial.begin(115200);

  // OLED Display //
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    pixels.setPixelColor(0, pixels.Color(126, 0, 0));
    pixels.show();
    for (;;); // Don't proceed, loop forever
  } else {
    pixels.setPixelColor(0, pixels.Color(0, 0, 126));
    pixels.show();
  }
  display.clearDisplay();
  display.display();

  delay(100);

  // Encoder //  必ずOLEDより後に入れる。
  encoder1 = new RotaryEncoder(Enc_1A_PIN, Enc_1B_PIN, RotaryEncoder::LatchMode::TWO03);
  attachInterrupt(digitalPinToInterrupt(Enc_1A_PIN), checkPosition, CHANGE);
  attachInterrupt(digitalPinToInterrupt(Enc_1B_PIN), checkPosition, CHANGE);

  // Switch //
  pinMode( SW_1_PIN, INPUT_PULLUP );

  // bBleKeyboard //
  bleKeyboard.begin();

  // 配列 //
  INFO = "  ";
}

void readKeypad()
{
  char key = keypad.getKey();
  if (key != NO_KEY)
  {
    switch (key)
    {
      case 'A':
        if ( z > 0 ) {  // 入力値がある場合は代入して実行
          strcpy( KEYA, KEYX );   // KEYAに代入
          strcpy( KEYX, KEYZ );   // KEYXを初期化
          z = 0;                  // KEYXを初期化
        }
        SendBT(KEYA);
        break;
      case 'B':
        if ( z > 0 ) {
          strcpy( KEYB, KEYX );
          strcpy( KEYX, KEYZ );
          z = 0;
        }
        SendBT(KEYB);
        break;
      case 'C':
        if ( z > 0 ) {
          strcpy( KEYC, KEYX );
          strcpy( KEYX, KEYZ );
          z = 0;
        }
        SendBT(KEYC);
        break;
      case 'D':
        if ( z > 0 ) {
          strcpy( KEYD, KEYX );
          strcpy( KEYX, KEYZ );
          z = 0;
        }
        SendBT(KEYD);
        break;
      case '#': //　単発出力
        SendBT(KEYX);
        strcpy( KEYX, KEYZ );
        z = 0;
        break;
      case '*': //　方向切り替え
        SendBT("88");
        strcpy( KEYX, KEYZ );
        z = 0;
        break;
      default:  //　3桁目がある場合はバッファを初期化
        if (z < 2 ) {
          KEYX[z] = key;
          z++;
        } else {
          strcpy( KEYX, KEYZ );
          z = 0;
        }
    }
    ValueChange = true;  // LCD更新をセット
  }
}

void SendBT(char Cmd[])
{
  if (Cmd[0] == '9' && Cmd[1] == '9') {         // 99 -> ' ' 電源入り切り
    bleKeyboard.print(" ");
    INFO = "PWR";
  }  else if (Cmd[0] == '9' && Cmd[1] == '8') { // 98 -> ESP32C3ソフトウエアリセット
    ESP_Restart();
  }  else if (Cmd[0] == '8' && Cmd[1] == '8') { // 88 -> 'z' 方向変換
    bleKeyboard.print("z");
    INFO = "DIR";
  }  else if (Cmd[1] != '_') {                  //　DSair2のコードに合わせて出力
    if (Cmd[0] == '1') {
      bleKeyboard.press(KEY_LEFT_SHIFT);
      bleKeyboard.print(Cmd[1]);
      bleKeyboard.releaseAll();
      INFO = "FNC";
    } else if (Cmd[0] == '2') {
      bleKeyboard.press(KEY_LEFT_CTRL);
      bleKeyboard.print(Cmd[1]);
      bleKeyboard.releaseAll();
      INFO = "FNC";
    } else {  // 30以上は出力しない
      INFO = "---";
    }
  } else {
    bleKeyboard.print(Cmd[0]);
    INFO = "FNC";
  }
}

void loop() {

  // 変数定義 //
  static unsigned long SpeedMillis = millis();

  static int pos1 = 0;
  static int Ex_pos1 = 0;
  static int XSW1 = 0;
  static int BT_Flag = 0;

  static unsigned long elapsed_time = millis();

  //　状態取得 //
  encoder1->tick(); // just call tick() to check the state.
  int newPos1 = encoder1->getPosition();

  int SW1 = digitalRead(SW_1_PIN);
  // char key = keypad.getKey(); //押されたキーを返す

  if (bleKeyboard.isConnected() and BT_Flag == 0) {
    ValueChange = true;
    BT_Flag = 1;
  }

  // イベント処理 //

  // ダイアル
  if (Ex_pos1 != newPos1) {
    delay(50);
    encoder1->tick(); 
    newPos1 = encoder1->getPosition();
  }
  if (Ex_pos1 != newPos1) {
    if (bleKeyboard.isConnected()) {
      if ((newPos1 - Ex_pos1) > 3 ) {
        bleKeyboard.print("d");
        INFO = ">>>";
      } else if ((newPos1 - Ex_pos1) > 1 ) {
        bleKeyboard.print("s");
        INFO = " > ";
      } else if ((newPos1 - Ex_pos1) < -3 ) {
        bleKeyboard.print("c");
        INFO = "<<<";
      } else if ((newPos1 - Ex_pos1) < 1 ) {
        bleKeyboard.print("x");
        INFO = " < ";
      }
      bleKeyboard.releaseAll();
    }
    Ex_pos1 =  newPos1 ;
    ValueChange = true;
  }
  // Keypad //
  readKeypad();
  // SW //
  if (SW1 != XSW1) {
    if (SW1 == 0) {
      bleKeyboard.print("q");
      INFO = "EMR";
      ValueChange = true;
    }
    XSW1 = SW1;
  }

  // LCD //
  if (ValueChange) {
    Draw_LCD();
    ValueChange = false;
    elapsed_time = millis();
  } else {
    if (elapsed_time + 30000 < millis()) {
      Draw_LCD_OFF();
      elapsed_time = millis();
    }
  }
}

void Draw_LCD() {

  if (bleKeyboard.isConnected()) {

    pixels.setPixelColor(0, pixels.Color( 0, 126, 0)); // LEDを緑に
    pixels.show();

    display.clearDisplay() ;

    if (KEYX[0] != '_') {
      display.setCursor(0, 0);
      display.setTextSize(1);
      display.print("INPUT : ");
      display.setTextSize(2);
      Draw_FucNum( 52, 0, KEYX);
    }

    display.setCursor(110, 0);
    display.setTextSize(1);
    display.print(INFO);

    display.setTextSize(1);
    display.setCursor(0, 25);
    display.print("A");
    display.setCursor(32, 25);
    display.print("B");
    display.setCursor(64, 25);
    display.print("C");
    display.setCursor(96, 25);
    display.print("D");

    display.setTextSize(2);
    Draw_FucNum(  8, 17, KEYA);
    Draw_FucNum( 40, 17, KEYB);
    Draw_FucNum( 72, 17, KEYC);
    Draw_FucNum(104, 17, KEYD);
    display.display();

  } else {

    pixels.setPixelColor(0, pixels.Color( 126, 0, 0)); // LEDを赤に
    pixels.show();

    display.setTextSize(1);
    display.clearDisplay() ;
    display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
    display.setCursor(5, 5);
    display.print("BT Cont for DSair2");
    display.setCursor(23, 20);
    display.print("BT Not Connect");
    display.display();
  }
}

void Draw_FucNum(int x, int y, char KEY[3]) {
  if (KEY[1] == '_') {
    display.setCursor(x + 6, y);
    display.print(KEY[0]);
  } else {
    display.setCursor(x, y);
    display.print(KEY);
  }
}

void Draw_LCD_OFF() {
  display.clearDisplay();
  display.display();
}

void ESP_Restart() {
  display.clearDisplay() ;
  display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
  display.setTextSize(2);
  display.setCursor(5, 5);
  display.print("98:Reset");
  display.display();
  delay(300);
  ESP.restart();
}
