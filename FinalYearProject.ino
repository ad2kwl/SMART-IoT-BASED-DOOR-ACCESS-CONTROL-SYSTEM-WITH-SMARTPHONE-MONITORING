#define BLYNK_TEMPLATE_ID "TMPL4A8AB1dHg"
#define BLYNK_TEMPLATE_NAME "Door Unlock"
#define BLYNK_AUTH_TOKEN "1V7etlitliI6Ee5w_XOiKL9tF7xnX1xM"
#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <Keypad.h>

// WiFi credentials
char ssid[] = "ADs iphon3";
char pass[] = "Alice@468";

// Nano ESP32 pin
#define BUZZER_PIN D10

// Passive buzzer settings
#define BUZZER_CHANNEL 0
#define BUZZER_FREQ 1000
#define BUZZER_RES 8

// Keypad setup
const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
  {'C','D','E','F'},
  {'8','9','A','B'},
  {'4','5','6','7'},
  {'0','1','2','3'}
};

byte rowPins[ROWS] = {D2, D3, D4, D5};
byte colPins[COLS] = {D6, D7, D8, D9};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Current input
String input = "";

// Lockout settings
int attempts = 0;
bool lockedOut = false;
unsigned long lockoutStart = 0;
const unsigned long LOCKOUT_DURATION = 30000; // 30 seconds

// Input timeout
unsigned long lastKeyPress = 0;
const unsigned long INPUT_TIMEOUT = 5000; // 5 seconds

// Unlock counter
int unlockCount = 0;

// Blynk reconnect timer
unsigned long lastBlynkReconnectAttempt = 0;

// System status
String doorStatus = "Locked";

// Last user
String lastUser = "None";

// ---------- USER PASSWORDS ----------
const String USER1_NAME = "Alison";
const String USER1_PASS = "2235";

const String USER2_NAME = "User 2";
const String USER2_PASS = "3355";

const String ADMIN_NAME = "Admin";
const String ADMIN_PASS = "5555";
// ------------------------------------

// Forward declarations
void beepClear();
void beepSuccess();
void beepError();
void beepStartup();
void unlockDoor(String source, String userName);
void checkPassword();
void updateBlynkStatus();

void setup() {
  Serial.begin(115200);
  delay(500);

  Serial.println("Starting...");

  // Passive buzzer setup
  ledcSetup(BUZZER_CHANNEL, BUZZER_FREQ, BUZZER_RES);
  ledcAttachPin(BUZZER_PIN, BUZZER_CHANNEL);
  ledcWrite(BUZZER_CHANNEL, 0);

  // Startup sound
  beepStartup();

  // WiFi setup
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, pass);

  unsigned long wifiStart = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - wifiStart < 10000) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.println("WiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    Blynk.config(BLYNK_AUTH_TOKEN);

    Serial.print("Connecting to Blynk");
    if (Blynk.connect(5000)) {
      Serial.println();
      Serial.println("Blynk connected");
    } else {
      Serial.println();
      Serial.println("Blynk not connected, will retry in loop");
    }
  } else {
    Serial.println();
    Serial.println("WiFi not connected, running offline");
  }

  updateBlynkStatus();

  Serial.println("=== SMART ACCESS CONTROL SYSTEM READY ===");
  Serial.println("E = clear");
  Serial.println("F = enter");
  Serial.println("Multi-user mode enabled");
}

void loop() {
  // Keep Blynk alive / reconnect if needed
  if (WiFi.status() == WL_CONNECTED) {
    if (Blynk.connected()) {
      Blynk.run();
    } else {
      unsigned long now = millis();
      if (now - lastBlynkReconnectAttempt > 5000) {
        lastBlynkReconnectAttempt = now;
        Serial.println("Trying to reconnect to Blynk...");
        Blynk.connect(3000);
      }
    }
  }

  // Lockout timer
  if (lockedOut) {
    if (millis() - lockoutStart >= LOCKOUT_DURATION) {
      lockedOut = false;
      attempts = 0;
      input = "";
      Serial.println("System unlocked. You can try again.");

      doorStatus = "Locked";
      if (Blynk.connected()) {
        Blynk.virtualWrite(V2, doorStatus);
      }
    } else {
      return;
    }
  }

  // Auto-clear input after timeout
  if (input.length() > 0 && millis() - lastKeyPress >= INPUT_TIMEOUT) {
    input = "";
    Serial.println("Input timed out and was cleared.");
  }

  char key = keypad.getKey();

  if (key) {
    lastKeyPress = millis();

    Serial.print("Pressed: ");
    Serial.println(key);

    if (key >= '0' && key <= '9') {
      input += key;

      Serial.print("Input: ");
      for (int i = 0; i < input.length(); i++) {
        Serial.print("*");
      }
      Serial.println();
    }
    else if (key == 'E') {
      input = "";
      Serial.println("Input Cleared");
      beepClear();
    }
    else if (key == 'F') {
      checkPassword();
      input = "";
    }
    else {
      Serial.println("Unused key");
    }
  }
}

void beepClear() {
  ledcWrite(BUZZER_CHANNEL, 128);
  delay(100);
  ledcWrite(BUZZER_CHANNEL, 0);
}

void beepSuccess() {
  for (int i = 0; i < 2; i++) {
    ledcWrite(BUZZER_CHANNEL, 128);
    delay(100);
    ledcWrite(BUZZER_CHANNEL, 0);
    delay(100);
  }
}

void beepError() {
  for (int i = 0; i < 3; i++) {
    ledcWrite(BUZZER_CHANNEL, 128);
    delay(300);
    ledcWrite(BUZZER_CHANNEL, 0);
    delay(100);
  }
}

void beepStartup() {
  ledcWrite(BUZZER_CHANNEL, 128);
  delay(120);
  ledcWrite(BUZZER_CHANNEL, 0);
  delay(100);
  ledcWrite(BUZZER_CHANNEL, 128);
  delay(120);
  ledcWrite(BUZZER_CHANNEL, 0);
}

void updateBlynkStatus() {
  if (Blynk.connected()) {
    Blynk.virtualWrite(V1, unlockCount);
    Blynk.virtualWrite(V2, doorStatus);
    Blynk.virtualWrite(V3, lastUser);
  }
}

void unlockDoor(String source, String userName) {
  Serial.print("Access Granted! Welcome ");
  Serial.println(userName);

  Serial.print("Unlock source: ");
  Serial.println(source);

  attempts = 0;
  beepSuccess();

  lastUser = userName;
  doorStatus = "Unlocked";

  if (Blynk.connected()) {
    Blynk.virtualWrite(V2, doorStatus);
    Blynk.virtualWrite(V3, lastUser);
  }

  unlockCount++;
  Serial.print("Total unlocks: ");
  Serial.println(unlockCount);

  if (Blynk.connected()) {
    Blynk.virtualWrite(V1, unlockCount);
    Blynk.logEvent("door_unlocked", "Access granted to " + userName + " via " + source);
    Blynk.run();
    delay(300);
  }

  Serial.println("Access granted for 5 seconds...");
  delay(5000);

  doorStatus = "Locked";
  Serial.println("System returned to locked state");

  if (Blynk.connected()) {
    Blynk.virtualWrite(V2, doorStatus);
  }
}

void checkPassword() {
  if (input == USER1_PASS) {
    unlockDoor("keypad password", USER1_NAME);
  }
  else if (input == USER2_PASS) {
    unlockDoor("keypad password", USER2_NAME);
  }
  else if (input == ADMIN_PASS) {
    unlockDoor("keypad password", ADMIN_NAME);
  }
  else {
    Serial.println("Wrong Password!");
    attempts++;
    beepError();

    if (Blynk.connected()) {
      Blynk.logEvent("wrong_attempt", "Wrong password entered!");
      Blynk.run();
      delay(300);
    }

    if (attempts >= 3) {
      lockedOut = true;
      lockoutStart = millis();
      Serial.println("Too many wrong attempts! System locked for 30 seconds.");

      if (Blynk.connected()) {
        Blynk.logEvent("intruder_alert", "Too many wrong attempts! System locked.");
        Blynk.virtualWrite(V2, "LOCKED OUT");
        Blynk.run();
        delay(300);
      }
    }
  }
}