/*
  Proyek Akhir Sistem Mikrokontroler
  Judul   : Pendeteksi Angin Kencang untuk Sistem Penutup Otomatis
  Board   : ESP32 DevKit V1
  Sensor  : Potentiometer sebagai simulasi sensor kecepatan angin
  Aktuator: Servo SG90, LED indikator, dan buzzer
  IoT     : ThingSpeak
  Protokol: HTTP GET
*/

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ESP32Servo.h>

// =======================
// KONFIGURASI PIN ESP32
// =======================
const int WIND_SENSOR_PIN = 34;   // Potentiometer SIG ke GPIO34
const int SERVO_PIN       = 18;   // Servo signal ke GPIO18
const int LED_GREEN_PIN   = 14;   // LED hijau
const int LED_YELLOW_PIN  = 26;   // LED kuning (menghindari strapping pin GPIO12)
const int LED_RED_PIN     = 13;   // LED merah
const int BUZZER_PIN      = 27;   // Buzzer
const int BUZZER_PWM_CHANNEL = 14; // Dipisahkan dari LEDC channel servo

// =======================
// KONFIGURASI WIFI WOKWI
// =======================
const char* WIFI_SSID     = "Wokwi-GUEST";
const char* WIFI_PASSWORD = "";

// =======================
// KONFIGURASI THINGSPEAK
// =======================
#define THINGSPEAK_WRITE_API_KEY "BM4BIX89CZH1C60G"
const char* thingSpeakServer = "https://api.thingspeak.com/update";

// =======================
// KONFIGURASI SISTEM
// =======================
const float MAX_WIND_SPEED = 60.0;  // Kecepatan maksimum simulasi, km/jam

const float SAFE_LIMIT     = 25.0;  // Di bawah ini: aman
const float DANGER_LIMIT   = 35.0;  // Di atas/sama dengan ini: bahaya
const float REOPEN_LIMIT   = 28.0;  // Hysteresis: buka lagi jika turun <= 28

const int SERVO_OPEN_ANGLE   = 0;   // Penutup terbuka
const int SERVO_CLOSED_ANGLE = 90;  // Penutup tertutup

const unsigned long SENSOR_INTERVAL     = 500;    // Baca sensor tiap 0.5 detik
const unsigned long THINGSPEAK_INTERVAL = 20000;  // Kirim data tiap 20 detik
const unsigned long WIFI_TIMEOUT         = 10000;  // Maksimum menunggu WiFi 10 detik

// =======================
// VARIABEL GLOBAL
// =======================
Servo coverServo;

enum SystemState {
  SAFE,
  WARNING,
  DANGER
};

SystemState currentState = SAFE;

int rawAdc = 0;
float windSpeed = 0.0;

int servoAngle = SERVO_OPEN_ANGLE;
int lastServoAngle = -1;
int alarmState = 0;
int lastAlarmState = -1;

unsigned long lastSensorReadTime = 0;
unsigned long lastThingSpeakSendTime = 0;

// Deklarasi fungsi diperlukan karena berkas PlatformIO memakai ekstensi .cpp.
bool connectWiFi(unsigned long timeoutMs);
void readWindSensor();
SystemState calculateSystemState(SystemState previousState, float speed);
void determineSystemState();
bool runLogicSelfTest();
void updateActuators();
void printSystemStatus();
void sendDataToThingSpeak();
String getStatusText();
int getStatusCode();

// =======================
// SETUP
// =======================
void setup() {
  Serial.begin(115200);

  pinMode(LED_GREEN_PIN, OUTPUT);
  pinMode(LED_YELLOW_PIN, OUTPUT);
  pinMode(LED_RED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  // tone() default-nya memakai LEDC channel 0, sama dengan servo pertama.
  setToneChannel(BUZZER_PWM_CHANNEL);

  digitalWrite(LED_GREEN_PIN, LOW);
  digitalWrite(LED_YELLOW_PIN, LOW);
  digitalWrite(LED_RED_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);

  // Konfigurasi ADC ESP32
  analogReadResolution(12); // ADC 12-bit: 0 sampai 4095
  analogSetPinAttenuation(WIND_SENSOR_PIN, ADC_11db);

  // Konfigurasi servo
  coverServo.setPeriodHertz(50); // Servo standar 50 Hz
  coverServo.attach(SERVO_PIN, 500, 2400);
  coverServo.write(SERVO_OPEN_ANGLE);

  if (!coverServo.attached()) {
    Serial.println("PERINGATAN: Servo gagal terhubung ke kanal PWM.");
  }

  Serial.println("==============================================");
  Serial.println("PROYEK AKHIR SISTEM MIKROKONTROLER");
  Serial.println("Pendeteksi Angin Kencang untuk Penutup Otomatis");
  Serial.println("Board: ESP32 | IoT: ThingSpeak | Protokol: HTTPS");
  Serial.println("==============================================");

  runLogicSelfTest();

  WiFi.setAutoReconnect(true);
  connectWiFi(WIFI_TIMEOUT);

  Serial.println("Sistem siap berjalan.");
  Serial.println();
}

// =======================
// LOOP UTAMA
// =======================
void loop() {
  unsigned long currentTime = millis();

  if (currentTime - lastSensorReadTime >= SENSOR_INTERVAL) {
    lastSensorReadTime = currentTime;

    readWindSensor();
    determineSystemState();
    updateActuators();
    printSystemStatus();
  }

  if (currentTime - lastThingSpeakSendTime >= THINGSPEAK_INTERVAL) {
    lastThingSpeakSendTime = currentTime;
    sendDataToThingSpeak();
  }
}

// =======================
// KONEKSI WIFI
// =======================
bool connectWiFi(unsigned long timeoutMs) {
  if (WiFi.status() == WL_CONNECTED) {
    return true;
  }

  Serial.print("Menghubungkan ke WiFi");

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD, 6);
  unsigned long startTime = millis();

  while (WiFi.status() != WL_CONNECTED && millis() - startTime < timeoutMs) {
    delay(300);
    Serial.print(".");
  }

  Serial.println();

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi belum terhubung. Sistem lokal tetap berjalan.");
    return false;
  }

  Serial.println("WiFi terhubung.");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  return true;
}

// =======================
// BACA SENSOR ANGIN
// =======================
void readWindSensor() {
  rawAdc = analogRead(WIND_SENSOR_PIN);

  // Konversi ADC 0-4095 menjadi kecepatan angin 0-60 km/jam
  windSpeed = (rawAdc / 4095.0) * MAX_WIND_SPEED;
}

// =======================
// LOGIKA STATUS SISTEM
// =======================
SystemState calculateSystemState(SystemState previousState, float speed) {
  if (previousState == DANGER) {
    if (speed <= REOPEN_LIMIT) {
      if (speed < SAFE_LIMIT) {
        return SAFE;
      } else {
        return WARNING;
      }
    }
    return DANGER;
  } else {
    if (speed >= DANGER_LIMIT) {
      return DANGER;
    } else if (speed >= SAFE_LIMIT) {
      return WARNING;
    } else {
      return SAFE;
    }
  }
}

void determineSystemState() {
  currentState = calculateSystemState(currentState, windSpeed);
}

bool runLogicSelfTest() {
  struct TestCase {
    SystemState previousState;
    float speed;
    SystemState expectedState;
  };

  const TestCase cases[] = {
    {SAFE, 0.0, SAFE},
    {SAFE, 24.9, SAFE},
    {SAFE, 25.0, WARNING},
    {WARNING, 34.9, WARNING},
    {WARNING, 35.0, DANGER},
    {DANGER, 30.0, DANGER},
    {DANGER, 28.0, WARNING},
    {DANGER, 24.0, SAFE}
  };

  bool passed = true;
  for (const TestCase& testCase : cases) {
    if (calculateSystemState(testCase.previousState, testCase.speed) != testCase.expectedState) {
      passed = false;
    }
  }

  Serial.print("Self-test logika: ");
  Serial.println(passed ? "LULUS (8/8)" : "GAGAL");
  return passed;
}

// =======================
// UPDATE AKTUATOR
// =======================
void updateActuators() {
  if (currentState == SAFE) {
    servoAngle = SERVO_OPEN_ANGLE;
    alarmState = 0;

    digitalWrite(LED_GREEN_PIN, HIGH);
    digitalWrite(LED_YELLOW_PIN, LOW);
    digitalWrite(LED_RED_PIN, LOW);

  }

  else if (currentState == WARNING) {
    servoAngle = SERVO_OPEN_ANGLE;
    alarmState = 0;

    digitalWrite(LED_GREEN_PIN, LOW);
    digitalWrite(LED_YELLOW_PIN, HIGH);
    digitalWrite(LED_RED_PIN, LOW);

  }

  else if (currentState == DANGER) {
    servoAngle = SERVO_CLOSED_ANGLE;
    alarmState = 1;

    digitalWrite(LED_GREEN_PIN, LOW);
    digitalWrite(LED_YELLOW_PIN, LOW);
    digitalWrite(LED_RED_PIN, HIGH);

  }

  if (servoAngle != lastServoAngle) {
    coverServo.write(servoAngle);
    lastServoAngle = servoAngle;
  }

  if (alarmState != lastAlarmState) {
    if (alarmState == 1) {
      tone(BUZZER_PIN, 1000);
    } else {
      noTone(BUZZER_PIN);
    }
    lastAlarmState = alarmState;
  }
}

// =======================
// CETAK STATUS SERIAL
// =======================
void printSystemStatus() {
  Serial.print("ADC: ");
  Serial.print(rawAdc);

  Serial.print(" | Kecepatan Angin: ");
  Serial.print(windSpeed, 1);
  Serial.print(" km/jam");

  Serial.print(" | Status: ");
  Serial.print(getStatusText());

  Serial.print(" | Servo: ");
  Serial.print(servoAngle);
  Serial.print(" derajat");

  Serial.print(" | Alarm: ");
  Serial.println(alarmState == 1 ? "AKTIF" : "MATI");
}

// =======================
// KIRIM DATA KE THINGSPEAK
// =======================
void sendDataToThingSpeak() {
  if (strlen(THINGSPEAK_WRITE_API_KEY) == 0) {
    Serial.println("ThingSpeak belum dikonfigurasi. Isi THINGSPEAK_WRITE_API_KEY di include/secrets.h.");
    return;
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi terputus. Mencoba menghubungkan ulang...");
    if (!connectWiFi(WIFI_TIMEOUT)) {
      Serial.println("Pengiriman ThingSpeak dilewati; akan dicoba lagi pada interval berikutnya.");
      return;
    }
  }

  int statusCode = getStatusCode();

  String url = String(thingSpeakServer);
  url += "?api_key=" + String(THINGSPEAK_WRITE_API_KEY);
  url += "&field1=" + String(windSpeed, 1);
  url += "&field2=" + String(statusCode);
  url += "&field3=" + String(servoAngle);
  url += "&field4=" + String(alarmState);

  WiFiClientSecure secureClient;
  secureClient.setInsecure(); // Simulasi Wokwi: TLS tanpa verifikasi sertifikat CA

  HTTPClient http;
  http.setConnectTimeout(10000);
  http.setTimeout(10000);

  if (!http.begin(secureClient, url)) {
    Serial.println("Kirim ThingSpeak gagal | Tidak dapat memulai koneksi HTTPS.");
    return;
  }

  int httpResponseCode = http.GET();

  Serial.print("Kirim ThingSpeak | HTTP Code: ");
  Serial.print(httpResponseCode);

  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.print(" | Response: ");
    Serial.println(response);
  } else {
    Serial.print(" | Error: ");
    Serial.println(http.errorToString(httpResponseCode));
  }

  http.end();
}

// =======================
// HELPER: STATUS TEXT
// =======================
String getStatusText() {
  if (currentState == SAFE) {
    return "AMAN";
  } else if (currentState == WARNING) {
    return "WASPADA";
  } else {
    return "BAHAYA";
  }
}

// =======================
// HELPER: STATUS CODE
// =======================
int getStatusCode() {
  if (currentState == SAFE) {
    return 0;
  } else if (currentState == WARNING) {
    return 1;
  } else {
    return 2;
  }
}
