#pragma once

// Wi-Fi Configuration
struct WifiNetwork {
  const char* ssid;
  const char* password;
};

// Application Secrets & Configuration Template
const WifiNetwork networks[] = {
  {"YOUR_SSID_1", "YOUR_PASSWORD_1"},
  {"YOUR_SSID_2", "YOUR_PASSWORD_2"}
};
const int numNetworks = 2;

// Add any future API keys, tokens, or sensitive settings here
