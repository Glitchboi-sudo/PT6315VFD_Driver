#include "PT6315VFD.h"

PT6315VFD::PT6315VFD(uint8_t pinEN, uint8_t pinCS, uint8_t pinCLK, uint8_t pinDIN, bool enActiveHigh)
: _en(pinEN), _cs(pinCS), _clk(pinCLK), _din(pinDIN), _enActiveHigh(enActiveHigh) {
  for (uint8_t i = 0; i < RAM_BYTES; i++) _shadow[i] = 0;
}

void PT6315VFD::begin(uint8_t brightness0to7, bool displayOn) {
  pinMode(_en, OUTPUT);
  pinMode(_cs, OUTPUT);
  pinMode(_clk, OUTPUT);
  pinMode(_din, OUTPUT);

  digitalWrite(_cs, HIGH);
  digitalWrite(_clk, LOW);
  digitalWrite(_din, LOW);

  enable(true);
  delay(10);

  clear();
  setDisplay(displayOn, brightness0to7);
}

void PT6315VFD::enable(bool on) {
  digitalWrite(_en, (on ^ !_enActiveHigh) ? HIGH : LOW);
  delayMicroseconds(50);
}

void PT6315VFD::setDisplay(bool on, uint8_t brightness0to7) {
  if (brightness0to7 > 7) brightness0to7 = 7;
  sendCommand(0x80 | (on ? 0x08 : 0x00) | brightness0to7);
}

void PT6315VFD::setBrightness(uint8_t brightness0to7) {
  setDisplay(true, brightness0to7);
}

void PT6315VFD::clear() {
  for (uint8_t i = 0; i < RAM_BYTES; i++) _shadow[i] = 0x00;

  sendCommand(0x40);
  start();
  writeByte(0xC0);
  for (uint8_t i = 0; i < RAM_BYTES; i++) writeByte(0x00);
  stop();
}

void PT6315VFD::setDigitPos(uint8_t pos1to8, char ch) {
  uint8_t addr = posToAddr(pos1to8);
  if (addr == 0xFF) return;

  uint8_t segs = encodeChar7seg(ch);
  bool preserveX = (addr == ADDR_D5) || (addr == ADDR_D7);
  writeRaw(addr, segs, preserveX);
}

void PT6315VFD::setDigits8(const char* s) {
  for (uint8_t i = 0; i < 8; i++) {
    char c = s[i];
    if (c == '\0') c = ' ';
    if (c == '.') c = ' ';
    setDigitPos(i + 1, c);
  }
}

void PT6315VFD::printLeft4(int value, bool leadingZeros) {
  value = constrain(value, 0, 9999);
  int d1 = (value / 1000) % 10;
  int d2 = (value / 100) % 10;
  int d3 = (value / 10) % 10;
  int d4 = value % 10;

  if (!leadingZeros && d1 == 0) setDigitPos(1, ' '); else setDigitPos(1, char('0' + d1));
  if (!leadingZeros && d1 == 0 && d2 == 0) setDigitPos(2, ' '); else setDigitPos(2, char('0' + d2));
  if (!leadingZeros && d1 == 0 && d2 == 0 && d3 == 0) setDigitPos(3, ' '); else setDigitPos(3, char('0' + d3));
  setDigitPos(4, char('0' + d4));
}

void PT6315VFD::printTimeMMSS(uint8_t mm, uint8_t ss, bool colonOn) {
  mm %= 100;
  ss %= 100;

  setDigitPos(5, char('0' + (mm / 10)));
  setDigitPos(6, char('0' + (mm % 10)));

  setColon(colonOn);

  setDigitPos(7, char('0' + (ss / 10)));
  setDigitPos(8, char('0' + (ss % 10)));
}

void PT6315VFD::setColon(bool on) {
  uint8_t addr = ADDR_D7;
  if (on) _shadow[addr] |= SEG_X;
  else    _shadow[addr] &= ~SEG_X;
  writeDataFixed(addr, _shadow[addr]);
}

void PT6315VFD::setOneIndicator(bool on) {
  uint8_t addr = ADDR_D5;
  if (on) _shadow[addr] |= SEG_X;
  else    _shadow[addr] &= ~SEG_X;
  writeDataFixed(addr, _shadow[addr]);
}

void PT6315VFD::setIconLeft(IconLeft icon, bool on) {
  uint8_t addr = ADDR_ICONS_L;
  if (on) _shadow[addr] |= uint8_t(icon);
  else    _shadow[addr] &= ~uint8_t(icon);
  writeDataFixed(addr, _shadow[addr]);
}

void PT6315VFD::setIconRight(IconRight icon, bool on) {
  uint8_t addr = ADDR_ICONS_R;
  if (on) _shadow[addr] |= uint8_t(icon);
  else    _shadow[addr] &= ~uint8_t(icon);
  writeDataFixed(addr, _shadow[addr]);
}

void PT6315VFD::setIconsLeftMask(uint8_t mask) {
  _shadow[ADDR_ICONS_L] = mask;
  writeDataFixed(ADDR_ICONS_L, _shadow[ADDR_ICONS_L]);
}

void PT6315VFD::setIconsRightMask(uint8_t mask) {
  _shadow[ADDR_ICONS_R] = mask;
  writeDataFixed(ADDR_ICONS_R, _shadow[ADDR_ICONS_R]);
}

void PT6315VFD::showMP3(bool on) {
  setIconLeft(ICONL_MP3, on);
}

void PT6315VFD::start() {
  digitalWrite(_cs, LOW);
  delayMicroseconds(_dlyUs);
}

void PT6315VFD::stop() {
  delayMicroseconds(_dlyUs);
  digitalWrite(_cs, HIGH);
  delayMicroseconds(_dlyUs);
}

void PT6315VFD::sendCommand(uint8_t cmd) {
  start();
  writeByte(cmd);
  stop();
}

void PT6315VFD::writeByte(uint8_t b) {
  for (uint8_t i = 0; i < 8; i++) {
    digitalWrite(_din, (b >> i) & 1);
    digitalWrite(_clk, HIGH); delayMicroseconds(_dlyUs);
    digitalWrite(_clk, LOW);  delayMicroseconds(_dlyUs);
  }
}

void PT6315VFD::writeDataFixed(uint8_t addr, uint8_t value) {
  sendCommand(0x44);
  start();
  writeByte(0xC0 | (addr & 0x0F));
  writeByte(value);
  stop();
}

void PT6315VFD::writeBlock0(const uint8_t buf[16]) {
  sendCommand(0x40);
  start();
  writeByte(0xC0);
  for (uint8_t i = 0; i < 16; i++) writeByte(buf[i]);
  stop();
}

void PT6315VFD::writeRaw(uint8_t addr, uint8_t value, bool preserveX) {
  addr &= 0x0F;
  if (preserveX) {
    uint8_t keep = _shadow[addr] & SEG_X;
    value = (value & ~SEG_X) | keep;
  }
  _shadow[addr] = value;
  writeDataFixed(addr, value);
}

void PT6315VFD::setText(const char* text, uint16_t scrollDelayMs) {
  _text = text;
  _textLen = strlen(text);
  _scrollDelayMs = scrollDelayMs;
  _scrollPos = 0;
  _lastScrollMs = 0;

  if (_textLen <= 8) {
    _scrollEnabled = false;
    setDigits8(text);
  } else {
    _scrollEnabled = true;
  }
}

void PT6315VFD::update() {
  if (!_scrollEnabled || !_text) return;

  uint32_t now = millis();
  if (now - _lastScrollMs < _scrollDelayMs) return;
  _lastScrollMs = now;

  char window[9];
  window[8] = '\0';

  for (uint8_t i = 0; i < 8; i++) {
    window[i] = _text[(_scrollPos + i) % _textLen];
  }

  setDigits8(window);

  _scrollPos++;
  if (_scrollPos >= _textLen) _scrollPos = 0;
}

uint8_t PT6315VFD::posToAddr(uint8_t pos) {
  switch (pos) {
    case 1: return ADDR_D1;
    case 2: return ADDR_D2;
    case 3: return ADDR_D3;
    case 4: return ADDR_D4;
    case 5: return ADDR_D5;
    case 6: return ADDR_D6;
    case 7: return ADDR_D7;
    case 8: return ADDR_D8;
    default: return 0xFF;
  }
}

uint8_t PT6315VFD::encodeChar7seg(char c) {
  if (c >= 'a' && c <= 'z') c = char(c - 'a' + 'A');

  switch (c) {
    case '0': return SEG_A|SEG_B|SEG_C|SEG_D|SEG_E|SEG_F;
    case '1': return SEG_B|SEG_C;
    case '2': return SEG_A|SEG_B|SEG_D|SEG_E|SEG_G;
    case '3': return SEG_A|SEG_B|SEG_C|SEG_D|SEG_G;
    case '4': return SEG_F|SEG_G|SEG_B|SEG_C;
    case '5': return SEG_A|SEG_F|SEG_G|SEG_C|SEG_D;
    case '6': return SEG_A|SEG_F|SEG_E|SEG_D|SEG_C|SEG_G;
    case '7': return SEG_A|SEG_B|SEG_C;
    case '8': return SEG_A|SEG_B|SEG_C|SEG_D|SEG_E|SEG_F|SEG_G;
    case '9': return SEG_A|SEG_B|SEG_C|SEG_D|SEG_F|SEG_G;

    case 'A': return SEG_A|SEG_B|SEG_C|SEG_E|SEG_F|SEG_G;
    case 'B': return SEG_C|SEG_D|SEG_E|SEG_F|SEG_G;
    case 'C': return SEG_A|SEG_D|SEG_E|SEG_F;
    case 'D': return SEG_B|SEG_C|SEG_D|SEG_E|SEG_G;
    case 'E': return SEG_A|SEG_D|SEG_E|SEG_F|SEG_G;
    case 'F': return SEG_A|SEG_E|SEG_F|SEG_G;
    case 'G': return SEG_A|SEG_F|SEG_B|SEG_G|SEG_C|SEG_D;
    case 'H': return SEG_B|SEG_C|SEG_E|SEG_F|SEG_G;
    case 'I': return SEG_B|SEG_C;
    case 'J': return SEG_D|SEG_B|SEG_C|SEG_E; 
    case 'K': return SEG_B|SEG_D|SEG_E|SEG_F|SEG_G;
    case 'L': return SEG_D|SEG_E|SEG_F;
    case 'M': return SEG_A|SEG_E|SEG_F|SEG_C|SEG_B;
    case 'N': return SEG_E|SEG_G|SEG_C; 
    case 'O': return SEG_A|SEG_B|SEG_C|SEG_D|SEG_E|SEG_F;
    case 'P': return SEG_A|SEG_B|SEG_E|SEG_F|SEG_G;
    case 'Q': return SEG_F|SEG_A|SEG_G|SEG_B|SEG_C;
    case 'R': return SEG_E|SEG_G; 
    case 'S': return SEG_A|SEG_F|SEG_G|SEG_C|SEG_D;
    case 'T': return SEG_G|SEG_F|SEG_E|SEG_D;
    case 'U': return SEG_B|SEG_C|SEG_D|SEG_E|SEG_F;
    case 'V': return SEG_C|SEG_D|SEG_E;
    case 'W': return SEG_C|SEG_D|SEG_E|SEG_A;
    case 'X': return SEG_G|SEG_F|SEG_E;
    case 'Y': return SEG_F|SEG_G|SEG_B|SEG_C|SEG_D;
    case 'Z': return SEG_A|SEG_D|SEG_G;
    case '-': return SEG_G;
    case '_': return SEG_D;
    case '=': return SEG_D|SEG_G;
    case ' ': return 0x00;
    default:  return 0x00;
  }
}
