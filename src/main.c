#include <pebble.h>

static Window *s_main_window;
static const int NB_LAYER = 8;
static TextLayer *s_time_layer[8];
static char buffer[][8] = {"01", "02", "03", "04", "05", "05", "06", "07"};
static GFont s_custom_font;
static GFont s_custom_font_mirror;
static GFont s_custom_font_mini;
static GFont s_custom_font_mini_mirror;

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  
  // Write the current hours and minutes into the buffer
  if(clock_is_24h_style() == true) {
    // Use 24 hour format
    strftime(buffer[0], sizeof("00"), "%H", tick_time);
    buffer[1][0] = buffer[0][1];
    buffer[1][1] = buffer[0][0];
  } else {
    // Use 12 hour format
    strftime(buffer[0], sizeof("00"), "%I", tick_time);
    buffer[1][0] = buffer[0][1];
    buffer[1][1] = buffer[0][0];
  }
  strftime(buffer[2], sizeof("00"), "%M", tick_time);
  buffer[3][0] = buffer[2][1];
  buffer[3][1] = buffer[2][0];
  
  strftime(buffer[4], sizeof("00"), "%d", tick_time);
  buffer[5][0] = buffer[4][1];
  buffer[5][1] = buffer[4][0];
  
  strftime(buffer[6], sizeof("00"), "%m", tick_time);
  buffer[7][0] = buffer[6][1];
  buffer[7][1] = buffer[6][0];
  
  // Display this time on the TextLayer
  for(int i = 0; i < NB_LAYER; i++){ 
    text_layer_set_text(s_time_layer[i], buffer[i]);
  }
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static TextLayer* create_text_layer(GRect rect, GFont font){
  TextLayer *part_layer = text_layer_create(rect);
  text_layer_set_background_color(part_layer, GColorClear);
  text_layer_set_text_color(part_layer, GColorWhite);
  text_layer_set_text(part_layer, "00");
  text_layer_set_font(part_layer, font);
  text_layer_set_text_alignment(part_layer, GTextAlignmentLeft);
  return part_layer;
}

static void main_window_load(Window *window) {
  // Load custom font
  s_custom_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_SQR_NUM_70));
  s_custom_font_mirror = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_SQR_NUM_MIRROR_70));
  s_custom_font_mini = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_SQR_NUM_34));
  s_custom_font_mini_mirror = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_SQR_NUM_MIRROR_34));
  
  // Create time TextLayer
  s_time_layer[0] = create_text_layer(GRect(8, 15, 65, 70),s_custom_font);
  s_time_layer[1] = create_text_layer(GRect(72, 15, 65, 70),s_custom_font_mirror);
  s_time_layer[2] = create_text_layer(GRect(8, 63, 65, 70),s_custom_font);
  s_time_layer[3] = create_text_layer(GRect(72, 63, 65, 70),s_custom_font_mirror);
  s_time_layer[4] = create_text_layer(GRect(41, 2, 65, 70),s_custom_font_mini);
  s_time_layer[5] = create_text_layer(GRect(73, 2, 65, 70),s_custom_font_mini_mirror);
  s_time_layer[6] = create_text_layer(GRect(41, 122, 65, 70),s_custom_font_mini);
  s_time_layer[7] = create_text_layer(GRect(73, 122, 65, 70),s_custom_font_mini_mirror);
  
  // Add it as a child layer to the Window's root layer
  for(int i = 0; i < NB_LAYER; i++){ 
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer[i]));
  }
  
  // Make sure the time is displayed from the start
  update_time();
}

static void main_window_unload(Window *window) {
  for(int i = 0; i < NB_LAYER; i++){ 
    text_layer_destroy(s_time_layer[i]);
  }  
  fonts_unload_custom_font(s_custom_font);
  fonts_unload_custom_font(s_custom_font_mirror);
  fonts_unload_custom_font(s_custom_font_mini);
  fonts_unload_custom_font(s_custom_font_mini_mirror);
}

static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  
  //set window background color
  window_set_background_color(s_main_window, GColorDarkCandyAppleRed);

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
  
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void deinit() {
  // Destroy Window
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}