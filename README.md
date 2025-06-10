# ⚡️ Lightning Bounties Stats Display

<img src="<!-- PLACEHOLDER: Insert your photo here (e.g. IMG_9975.jpeg) -->" width="450">

A plug-and-play real-time stats display for **BTC price** and your **Lightning Bounties** platform, running on a [LilyGo T-Display]([https://www.lilygo.cc/products/ttgo-t-display](https://lilygo.cc/products/lilygo%C2%AE-ttgo-t-display-1-14-inch-lcd-esp32-control-board?srsltid=AfmBOop1Orw54ybUnWBW5T9pSJAL4fXyroY8yj7QDUZamMnaSTNfYezn)) (ESP32) board. This device shows live metrics like the current Bitcoin (BTC) price in USD and stats from the Lightning Bounties API, rotating through all key platform stats on its crisp little screen.

---

## 📦 Features

- **Real-Time BTC Price**: Fetches the latest BTC/USD spot price from Coinbase every **60 seconds**.
- **Lightning Bounties API Integration**: Displays total bounties, developers, sats rewarded, and average bounty size.
- **Rotating Stats**: Cycles through multiple metrics every 3 seconds for continuous insights.
- **WiFiManager Support**: Easy web-based WiFi setup/reset via captive portal.
- **Compact & Portable**: Runs on a LilyGo T-Display ESP32 board.
- **Configurable & Open Source**: Tweak APIs, UI, and intervals as needed!

---

## 📸 Demo

![Demo Photo](https://github.com/lightningbounties-dev/Lightning-Bounties-Stats-Display/blob/main/T-Display-LB-DATA/photos/IMG_9976.jpeg)

---

## 🚀 How It Works

- **Display:** Renders real-time stats using [TFT_eSPI](https://github.com/Bodmer/TFT_eSPI) and a TFT_eSprite buffer.
- **APIs:** Calls [Coinbase API](https://api.coinbase.com/v2/prices/BTC-USD/spot) for BTC price and your [Lightning Bounties metrics endpoint](https://app.lightningbounties.com/).
- **WiFi Setup:** Uses [WiFiManager](https://github.com/tzapu/WiFiManager) for easy captive-portal-based configuration (SSID: `lb-network-stats`).
- **Periodic Refresh:** 
  - BTC price and LB stats: **Every 60 seconds** (`REFRESH_MS`).
  - Rotates through stats: **Every 3 seconds** (`ROTATE_MS`).
- **Reset Button:** Hold the left button (GPIO 35) for 3 seconds to wipe WiFi settings and reconfigure.

---

## 🛠️ Hardware

- LilyGo T-Display (ESP32 + TFT)
- Micro-USB cable
- Optional: Battery, 3D-printed stand, magnifying glass, etc.

---

## 📲 Setup & Installation

1. **Clone this repo and open the code in Arduino IDE or PlatformIO.**
2. **Install these libraries:**
    - `WiFiManager`
    - `WiFiClientSecure`
    - `HTTPClient`
    - `ArduinoJson`
    - `TFT_eSPI`
3. **Configure TFT_eSPI** for your LilyGo (see the [TFT_eSPI Wiki](https://github.com/Bodmer/TFT_eSPI/wiki) for correct `User_Setup.h`).
4. **Flash to your T-Display board.**
5. **Power up:** The device will start in WiFi config mode if no WiFi is set—connect and set up via captive portal (SSID: `lb-network-stats`).

---

## 🔧 Configuration

- **BTC Refresh interval:** Change `REFRESH_MS` in code (default: 60 seconds).
- **Stats Rotate interval:** Change `ROTATE_MS` (default: 3 seconds).
- **AP SSID:** Set `AP_SSID` at the top of the code.
- **API Endpoints:** Can be updated in code (`BTC_API`, `LB_API`).

---

---

## 📷 Gallery

![Project Demo 1](https://github.com/lightningbounties-dev/Lightning-Bounties-Stats-Display/blob/main/T-Display-LB-DATA/photos/IMG_9975.jpeg)
![Project Demo 1](https://github.com/lightningbounties-dev/Lightning-Bounties-Stats-Display/blob/main/T-Display-LB-DATA/photos/IMG_9978.jpeg)

