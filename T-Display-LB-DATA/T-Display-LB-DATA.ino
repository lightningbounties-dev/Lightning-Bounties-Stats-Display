#include <WiFiManager.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <TFT_eSPI.h>
#include <vector>

/* ───────── USER CONFIG ───────── */
#define DEBUG 1
const char* AP_SSID        = "lb-network-stats";
const uint8_t WIPE_BTN_PIN = 35;     // left button
const uint16_t WIPE_HOLD_MS = 3000;  // 3 s hold to wipe
const uint16_t PORTAL_TIMEOUT = 180; // captive-portal lifetime
/* ─────────────────────────────── */

#if DEBUG
  #define DBG(...) Serial.printf(__VA_ARGS__)
#else
  #define DBG(...)
#endif

/* ───── Display ───── */
TFT_eSPI    tft;
TFT_eSprite spr(&tft);

/* ───── Network & APIs ───── */
WiFiClientSecure net;

const char* BTC_API =
  "https://api.coinbase.com/v2/prices/BTC-USD/spot";

const char* LB_API =
  "https://script.googleusercontent.com/a/macros/lightningbounties.com/echo?user_content_key=AehSKLhseuRWqw43GKRNb8ukFgHFO2e6fY6kzcncK3SnaFmkOBuGqLx0-hWzkyxrQDIqdZqpM8AYv6HQSinrbeAQ_zQeyh7QRgcJr5iWZTrm-6YpfUEk93iQESp1HukdRS6RfLNAdm4Id83_HQgRP_I9N03q44LPuxne9K_mE8ahCT_g991-nDtQY-mBZwlXNwA3bEaBS6F3uvI7JrG0j_fMTe0OyUfN46YJ0fL1EVD3ZFptVLuqzQV1ZZk1UEyTDuAqGXHWEnGReQQcDIHSX48ds_O7ZGawjoD7eS6dWPhNlNQ5-2TCd2fZsy8_xwyQiw&lib=MqxvYqYjE3p9MX406Ezcl_fjE1tO6e3DW";

/* ───── Timing ───── */
const uint32_t REFRESH_MS = 60000;
const uint32_t ROTATE_MS  =  3000;

/* ───── Globals ───── */
float   g_btc = -1;
String  g_label[4] = { "Total Bounties", "Developers",
                       "Sats Rewarded",  "Avg bounty sats" };
String  g_value[4] = { "--","--","--","--" };
uint8_t  statIdx    = 0;
uint32_t lastFetch  = 0;
uint32_t lastRotate = 0;

/* ───── Colours ───── */
#define BTC_ORANGE 0xFDA0

const char* LB_LOGO =
  "https://app.lightningbounties.com/_next/static/media/LB_nobg.96a23fa3.svg";

/* ───── Helpers ───── */
String addCommas(String s){
  int d=s.indexOf('.');
  String i=d==-1? s : s.substring(0,d);
  String f=d==-1? "" : s.substring(d);
  for(int p=i.length()-3;p>0;p-=3)
      i=i.substring(0,p)+','+i.substring(p);
  return i+f;
}

/* ───── OLED screens ───── */
void drawPortalMsg(){
  spr.fillSprite(TFT_BLACK);
  spr.setTextSize(2);
  spr.setTextColor(TFT_WHITE); spr.setCursor(20,40); spr.println("WIFI CONFIG:");
  spr.setTextColor(TFT_CYAN);  spr.setCursor( 8,70); spr.println(AP_SSID);
  spr.pushSprite(0,0);
}
void drawStats(){
  spr.fillSprite(TFT_BLACK);

  spr.setTextSize(2); spr.setTextColor(TFT_WHITE);
  spr.setCursor(10,15); spr.println("BTC / USD");

  spr.setTextSize(3); spr.setTextColor(BTC_ORANGE);
  spr.setCursor(10,45);
  if(g_btc>0){ spr.print('$'); spr.println(g_btc,2);} else spr.println("---");

  spr.setTextSize(2); spr.setTextColor(TFT_CYAN);
  spr.setCursor(10,95);  spr.println(g_label[statIdx]);

  spr.setTextColor(TFT_GREEN);
  spr.setCursor(10,115); spr.println(g_value[statIdx]);

  spr.pushSprite(0,0);
}

/* ───── API calls ───── */
float fetchBTC(){
  HTTPClient https; https.setTimeout(10000);
  if(!https.begin(net,BTC_API)          ) return -1;
  if(https.GET()!=HTTP_CODE_OK          ) return -1;
  DynamicJsonDocument d(512);
  if(deserializeJson(d,https.getString())) return -1;
  return d["data"]["amount"].as<float>();
}
bool fetchLB(){
  HTTPClient https;
  https.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);
  https.setTimeout(35000);
  if(!https.begin(net,LB_API))          return false;
  if(https.GET()!=HTTP_CODE_OK)         return false;
  DynamicJsonDocument d(8192);
  if(deserializeJson(d,https.getString())) return false;

  for(JsonObject m: d["metrics"].as<JsonArray>()){
    const char* k=m["metric"];
    String v=String(m["value"].as<double>(),
                    strcmp(k,"Average Sats Rewarded")==0?2:0);
    if      (!strcmp(k,"Total Bounties"))           g_value[0]=v;
    else if (!strcmp(k,"Total Developers"))         g_value[1]=v;
    else if (!strcmp(k,"Total Sats Rewarded"))      g_value[2]=addCommas(v);
    else if (!strcmp(k,"Average Sats Rewarded"))    g_value[3]=addCommas(v);
  }
  return true;
}

/* ───── Wi-Fi wipe ───── */
void checkWipe(){
  static uint32_t t0=0;
  if(digitalRead(WIPE_BTN_PIN)==LOW){
      if(!t0) t0=millis();
      else if(millis()-t0>WIPE_HOLD_MS){
          WiFiManager().resetSettings();
          delay(500); ESP.restart();
      }
  }else t0=0;
}

/* ───── setup ───── */
void setup(){
  Serial.begin(115200);
  pinMode(WIPE_BTN_PIN, INPUT_PULLUP);

  tft.init(); tft.setRotation(3);
  spr.createSprite(240,135); spr.fillSprite(TFT_BLACK); spr.pushSprite(0,0);

  WiFiManager wm;
  wm.setConfigPortalTimeout(PORTAL_TIMEOUT);
  wm.setTitle("⚡ Lightning Bounties Stats");

  /* custom head (dark CSS + favicon) */
  String head =
  "<style>"
    "body{background:#ffffff;color:#111111;font-family:Inter,Arial}"
    "h2{color:#ff6600;text-align:center;margin:0.3em 0}"
    ".c{text-align:center}"
    "input{border-radius:6px;padding:6px;border:1px solid #ccc}"
    "label{color:#333333}"
    ".btn{background:#007BFF;color:#fff;border:0;border-radius:6px;padding:10px 18px}"
  "</style>"
  "<link rel='icon' href='" + String(LB_LOGO) + "'>";
  wm.setCustomHeadElement(head.c_str());

  /* menu: Wi-Fi + Restart */
  std::vector<const char*> menu = {"wifi","sep","restart"};
  wm.setMenu(menu);

  /* OLED hint while portal is up */
  wm.setAPCallback([](WiFiManager*){ drawPortalMsg(); });

  if(!wm.autoConnect(AP_SSID)){
      drawPortalMsg(); delay(15000); ESP.restart();
  }

  net.setInsecure();
  g_btc = fetchBTC();
  fetchLB();
  drawStats();
}

/* ───── loop ───── */
void loop(){
  checkWipe();

  uint32_t now=millis();
  if(now-lastFetch>=REFRESH_MS){
      lastFetch=now;
      float p=fetchBTC(); if(p>0) g_btc=p;
      fetchLB(); drawStats();
  }
  if(now-lastRotate>=ROTATE_MS){
      lastRotate=now;
      statIdx=(statIdx+1)&3;
      drawStats();
  }
}