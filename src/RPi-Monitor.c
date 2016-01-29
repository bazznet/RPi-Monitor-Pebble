#include <pebble.h>
#include "progress_bar.h"

static Window *window;
static BitmapLayer *slider_layer, *logo_layer;
static InverterLayer *inverter_layer;
static GBitmap *logo_bitmap, *slider0_bitmap, *slider1_bitmap, *slider2_bitmap, *slider3_bitmap;

static Layer *slide_start;
static TextLayer *caption_text_layer, *status_text_layer;

static Layer *slide0; 
static TextLayer *cpu_title_text_layer, *cpu_text_layer;

static Layer *slide1;
static TextLayer *memory_title_text_layer, *swap_title_text_layer, *memory_text_layer, *swap_text_layer;
static ProgressBarLayer *memory_bar, *swap_bar;

static Layer *slide2;
static TextLayer *sdcard_title_text_layer, *network_title_text_layer, *sdcard_text_layer, *network_text_layer;
static ProgressBarLayer *root_bar;

static Layer *slide3;
static TextLayer *version_title_text_layer, *version_text_layer;

char cpu[256], memory[128], swap[128], sdcard[128], net[128], version[256];
bool load;

enum {
	OK = 0, CPU = 1, MEMORY = 2, MEMORY_P = 3, SWAP = 4, SWAP_P = 5, SDCARD = 6, SDCARD_P = 7, NET = 8, HOME = 9
};


int slide = 0; 

void set_slide(int s)
{
  switch(s) {
    case 0:
      bitmap_layer_set_bitmap(slider_layer, slider0_bitmap);
  	  layer_add_child((Layer *)slide0, bitmap_layer_get_layer(slider_layer));
  	  layer_add_child((Layer *)slide0, inverter_layer_get_layer(inverter_layer));
      layer_set_hidden((Layer *)slide0, false);
      layer_set_hidden((Layer *)slide1, true);
      layer_set_hidden((Layer *)slide2, true);
      layer_set_hidden((Layer *)slide3, true);
      break;
    case 1:
      bitmap_layer_set_bitmap(slider_layer, slider1_bitmap);
  	  layer_add_child((Layer *)slide1, bitmap_layer_get_layer(slider_layer));
  	  layer_add_child((Layer *)slide1, inverter_layer_get_layer(inverter_layer));
      layer_set_hidden((Layer *)slide0, true);
      layer_set_hidden((Layer *)slide1, false);
      layer_set_hidden((Layer *)slide2, true);
      layer_set_hidden((Layer *)slide3, true);
      break;
    case 2:
      bitmap_layer_set_bitmap(slider_layer, slider2_bitmap);
  	  layer_add_child((Layer *)slide2, bitmap_layer_get_layer(slider_layer));
      layer_add_child((Layer *)slide2, inverter_layer_get_layer(inverter_layer));
      layer_set_hidden((Layer *)slide0, true);
      layer_set_hidden((Layer *)slide1, true);
      layer_set_hidden((Layer *)slide2, false);
      layer_set_hidden((Layer *)slide3, true);
      break;
    case 3:
      bitmap_layer_set_bitmap(slider_layer, slider3_bitmap);
  	  layer_add_child((Layer *)slide3, bitmap_layer_get_layer(slider_layer));
      layer_add_child((Layer *)slide3, inverter_layer_get_layer(inverter_layer));
      layer_set_hidden((Layer *)slide0, true);
      layer_set_hidden((Layer *)slide1, true);
      layer_set_hidden((Layer *)slide2, true);
      layer_set_hidden((Layer *)slide3, false);
      break;
  }
}

void up_click_handler(ClickRecognizerRef recognizer, void *context)
{
  if (slide > 0 && load) {
    set_slide(--slide);
  }
}

void down_click_handler(ClickRecognizerRef recognizer, void *context)
{
  if (slide < 3 && load) {
    set_slide(++slide);
  }
}

void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

void process_tuple(Tuple *t)
{
	int key = t->key;
	int value = t->value->uint8;
	char string_value[256];
  	strcpy(string_value, t->value->cstring);

	switch(key) {
		case OK:
			if (value == 1)
			{
				load = true;
				layer_set_hidden((Layer *)slide_start, true);
				set_slide(0);
			}
			break;
		case CPU:
			strcat(cpu, string_value);
			text_layer_set_text(cpu_text_layer, (char *) &cpu);
			break;
		case MEMORY:
			strcat(memory, string_value);
			text_layer_set_text(memory_text_layer, (char *) &memory);
			break;
		case MEMORY_P:
			progress_bar_layer_set_value(memory_bar, value);
			break;
		case SWAP:
			strcat(swap, string_value);
			text_layer_set_text(swap_text_layer, (char *) &swap);
			break;
		case SWAP_P:
			progress_bar_layer_set_value(swap_bar, value);
			break;
		case SDCARD:
			strcat(sdcard, string_value);
			text_layer_set_text(sdcard_text_layer, (char *) &sdcard);
			break;
		case SDCARD_P:
			progress_bar_layer_set_value(root_bar, value);
			break;
		case NET:
			strcat(net, string_value);
			text_layer_set_text(network_text_layer, (char *) &net);
			break;
		case HOME:
			strcat(version, string_value);
			text_layer_set_text(version_text_layer, (char *) &version);
			break;
	}

}

static void in_received_handler(DictionaryIterator *iter, void *context)
{
	(void) context;

	Tuple *t = dict_read_first(iter);
	while(t != NULL){
		process_tuple(t);
		t = dict_read_next(iter);
	}
}

static TextLayer* init_text_layer(GRect location, GColor colour, GColor background, const char *res_id, GTextAlignment alignment)
{
  TextLayer *layer = text_layer_create(location);
  text_layer_set_text_color(layer, colour);
  text_layer_set_background_color(layer, background);
  text_layer_set_font(layer, fonts_get_system_font(res_id));
  text_layer_set_text_alignment(layer, alignment);
 
  return layer;
}

void window_load(Window *window)
{
  	inverter_layer = inverter_layer_create(GRect(0, 0, 144, 13));
  	slider_layer = bitmap_layer_create(GRect(49, 1, 46, 10));
  	slider0_bitmap = gbitmap_create_with_resource(RESOURCE_ID_SLIDER0);
  	slider1_bitmap = gbitmap_create_with_resource(RESOURCE_ID_SLIDER1);
  	slider2_bitmap = gbitmap_create_with_resource(RESOURCE_ID_SLIDER2);
  	slider3_bitmap = gbitmap_create_with_resource(RESOURCE_ID_SLIDER3);

  	/////////////////////
  	slide_start = layer_create(GRect(0, 0, 144, 168));
  	layer_add_child(window_get_root_layer(window), (Layer *)slide_start);

  	logo_layer = bitmap_layer_create(GRect(32, 10, 80, 103));
  	logo_bitmap = gbitmap_create_with_resource(RESOURCE_ID_LOGO);
  	bitmap_layer_set_bitmap(logo_layer, logo_bitmap);
  	layer_add_child((Layer *)slide_start, bitmap_layer_get_layer(logo_layer));

  	caption_text_layer = init_text_layer(GRect(0, 105, 144, 40), GColorBlack, GColorClear, "RESOURCE_ID_GOTHIC_28_BOLD", GTextAlignmentCenter);
  	text_layer_set_text(caption_text_layer, "RPi-Monitor");
  	layer_add_child((Layer *)slide_start, text_layer_get_layer(caption_text_layer));

  	status_text_layer = init_text_layer(GRect(0, 135, 144, 25), GColorBlack, GColorClear, "RESOURCE_ID_GOTHIC_18", GTextAlignmentCenter);
  	text_layer_set_text(status_text_layer, "Loading...");
  	layer_add_child((Layer *)slide_start, text_layer_get_layer(status_text_layer));

  	/////////////////////
  	slide0 = layer_create(GRect(0, 0, 144, 168));
  	layer_add_child(window_get_root_layer(window), (Layer *)slide0);
  	layer_set_hidden((Layer *)slide0, true);

  	cpu_title_text_layer = init_text_layer(GRect(0, 12, 144, 20), GColorBlack, GColorClear, "RESOURCE_ID_GOTHIC_18_BOLD", GTextAlignmentCenter);
  	text_layer_set_text(cpu_title_text_layer, "CPU");
  	layer_add_child((Layer *)slide0, text_layer_get_layer(cpu_title_text_layer));

  	cpu_text_layer = init_text_layer(GRect(2, 32, 140, 136), GColorBlack, GColorClear, "RESOURCE_ID_GOTHIC_14", GTextAlignmentLeft);
  	layer_add_child((Layer *)slide0, text_layer_get_layer(cpu_text_layer));

  	/////////////////////
  	slide1 = layer_create(GRect(0, 0, 144, 168));
  	layer_add_child(window_get_root_layer(window), (Layer *)slide1);
  	layer_set_hidden((Layer *)slide1, true);

  	memory_title_text_layer = init_text_layer(GRect(0, 12, 144, 20), GColorBlack, GColorClear, "RESOURCE_ID_GOTHIC_18_BOLD", GTextAlignmentCenter);
  	text_layer_set_text(memory_title_text_layer, "MEMORY");
  	layer_add_child((Layer *)slide1, text_layer_get_layer(memory_title_text_layer));

  	memory_bar = progress_bar_layer_create((GRect) { .origin = { 5, 37 }, .size = { 134, 5 } });
  	layer_add_child((Layer *)slide1, memory_bar);

  	memory_text_layer = init_text_layer(GRect(2, 42, 140, 48), GColorBlack, GColorClear, "RESOURCE_ID_GOTHIC_14", GTextAlignmentLeft);
  	layer_add_child((Layer *)slide1, text_layer_get_layer(memory_text_layer));

  	swap_title_text_layer = init_text_layer(GRect(0, 90, 144, 20), GColorBlack, GColorClear, "RESOURCE_ID_GOTHIC_18_BOLD", GTextAlignmentCenter);
  	text_layer_set_text(swap_title_text_layer, "SWAP");
  	layer_add_child((Layer *)slide1, text_layer_get_layer(swap_title_text_layer));

  	swap_bar = progress_bar_layer_create((GRect) { .origin = { 5, 115 }, .size = { 134, 5 } });
  	layer_add_child((Layer *)slide1, swap_bar);
  	
  	swap_text_layer = init_text_layer(GRect(2, 120, 140, 48), GColorBlack, GColorClear, "RESOURCE_ID_GOTHIC_14", GTextAlignmentLeft);
  	layer_add_child((Layer *)slide1, text_layer_get_layer(swap_text_layer));

  	////////////////////
  	slide2 = layer_create(GRect(0, 0, 144, 168));
  	layer_add_child(window_get_root_layer(window), (Layer *)slide2);
  	layer_set_hidden((Layer *)slide2, true);

  	sdcard_title_text_layer = init_text_layer(GRect(0, 12, 144, 20), GColorBlack, GColorClear, "RESOURCE_ID_GOTHIC_18_BOLD", GTextAlignmentCenter);
  	text_layer_set_text(sdcard_title_text_layer, "SD CARD");
  	layer_add_child((Layer *)slide2, text_layer_get_layer(sdcard_title_text_layer));

  	root_bar = progress_bar_layer_create((GRect) { .origin = { 5, 37 }, .size = { 134, 5 } });
  	layer_add_child((Layer *)slide2, root_bar);

  	sdcard_text_layer = init_text_layer(GRect(2, 42, 140, 48), GColorBlack, GColorClear, "RESOURCE_ID_GOTHIC_14", GTextAlignmentLeft);
  	layer_add_child((Layer *)slide2, text_layer_get_layer(sdcard_text_layer));

  	network_title_text_layer = init_text_layer(GRect(0, 90, 144, 20), GColorBlack, GColorClear, "RESOURCE_ID_GOTHIC_18_BOLD", GTextAlignmentCenter);
  	text_layer_set_text(network_title_text_layer, "NETWORK");
  	layer_add_child((Layer *)slide2, text_layer_get_layer(network_title_text_layer));

  	network_text_layer = init_text_layer(GRect(2, 115, 140, 48), GColorBlack, GColorClear, "RESOURCE_ID_GOTHIC_14", GTextAlignmentLeft);
  	layer_add_child((Layer *)slide2, text_layer_get_layer(network_text_layer));

  	/////////////////////
  	slide3 = layer_create(GRect(0, 0, 144, 168));
  	layer_add_child(window_get_root_layer(window), (Layer *)slide3);
  	layer_set_hidden((Layer *)slide3, true);

  	version_title_text_layer = init_text_layer(GRect(0, 12, 144, 20), GColorBlack, GColorClear, "RESOURCE_ID_GOTHIC_18_BOLD", GTextAlignmentCenter);
  	text_layer_set_text(version_title_text_layer, "HOME");
  	layer_add_child((Layer *)slide3, text_layer_get_layer(version_title_text_layer));

  	version_text_layer = init_text_layer(GRect(2, 32, 140, 136), GColorBlack, GColorClear, "RESOURCE_ID_GOTHIC_14", GTextAlignmentLeft);
  	layer_add_child((Layer *)slide3, text_layer_get_layer(version_text_layer));
}

void window_unload(Window *window) {
	inverter_layer_destroy(inverter_layer);
	bitmap_layer_destroy(slider_layer);
	gbitmap_destroy(slider0_bitmap);
	gbitmap_destroy(slider1_bitmap);
	gbitmap_destroy(slider2_bitmap);
	gbitmap_destroy(slider3_bitmap);

	layer_destroy(slide_start);
	text_layer_destroy(caption_text_layer);
	text_layer_destroy(status_text_layer);

  	layer_destroy(slide0);  
  	text_layer_destroy(cpu_title_text_layer);
  	text_layer_destroy(cpu_text_layer);

  	layer_destroy(slide1);
  	text_layer_destroy(memory_title_text_layer);
  	progress_bar_layer_destroy(memory_bar);
  	text_layer_destroy(swap_title_text_layer);
  	progress_bar_layer_destroy(swap_bar);
  	text_layer_destroy(swap_text_layer);

  	layer_destroy(slide2);
  	text_layer_destroy(sdcard_title_text_layer);
  	progress_bar_layer_destroy(root_bar);
  	text_layer_destroy(sdcard_text_layer);
  	text_layer_destroy(network_title_text_layer);
  	text_layer_destroy(network_text_layer);

  	layer_destroy(slide3);
  	text_layer_destroy(version_title_text_layer);
  	text_layer_destroy(version_text_layer);
}

void init(void) {
 
  window = window_create();
  window_set_fullscreen(window, true);

  WindowHandlers handlers = {
    .load = window_load,
    .unload = window_unload
  };
  window_set_window_handlers(window, handlers);
  window_set_click_config_provider(window, click_config_provider);

  app_message_register_inbox_received(in_received_handler);
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());

  window_stack_push(window, true);
}

void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
