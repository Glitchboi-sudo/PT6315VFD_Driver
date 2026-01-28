#pragma once
#include <Arduino.h>

class PT6315VFD {
public:
  PT6315VFD(uint8_t pinEN, uint8_t pinCS, uint8_t pinCLK, uint8_t pinDIN, bool enActiveHigh = true);

  void begin(uint8_t brightness0to7 = 7, bool displayOn = true);
  void enable(bool on);
  void setDisplay(bool on, uint8_t brightness0to7);
  void setBrightness(uint8_t brightness0to7);
  void clear();

  // Digitos
  void setDigitPos(uint8_t pos1to8, char ch);
  void setDigits8(const char* s);
  void printLeft4(int value, bool leadingZeros = false);
  void printTimeMMSS(uint8_t mm, uint8_t ss, bool colonOn);

  // Indicadores especiales
  void setColon(bool on);
  void setOneIndicator(bool on);

  // Iconos
  enum IconLeft : uint8_t {
    ICONL_MP3   = 0x01,
    ICONL_PAUSE = 0x02,
    ICONL_PLAY  = 0x04,
    ICONL_CD    = 0x08,
    ICONL_V     = 0x10,
    ICONL_S     = 0x20,
    ICONL_DVD   = 0x40
  };

  enum IconRight : uint8_t {
    ICONR_DTS   = 0x01,
    ICONR_DOLBY = 0x02
  };

  void setIconLeft(IconLeft icon, bool on);
  void setIconRight(IconRight icon, bool on);
  void setIconsLeftMask(uint8_t mask);
  void setIconsRightMask(uint8_t mask);

  void showMP3(bool on);
  void setText(const char* text, uint16_t scrollDelayMs = 250);
  void update();


private:
  // ----- Configuracion interna -----
  static constexpr uint8_t RAM_BYTES = 16;

  // Direcciones (ajusta si hiciera falta)
  static constexpr uint8_t ADDR_ICONS_L = 0;
  static constexpr uint8_t ADDR_ICONS_R = 13;

  static constexpr uint8_t ADDR_D1 = 1;
  static constexpr uint8_t ADDR_D2 = 4;
  static constexpr uint8_t ADDR_D3 = 3;
  static constexpr uint8_t ADDR_D4 = 7;
  static constexpr uint8_t ADDR_D5 = 6;
  static constexpr uint8_t ADDR_D6 = 10;
  static constexpr uint8_t ADDR_D7 = 9;
  static constexpr uint8_t ADDR_D8 = 12;

  // Segmentos
  static constexpr uint8_t SEG_D = 0x01;
  static constexpr uint8_t SEG_E = 0x02;
  static constexpr uint8_t SEG_C = 0x04;
  static constexpr uint8_t SEG_G = 0x08;
  static constexpr uint8_t SEG_F = 0x10;
  static constexpr uint8_t SEG_B = 0x20;
  static constexpr uint8_t SEG_A = 0x40;
  static constexpr uint8_t SEG_X = 0x80;

  uint8_t _en, _cs, _clk, _din;
  bool _enActiveHigh;
  uint8_t _shadow[RAM_BYTES];
  uint8_t _dlyUs = 2;
  // --- Scroll interno ---
  const char* _text = nullptr;
  size_t _textLen = 0;
  size_t _scrollPos = 0;
  uint16_t _scrollDelayMs = 250;
  uint32_t _lastScrollMs = 0;
  bool _scrollEnabled = false;

  void start();
  void stop();
  void sendCommand(uint8_t cmd);
  void writeByte(uint8_t b);
  void writeDataFixed(uint8_t addr, uint8_t value);
  void writeBlock0(const uint8_t buf[16]);
  void writeRaw(uint8_t addr, uint8_t value, bool preserveX);

  uint8_t posToAddr(uint8_t pos);
  uint8_t encodeChar7seg(char c);
};
