/*

   A slightly less simple design inspired by the Simplicity design

*/

#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

#define MY_UUID {0x39, 0xCE, 0xD3, 0xEA, 0x04, 0x56, 0x46, 0x87, 0x83, 0x61, 0xDC, 0xBD, 0x46, 0xC2, 0xDA, 0x0B}
PBL_APP_INFO(MY_UUID, "Less Simple", "Travis Petticrew", 0x2, 0x0, RESOURCE_ID_IMAGE_MENU_ICON, APP_INFO_WATCH_FACE);

#define SCREEN_WIDTH 144
#define SCREEN_HEIGHT 168
#define DATE_TOP_PADDING 12
#define PADDING 10
#define LINE_HEIGHT 50

Window window;

PblTm display_time;

TextLayer date_layer;
TextLayer time_layer;
TextLayer second_layer;

Layer lineLayer;

void update_seconds_text(PblTm *current_time) {
  static char second_text[] = "00";
  string_format_time(second_text, sizeof(second_text), "%S", current_time);
  text_layer_set_text(&second_layer, second_text);
}

void update_time_text(PblTm *current_time) {
  static char time_text[] = "00:00";
  string_format_time(time_text, sizeof(time_text), "%R", current_time);
  text_layer_set_text(&time_layer, time_text);
  display_time.tm_min = current_time->tm_min;
}

void update_date_text(PblTm *current_time) {
  static char date_text[] = "Xxxxxxxxx 00";
  string_format_time(date_text, sizeof(date_text), "%A %e", current_time);
  text_layer_set_text(&date_layer, date_text);
  display_time.tm_mday = current_time->tm_mday;
}

void handle_second_tick(AppContextRef ctx, PebbleTickEvent *t) {
  (void)t;
  (void)ctx;

  PblTm current_time;

  if(t)
    current_time = *t->tick_time;
  else
    get_time(&current_time);

  update_seconds_text(&current_time);

  if(current_time.tm_min != display_time.tm_min)
    update_time_text(&current_time);

  if(current_time.tm_mday != display_time.tm_mday)
    update_date_text(&current_time);
}


void line_layer_update_callback(Layer *me, GContext* ctx) {
  (void)me;

  graphics_context_set_stroke_color(ctx, GColorWhite);

  graphics_draw_line(ctx, GPoint(PADDING, DATE_TOP_PADDING + LINE_HEIGHT - 3), GPoint(SCREEN_WIDTH - PADDING, DATE_TOP_PADDING + LINE_HEIGHT - 3));
  graphics_draw_line(ctx, GPoint(PADDING, DATE_TOP_PADDING + LINE_HEIGHT - 4), GPoint(SCREEN_WIDTH - PADDING, DATE_TOP_PADDING + LINE_HEIGHT - 4));
}

void handle_init_app(AppContextRef app_ctx) {
  window_init(&window, "Less Simple");
  window_stack_push(&window, true);
  window_set_background_color(&window, GColorBlack);

  text_layer_init(&date_layer, GRect(PADDING*1.5, PADDING + DATE_TOP_PADDING, SCREEN_WIDTH-PADDING, LINE_HEIGHT));
  text_layer_set_text_color(&date_layer, GColorWhite);
  text_layer_set_background_color(&date_layer, GColorClear);
  text_layer_set_font(&date_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28));
  layer_add_child(&window.layer, &date_layer.layer);

  text_layer_init(&time_layer, GRect(0, PADDING + LINE_HEIGHT*1, SCREEN_WIDTH, LINE_HEIGHT));
  text_layer_set_text_color(&time_layer, GColorWhite);
  text_layer_set_text_alignment(&time_layer, GTextAlignmentCenter);
  text_layer_set_background_color(&time_layer, GColorClear);
  text_layer_set_font(&time_layer, fonts_get_system_font(FONT_KEY_GOTHAM_42_MEDIUM_NUMBERS));
  layer_add_child(&window.layer, &time_layer.layer);

  text_layer_init(&second_layer, GRect(0, LINE_HEIGHT*2, SCREEN_WIDTH, LINE_HEIGHT));
  text_layer_set_text_color(&second_layer, GColorWhite);
  text_layer_set_text_alignment(&second_layer, GTextAlignmentCenter);
  text_layer_set_background_color(&second_layer, GColorClear);
  text_layer_set_font(&second_layer, fonts_get_system_font(FONT_KEY_GOTHAM_42_LIGHT));
  layer_add_child(&window.layer, &second_layer.layer);

  // Ensures time is displayed immediately (will break if NULL tick event accessed).
  // (This is why it's a good idea to have a separate routine to do the update itself.)
  handle_second_tick(app_ctx, NULL);

  layer_init(&lineLayer, window.layer.frame);
  lineLayer.update_proc = &line_layer_update_callback;
  layer_add_child(&window.layer, &lineLayer);
}


void pbl_main(void *params) {
  PebbleAppHandlers handlers = {

    .init_handler = &handle_init_app,

    .tick_info = {
      .tick_handler = &handle_second_tick,
      .tick_units = SECOND_UNIT
    }

  };
  app_event_loop(params, &handlers);
}

