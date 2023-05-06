// headers
#include "esp32.h"
#include <AudioFileSourceSPIFFS.h>
#include <AudioGeneratorMP3.h>
#include <AudioOutputI2S.h>
#include <TM1637.h>

// pins
#define TUBE_CLK 18
#define TUBE_IO 19

// NTP servers
#define NTP1 "time.pool.aliyun.com"
#define NTP2 "cn.pool.ntp.org"
#define NTP3 "time1.cloud.tencent.com"

// controling objects
TM1637 tube(TUBE_CLK, TUBE_IO);

// audio objects
AudioFileSourceSPIFFS *in;
AudioGeneratorMP3 *mp3;
AudioOutputI2S *out;

void setup() {
    // hardware setup
    Serial.begin(1200);
    tube.begin();
    tube.setBrightness(128);
    tube.colonOn();
    audioLogger = &Serial;
    wifiSetup();
    SPIFFS.begin();
    // time config
    configTime(8 * 3600, 0, NTP1, NTP2, NTP3);
    // audio setup
    in = new AudioFileSourceSPIFFS("/a.mp3");
    mp3 = new AudioGeneratorMP3();
    out = new AudioOutputI2S();
    out->SetOutputModeMono(true);
    out->SetRate(16000);
    out->SetBitsPerSample(16);
    out->SetPinout(1, 0, 12);
    out->SetGain(0.75);
}

void loop() {
    struct tm now;
    getLocalTime(&now);
    tube.display(tmf(now));
    mp3->begin(in, out);
    if (mp3->isRunning() && !mp3->loop())
        mp3->stop();
}
