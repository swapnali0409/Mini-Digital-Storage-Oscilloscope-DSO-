#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <arduinoFFT.h>

// -------------------- PIN DEFINITIONS --------------------
#define TFT_CS    D8
#define TFT_DC    D1
#define TFT_RST   -1

#define BTN_MODE  D3
#define BTN_TIME  D6
#define BTN_VOLT  D2
#define BTN_HOLD  D4

#define ANALOG_PIN A0

// -------------------- DISPLAY SETTINGS --------------------
#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 240
#define UI_HEIGHT     46
#define WAVE_AREA_H   192
#define GRID_COLS 6
#define GRID_ROWS 4

#define DIV_W (SCREEN_WIDTH/ GRID_COLS)
#define DIV_H (WAVE_AREA_H / GRID_ROWS)

// -------------------- UI LAYOUT --------------------
#define PLOT_X_START 35   // Margin for Voltage labels
#define PLOT_W       280  // Width of the actual grid
#define PLOT_Y_START 48   // Below the header
#define PLOT_H       172  // Height of the actual grid
#define PLOT_X_END   (PLOT_X_START + PLOT_W)
#define PLOT_Y_END   (PLOT_Y_START + PLOT_H)

// New Color Scheme
#define BG_COLOR    ST77XX_WHITE
#define GRID_COLOR  0xC618 // Light Gray
#define TEXT_COLOR  ST77XX_BLACK
#define WAVE_COLOR  ST77XX_BLUE  // Blue looks better on white than yellow
#define UI_BG       0xEF7D // Very light gray for header

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
ArduinoFFT<double> FFT = ArduinoFFT<double>();

// -------------------- BUFFERS --------------------
int waveBuff[320], prevWaveBuff[320];
double vReal[256], vImag[256];
int prevFFTBuff[128]; // To store previous bar heights for flicker-free erase

// -------------------- SCALE SETTINGS --------------------
const int timeArray[] = {5, 10, 20, 40, 80, 160, 320};
int tIndex = 0;

const float voltArray[] = {0.2,0.4,0.6,0.8,1,1.2,1.4,1.6,1.8,2};
int vIndex = 4;

bool hold = false;
bool fftMode = false;

float avgVoltage = 0;
float frequency = 0;

// -------------------- BUTTON STATE --------------------
bool lastMode = HIGH, lastTime = HIGH, lastVolt = HIGH, lastHold = HIGH;

// ========================================================
// SETUP
// ========================================================
void setup() {
  Serial.begin(115200);

  tft.init(240, 320);
  tft.setRotation(1);
  tft.fillScreen(BG_COLOR);

  pinMode(BTN_MODE, INPUT_PULLUP);
  pinMode(BTN_TIME, INPUT_PULLUP);
  pinMode(BTN_VOLT, INPUT_PULLUP);
  pinMode(BTN_HOLD, INPUT_PULLUP);

  drawHeader();
  fullRefresh();

  for (int i = 0; i < 320; i++)
    prevWaveBuff[i] = 512;
}

// ========================================================
// LOOP
// ========================================================
void loop() {

  checkButtons();

  if (!hold)
    captureData();

  updateHeader();

  if (fftMode)
    renderFFT();
  else
    renderOscilloscope();

  yield();
}

// ========================================================
// BUTTON HANDLING
// ========================================================
void checkButtons() {

  bool m = digitalRead(BTN_MODE);
  if (m == LOW && lastMode == HIGH) {
    fftMode = !fftMode;
    fullRefresh();
    delay(150);
  }
  lastMode = m;

  bool t = digitalRead(BTN_TIME);
  if (t == LOW && lastTime == HIGH) {
    tIndex = (tIndex + 1) % 7;
    fullRefresh();
    delay(150);
  }
  lastTime = t;

  bool v = digitalRead(BTN_VOLT);
  if (v == LOW && lastVolt == HIGH) {
    vIndex = (vIndex + 1) % 10;
    fullRefresh();
    delay(150);
  }
  lastVolt = v;

  bool h = digitalRead(BTN_HOLD);
  if (h == LOW && lastHold == HIGH) {
    hold = !hold;
    delay(150);
  }
  lastHold = h;
}

// ========================================================
// DATA CAPTURE
// ========================================================
void captureData() {

  long sum = 0;
  int crossings = 0;
  int mid = 512;

  unsigned long startT = micros();
  int delayVal = timeArray[tIndex];

  for (int i = 0; i < 320; i++) {

    waveBuff[i] = analogRead(ANALOG_PIN);
    sum += waveBuff[i];

    if (i > 0 && waveBuff[i - 1] < mid && waveBuff[i] >= mid)
      crossings++;

    if (i < 256) {
      vReal[i] = waveBuff[i];
      vImag[i] = 0;
    }

    if (delayVal > 0)
      delayMicroseconds(delayVal);
  }

  float elapsed = (micros() - startT) / 1000000.0;

  avgVoltage = (sum / 320.0) * (3.3 / 1023.0);
  frequency = (elapsed > 0) ? (crossings / elapsed) : 0;
}

// ========================================================
// HEADER
// ========================================================
void drawHeader() {
  tft.fillRect(0, 0, 320, 44, 0x0008);
  tft.drawFastHLine(0, 44, 320, TEXT_COLOR);
}

void updateHeader() {

  tft.setTextSize(1);

  tft.setTextColor(0x07FF, 0x0008);
  tft.setCursor(5, 8);
  tft.printf("MODE:%s", fftMode ? "FFT" : "OSC");

  tft.setTextColor(hold ? ST77XX_RED : ST77XX_GREEN, 0x0008);
  tft.setCursor(80, 8);
  tft.printf("STAT:%s", hold ? "HOLD" : "RUN ");

  tft.setTextColor(GRID_COLOR, 0x0008);
  tft.setCursor(160, 8);
  tft.printf("T:%dus ", timeArray[tIndex]);

  tft.setTextColor(GRID_COLOR, 0x0008);
  tft.setCursor(5, 26);
  tft.printf("V/div:%.1f", voltArray[vIndex]);

  tft.setTextColor(GRID_COLOR, 0x0008);
  tft.setCursor(110, 26);
  tft.printf("AVG:%.2fV", avgVoltage);

  tft.setTextColor(GRID_COLOR, 0x0008);
  tft.setCursor(210, 26);
  if (frequency > 999)
    tft.printf("F:%.1fkHz", frequency / 1000.0);
  else
    tft.printf("F:%.0fHz ", frequency);
}

// ========================================================
// GRID
// ========================================================
void drawStaticGrid() {
  tft.setTextSize(1);
  
  for (int c = 0; c <= GRID_COLS; c++) {
    int x = PLOT_X_START + (c * (PLOT_W / GRID_COLS));
    for (int y = PLOT_Y_START; y <= PLOT_Y_END; y += 4) {
      tft.drawPixel(x, y, GRID_COLOR); // Light gray dots
    }
    tft.setTextColor(TEXT_COLOR);
    tft.setCursor(x - 6, PLOT_Y_END + 5);
    tft.printf("%d", c * timeArray[tIndex]);
  }

  for (int r = 0; r <= GRID_ROWS; r++) {
    int y = PLOT_Y_START + (r * (PLOT_H / GRID_ROWS));
    for (int x = PLOT_X_START; x <= PLOT_X_END; x += 4) {
      tft.drawPixel(x, y, GRID_COLOR);
    }
    float vVal = 3.3 - (r * (3.3 / GRID_ROWS)); 
    tft.setTextColor(TEXT_COLOR);
    tft.setCursor(2, y - 3); 
    tft.printf("%.1fV", vVal);
  }
}

void fullRefresh() {
  // Clear the plot area and labels with White
  tft.fillRect(0, UI_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT - UI_HEIGHT, BG_COLOR);
  drawStaticGrid();
}
// ========================================================
// OSCILLOSCOPE RENDER
// ========================================================
void renderOscilloscope() {
  float gain = 1.0 / voltArray[vIndex];

  for (int i = 0; i < PLOT_W - 1; i++) {
    // 1. Erase previous line using BG_COLOR (White)
    tft.drawLine(PLOT_X_START + i, prevWaveBuff[i], PLOT_X_START + i + 1, prevWaveBuff[i+1], BG_COLOR);

    float s1 = (waveBuff[i] - 512) * gain + 512;
    float s2 = (waveBuff[i+1] - 512) * gain + 512;

    int ny1 = constrain(map(s1, 0, 1023, PLOT_Y_END, PLOT_Y_START), PLOT_Y_START, PLOT_Y_END);
    int ny2 = constrain(map(s2, 0, 1023, PLOT_Y_END, PLOT_Y_START), PLOT_Y_START, PLOT_Y_END);

    // 2. Draw new waveform in Blue or Black
    tft.drawLine(PLOT_X_START + i, ny1, PLOT_X_START + i + 1, ny2, WAVE_COLOR);

    prevWaveBuff[i] = ny1;
    if (i == PLOT_W - 2) prevWaveBuff[i+1] = ny2;
  }
}
// ========================================================
// FFT RENDER
// ========================================================
void renderFFT() {
  // We do NOT call fillRect(0, 46, 320, 192) here anymore to prevent flicker.
  // Instead, we redraw the grid pixels only where they were "stepped on".
  
  FFT.windowing(vReal, 256, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  FFT.compute(vReal, vImag, 256, FFT_FORWARD);
  FFT.complexToMagnitude(vReal, vImag, 256);

  for (int i = 2; i < 128; i++) {
    int x = PLOT_X_START + (i * 2);
    if (x >= PLOT_X_END) break; // Keep it bounded within the grid

    // Calculate new bar height
    int newBarH = map(vReal[i], 0, 4000, 0, PLOT_H);
    newBarH = constrain(newBarH, 0, PLOT_H);
    int prevBarH = prevFFTBuff[i];

    // --- Flicker-Free Logic ---
    if (newBarH < prevBarH) {
      // New bar is shorter: Erase the top part with BG_COLOR
      tft.fillRect(x, PLOT_Y_END - prevBarH, 2, prevBarH - newBarH, BG_COLOR);
      
      // Redraw grid dots in the erased area if necessary
      // (Optional: for simplicity, we refresh grid via checkButtons when needed)
    } 
    else if (newBarH > prevBarH) {
      // New bar is taller: Draw only the extra height
      tft.fillRect(x, PLOT_Y_END - newBarH, 2, newBarH - prevBarH, WAVE_COLOR);
    }
    // If they are equal, do nothing (no flicker!)

    prevFFTBuff[i] = newBarH;
  }
}   
