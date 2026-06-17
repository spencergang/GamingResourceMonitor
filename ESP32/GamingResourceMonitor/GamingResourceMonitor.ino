#include <TFT_eSPI.h>
#include <ArduinoJson.h>
#include <lvgl.h>
#include "src/ui.h" // Your EEZ Studio exported UI header
#include "src/vars.h" // Your EEZ Studio exported UI header

/* CYD Screen Resolution */
static const uint16_t screenWidth  = 320;
static const uint16_t screenHeight = 240;

TFT_eSPI tft = TFT_eSPI(screenWidth, screenHeight);

/* LVGL draw buffer - 1/10th screen size in bytes (16-bit RGB565 color format = 2 bytes per pixel) */
#define BYTE_PER_PIXEL 2
static uint8_t buf[screenWidth * screenHeight * BYTE_PER_PIXEL / 10];

int32_t cpu_percentage_bar_value = 0;
char cpu_percentage_value[100] = "0%";

int32_t ram_percentage_bar_value = 0;
char ram_percentage_value[100] = "0%";

int32_t vram_percentage_bar_value = 0;
char vram_percentage_value[100] = "0%";

int32_t get_var_cpu_percentage_bar_value() {
    return cpu_percentage_bar_value;
}

void set_var_cpu_percentage_bar_value(int32_t value) {
    cpu_percentage_bar_value = value;
}

const char *get_var_cpu_percentage_value() {
    return cpu_percentage_value;
}

void set_var_cpu_percentage_value(const char *value) {
    strncpy(cpu_percentage_value, value, sizeof(cpu_percentage_value) / sizeof(char));
    cpu_percentage_value[sizeof(cpu_percentage_value) / sizeof(char) - 1] = 0;
}

const char *get_var_ram_percentage_value() {
    return ram_percentage_value;
}

void set_var_ram_percentage_value(const char *value) {
    strncpy(ram_percentage_value, value, sizeof(ram_percentage_value) / sizeof(char));
    ram_percentage_value[sizeof(ram_percentage_value) / sizeof(char) - 1] = 0;
}

int32_t get_var_ram_percentage_bar_value() {
    return ram_percentage_bar_value;
}

void set_var_ram_percentage_bar_value(int32_t value) {
    ram_percentage_bar_value = value;
}

const char *get_var_vram_percentage_value() {
    return vram_percentage_value;
}

void set_var_vram_percentage_value(const char *value) {
    strncpy(vram_percentage_value, value, sizeof(vram_percentage_value) / sizeof(char));
    vram_percentage_value[sizeof(vram_percentage_value) / sizeof(char) - 1] = 0;
}

int32_t get_var_vram_percentage_bar_value() {
    return vram_percentage_bar_value;
}

void set_var_vram_percentage_bar_value(int32_t value) {
    vram_percentage_bar_value = value;
}

/* * 1. Display Flushing Function (LVGL 9 Callback Signature)
 * Passes a generic byte pointer instead of the old color struct array
 */
void my_disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *color_p) {
    uint32_t w = lv_area_get_width(area);
    uint32_t h = lv_area_get_height(area);


    tft.startWrite();
    tft.setAddrWindow(area->x1, area->y1, w, h);
    tft.pushColors((uint16_t *)color_p, w * h, true);
    tft.endWrite();

    // Notify LVGL that flushing is complete
    lv_display_flush_ready(disp);
}

/* * 3. Custom Tick Callback
 * LVGL 9 allows you to register a function that returns milliseconds directly.
 * This completely removes the need to track deltas manually in the loop.
 */
uint32_t my_tick_cb(void) {
    return millis();
}

void setup() {
    Serial.begin(115200);

    pinMode(21, OUTPUT);
    digitalWrite(21, HIGH);
    tft.begin();
    tft.setRotation(1); // Landscape orientation

    lv_init();
    
    lv_tick_set_cb(my_tick_cb);

    lv_display_t *disp = lv_display_create(screenWidth, screenHeight);
    lv_display_set_flush_cb(disp, my_disp_flush);
    lv_display_set_buffers(disp, buf, NULL, sizeof(buf), LV_DISPLAY_RENDER_MODE_PARTIAL);

    ui_init(); 

    lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0x1A1A1A), LV_PART_MAIN); 
    lv_obj_set_style_bg_opa(lv_screen_active(), LV_OPA_COVER, LV_PART_MAIN);
}

void update_metrics() {
  if (Serial.available() > 0) {
    String jsonPayload = Serial.readStringUntil('\n');

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, jsonPayload);

    if (!error) {
      float cpu_percentage = doc["CpuPercentage"];
      float ram_percentage = doc["RamPercentage"];
      float vram_percentage = doc["VramPercentage"];

      String cpu_pct_str = String(cpu_percentage, 0);
      cpu_pct_str += "%";
      set_var_cpu_percentage_value(cpu_pct_str.c_str());
      set_var_cpu_percentage_bar_value(static_cast<int>(cpu_percentage));

      String ram_pct_str = String(ram_percentage, 0);
      ram_pct_str += "%";
      set_var_ram_percentage_value(ram_pct_str.c_str());
      set_var_ram_percentage_bar_value(static_cast<int>(ram_percentage));

      String vram_pct_str = String(vram_percentage, 0);
      vram_pct_str += "%";
      set_var_vram_percentage_value(vram_pct_str.c_str());
      set_var_vram_percentage_bar_value(static_cast<int>(vram_percentage));
    }
  }
}

void loop() {
    update_metrics();
    lv_timer_handler(); 
    ui_tick();
    delay(5);
}