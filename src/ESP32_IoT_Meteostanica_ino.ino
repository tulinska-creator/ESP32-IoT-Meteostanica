#include <WiFi.h>
#include <HTTPClient.h>
#include <WebServer.h>
#include <DHT.h>

// ---------- WIFI ----------
const char* ssid = "TuliTuli";
const char* password = "jednoduche";

// ---------- DHT ----------
#define DHTPIN 15
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// ---------- LED + ALARM ----------
const int LED_PIN = 2;
float T_HIGH = 23.0;
float H_HIGH = 60.0;

// ---------- ThingSpeak ----------
const char* thingspeakWriteKey = "WSCEWE0L7PAX0TPV";
const char* thingspeakUrl = "http://api.thingspeak.com/update"; // HTTP = jednoduché a stabilné

// ---------- WEB SERVER ----------
WebServer server(80);

// posledné hodnoty pre web (aby sa zobrazili aj medzi odosielaniami)
float lastT = NAN;
float lastH = NAN;
bool  lastAlarm = false;

void handleRoot() {
  // pripravené hodnoty ako text
  String tStr = isnan(lastT) ? "N/A" : String(lastT, 1);
  String hStr = isnan(lastH) ? "N/A" : String(lastH, 1);
  String alarmStr = lastAlarm ? "YES" : "NO";
  String alarmClass = lastAlarm ? "bad" : "ok";

  String ipStr = (WiFi.status() == WL_CONNECTED) ? WiFi.localIP().toString() : "not connected";
  String uptimeStr = String(millis() / 1000) + " s";

  String html = R"rawliteral(
<!doctype html>
<html lang="sk">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <meta http-equiv="refresh" content="5">
  <title>ESP32 Meteostanica</title>
  <style>
    :root{
      --bg:#0b1220; --text:#e9eefc; --muted:#a9b4d6;
      --ok:#22c55e; --bad:#ef4444; --card:rgba(255,255,255,.06);
      --line:rgba(255,255,255,.12);
    }
    body{
      margin:0; min-height:100vh; display:grid; place-items:center;
      font-family:system-ui,-apple-system,Segoe UI,Roboto,Arial;
      background: radial-gradient(1200px 800px at 20% 10%, #1b2a5a 0%, var(--bg) 55%);
      color:var(--text); padding:24px;
    }
    .card{
      width:min(520px, 100%);
      background: var(--card);
      border:1px solid var(--line);
      border-radius:18px;
      box-shadow: 0 18px 55px rgba(0,0,0,.45);
      padding:22px 22px 16px;
    }
    h1{ margin:0 0 14px; font-size:26px; letter-spacing:.2px; }
    .sub{ color:var(--muted); margin-top:-8px; margin-bottom:14px; font-size:13px; }
    .row{
      display:flex; justify-content:space-between; align-items:center;
      padding:12px 12px; margin:10px 0;
      border:1px solid rgba(255,255,255,.10);
      border-radius:14px;
      background: rgba(0,0,0,.10);
    }
    .label{ color:var(--muted); font-weight:700; }
    .value{
      font-size:18px; font-weight:900;
      padding:6px 10px; border-radius:999px;
      background: rgba(255,255,255,.06);
      border:1px solid rgba(255,255,255,.10);
    }
    .alarm{
      font-weight:950; letter-spacing:.4px;
      padding:7px 12px; border-radius:999px;
      border:1px solid rgba(255,255,255,.18);
      display:inline-block;
    }
    .alarm.ok{ background: rgba(34,197,94,.14); border-color: rgba(34,197,94,.35); color: var(--ok); }
    .alarm.bad{ background: rgba(239,68,68,.14); border-color: rgba(239,68,68,.35); color: var(--bad); }
    .footer{
      margin-top:14px; padding-top:12px;
      border-top:1px solid rgba(255,255,255,.10);
      display:flex; justify-content:space-between; gap:10px; flex-wrap:wrap;
      color:var(--muted); font-size:13px;
    }
    .pill{
      padding:6px 10px; border-radius:999px;
      background: rgba(255,255,255,.06);
      border:1px solid rgba(255,255,255,.10);
    }
  </style>
</head>
<body>
  <div class="card">
    <h1>ESP32 Meteostanica</h1>
    <div class="sub">Meranie teploty a vlhkosti + vizuálny alarm (auto refresh 5 s)</div>

    <div class="row">
      <div class="label">🌡️ Teplota</div>
      <div class="value">%TEMP% &deg;C</div>
    </div>

    <div class="row">
      <div class="label">💧 Vlhkosť</div>
      <div class="value">%HUM% %</div>
    </div>

    <div class="row">
      <div class="label">🚨 ALARM</div>
      <div class="alarm %ALARMCLASS%">%ALARM%</div>
    </div>

    <div class="footer">
      <div class="pill">Prahy: T≥%TH%°C, H≥%HH%%</div>
      <div class="pill">IP: %IP%</div>
      <div class="pill">Uptime: %UP%</div>
    </div>
  </div>
</body>
</html>
)rawliteral";

  // nahradenie placeholderov
  html.replace("%TEMP%", tStr);
  html.replace("%HUM%", hStr);
  html.replace("%ALARM%", alarmStr);
  html.replace("%ALARMCLASS%", alarmClass);
  html.replace("%IP%", ipStr);
  html.replace("%UP%", uptimeStr);
  html.replace("%TH%", String(T_HIGH, 1));
  html.replace("%HH%", String(H_HIGH, 0));

  server.send(200, "text/html; charset=utf-8", html);
}

void sendToThingSpeak(float t, float h, bool alarm) {
  HTTPClient http;
  http.begin(thingspeakUrl);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  String body = "api_key=" + String(thingspeakWriteKey) +
                "&field1=" + String(t, 1) +
                "&field2=" + String(h, 1) +
                "&field3=" + String(alarm ? 1 : 0);

  int code = http.POST(body);
  String resp = http.getString();
  http.end();

  Serial.print("ThingSpeak HTTP code: ");
  Serial.println(code);
  Serial.print("Response: ");
  Serial.println(resp);
}

void setup() {
  Serial.begin(115200);
  delay(300);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  dht.begin();
  delay(1000);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.print("Pripajam sa na WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.print(".");
  }

  Serial.println("\nWiFi OK");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("Otvor v prehliadaci: http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");

  server.on("/", handleRoot);
  server.begin();
  Serial.println("Web server bezi na porte 80.");
}

void loop() {
  // aby web reagoval, musí sa obsluhovať stále
  server.handleClient();

  // meranie
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Chyba citania DHT!");
    delay(2000);
    return;
  }

  lastH = h;
  lastT = t;
  lastAlarm = (t >= T_HIGH) || (h >= H_HIGH);

  digitalWrite(LED_PIN, lastAlarm ? HIGH : LOW);

  Serial.printf("T=%.1f C, H=%.1f %%, ALARM=%s\n", lastT, lastH, lastAlarm ? "YES" : "no");

  // odoslanie do ThingSpeak
  if (WiFi.status() == WL_CONNECTED) {
    sendToThingSpeak(lastT, lastH, lastAlarm);
  }

  // ThingSpeak free limit + DHT potrebuje pauzu
  delay(20000);
}
