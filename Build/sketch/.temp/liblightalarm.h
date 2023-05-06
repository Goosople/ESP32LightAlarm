#line 1 "D:\\esp32\\.temp\\liblightalarm.h"
#ifndef LIBLIGHTALARM_CUSTOM
#define RTC_IO 3
#define RTC_CLK 4
#define RTC_CE 2
#define TUBE_CLK 6
#define TUBE_IO 5
#define LED_NUM 15
#define LED_IO 7
#endif

#ifndef __LIBLIGHTALARM_H__
#define __LIBLIGHTALARM_H__

#include <EEPROM.h>
#include <FastLED.h>
#include <RtcDS1302.h>
#include <TM1637.h>

#define now (mRtc.GetDateTime())
#define waitforserial while (!Serial.available())

ThreeWire mWire(RTC_IO, RTC_CLK, RTC_CE); // IO, SCLK, CE
RtcDS1302<ThreeWire> mRtc(mWire);
TM1637 mTube(TUBE_CLK, TUBE_IO);
CRGB mLeds[LED_NUM];
bool tubeon;
uint8_t brightness = 0;
char sread;

class mTime {
  private:
    uint8_t h = 0, m = 0, s = 0;

  public:
    mTime() = default;
    ~mTime() = default;
    mTime(String ts)
        : h((ts[0] - '0') * 10 + (ts[1] - '0')),
          m((ts[2] - '0') * 10 + (ts[3] - '0')) {}
    mTime(RtcDateTime rtcdt)
        : h(rtcdt.Hour()), m(rtcdt.Minute()), s(rtcdt.Second()) {}
    mTime(uint8_t h, uint8_t m, uint8_t s = 0) : h(h), m(m), s(s) {}
    operator String() {
        char c_str[5];
        sprintf(c_str, "%02d%02d", h, m);
        return c_str;
    }
    operator RtcDateTime() {
        return RtcDateTime(now.Year(), now.Month(), now.Day(), h, m, s);
    }
};

namespace libLA {
    inline void init() {
        Serial.begin(300);
        mTube.begin();
        mRtc.Begin();
        FastLED.addLeds<WS2812B, LED_IO, GRB>(mLeds, LED_NUM);
        pinMode(LED_BUILTIN, OUTPUT);
        tubeon = true;

        RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
        RtcDateTime now = mRtc.GetDateTime();
        if (now < compiled)
            mRtc.SetDateTime(compiled);
        if (!mRtc.IsDateTimeValid())
            mRtc.SetDateTime(compiled);
        if (mRtc.GetIsWriteProtected())
            mRtc.SetIsWriteProtected(false);
        if (!mRtc.GetIsRunning())
            mRtc.SetIsRunning(true);
    }

    inline bool isTime(String ts) {
        for (auto i : ts)
            if (!isDigit(i))
                return false;
        if (ts[0] != '0' && ts[0] != '1')
            return false;
        if (ts[2] > '6')
            return false;
    }

    inline bool isBellOn() { return EEPROM.read(4); }

    inline void setBell(bool bellon) { EEPROM.write(4, bellon); }

    inline bool isAlarmActivated() { return EEPROM.read(5) == now.Day(); }

    inline void setAlarmActivated(bool al) {
        return EEPROM.write(5, al ? now.Day() : al);
    }

    mTime readAlarm() {
        char tstr[4];
        for (uint8_t i = 0; i < 4; i++)
            tstr[i] = EEPROM.read(i);
        return String(tstr);
    }

    inline void setAlarm(String tstr) {
        for (uint8_t i = 0; i < 4; i++)
            EEPROM.write(i, tstr[i]);
        setAlarmActivated(now >= RtcDateTime(mTime(tstr)));
    }

    inline bool setTimetoRtc(mTime t) {
        mRtc.SetDateTime(t);
        if (now != RtcDateTime(t))
            return false;
        return true;
    }

    inline void activateAlarm() {
#ifdef DEBUG
        Serial.println("Activate alarm.");
        return;
#endif
        FastLED.setBrightness(16);
        for (uint8_t ir = 0; ir < 64; ir++) {
            for (auto &&led : mLeds)
                led = CRGB(ir, 0, 0);
            FastLED.show();
            digitalWrite(LED_BUILTIN, ir % 2);
            delay(1000);
            if (tubeon) {
                mTube.display(String(mTime(now)));
                mTube.colonOn();
                sread = Serial.read();
            }
            if (sread == '#')
                return;
            if (sread == '*') {
                libLA::setAlarm(mTime(now + 120));
                return;
            }
        }
        for (uint8_t ig = 0; ig < 64; ig++) {
            for (auto &&led : mLeds)
                led = CRGB(64 + ig, ig, 0);
            FastLED.show();
            digitalWrite(LED_BUILTIN, ig % 2);
            delay(1000);
            if (tubeon) {
                mTube.display(String(mTime(now)));
                mTube.colonOn();
            }
            if (sread == '#')
                return;
            if (sread == '*') {
                libLA::setAlarm(mTime(now + 120));
                return;
            }
        }
        for (uint8_t i = 16; i < 128; i++) {
            FastLED.show(i);
            delay(1000);
            if (tubeon) {
                mTube.display(String(mTime(now)));
                mTube.colonOn();
            }
            if (sread == '#')
                return;
            if (sread == '*') {
                libLA::setAlarm(mTime(now + 120));
                return;
            }
        }
        FastLED.setBrightness(128);
        for (uint8_t ib = 0; ib < 128; ib++) {
            for (auto &&led : mLeds)
                led = CRGB(128 + ib, 64 + ib, ib);
            FastLED.show();
            digitalWrite(LED_BUILTIN, ib % 2);
            delay(1000);
            if (tubeon) {
                mTube.display(String(mTime(now)));
                mTube.colonOn();
            }
            if (sread == '#')
                return;
            if (sread == '*') {
                libLA::setAlarm(mTime(now + 120));
                return;
            }
        }
        for (uint8_t i = 128; i < 255; i++) {
            FastLED.show(i);
            delay(1000);
            if (tubeon) {
                mTube.display(String(mTime(now)));
                mTube.colonOn();
            }
            if (sread == '#')
                return;
            if (sread == '*') {
                libLA::setAlarm(mTime(now + 120));
                return;
            }
        }
        FastLED.setBrightness(255);
        for (uint8_t i = 0; i < 64; i++) {
            for (auto &&led : mLeds)
                led = CRGB(255, 192 + i, 128 + i);
            FastLED.show();
            digitalWrite(LED_BUILTIN, i % 2);
            delay(500);
            if (tubeon) {
                mTube.display(String(mTime(now)));
                mTube.colonOn();
            }
            if (sread == '#')
                return;
            if (sread == '*') {
                libLA::setAlarm(mTime(now + 120));
                return;
            }
        }
        for (uint8_t i = 0; i < 64; i++) {
            for (auto &&led : mLeds)
                led = CRGB(255, 255, 192 + i);
            FastLED.show();
            digitalWrite(LED_BUILTIN, i % 2);
            delay(500);
            if (tubeon) {
                mTube.display(String(mTime(now)));
                mTube.colonOn();
            }
            if (sread == '#')
                return;
            if (sread == '*') {
                libLA::setAlarm(mTime(now + 120));
                return;
            }
        }
    }
} // namespace libLA

#endif // __LIBLIGHTALARM_H__
