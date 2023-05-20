//            
//          CustomMouseLights
//           By MagAcademy30
//          Firmawe to ESP32.
//
#include "BluetoothSerial.h" // Импорт Блютуз библиотеки

// Настройки
#define VERSION "0.1v By MagAcademy30" // Версия прошивки
#define BT_NAME "CML_BL" // Имя в сети блютуз устроиств
#define BT_PASS "0000" // Пароль от блютуза, не работает пока.
#define WL_DEFSPEED 5 // Стандартная скорость колеса мыши
#define LG_DEFSPEED 5 // Стандартная скорость лого/корпуса мыши
#define DEFAULT_MODE 2 // Стандартный эффект (1 - Пульс, 2 - "Дыхание", 3 - Сплошной цвет)
#define ST_PULSE 3 // Стандартный цвет для пульса (1 = Красный(Лого/Корпус), 2 = Зелёный(Лого/Корпус), 3 = Синий(Лого/Корпус))
#define DEFAULT_R 0 // Стандартный красный для сплошного цвета
#define DEFAULT_G 0 // Стандартный зелёный для сплошного цвета
#define DEFAULT_B 255 // Стандартный синий для сплошного цвета


#define freq 5000 // --
#define resolution 8 // --
#define WL_CH 0 // Канал для колеса
#define R_CH 1 // Канал для ргб
#define G_CH 2 // Канал для ргб
#define B_CH 3 // Канал для ргб

// Пины
#define WL_PIN 32 // Пин светодиода колеса мыши
#define R_PIN 25 // R пин ргб светодиода
#define G_PIN 26 // G пин ргб светодиода
#define B_PIN 27 // B пин ргб светодиода

// Не изменять
int WL_TEMP;
int R_TEMP;
int G_TEMP;
int B_TEMP;
bool WL_TEMPBL;
bool R_TEMPBL;
bool G_TEMPBL;
bool B_TEMPBL;
int NEED = 1;
int R = DEFAULT_R;
int G = DEFAULT_G;
int B = DEFAULT_B;
int TH_PL = ST_PULSE;
int MODE = DEFAULT_MODE;
int WLSP = WL_DEFSPEED;
int LGSP = LG_DEFSPEED;
unsigned long swim_time, pulse_time, wl_time;

// Инит библиотек
BluetoothSerial mon;


void SetColor(int RC, int GC, int BC) {
  R_TEMP = RC;
  G_TEMP = GC;
  B_TEMP = BC;
}

void Pulse(int ch, int funcdelay) {
  int tempval = 0; bool pulsereverse;
  if (ch == 1) {tempval = R_TEMP; pulsereverse = R_TEMPBL;}
  if (ch == 2) {tempval = G_TEMP; pulsereverse = G_TEMPBL;}
  if (ch == 3) {tempval = B_TEMP; pulsereverse = B_TEMPBL;}
  if (tempval >= 255) {pulsereverse = 1;}
  if (tempval <= 0) {pulsereverse = 0;}
  if (millis() - pulse_time > funcdelay) {
    pulse_time = millis();
    if (pulsereverse) {tempval--;}
    if (!pulsereverse) {tempval++;}
  }
  if (ch == 1) {R_TEMP = tempval; R_TEMPBL = pulsereverse;}
  if (ch == 2) {G_TEMP = tempval; G_TEMPBL = pulsereverse;}
  if (ch == 3) {B_TEMP = tempval; B_TEMPBL = pulsereverse;}
}

void Wheel(int funcdelay) {
  int tempval = 0; bool pulsereverse;
  tempval = WL_TEMP; pulsereverse = WL_TEMPBL;
  if (tempval >= 255) {pulsereverse = 1;}
  if (tempval <= 0) {pulsereverse = 0;}
  if (millis() - wl_time > funcdelay) {
    wl_time = millis();
    if (pulsereverse) {tempval--;}
    if (!pulsereverse) {tempval++;}
  }
  WL_TEMP = tempval; WL_TEMPBL = pulsereverse;
}

void Swim(int funcdelay) {
  if (millis() - swim_time > funcdelay) {
    swim_time = millis();
    if (R_TEMP <= 255) {
      if (NEED == 1) {
        if (B_TEMP != 0) {B_TEMP--;}
        R_TEMP++;
        Serial.println("RED");
      }
    } else {NEED = 2;}
    if (G_TEMP <= 255) {
      if (NEED == 2) {
        if (R_TEMP != 0) {R_TEMP--;}
        G_TEMP++;
        Serial.println("GREEN");
      }
    } else {NEED = 3;}
    if (B_TEMP <= 255) {
      if (NEED == 3) {
        if (G_TEMP != 0) {G_TEMP--;}
        B_TEMP++;
        Serial.println("BLUE");
      }
    } else {NEED = 1;}
  }
}

void UpdateLed() {
  ledcWrite(WL_CH, WL_TEMP);
  ledcWrite(R_CH, R_TEMP);
  ledcWrite(G_CH, G_TEMP);
  ledcWrite(B_CH, B_TEMP);
}

void ClearLedInt() {
  WL_TEMP = 0;
  R_TEMP = 0;
  G_TEMP = 0;
  B_TEMP = 0;
}

void EffectsTick() {
  if (MODE == 1) {
    Pulse(TH_PL, LGSP);
  }
  if (MODE == 2) {
    Swim(LGSP);
  }
  if (MODE == 3) {
    SetColor(R, G, B);
  }
  Wheel(WLSP);
  UpdateLed();
}

void BluetoothTick() {
  if (mon.available() > 0) {
    char recv = mon.read();
    int rint = mon.parseInt();
    switch (recv) {
      case 'h':
        mon.print("CustomMouseLights ");
        mon.println(VERSION);
        mon.println("comma:");
        mon.println(" h - help");
        mon.println(" m<val> - Mode (1 - Pulse, 2 - Swim, 3 - JCM(JustColorMode)");
        mon.println(" t<VAL> - Standard color for pulse(1 - R, 2 - G, 3 - B)");
        mon.println(" r<1-255> - R color for JCM");
        mon.println(" g<1-255> - G color for JCM");
        mon.println(" b<1-255> - B color for JCM");
        mon.println(" k - Wheel speed");
        mon.println(" l - Logo speed");
        mon.println(" f - Reboot ESP32");
        break;
      case 'm':
        MODE = rint;
        ClearLedInt();
        mon.println("\n OK");
        break;
      case 't':
        TH_PL = rint;
        ClearLedInt();
        break;
      case 'r':
        R = rint;
        mon.println("\n OK");
        break;
      case 'g':
        G = rint;
        mon.println("\n OK");
        break;
      case 'b':
        B = rint;
        mon.println("\n OK");
        break;
      case 'k':
        WLSP = rint;
        mon.println("\n OK");
        break;
      case 'l':
        LGSP = rint;
        mon.println("\n OK");
        break;
      case 'f':
        ESP.restart();
        break;
    }
  }
}

void setup() {
  Serial.begin(115200);
  mon.begin(BT_NAME);
  
  ledcSetup(WL_CH, freq, resolution);
  ledcSetup(R_CH, freq, resolution);
  ledcSetup(G_CH, freq, resolution);
  ledcSetup(B_CH, freq, resolution);
  ledcAttachPin(WL_PIN, WL_CH);
  ledcAttachPin(R_PIN, R_CH);
  ledcAttachPin(G_PIN, G_CH);
  ledcAttachPin(B_PIN, B_CH);
  ClearLedInt();
}

void loop() {
  BluetoothTick();
  EffectsTick();
}
