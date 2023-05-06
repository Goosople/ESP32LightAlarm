# 1 "D:\\esp32\\esp32.ino"
// headers
# 3 "D:\\esp32\\esp32.ino" 2
# 4 "D:\\esp32\\esp32.ino" 2
# 5 "D:\\esp32\\esp32.ino" 2
# 6 "D:\\esp32\\esp32.ino" 2
# 7 "D:\\esp32\\esp32.ino" 2

// pins



// NTP servers




// controling objects
TM1637 tube(18, 19);

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
    configTime(8 * 3600, 0, "time.pool.aliyun.com", "cn.pool.ntp.org", "time1.cloud.tencent.com");
    // audio setup
    in = new AudioFileSourceSPIFFS("/a.mp3");
    mp3 = new AudioGeneratorMP3();
    out = new AudioOutputI2S();
    out->SetOutputModeMono(true);
    out->SetRate(16000);
    out->SetBitsPerSample(16);
    out->SetPinout(1, 0, 12);
    out->SetGain(0.75);
    mp3->begin(in, out);
}

void loop() {
    struct tm now;
    getLocalTime(&now);
    if (mp3->isRunning())
        mp3->loop();
    tube.display(tmf(now));
    if (mp3->isRunning())
        mp3->loop();
}
