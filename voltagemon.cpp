#include <math.h>
#include <vector>

#include "hardware/adc.h"

#include "pico/multicore.h"

#include "drivers/st7789/st7789.hpp"
#include "libraries/pico_graphics/pico_graphics.hpp"

using namespace pimoroni;

const int WIDTH = 240;
const int HEIGHT = 240;

ST7789 st7789(WIDTH, HEIGHT, ROTATE_0, true, get_spi_pins(BG_SPI_FRONT));
PicoGraphics_PenP4 graphics(st7789.width, st7789.height, nullptr);

#define NUM_READINGS 1024
#define ANGLE_PER_READING 0.0122718463f
static float readings[NUM_READINGS] = {0};

Point pt_for_reading(int i) {
  float angle = i * ANGLE_PER_READING;
  Point pt;
  pt.x = 120 + sinf(angle) * 24 * readings[i];
  pt.y = 120 + cosf(angle) * 24 * readings[i];
  return pt;
}

void core1_main() {
  const float adc_conversion_factor = 6.6f / (1 << 12);

  Pen BG = graphics.create_pen(0, 0, 0);
  Pen WHITE = graphics.create_pen(255, 255, 255);
  Pen GREEN = graphics.create_pen(0, 255, 0);
  Pen DARK_GREEN = graphics.create_pen(0, 170, 0);

  graphics.set_pen(BG);
  graphics.clear();
  graphics.set_pen(WHITE);
  graphics.rectangle(Rect(0, 120, 240, 1));
  graphics.rectangle(Rect(120, 0, 1, 240));
  for (int i = 1; i < 5; ++i) {
    graphics.rectangle(Rect(120+24*i, 110, 1, 10));
    graphics.rectangle(Rect(120-24*i, 110, 1, 10));
    graphics.rectangle(Rect(120, 120+24*i, 10, 1));
    graphics.rectangle(Rect(120, 120-24*i, 10, 1));
  }

  int i = 0;

  while (true) {
    // Clear previous value
    Point old_pt = pt_for_reading(i);
    if (old_pt.x == 120 || old_pt.y == 120 || 
        ((old_pt.x % 24) == 0 && old_pt.y < 120 && old_pt.y >= 110) ||
        ((old_pt.y % 24) == 0 && old_pt.x > 120 && old_pt.x <= 130)) 
    {
      graphics.set_pen(WHITE);
    } else {
      graphics.set_pen(BG);
    }
    graphics.set_pixel(old_pt);
    graphics.set_pen(DARK_GREEN);
    graphics.set_pixel(pt_for_reading(i ^ 0x200));

    // Read ADC
    readings[i] = adc_read() * adc_conversion_factor;
    graphics.set_pen(GREEN);
    graphics.set_pixel(pt_for_reading(i));

    i = (i + 1) & (NUM_READINGS - 1);

    sleep_us(1900);
  }
}

int main() {
  stdio_init_all();

  st7789.set_backlight(200);

  adc_init();
  adc_gpio_init(28);
  adc_select_input(2);

  multicore_launch_core1(core1_main);
  sleep_ms(1);

  while(true) {
    // update screen
    st7789.update(&graphics);
  }

  return 0;
}
