#include <FastLED.h>

#define DATA_PIN    5
#define W           8
#define H           8
#define P           3
#define BRIGHTNESS  33
#define DELAY_MS    33
#define MAX_CYCLES  199

CRGB leds[W * H];
bool board[2][P][W][H];
int t = 0;
int cycles = 0;

void randomizeBoard() {
  cycles = 0;
  for (int k = 0; k < P; k++) {
    for (int x = 0; x < W; x++) {
      for (int y = 0; y < H; y++) {
        board[t][k][x][y] = random(2);
      }
    }
  }
}

void setup() {
  Serial.begin(115200);
  randomSeed(analogRead(0));
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, W * H);
  FastLED.setBrightness(BRIGHTNESS);
  randomizeBoard();
}

void loop() {
  cycles++;
  int n = (t + 1) % 2;
  int changes = 0;

  // Compute next state
  for (int k = 0; k < P; k++) {
    for (int x = 0; x < W; x++) {
      for (int y = 0; y < H; y++) {
        int neighbours = 0;
        for (int dx = -1; dx <= 1; dx++) {
          for (int dy = -1; dy <= 1; dy++) {
            if (dx == 0 && dy == 0) continue;
            int nx = (x + dx + W) % W;
            int ny = (y + dy + H) % H;
            neighbours += board[t][k][nx][ny];
          }
        }

        bool alive = board[t][k][x][y];
        bool nextState = (neighbours == 3) || (alive && neighbours == 2);
        board[n][k][x][y] = nextState;
        if (nextState != alive) changes++;
      }
    }
  }

  t = n;

  // Reset if static or after a time
  if (changes == 0 || MAX_CYCLES < cycles) {
    delay(1000);
    randomizeBoard();
  }

  // Display update with mild fading
  for (int x = 0; x < W; x++) {
    for (int y = 0; y < H; y++) {
      CRGB color(0, 0, 0);
      if (board[t][0][x][y]) color.r = 255;
      if (board[t][1][x][y]) color.g = 255;
      if (board[t][2][x][y]) color.b = 255;

      leds[y * W + x].nscale8(128);
      leds[y * W + x] += color;
    }
  }

  FastLED.show();
  delay(DELAY_MS);
}
