#include "MenuFunctions.h"
#include "lang_var.h"
#include "BLEDeviceDetectors.h"
#include "WiFiCameraDetector.h"
#include "WiFiFlockDetector.h"
#include "WirelessActivityTools.h"
#include "WirelessDeviceScout.h"
#include "DroneRemoteID.h"
#include "DroneRemoteIDSpoofer.h"
#include "BLESecurityTools.h"

#ifdef HAS_SCREEN

extern const unsigned char menu_icons[][66];
extern LinkedList<AccessPoint>* access_points;
extern LinkedList<Station>* stations;
extern LinkedList<AirTag>* airtags;
extern LinkedList<Flipper>* flippers;
extern LinkedList<BleDevice>* ble_devices;

#ifdef HAS_MINI_SCREEN
namespace {
constexpr uint16_t MINI_MARQUEE_STEP_MS = 40;
constexpr uint16_t MINI_MARQUEE_PAUSE_MS = 900;

#ifdef MARAUDER_MINI_V3
constexpr uint16_t MINI_MENU_REPEAT_DELAY_MS = 450;
constexpr uint16_t MINI_MENU_REPEAT_INTERVAL_MS = 120;
constexpr uint16_t MINI_UI_SURFACE = 0x18E3;
constexpr uint16_t MINI_UI_BORDER = 0x31A6;
constexpr uint16_t MINI_UI_ACCENT = 0x733F;
constexpr uint16_t MINI_UI_SELECTED = 0x03E0;
constexpr uint16_t MINI_UI_TEXT = TFT_WHITE;
constexpr uint16_t MINI_UI_MUTED = 0xA514;
constexpr uint16_t MINI_UI_DANGER = TFT_RED;

bool miniMenuButtonDown(Switches& button) {
  const bool level = digitalRead(button.getPin());
  return button.getPullup() ? level == LOW : level == HIGH;
}

void drawMiniBackGlyph(int16_t x, int16_t y, uint16_t color) {
  display_obj.tft.drawLine(x + 4, y, x, y + 4, color);
  display_obj.tft.drawLine(x, y + 4, x + 4, y + 8, color);
  display_obj.tft.drawFastHLine(x, y + 4, 7, color);
}

void drawMiniChevron(int16_t x, int16_t y, uint16_t color) {
  display_obj.tft.drawLine(x, y, x + 3, y + 3, color);
  display_obj.tft.drawLine(x + 3, y + 3, x, y + 6, color);
}

void drawMiniSymbol(uint8_t icon, int16_t cx, int16_t cy, uint16_t color) {
  switch (icon) {
    case WIFI:
      display_obj.tft.drawLine(cx - 6, cy - 3, cx, cy - 7, color);
      display_obj.tft.drawLine(cx, cy - 7, cx + 6, cy - 3, color);
      display_obj.tft.drawLine(cx - 4, cy, cx, cy - 3, color);
      display_obj.tft.drawLine(cx, cy - 3, cx + 4, cy, color);
      display_obj.tft.fillCircle(cx, cy + 3, 1, color);
      break;
    case BLUETOOTH:
    case BLUETOOTH_SNIFF:
      display_obj.tft.drawFastVLine(cx, cy - 7, 15, color);
      display_obj.tft.drawLine(cx, cy - 7, cx + 5, cy - 2, color);
      display_obj.tft.drawLine(cx + 5, cy - 2, cx - 4, cy + 5, color);
      display_obj.tft.drawLine(cx, cy + 7, cx + 5, cy + 2, color);
      display_obj.tft.drawLine(cx + 5, cy + 2, cx - 4, cy - 5, color);
      break;
    case GPS_MENU:
      display_obj.tft.drawCircle(cx, cy - 2, 5, color);
      display_obj.tft.fillCircle(cx, cy - 2, 1, color);
      display_obj.tft.drawLine(cx - 3, cy + 2, cx, cy + 7, color);
      display_obj.tft.drawLine(cx + 3, cy + 2, cx, cy + 7, color);
      break;
    case GENERAL_APPS:
      display_obj.tft.drawRoundRect(cx - 7, cy - 4, 15, 9, 3, color);
      display_obj.tft.drawFastHLine(cx - 5, cy, 5, color);
      display_obj.tft.drawFastVLine(cx - 3, cy - 2, 5, color);
      display_obj.tft.fillCircle(cx + 4, cy - 1, 1, color);
      display_obj.tft.fillCircle(cx + 6, cy + 2, 1, color);
      break;
    case DEVICE:
    case DEVICE_INFO:
      display_obj.tft.drawRect(cx - 5, cy - 5, 11, 11, color);
      display_obj.tft.drawRect(cx - 2, cy - 2, 5, 5, color);
      for (int8_t p = -3; p <= 3; p += 3) {
        display_obj.tft.drawFastVLine(cx + p, cy - 7, 2, color);
        display_obj.tft.drawFastVLine(cx + p, cy + 6, 2, color);
        display_obj.tft.drawFastHLine(cx - 7, cy + p, 2, color);
        display_obj.tft.drawFastHLine(cx + 6, cy + p, 2, color);
      }
      break;
    case REBOOT:
      display_obj.tft.drawCircle(cx, cy, 6, color);
      display_obj.tft.drawFastVLine(cx, cy - 8, 7, color);
      break;
    case SNIFFERS:
    case BEACON_SNIFF:
    case DEAUTH_SNIFF:
    case PROBE_SNIFF:
      display_obj.tft.drawFastVLine(cx, cy - 1, 8, color);
      display_obj.tft.fillCircle(cx, cy - 3, 1, color);
      display_obj.tft.drawLine(cx - 3, cy - 5, cx - 5, cy - 2, color);
      display_obj.tft.drawLine(cx + 3, cy - 5, cx + 5, cy - 2, color);
      display_obj.tft.drawLine(cx - 5, cy + 7, cx + 5, cy + 7, color);
      break;
    case SCANNERS:
      display_obj.tft.drawCircle(cx - 2, cy - 2, 5, color);
      display_obj.tft.drawLine(cx + 2, cy + 2, cx + 7, cy + 7, color);
      break;
    case ATTACKS:
      display_obj.tft.drawLine(cx + 1, cy - 8, cx - 5, cy + 1, color);
      display_obj.tft.drawLine(cx - 5, cy + 1, cx, cy + 1, color);
      display_obj.tft.drawLine(cx, cy + 1, cx - 1, cy + 8, color);
      display_obj.tft.drawLine(cx - 1, cy + 8, cx + 6, cy - 2, color);
      display_obj.tft.drawLine(cx + 6, cy - 2, cx + 1, cy - 2, color);
      display_obj.tft.drawLine(cx + 1, cy - 2, cx + 1, cy - 8, color);
      break;
    case SETTINGS:
    case BRIGHTNESS:
      display_obj.tft.drawCircle(cx, cy, 5, color);
      display_obj.tft.fillCircle(cx, cy, 1, color);
      display_obj.tft.drawFastVLine(cx, cy - 8, 3, color);
      display_obj.tft.drawFastVLine(cx, cy + 6, 3, color);
      display_obj.tft.drawFastHLine(cx - 8, cy, 3, color);
      display_obj.tft.drawFastHLine(cx + 6, cy, 3, color);
      break;
    default:
      display_obj.tft.drawCircle(cx, cy, 5, color);
      display_obj.tft.fillCircle(cx, cy, 1, color);
      break;
  }
}

void drawMiniTileLabel(const String& label, int16_t x, int16_t y, int16_t width,
                       uint16_t foreground, uint16_t background, uint16_t text_offset) {
  const int16_t text_width = display_obj.tft.textWidth(label);
  const int16_t viewport_width = max((int16_t)1, (int16_t)(width - 4));
  const int16_t cursor_x = text_width > viewport_width
                               ? -static_cast<int16_t>(text_offset)
                               : max((int16_t)0, (int16_t)((viewport_width - text_width) / 2));
  display_obj.tft.setTextColor(foreground, background);
  display_obj.tft.setViewport(x + 2, y, viewport_width, 9);
  display_obj.tft.setCursor(cursor_x, 0);
  display_obj.tft.print(label);
  display_obj.tft.resetViewport();
}
#endif
}

void MenuFunctions::drawMiniMenuButton(int b, int x, bool selected, uint16_t text_offset) {
  if (!current_menu || !current_menu->list || x < 0 || x >= current_menu->list->size())
    return;

  MenuNode mini_node = current_menu->list->get(x);

  #ifdef MARAUDER_MINI_V3
  (void)selected;
  String label = mini_node.name;
  label.trim();
  const bool is_setting_node = mini_node.icon == SETTINGS && mini_node.color == TFTLIGHTGREY;
  const bool cursor_highlighted = current_menu->selected == x;
  const bool item_selected = mini_node.selected && !is_setting_node;
  const bool emphasized = cursor_highlighted || item_selected;
  const uint16_t background = item_selected ? MINI_UI_SELECTED
      : (cursor_highlighted ? MINI_UI_ACCENT : MINI_UI_SURFACE);
  const uint16_t border = cursor_highlighted ? MINI_UI_ACCENT
      : (item_selected ? MINI_UI_SELECTED : MINI_UI_BORDER);
  const uint16_t icon_color = emphasized ? MINI_UI_TEXT : MINI_UI_MUTED;

  display_obj.tft.setFreeFont(NULL);
  display_obj.tft.setTextSize(1);
  display_obj.tft.setTextWrap(false);

  if (current_menu == &mainMenu) {
    const int16_t card_x = 3 + ((x % 2) * 63);
    const int16_t card_y = 21 + ((x / 2) * 35);
    display_obj.tft.fillRoundRect(card_x, card_y, 59, 32, 5, background);
    display_obj.tft.drawRoundRect(card_x, card_y, 59, 32, 5, border);
    drawMiniSymbol(mini_node.icon, card_x + 29, card_y + 10,
                   (!emphasized && mini_node.icon == REBOOT) ? MINI_UI_DANGER : icon_color);
    drawMiniTileLabel(label, card_x, card_y + 22, 59, MINI_UI_TEXT, background, text_offset);
    return;
  }

  const bool wifi_category = current_menu == &wifiMenu;
  const bool bluetooth_category = current_menu == &bluetoothMenu;
  if ((wifi_category || bluetooth_category) && x == 0) {
    display_obj.tft.fillRoundRect(2, 9, 17, 10, 4, background);
    drawMiniBackGlyph(7, 10, emphasized ? MINI_UI_TEXT : MINI_UI_MUTED);
    return;
  }

  if (wifi_category) {
    const int16_t tile = x - 1;
    const int16_t card_x = 3 + ((tile % 2) * 63);
    const int16_t card_y = 22 + ((tile / 2) * 51);
    display_obj.tft.fillRoundRect(card_x, card_y, 59, 47, 6, background);
    display_obj.tft.drawRoundRect(card_x, card_y, 59, 47, 6, border);
    drawMiniSymbol(x == 4 ? SETTINGS : mini_node.icon, card_x + 29, card_y + 17, icon_color);
    drawMiniTileLabel(label, card_x, card_y + 34, 59, MINI_UI_TEXT, background, text_offset);
    return;
  }

  if (bluetooth_category) {
    const int16_t card_y = 23 + ((x - 1) * 33);
    display_obj.tft.fillRoundRect(4, card_y, 120, 29, 6, background);
    display_obj.tft.drawRoundRect(4, card_y, 120, 29, 6, border);
    drawMiniSymbol(mini_node.icon, 16, card_y + 14, icon_color);
    display_obj.tft.setTextColor(MINI_UI_TEXT, background);
    display_obj.tft.setViewport(28, card_y + 10, 80, 9);
    display_obj.tft.setCursor(-static_cast<int16_t>(text_offset), 0);
    display_obj.tft.print(label);
    display_obj.tft.resetViewport();
    drawMiniChevron(113, card_y + 11, icon_color);
    return;
  }

  const int16_t row_y = 22 + (b * 17);
  String lower_label = label;
  lower_label.toLowerCase();
  const bool danger = mini_node.icon == REBOOT || lower_label.indexOf("delete") >= 0 ||
                      lower_label.indexOf("erase") >= 0 || lower_label.indexOf("clear") >= 0;
  const uint16_t detail_color = emphasized ? MINI_UI_TEXT : (danger ? MINI_UI_DANGER : MINI_UI_MUTED);
  display_obj.tft.fillRoundRect(4, row_y, 120, 15, 4, background);
  display_obj.tft.drawRoundRect(4, row_y, 120, 15, 4, border);
  if (x == 0 && current_menu->parentMenu != NULL)
    drawMiniBackGlyph(11, row_y + 3, detail_color);
  else
    drawMiniSymbol(mini_node.icon, 13, row_y + 7, detail_color);

  const int16_t text_end = is_setting_node ? 95 : 112;
  display_obj.tft.setTextColor(MINI_UI_TEXT, background);
  display_obj.tft.setViewport(23, row_y, max((int16_t)1, (int16_t)(text_end - 23)), 15);
  display_obj.tft.setCursor(-static_cast<int16_t>(text_offset), 4);
  display_obj.tft.print(label);
  display_obj.tft.resetViewport();
  if (is_setting_node) {
    const uint16_t toggle_color = mini_node.selected ? MINI_UI_SELECTED : MINI_UI_BORDER;
    display_obj.tft.fillRoundRect(98, row_y + 3, 22, 9, 5, toggle_color);
    display_obj.tft.fillCircle(98 + (mini_node.selected ? 17 : 5), row_y + 7, 3, MINI_UI_TEXT);
  } else {
    drawMiniChevron(115, row_y + 4, detail_color);
  }
  return;
  #else
  bool is_setting_node = (mini_node.icon == SETTINGS && mini_node.color == TFTLIGHTGREY);
  uint16_t color = is_setting_node ? (mini_node.selected ? TFT_GREEN : TFT_RED) : this->getColor(mini_node.color);
  int16_t button_x = KEY_X - (KEY_W / 2);
  int16_t button_y = (KEY_Y + (b * (KEY_H + KEY_SPACING_Y))) - (KEY_H / 2);

  uint16_t background = selected ? (is_setting_node ? TFT_LIGHTGREY : color) : TFT_BLACK;
  uint16_t text_color = (selected && !is_setting_node) ? TFT_BLACK : color;

  display_obj.tft.setFreeFont(NULL);
  display_obj.tft.setTextSize(1);
  display_obj.tft.setTextWrap(false);
  display_obj.tft.fillRect(button_x, button_y - 4, KEY_W, KEY_H, background);
  display_obj.tft.setTextColor(text_color, background);
  display_obj.tft.setViewport(button_x + BUTTON_PADDING, button_y - 4,
                              max(1, KEY_W - (BUTTON_PADDING * 2)), KEY_H);
  display_obj.tft.setCursor(-static_cast<int16_t>(text_offset), (KEY_H / 2) - 4);
  display_obj.tft.print(mini_node.name);
  display_obj.tft.resetViewport();
  #endif
}

void MenuFunctions::resetMiniMenuMarquee(uint32_t current_time) {
  if (current_time == 0)
    current_time = millis();
  mini_marquee_menu = current_menu;
  mini_marquee_index = current_menu ? current_menu->selected : -1;
  mini_marquee_offset = 0;
  mini_marquee_direction = 1;
  mini_marquee_next_step = current_time + MINI_MARQUEE_PAUSE_MS;
}

void MenuFunctions::updateMiniMenuMarquee(uint32_t current_time) {
  if (!current_menu || !current_menu->list || current_menu->list->size() == 0)
    return;
  const int selected_index = current_menu->selected;
  if (selected_index < 0 || selected_index >= current_menu->list->size())
    return;
  if (mini_marquee_menu != current_menu || mini_marquee_index != selected_index) {
    resetMiniMenuMarquee(current_time);
    return;
  }
  const int visible_row = selected_index - menu_start_index;
  if (visible_row < 0 || visible_row >= BUTTON_SCREEN_LIMIT)
    return;

  display_obj.tft.setFreeFont(NULL);
  display_obj.tft.setTextSize(1);
  int16_t available_width = max(1, KEY_W - (BUTTON_PADDING * 2));
  #ifdef MARAUDER_MINI_V3
    const MenuNode selected_node = current_menu->list->get(selected_index);
    if (current_menu == &mainMenu)
      available_width = 55;
    else if (current_menu == &wifiMenu) {
      if (selected_index == 0) return;
      available_width = 55;
    }
    else if (current_menu == &bluetoothMenu) {
      if (selected_index == 0) return;
      available_width = 80;
    }
    else
      available_width = selected_node.icon == SETTINGS && selected_node.color == TFTLIGHTGREY ? 72 : 89;
  #endif
  const int16_t label_width = display_obj.tft.textWidth(current_menu->list->get(selected_index).name);
  if (label_width <= available_width || static_cast<int32_t>(current_time - mini_marquee_next_step) < 0)
    return;

  const uint16_t maximum_offset = label_width - available_width;
  if (mini_marquee_direction > 0) {
    if (mini_marquee_offset < maximum_offset) mini_marquee_offset++;
    if (mini_marquee_offset >= maximum_offset) {
      mini_marquee_offset = maximum_offset;
      mini_marquee_direction = -1;
      mini_marquee_next_step = current_time + MINI_MARQUEE_PAUSE_MS;
    } else {
      mini_marquee_next_step = current_time + MINI_MARQUEE_STEP_MS;
    }
  } else {
    if (mini_marquee_offset > 0) mini_marquee_offset--;
    if (mini_marquee_offset == 0) {
      mini_marquee_direction = 1;
      mini_marquee_next_step = current_time + MINI_MARQUEE_PAUSE_MS;
    } else {
      mini_marquee_next_step = current_time + MINI_MARQUEE_STEP_MS;
    }
  }
  drawMiniMenuButton(visible_row, selected_index, true, mini_marquee_offset);
}

#ifdef MARAUDER_MINI_V3
void MenuFunctions::selectMiniMenuIndex(int target_index) {
  if (!current_menu || !current_menu->list || current_menu->list->size() == 0)
    return;
  const int item_count = current_menu->list->size();
  target_index = constrain(target_index, 0, item_count - 1);
  const int previous_index = current_menu->selected;
  if (target_index == previous_index)
    return;
  current_menu->selected = target_index;
  resetMiniMenuMarquee();

  int new_start = menu_start_index;
  const bool fixed_layout = current_menu == &mainMenu ||
                            current_menu == &wifiMenu ||
                            current_menu == &bluetoothMenu;
  if (!fixed_layout && item_count > BUTTON_SCREEN_LIMIT) {
    // Keep the cursor around the middle of long lists. This makes the content
    // follow the selection instead of waiting until the cursor hits the final
    // visible row before paging.
    const int maximum_start = item_count - BUTTON_SCREEN_LIMIT;
    new_start = constrain(target_index - (BUTTON_SCREEN_LIMIT / 2),
                          0, maximum_start);
  }
  else if (target_index < menu_start_index ||
           target_index >= menu_start_index + BUTTON_SCREEN_LIMIT) {
    new_start = target_index < menu_start_index
                    ? target_index
                    : target_index + 1 - BUTTON_SCREEN_LIMIT;
  }

  if (new_start != menu_start_index) {
    buildButtons(current_menu, new_start);
    displayCurrentMenu(new_start);
    return;
  }
  buttonSelected(target_index - menu_start_index, target_index);
  if (previous_index >= menu_start_index && previous_index < menu_start_index + BUTTON_SCREEN_LIMIT)
    buttonNotSelected(previous_index - menu_start_index, previous_index);
}

void MenuFunctions::navigateMiniMenu(int8_t horizontal, int8_t vertical) {
  if (!current_menu || !current_menu->list || current_menu->list->size() == 0)
    return;
  const int selected_index = current_menu->selected;
  const int item_count = current_menu->list->size();
  int target_index = selected_index;
  if (current_menu == &mainMenu) {
    if (horizontal != 0) {
      const int row_start = (selected_index / 2) * 2;
      const int candidate = selected_index + horizontal;
      if (candidate >= row_start && candidate < row_start + 2 && candidate < item_count)
        target_index = candidate;
    } else if (vertical != 0) {
      const int column = selected_index % 2;
      const int row_count = (item_count + 1) / 2;
      const int target_row = (selected_index / 2 + vertical + row_count) % row_count;
      target_index = min(target_row * 2 + column, item_count - 1);
    }
  } else if (current_menu == &wifiMenu) {
    if (selected_index == 0) {
      if (vertical < 0) target_index = item_count - 1;
      else if (vertical > 0 || horizontal > 0) target_index = 1;
    } else {
      const int tile = selected_index - 1;
      const int column = tile % 2;
      const int row = tile / 2;
      if (horizontal < 0 && column == 1) target_index--;
      else if (horizontal > 0 && column == 0 && selected_index + 1 < item_count) target_index++;
      else if (vertical < 0) target_index = row == 0 ? 0 : selected_index - 2;
      else if (vertical > 0) target_index = selected_index + 2 < item_count ? selected_index + 2 : 0;
    }
  } else if (vertical != 0) {
    target_index = (selected_index + vertical + item_count) % item_count;
  } else if (horizontal < 0) {
    if (current_menu->parentMenu != NULL) current_menu->list->get(0).callable();
    return;
  } else if (horizontal > 0) {
    current_menu->list->get(selected_index).callable();
    return;
  }
  selectMiniMenuIndex(target_index);
}

void MenuFunctions::updateMiniMenuNavigationRepeat(uint32_t current_time) {
  const bool menu_active = wifi_scan_obj.currentScanMode == WIFI_SCAN_OFF ||
                           wifi_scan_obj.currentScanMode == WIFI_CONNECTED ||
                           wifi_scan_obj.currentScanMode == OTA_UPDATE;
  if (!menu_active || !current_menu || !current_menu->list || current_menu->list->size() == 0) {
    mini_menu_repeat_direction = 0;
    mini_menu_repeat_enabled = false;
    return;
  }
  uint8_t direction = 0;
  uint8_t pressed_count = 0;
  if (miniMenuButtonDown(u_btn)) { direction = 1; pressed_count++; }
  if (miniMenuButtonDown(d_btn)) { direction = 2; pressed_count++; }
  if (miniMenuButtonDown(l_btn)) { direction = 3; pressed_count++; }
  if (miniMenuButtonDown(r_btn)) { direction = 4; pressed_count++; }
  if (pressed_count != 1) {
    mini_menu_repeat_direction = 0;
    mini_menu_repeat_enabled = false;
    return;
  }
  if (direction != mini_menu_repeat_direction) {
    mini_menu_repeat_direction = direction;
    mini_menu_repeat_enabled = direction <= 2 || current_menu == &mainMenu || current_menu == &wifiMenu;
    mini_menu_repeat_next_step = current_time + MINI_MENU_REPEAT_DELAY_MS;
    return;
  }
  if (!mini_menu_repeat_enabled || static_cast<int32_t>(current_time - mini_menu_repeat_next_step) < 0)
    return;
  switch (direction) {
    case 1: navigateMiniMenu(0, -1); break;
    case 2: navigateMiniMenu(0, 1); break;
    case 3: navigateMiniMenu(-1, 0); break;
    case 4: navigateMiniMenu(1, 0); break;
    default: break;
  }
  mini_menu_repeat_next_step = current_time + MINI_MENU_REPEAT_INTERVAL_MS;
}
#endif
#endif

void MenuFunctions::buttonNotSelected(int b, int x) {
  if (x == -1)
    x = b;

  // Ensure b is within valid button index range
  b = (x - menu_start_index) % BUTTON_SCREEN_LIMIT;

  #ifdef HAS_MINI_SCREEN
    this->drawMiniMenuButton(b, x, false);
  #endif

  uint16_t color = (current_menu->list->get(x).icon == SETTINGS && current_menu->list->get(x).color == TFTLIGHTGREY) ? (current_menu->list->get(x).selected ? TFT_GREEN : TFT_RED) : this->getColor(current_menu->list->get(x).color);
  uint16_t icon_color = (current_menu->list->get(x).icon == SETTINGS && current_menu->list->get(x).color == TFTLIGHTGREY) ? TFT_LIGHTGREY : color;

  #ifdef HAS_FULL_SCREEN
    display_obj.tft.setFreeFont(MENU_FONT);
    display_obj.key[b].initButton(&display_obj.tft, KEY_X, KEY_Y + b * (KEY_H + KEY_SPACING_Y), KEY_W, KEY_H, TFT_BLACK, TFT_BLACK, color, (char*)"", KEY_TEXTSIZE);
    display_obj.key[b].drawButton(false, current_menu->list->get(x).name);
    if ((current_menu->list->get(x).name != text09) && (current_menu->list->get(x).icon != 255))
          display_obj.tft.drawXBitmap(0,
                                      KEY_Y + (b * (KEY_H + KEY_SPACING_Y)) - (ICON_H / 2),
                                      menu_icons[current_menu->list->get(x).icon],
                                      ICON_W,
                                      ICON_H,
                                      TFT_BLACK,
                                      icon_color);
    display_obj.tft.setFreeFont(NULL);
  #endif
}

void MenuFunctions::buttonSelected(int b, int x) {
  if (x == -1)
    x = b;

  // Ensure b is within valid button index range
  b = (x - menu_start_index) % BUTTON_SCREEN_LIMIT;

  uint16_t color = this->getColor(current_menu->list->get(x).color);

  #ifdef HAS_MINI_SCREEN
    this->drawMiniMenuButton(b, x, true);
  #endif

  #ifdef HAS_FULL_SCREEN
    display_obj.tft.setFreeFont(MENU_FONT);
    if (current_menu->list->get(x).icon == SETTINGS && current_menu->list->get(x).color == TFTLIGHTGREY) {
      uint16_t setting_color = current_menu->list->get(x).selected ? TFT_GREEN : TFT_RED;
      display_obj.key[b].initButton(&display_obj.tft, KEY_X, KEY_Y + b * (KEY_H + KEY_SPACING_Y), KEY_W, KEY_H, TFT_BLACK, TFT_LIGHTGREY, setting_color, (char*)"", KEY_TEXTSIZE);
      display_obj.key[b].drawButton(false, current_menu->list->get(x).name);
      display_obj.tft.drawXBitmap(0,
                                      KEY_Y + (b * (KEY_H + KEY_SPACING_Y)) - (ICON_H / 2),
                                      menu_icons[current_menu->list->get(x).icon],
                                      ICON_W,
                                      ICON_H,
                                      TFT_BLACK,
                                      TFT_LIGHTGREY);
    } else {
      display_obj.key[b].drawButton(true, current_menu->list->get(x).name);
      if ((current_menu->list->get(x).name != text09) && (current_menu->list->get(x).icon != 255))
            display_obj.tft.drawXBitmap(0,
                                        KEY_Y + (b * (KEY_H + KEY_SPACING_Y)) - (ICON_H / 2),
                                        menu_icons[current_menu->list->get(x).icon],
                                        ICON_W,
                                        ICON_H,
                                        TFT_BLACK,
                                        color);
    }
    display_obj.tft.setFreeFont(NULL);
  #endif
}

void MenuFunctions::displayMenuButtons() {
  #ifdef HAS_ILI9341
    // Draw lines to show each menu button
    for (int i = 0; i < 3; i++) {

      // Draw horizontal line on left
      display_obj.tft.drawLine(0, 
                              TFT_HEIGHT / 3 * (i),
                              (TFT_WIDTH / 12) / 2,
                              TFT_HEIGHT / 3 * (i),
                              TFT_FARTGRAY);

      // Draw horizontal line on right
      display_obj.tft.drawLine(TFT_WIDTH - 1 - ((TFT_WIDTH / 12) / 2), 
                              TFT_HEIGHT / 3 * (i),
                              TFT_WIDTH,
                              TFT_HEIGHT / 3 * (i),
                              TFT_FARTGRAY);

      // Draw vertical line on left
      display_obj.tft.drawLine(0, 
                              (TFT_HEIGHT / 3 * (i)) - ((TFT_WIDTH / 12) / 2),
                              0,
                              (TFT_HEIGHT / 3 * (i)) + ((TFT_WIDTH / 12) / 2),
                              TFT_FARTGRAY);

      // Draw vertical line on right
      display_obj.tft.drawLine(TFT_WIDTH - 1, 
                              (TFT_HEIGHT / 3 * (i)) - ((TFT_WIDTH / 12) / 2),
                              TFT_WIDTH - 1,
                              (TFT_HEIGHT / 3 * (i)) + ((TFT_WIDTH / 12) / 2),
                              TFT_FARTGRAY);
    }
  #endif
}

// Function to check menu input
void MenuFunctions::main(uint32_t currentTime)
{
  #if defined(MARAUDER_CARDPUTER) || defined(MARAUDER_CARDPUTER_ADV)
    this->updateKeyboard();
  #endif

  #ifdef HAS_MINI_SCREEN
    if ((wifi_scan_obj.currentScanMode == WIFI_SCAN_OFF) ||
        (wifi_scan_obj.currentScanMode == WIFI_CONNECTED) ||
        (wifi_scan_obj.currentScanMode == OTA_UPDATE))
      this->updateMiniMenuMarquee(currentTime);
  #endif

  // Some function exited and we need to go back to normal
  if (display_obj.exit_draw) {
    if (wifi_scan_obj.currentScanMode != WIFI_CONNECTED)
      wifi_scan_obj.currentScanMode = WIFI_SCAN_OFF;
    display_obj.exit_draw = false;
    this->orientDisplay();
  }
  if ((wifi_scan_obj.currentScanMode == WIFI_SCAN_OFF) ||
      (wifi_scan_obj.currentScanMode == WIFI_CONNECTED) ||
      (wifi_scan_obj.currentScanMode == OTA_UPDATE) ||
      (wifi_scan_obj.currentScanMode == ESP_UPDATE) ||
      (wifi_scan_obj.currentScanMode == SHOW_INFO) ||
      (wifi_scan_obj.currentScanMode == WIFI_SCAN_GPS_DATA) ||
      (wifi_scan_obj.currentScanMode == GPS_POI) ||
      (wifi_scan_obj.currentScanMode == GPS_TRACKER) ||
      (wifi_scan_obj.currentScanMode == WIFI_SCAN_GPS_NMEA)) {
    if (wifi_scan_obj.orient_display) {
      this->orientDisplay();
      wifi_scan_obj.orient_display = false;
    }
  }

  if (currentTime != 0) {
    if (currentTime - initTime >= BANNER_TIME) {
      this->initTime = millis();
      if ((wifi_scan_obj.currentScanMode != LV_JOIN_WIFI) &&
          (wifi_scan_obj.currentScanMode != LV_ADD_SSID) &&
          (wifi_scan_obj.currentScanMode != WIFI_SCAN_SSID_FINDER))
        this->updateStatusBar();
      
      // Do channel analyzer stuff
      if ((wifi_scan_obj.currentScanMode == WIFI_SCAN_CHAN_ANALYZER) ||
          (wifi_scan_obj.currentScanMode == BT_SCAN_ANALYZER)){
        #ifdef HAS_SCREEN
          this->setGraphScale(this->graphScaleCheck(wifi_scan_obj._analyzer_values));

          this->drawGraph(wifi_scan_obj._analyzer_values);
        #endif
      }

      if (wifi_scan_obj.currentScanMode == WIFI_SCAN_CHAN_ACT) {
        #ifdef HAS_SCREEN
          this->setGraphScale(this->graphScaleCheckSmall(wifi_scan_obj.channel_activity));

          this->drawGraphSmall(wifi_scan_obj.channel_activity);

        #endif
      }
    }
  }


  boolean pressed = false;
  // This is code from bodmer's keypad example
  uint16_t t_x = 0, t_y = 0; // To store the touch coordinates

  // Get the display buffer out of the way
  if ((wifi_scan_obj.currentScanMode != WIFI_SCAN_OFF ) &&
      (wifi_scan_obj.currentScanMode != WIFI_CONNECTED) &&
      (wifi_scan_obj.currentScanMode != WIFI_SCAN_SSID_FINDER) &&
      (wifi_scan_obj.currentScanMode != WIFI_ATTACK_BEACON_SPAM) &&
      (wifi_scan_obj.currentScanMode != WIFI_ATTACK_AP_SPAM) &&
      (wifi_scan_obj.currentScanMode != WIFI_ATTACK_SSID_GROUP_CLONE) &&
      (wifi_scan_obj.currentScanMode != WIFI_ATTACK_CSA) &&
      (wifi_scan_obj.currentScanMode != WIFI_ATTACK_QUIET) &&
      (wifi_scan_obj.currentScanMode != WIFI_ATTACK_AUTH) &&
      (wifi_scan_obj.currentScanMode != WIFI_ATTACK_DEAUTH) &&
      (wifi_scan_obj.currentScanMode != WIFI_ATTACK_DEAUTH_MANUAL) &&
      (wifi_scan_obj.currentScanMode != WIFI_ATTACK_CAMERA_DEAUTH) &&
      (wifi_scan_obj.currentScanMode != WIFI_ATTACK_DEAUTH_TARGETED) &&
      (wifi_scan_obj.currentScanMode != WIFI_ATTACK_BAD_MSG_TARGETED) &&
      (wifi_scan_obj.currentScanMode != WIFI_ATTACK_BAD_MSG) &&
      (wifi_scan_obj.currentScanMode != WIFI_ATTACK_SLEEP) &&
      (wifi_scan_obj.currentScanMode != WIFI_ATTACK_SLEEP_TARGETED) &&
      (wifi_scan_obj.currentScanMode != WIFI_ATTACK_MIMIC) &&
	  (wifi_scan_obj.currentScanMode != WIFI_ATTACK_FUNNY_BEACON) &&
      (wifi_scan_obj.currentScanMode != WIFI_ATTACK_RICK_ROLL))
    display_obj.displayBuffer();


  int pre_getTouch = millis();

  #ifdef HAS_ILI9341
    if (!this->disable_touch)
      pressed = display_obj.updateTouch(&t_x, &t_y);
  #endif


  // Brightness gesture: hold top or bottom zone 1.5s to enter brightness mode
  #ifdef HAS_ILI9341
    if (pressed && (wifi_scan_obj.currentScanMode == WIFI_SCAN_OFF ||
                    wifi_scan_obj.currentScanMode == WIFI_CONNECTED)) {
      uint16_t zoneUp = TFT_HEIGHT * 25 / 100;
      uint16_t zoneDown = TFT_HEIGHT * 75 / 100;
      if (t_y < zoneUp || t_y >= zoneDown) {
        uint32_t hold_start = millis();
        uint16_t hx, hy;
        bool held = false;
        while (display_obj.updateTouch(&hx, &hy)) {
          if (millis() - hold_start >= 1500) {
            held = true;
            break;
          }
          delay(10);
        }
        if (held) {
          // Wait for release before entering brightness mode
          while (display_obj.updateTouch(&hx, &hy)) delay(10);
          this->brightnessMode();
          return;
        }
      }
    }
  #endif

  // POI button interception during wardrive — full width bottom bar
  #ifdef HAS_ILI9341
    if (pressed &&
        (wifi_scan_obj.currentScanMode == WIFI_SCAN_WAR_DRIVE ||
         wifi_scan_obj.currentScanMode == WIFI_SCAN_STATION_WAR_DRIVE)) {
      if (t_y >= (SCREEN_HEIGHT - 50)) {
        wifi_scan_obj.tagPOI(nullptr);
        // Brief green flash
        display_obj.tft.fillRect(0, SCREEN_HEIGHT - 50, SCREEN_WIDTH, 50, TFT_GREEN);
        display_obj.tft.setTextSize(2);
        #ifdef HAS_GPS
        if (gps_obj.getFixStatus())
          display_obj.tft.setTextColor(TFT_BLACK, TFT_GREEN);
        else
        #endif
          display_obj.tft.setTextColor(TFT_BLACK, TFT_RED);
        String poiFlash = "POI (" + String(wifi_scan_obj.poiCount) + ")";
        int16_t flashWidth = poiFlash.length() * 12;
        display_obj.tft.setCursor((SCREEN_WIDTH - flashWidth) / 2, SCREEN_HEIGHT - 33);
        display_obj.tft.print(poiFlash);
        delay(200);
        x = -1;
        y = -1;
        return;
      }
    }
  #endif

  // This is if there are scans/attacks going on
  #ifdef HAS_ILI9341
    if ((wifi_scan_obj.currentScanMode != WIFI_SCAN_OFF) &&
        (pressed) &&
        (wifi_scan_obj.currentScanMode != WIFI_CONNECTED) &&
        (wifi_scan_obj.currentScanMode != OTA_UPDATE) &&
        (wifi_scan_obj.currentScanMode != ESP_UPDATE) &&
        (wifi_scan_obj.currentScanMode != SHOW_INFO) &&
        (wifi_scan_obj.currentScanMode != WIFI_SCAN_GPS_DATA) &&
        (wifi_scan_obj.currentScanMode != GPS_POI) &&
        (wifi_scan_obj.currentScanMode != GPS_TRACKER) &&
        (wifi_scan_obj.currentScanMode != WIFI_SCAN_GPS_NMEA))
    {
      // Stop the current scan
      if ((wifi_scan_obj.currentScanMode == WIFI_SCAN_SAE_COMMIT) ||
          (wifi_scan_obj.currentScanMode == WIFI_SCAN_DETECT_FOLLOW) ||
          (wifi_scan_obj.currentScanMode == WIFI_SCAN_STATION_WAR_DRIVE) ||
          (wifi_scan_obj.currentScanMode == WIFI_SCAN_STATION) ||
          (wifi_scan_obj.currentScanMode == WIFI_SCAN_WAR_DRIVE) ||
          (wifi_scan_obj.currentScanMode == WIFI_SCAN_DISPLAY_AP_INFO) ||
          (wifi_scan_obj.currentScanMode == WIFI_SCAN_EVIL_PORTAL) ||
          (wifi_scan_obj.currentScanMode == WIFI_SCAN_AP_STA) ||
          (wifi_scan_obj.currentScanMode == WIFI_PING_SCAN) ||
          (wifi_scan_obj.currentScanMode == WIFI_ARP_SCAN) ||
          (wifi_scan_obj.currentScanMode == WIFI_PORT_SCAN_ALL) ||
          (wifi_scan_obj.currentScanMode == WIFI_SCAN_SSH) ||
          (wifi_scan_obj.currentScanMode == WIFI_SCAN_TELNET) ||
          (wifi_scan_obj.currentScanMode == WIFI_SCAN_DNS) ||
          (wifi_scan_obj.currentScanMode == WIFI_SCAN_SMTP) ||
          (wifi_scan_obj.currentScanMode == WIFI_SCAN_HTTP) ||
          (wifi_scan_obj.currentScanMode == WIFI_SCAN_HTTPS) ||
          (wifi_scan_obj.currentScanMode == WIFI_SCAN_RDP) ||
          (wifi_scan_obj.currentScanMode == WIFI_SCAN_PWN) ||
          (wifi_scan_obj.currentScanMode == WIFI_SCAN_PINESCAN) ||
          (wifi_scan_obj.currentScanMode == WIFI_SCAN_MULTISSID) ||
          (wifi_scan_obj.currentScanMode == WIFI_SCAN_ESPRESSIF) ||
          (wifi_scan_obj.currentScanMode == WIFI_SCAN_ALL) ||
          (wifi_scan_obj.currentScanMode == WIFI_ATTACK_BEACON_SPAM) ||
          (wifi_scan_obj.currentScanMode == WIFI_ATTACK_AP_SPAM) ||
          (wifi_scan_obj.currentScanMode == WIFI_ATTACK_CSA) ||
          (wifi_scan_obj.currentScanMode == WIFI_ATTACK_QUIET) ||
          (wifi_scan_obj.currentScanMode == WIFI_ATTACK_AUTH) ||
          (wifi_scan_obj.currentScanMode == WIFI_ATTACK_DEAUTH) ||
          (wifi_scan_obj.currentScanMode == WIFI_ATTACK_DEAUTH_MANUAL) ||
          (wifi_scan_obj.currentScanMode == WIFI_ATTACK_CAMERA_DEAUTH) ||
          (wifi_scan_obj.currentScanMode == WIFI_ATTACK_DEAUTH_TARGETED) ||
          (wifi_scan_obj.currentScanMode == WIFI_ATTACK_BAD_MSG_TARGETED) ||
          (wifi_scan_obj.currentScanMode == WIFI_ATTACK_BAD_MSG) ||
          (wifi_scan_obj.currentScanMode == WIFI_ATTACK_SLEEP) ||
          (wifi_scan_obj.currentScanMode == WIFI_ATTACK_SLEEP_TARGETED) ||
          (wifi_scan_obj.currentScanMode == WIFI_ATTACK_SAE_COMMIT) ||
          (wifi_scan_obj.currentScanMode == WIFI_ATTACK_MIMIC) ||
		      (wifi_scan_obj.currentScanMode == WIFI_ATTACK_FUNNY_BEACON) ||
          (wifi_scan_obj.currentScanMode == WIFI_ATTACK_RICK_ROLL) ||
          (wifi_scan_obj.currentScanMode == WIFI_ATTACK_BEACON_LIST) ||
          (wifi_scan_obj.currentScanMode == BT_SCAN_ALL) ||
          (wifi_scan_obj.currentScanMode == BT_SCAN_ADVERTISEMENT_CAPTURE) ||
          (wifi_scan_obj.currentScanMode == BT_SCAN_FOX_HUNT) ||
          (wifi_scan_obj.currentScanMode == WIFI_SCAN_SIG_STREN) ||
          (wifi_scan_obj.currentScanMode == BT_ATTACK_FINDMY_LIVE) ||
          (wifi_scan_obj.currentScanMode == BT_SCAN_RAYBAN) ||
          (wifi_scan_obj.currentScanMode == BT_SCAN_AIRTAG) ||
          (wifi_scan_obj.currentScanMode == BT_SCAN_AIRTAG_MON) ||
          (wifi_scan_obj.currentScanMode == BT_SCAN_FLIPPER) ||
          (wifi_scan_obj.currentScanMode == BT_SCAN_SIMPLE) ||
          (wifi_scan_obj.currentScanMode == BT_SCAN_SIMPLE_TWO) ||
          (wifi_scan_obj.currentScanMode == BT_ATTACK_SOUR_APPLE) ||
          (wifi_scan_obj.currentScanMode == BT_ATTACK_APPLE_JUICE) ||
          (wifi_scan_obj.currentScanMode == BT_ATTACK_SWIFTPAIR_SPAM) ||
          (wifi_scan_obj.currentScanMode == BT_ATTACK_SPAM_ALL) ||
          (wifi_scan_obj.currentScanMode == BT_ATTACK_SAMSUNG_SPAM) ||
          (wifi_scan_obj.currentScanMode == BT_ATTACK_GOOGLE_SPAM) ||
          (wifi_scan_obj.currentScanMode == BT_ATTACK_FLIPPER_SPAM) ||
          (wifi_scan_obj.currentScanMode == BT_SPOOF_AIRTAG) ||
          (wifi_scan_obj.currentScanMode == BT_SCAN_WAR_DRIVE) ||
          (wifi_scan_obj.currentScanMode == BT_SCAN_WAR_DRIVE_CONT) ||
          (wifi_scan_obj.currentScanMode == BT_SCAN_SKIMMERS) ||
          (wifi_scan_obj.currentScanMode == BT_SCAN_ANALYZER))
      {
        wifi_scan_obj.StartScan(WIFI_SCAN_OFF);
  
        // If we don't do this, the text and button coordinates will be off
        display_obj.init();
  
        // Take us back to the menu
        changeMenu(current_menu, true);
      }
  
      x = -1;
      y = -1;
  
      return;
    }
  #endif

  #ifdef HAS_BUTTONS

    #if (C_BTN >= 0) && !defined(MARAUDER_CARDPUTER) && !defined(MARAUDER_CARDPUTER_ADV)
      bool c_btn_press = c_btn.justPressed();
    #elif defined(MARAUDER_CARDPUTER) || defined(MARAUDER_CARDPUTER_ADV)
      bool c_btn_press = this->isKeyPressed('(');
    #endif

    #ifdef MARAUDER_MINI_V3
      if (wifi_scan_obj.currentScanMode == WIFI_SCAN_SSID_FINDER) {
        u_btn.justPressed();
        d_btn.justPressed();
        const bool left_pressed = l_btn.justPressed();
        const bool right_pressed = r_btn.justPressed();

        if (left_pressed) {
          wifi_scan_obj.StartScan(WIFI_SCAN_OFF);
          display_obj.init();
          this->changeMenu(current_menu, true);
        }
        else if (c_btn_press)
          wifi_scan_obj.toggleSSIDFinderLock();
        else if (right_pressed)
          wifi_scan_obj.markSSIDFinderFound();
        return;
      }
    #endif

    #ifndef HAS_ILI9341
    
      if ((c_btn_press) &&
          (wifi_scan_obj.currentScanMode != WIFI_SCAN_OFF) &&
          (wifi_scan_obj.currentScanMode != WIFI_CONNECTED) &&
          (wifi_scan_obj.currentScanMode != OTA_UPDATE) &&
          (wifi_scan_obj.currentScanMode != ESP_UPDATE) &&
          (wifi_scan_obj.currentScanMode != SHOW_INFO) &&
          (wifi_scan_obj.currentScanMode != WIFI_SCAN_GPS_DATA) &&
          (wifi_scan_obj.currentScanMode != GPS_POI) &&
          (wifi_scan_obj.currentScanMode != GPS_TRACKER) &&
          (wifi_scan_obj.currentScanMode != WIFI_SCAN_GPS_NMEA))
      {
        // Stop the current scan
        if ((wifi_scan_obj.currentScanMode == WIFI_SCAN_PROBE) ||
            (wifi_scan_obj.currentScanMode == WIFI_SCAN_SAE_COMMIT) ||
            (wifi_scan_obj.currentScanMode == WIFI_SCAN_DETECT_FOLLOW) ||
            (wifi_scan_obj.currentScanMode == WIFI_SCAN_STATION_WAR_DRIVE) ||
            (wifi_scan_obj.currentScanMode == WIFI_SCAN_RAW_CAPTURE) ||
            (wifi_scan_obj.currentScanMode == WIFI_SCAN_STATION) ||
            (wifi_scan_obj.currentScanMode == WIFI_SCAN_AP) ||
            (wifi_scan_obj.currentScanMode == WIFI_SCAN_WAR_DRIVE) ||
            (wifi_scan_obj.currentScanMode == WIFI_SCAN_DISPLAY_AP_INFO) ||
            (wifi_scan_obj.currentScanMode == WIFI_SCAN_EVIL_PORTAL) ||
            (wifi_scan_obj.currentScanMode == WIFI_SCAN_SIG_STREN) ||
            (wifi_scan_obj.currentScanMode == BT_ATTACK_FINDMY_LIVE) ||
            (wifi_scan_obj.currentScanMode == WIFI_SCAN_AP_STA) ||
            (wifi_scan_obj.currentScanMode == WIFI_PING_SCAN) ||
            (wifi_scan_obj.currentScanMode == WIFI_ARP_SCAN) ||
            (wifi_scan_obj.currentScanMode == WIFI_PORT_SCAN_ALL) ||
            (wifi_scan_obj.currentScanMode == WIFI_SCAN_SSH) ||
            (wifi_scan_obj.currentScanMode == WIFI_SCAN_TELNET) ||
            (wifi_scan_obj.currentScanMode == WIFI_SCAN_DNS) ||
            (wifi_scan_obj.currentScanMode == WIFI_SCAN_SMTP) ||
            (wifi_scan_obj.currentScanMode == WIFI_SCAN_HTTP) ||
            (wifi_scan_obj.currentScanMode == WIFI_SCAN_HTTPS) ||
            (wifi_scan_obj.currentScanMode == WIFI_SCAN_RDP) ||
            (wifi_scan_obj.currentScanMode == WIFI_SCAN_PWN) ||
            (wifi_scan_obj.currentScanMode == WIFI_SCAN_PINESCAN) ||
            (wifi_scan_obj.currentScanMode == WIFI_SCAN_MULTISSID) ||
            (wifi_scan_obj.currentScanMode == WIFI_SCAN_ESPRESSIF) ||
            (wifi_scan_obj.currentScanMode == WIFI_SCAN_ALL) ||
            (wifi_scan_obj.currentScanMode == WIFI_SCAN_DEAUTH) ||
            (wifi_scan_obj.currentScanMode == WIFI_ATTACK_BEACON_SPAM) ||
            (wifi_scan_obj.currentScanMode == WIFI_ATTACK_AP_SPAM) ||
            (wifi_scan_obj.currentScanMode == WIFI_ATTACK_CSA) ||
            (wifi_scan_obj.currentScanMode == WIFI_ATTACK_QUIET) ||
            (wifi_scan_obj.currentScanMode == WIFI_ATTACK_AUTH) ||
            (wifi_scan_obj.currentScanMode == WIFI_ATTACK_DEAUTH) ||
            (wifi_scan_obj.currentScanMode == WIFI_ATTACK_DEAUTH_MANUAL) ||
            (wifi_scan_obj.currentScanMode == WIFI_ATTACK_CAMERA_DEAUTH) ||
            (wifi_scan_obj.currentScanMode == WIFI_ATTACK_DEAUTH_TARGETED) ||
            (wifi_scan_obj.currentScanMode == WIFI_ATTACK_BAD_MSG_TARGETED) ||
            (wifi_scan_obj.currentScanMode == WIFI_ATTACK_BAD_MSG) ||
            (wifi_scan_obj.currentScanMode == WIFI_ATTACK_SLEEP) ||
            (wifi_scan_obj.currentScanMode == WIFI_ATTACK_SLEEP_TARGETED) ||
            (wifi_scan_obj.currentScanMode == WIFI_ATTACK_SAE_COMMIT) ||
            (wifi_scan_obj.currentScanMode == WIFI_ATTACK_MIMIC) ||
			      (wifi_scan_obj.currentScanMode == WIFI_ATTACK_FUNNY_BEACON) ||
            (wifi_scan_obj.currentScanMode == WIFI_ATTACK_RICK_ROLL) ||
            (wifi_scan_obj.currentScanMode == WIFI_ATTACK_BEACON_LIST) ||
            (wifi_scan_obj.currentScanMode == BT_SCAN_ALL) ||
            (wifi_scan_obj.currentScanMode == BT_SCAN_ADVERTISEMENT_CAPTURE) ||
            (wifi_scan_obj.currentScanMode == BT_SCAN_FOX_HUNT) ||
            (wifi_scan_obj.currentScanMode == BT_SCAN_RAYBAN) ||
            (wifi_scan_obj.currentScanMode == BT_SCAN_AIRTAG) ||
            (wifi_scan_obj.currentScanMode == BT_SCAN_AIRTAG_MON) ||
            (wifi_scan_obj.currentScanMode == BT_SCAN_FLIPPER) ||
            (wifi_scan_obj.currentScanMode == BT_SCAN_FLOCK) ||
            (wifi_scan_obj.currentScanMode == BT_SCAN_SIMPLE) ||
            (wifi_scan_obj.currentScanMode == BT_SCAN_SIMPLE_TWO) ||
            (wifi_scan_obj.currentScanMode == BT_ATTACK_SOUR_APPLE) ||
            (wifi_scan_obj.currentScanMode == BT_ATTACK_APPLE_JUICE) ||
            (wifi_scan_obj.currentScanMode == BT_ATTACK_SWIFTPAIR_SPAM) ||
            (wifi_scan_obj.currentScanMode == BT_ATTACK_SPAM_ALL) ||
            (wifi_scan_obj.currentScanMode == BT_ATTACK_SAMSUNG_SPAM) ||
            (wifi_scan_obj.currentScanMode == BT_ATTACK_GOOGLE_SPAM) ||
            (wifi_scan_obj.currentScanMode == BT_ATTACK_FLIPPER_SPAM) ||
            (wifi_scan_obj.currentScanMode == BT_SPOOF_AIRTAG) ||
            (wifi_scan_obj.currentScanMode == BT_SCAN_WAR_DRIVE) ||
            (wifi_scan_obj.currentScanMode == BT_SCAN_WAR_DRIVE_CONT) ||
            (wifi_scan_obj.currentScanMode == BT_SCAN_SKIMMERS) ||
            (wifi_scan_obj.currentScanMode == WIFI_SCAN_EAPOL) ||
            (wifi_scan_obj.currentScanMode == WIFI_SCAN_ACTIVE_EAPOL) ||
            (wifi_scan_obj.currentScanMode == WIFI_SCAN_ACTIVE_LIST_EAPOL) ||
            (wifi_scan_obj.currentScanMode == WIFI_PACKET_MONITOR) ||
            (wifi_scan_obj.currentScanMode == WIFI_SCAN_CHAN_ANALYZER) ||
            (wifi_scan_obj.currentScanMode == WIFI_SCAN_CHAN_ACT) ||
            (wifi_scan_obj.currentScanMode == WIFI_SCAN_PACKET_RATE) ||
            (wifi_scan_obj.currentScanMode == BT_SCAN_ANALYZER))
        {
          wifi_scan_obj.StartScan(WIFI_SCAN_OFF);

          // Restore display state without full reinit to avoid screen flash
          #if defined(MARAUDER_CARDPUTER) || defined(MARAUDER_CARDPUTER_ADV)
            display_obj.tft.setRotation(SCREEN_ORIENTATION);
            display_obj.clearScreen();
          #else
            display_obj.init();
          #endif

          // Take us back to the menu
          changeMenu(current_menu, true);
        }
    
        x = -1;
        y = -1;
    
        return;
      }
    #endif

  #endif


  // Check if any key coordinate boxes contain the touch coordinates
  // This is for when on a menu
  // Make sure to add certain scanning functions here or else
  // menu items will be selected while scans and attacks are running
  #ifdef HAS_ILI9341
    if ((wifi_scan_obj.currentScanMode != WIFI_ATTACK_BEACON_SPAM) &&
        (wifi_scan_obj.currentScanMode != WIFI_ATTACK_AP_SPAM) &&
        (wifi_scan_obj.currentScanMode != WIFI_ATTACK_CSA) &&
        (wifi_scan_obj.currentScanMode != WIFI_ATTACK_QUIET) &&
        (wifi_scan_obj.currentScanMode != WIFI_ATTACK_AUTH) &&
        (wifi_scan_obj.currentScanMode != WIFI_ATTACK_DEAUTH) &&
        (wifi_scan_obj.currentScanMode != WIFI_ATTACK_DEAUTH_MANUAL) &&
        (wifi_scan_obj.currentScanMode != WIFI_ATTACK_CAMERA_DEAUTH) &&
        (wifi_scan_obj.currentScanMode != WIFI_ATTACK_DEAUTH_TARGETED) &&
        (wifi_scan_obj.currentScanMode != WIFI_ATTACK_BAD_MSG_TARGETED) &&
        (wifi_scan_obj.currentScanMode != WIFI_ATTACK_BAD_MSG) &&
        (wifi_scan_obj.currentScanMode != WIFI_ATTACK_SLEEP) &&
        (wifi_scan_obj.currentScanMode != WIFI_ATTACK_SLEEP_TARGETED) &&
        (wifi_scan_obj.currentScanMode != WIFI_ATTACK_SAE_COMMIT) &&
        (wifi_scan_obj.currentScanMode != WIFI_ATTACK_MIMIC) &&
        (wifi_scan_obj.currentScanMode != WIFI_SCAN_PACKET_RATE) &&
        (wifi_scan_obj.currentScanMode != WIFI_SCAN_RAW_CAPTURE) &&
        (wifi_scan_obj.currentScanMode != WIFI_SCAN_CHAN_ANALYZER) &&
        (wifi_scan_obj.currentScanMode != WIFI_SCAN_CHAN_ACT) &&
        (wifi_scan_obj.currentScanMode != WIFI_SCAN_SIG_STREN) &&
        (wifi_scan_obj.currentScanMode != WIFI_SCAN_AP) &&
        (wifi_scan_obj.currentScanMode != BT_SCAN_FLOCK) &&
        (wifi_scan_obj.currentScanMode != WIFI_SCAN_PROBE) &&
        (wifi_scan_obj.currentScanMode != WIFI_SCAN_DEAUTH) &&
		    (wifi_scan_obj.currentScanMode != WIFI_ATTACK_FUNNY_BEACON) &&
        (wifi_scan_obj.currentScanMode != WIFI_SCAN_EAPOL) &&
        (wifi_scan_obj.currentScanMode != WIFI_ATTACK_RICK_ROLL))
    {
      // Need this to set all keys to false
      /*for (uint8_t b = 0; b < BUTTON_ARRAY_LEN; b++) {
        if (pressed && display_obj.key[b].contains(t_x, t_y)) {
          display_obj.key[b].press(true);  // tell the button it is pressed
        } else {
          display_obj.key[b].press(false);  // tell the button it is NOT pressed
        }
      }*/

      // Detect up, down, select
      uint8_t menu_button = display_obj.menuButton(&t_x, &t_y, pressed);

      if (menu_button > -1) {
        if (menu_button == UP_BUTTON) {
          if ((wifi_scan_obj.currentScanMode == WIFI_SCAN_OFF) ||
              (wifi_scan_obj.currentScanMode == WIFI_CONNECTED) ||
              (wifi_scan_obj.currentScanMode == OTA_UPDATE)) {
            if (current_menu->selected > 0) {
              current_menu->selected--;
              // Page up
              if (current_menu->selected < this->menu_start_index) {
                this->buildButtons(current_menu, current_menu->selected);
                this->displayCurrentMenu(current_menu->selected);
              }
              this->buttonSelected(current_menu->selected - this->menu_start_index, current_menu->selected);
              if (!current_menu->list->get(current_menu->selected + 1).selected || (current_menu->list->get(current_menu->selected + 1).icon == SETTINGS && current_menu->list->get(current_menu->selected + 1).color == TFTLIGHTGREY))
                this->buttonNotSelected(current_menu->selected + 1 - this->menu_start_index, current_menu->selected + 1);
            }
            // Loop to end
            else {
              current_menu->selected = current_menu->list->size() - 1;
              if (current_menu->selected >= BUTTON_SCREEN_LIMIT) {
                this->buildButtons(current_menu, current_menu->selected + 1 - BUTTON_SCREEN_LIMIT);
                this->displayCurrentMenu(current_menu->selected + 1 - BUTTON_SCREEN_LIMIT);
              }
              this->buttonSelected(current_menu->selected, current_menu->selected);
              if (!current_menu->list->get(0).selected || (current_menu->list->get(0).icon == SETTINGS && current_menu->list->get(0).color == TFTLIGHTGREY))
                this->buttonNotSelected(0, this->menu_start_index);
            }
          }
          else if ((wifi_scan_obj.currentScanMode == WIFI_PACKET_MONITOR) ||
                  (wifi_scan_obj.currentScanMode == WIFI_SCAN_EAPOL) ||
                  (wifi_scan_obj.currentScanMode == WIFI_SCAN_CHAN_ANALYZER) ||
                  (wifi_scan_obj.currentScanMode == WIFI_SCAN_PACKET_RATE) ||
                  (wifi_scan_obj.currentScanMode == WIFI_SCAN_RAW_CAPTURE) ||
                  (wifi_scan_obj.currentScanMode == WIFI_SCAN_AP) ||
                  (wifi_scan_obj.currentScanMode == BT_SCAN_FLOCK) ||
                  (wifi_scan_obj.currentScanMode == WIFI_SCAN_PROBE) ||
                  (wifi_scan_obj.currentScanMode == WIFI_SCAN_DEAUTH) ||
                  (wifi_scan_obj.currentScanMode == WIFI_SCAN_SIG_STREN)) {
            #ifndef HAS_DUAL_BAND
              if (wifi_scan_obj.set_channel < 14)
                wifi_scan_obj.changeChannel(wifi_scan_obj.set_channel + 1);
              else
                wifi_scan_obj.changeChannel(1);
            #else
              if (wifi_scan_obj.dual_band_channel_index < DUAL_BAND_CHANNELS - 1)
                wifi_scan_obj.dual_band_channel_index++;
              else
                wifi_scan_obj.dual_band_channel_index = 0;

              wifi_scan_obj.changeChannel(wifi_scan_obj.dual_band_channels[wifi_scan_obj.dual_band_channel_index]);
            #endif
          }
          else if (wifi_scan_obj.currentScanMode == WIFI_SCAN_CHAN_ACT) {
            #ifndef HAS_DUAL_BAND
              if (wifi_scan_obj.activity_page < MAX_CHANNEL / CHAN_PER_PAGE) {
                wifi_scan_obj.activity_page++;
              }
            #else
              if (wifi_scan_obj.activity_page < DUAL_BAND_CHANNELS / CHAN_PER_PAGE) {
                wifi_scan_obj.activity_page++;
              }
            #endif
            wifi_scan_obj.drawChannelLine();
          }
        }
        if (menu_button == DOWN_BUTTON) {
          if ((wifi_scan_obj.currentScanMode == WIFI_SCAN_OFF) ||
              (wifi_scan_obj.currentScanMode == WIFI_CONNECTED) ||
              (wifi_scan_obj.currentScanMode == OTA_UPDATE)) {
            if (current_menu->selected < current_menu->list->size() - 1) {
              current_menu->selected++;
              // Page down
              if (current_menu->selected - this->menu_start_index >= BUTTON_SCREEN_LIMIT) {
                this->buildButtons(current_menu, current_menu->selected + 1 - BUTTON_SCREEN_LIMIT);
                this->displayCurrentMenu(current_menu->selected + 1 - BUTTON_SCREEN_LIMIT);
              }
              else
                this->buttonSelected(current_menu->selected - this->menu_start_index, current_menu->selected);
              if (!current_menu->list->get(current_menu->selected - 1).selected || (current_menu->list->get(current_menu->selected - 1).icon == SETTINGS && current_menu->list->get(current_menu->selected - 1).color == TFTLIGHTGREY))
                this->buttonNotSelected(current_menu->selected - 1 - this->menu_start_index, current_menu->selected - 1);
            }
            // Loop to beginning
            else {
              if (current_menu->selected >= BUTTON_SCREEN_LIMIT) {
                current_menu->selected = 0;
                this->buildButtons(current_menu);
                this->displayCurrentMenu();
                this->buttonSelected(current_menu->selected);
              }
              else {
                current_menu->selected = 0;
                this->buttonSelected(current_menu->selected);
                if (!current_menu->list->get(current_menu->list->size() - 1).selected || (current_menu->list->get(current_menu->list->size() - 1).icon == SETTINGS && current_menu->list->get(current_menu->list->size() - 1).color == TFTLIGHTGREY))
                  this->buttonNotSelected(current_menu->list->size() - 1);
              }
            }
          }
          else if ((wifi_scan_obj.currentScanMode == WIFI_PACKET_MONITOR) ||
                  (wifi_scan_obj.currentScanMode == WIFI_SCAN_EAPOL) ||
                  (wifi_scan_obj.currentScanMode == WIFI_SCAN_CHAN_ANALYZER) ||
                  (wifi_scan_obj.currentScanMode == WIFI_SCAN_PACKET_RATE) ||
                  (wifi_scan_obj.currentScanMode == WIFI_SCAN_RAW_CAPTURE) ||
                  (wifi_scan_obj.currentScanMode == WIFI_SCAN_AP) ||
                  (wifi_scan_obj.currentScanMode == BT_SCAN_FLOCK) ||
                  (wifi_scan_obj.currentScanMode == WIFI_SCAN_PROBE) ||
                  (wifi_scan_obj.currentScanMode == WIFI_SCAN_DEAUTH) ||
                  (wifi_scan_obj.currentScanMode == WIFI_SCAN_SIG_STREN)) {
            #ifndef HAS_DUAL_BAND
              if (wifi_scan_obj.set_channel > 1)
                wifi_scan_obj.changeChannel(wifi_scan_obj.set_channel - 1);
              else
                wifi_scan_obj.changeChannel(14);
            #else
              if (wifi_scan_obj.dual_band_channel_index > 0)
                wifi_scan_obj.dual_band_channel_index--;
              else
                wifi_scan_obj.dual_band_channel_index = DUAL_BAND_CHANNELS - 1;

              wifi_scan_obj.changeChannel(wifi_scan_obj.dual_band_channels[wifi_scan_obj.dual_band_channel_index]);
            #endif
          }
          else if (wifi_scan_obj.currentScanMode == WIFI_SCAN_CHAN_ACT) {
            #ifndef HAS_DUAL_BAND
              if (wifi_scan_obj.activity_page > 1) {
                wifi_scan_obj.activity_page--;
              }
            #else
              if (wifi_scan_obj.activity_page > 0) {
                wifi_scan_obj.activity_page--;
              }
            #endif
            wifi_scan_obj.drawChannelLine();
          }
        }
        if(menu_button == SELECT_BUTTON) {
          current_menu->list->get(current_menu->selected).callable();
        }
        else {
          if ((wifi_scan_obj.currentScanMode == WIFI_SCAN_OFF) ||
              (wifi_scan_obj.currentScanMode == WIFI_CONNECTED))
            this->displayMenuButtons();
        }
      }
    }
    x = -1;
    y = -1;
  #endif

  // Menu navigation and paging
  #ifdef HAS_BUTTONS
    // Don't do this for touch screens
    #if !(defined(MARAUDER_V6) || defined(MARAUDER_V6_1) || defined(MARAUDER_CYD_MICRO) || defined(MARAUDER_CYD_GUITION) || defined(MARAUDER_CYD_2USB) || defined(MARAUDER_CYD_3_5_INCH))
      #if !defined(MARAUDER_M5STICKC) || defined(MARAUDER_M5STICKCP2)
        #ifdef MARAUDER_MINI_V3
          this->updateMiniMenuNavigationRepeat(currentTime);
        #endif

        #if (U_BTN >= 0 || defined(MARAUDER_CARDPUTER) || defined(MARAUDER_CARDPUTER_ADV))
          #if (U_BTN >= 0)
            if (u_btn.justPressed()) {
          #elif defined(MARAUDER_CARDPUTER) || defined(MARAUDER_CARDPUTER_ADV)
            if (this->isKeyPressed(';')) {
          #endif
              #ifdef MARAUDER_MINI_V3
              if ((wifi_scan_obj.currentScanMode == WIFI_SCAN_OFF) ||
                  (wifi_scan_obj.currentScanMode == WIFI_CONNECTED) ||
                  (wifi_scan_obj.currentScanMode == OTA_UPDATE)) {
                this->navigateMiniMenu(0, -1);
              }
              else
              #endif
              if ((wifi_scan_obj.currentScanMode == WIFI_SCAN_OFF) ||
                  (wifi_scan_obj.currentScanMode == WIFI_CONNECTED) ||
                  (wifi_scan_obj.currentScanMode == OTA_UPDATE)) {
                if (current_menu->selected > 0) {
                  current_menu->selected--;
                  // Page up
                  if (current_menu->selected < this->menu_start_index) {
                    this->buildButtons(current_menu, current_menu->selected);
                    this->displayCurrentMenu(current_menu->selected);
                  }
                  this->buttonSelected(current_menu->selected - this->menu_start_index, current_menu->selected);
                  if (!current_menu->list->get(current_menu->selected + 1).selected || (current_menu->list->get(current_menu->selected + 1).icon == SETTINGS && current_menu->list->get(current_menu->selected + 1).color == TFTLIGHTGREY))
                    this->buttonNotSelected(current_menu->selected + 1 - this->menu_start_index, current_menu->selected + 1);
                }
                // Loop to end
                else {
                  current_menu->selected = current_menu->list->size() - 1;
                  if (current_menu->selected >= BUTTON_SCREEN_LIMIT) {
                    this->buildButtons(current_menu, current_menu->selected + 1 - BUTTON_SCREEN_LIMIT);
                    this->displayCurrentMenu(current_menu->selected + 1 - BUTTON_SCREEN_LIMIT);
                  }
                  this->buttonSelected(current_menu->selected, current_menu->selected);
                  if (!current_menu->list->get(0).selected || (current_menu->list->get(0).icon == SETTINGS && current_menu->list->get(0).color == TFTLIGHTGREY))
                    this->buttonNotSelected(0, this->menu_start_index);
                }
              }
              else if ((wifi_scan_obj.currentScanMode == WIFI_PACKET_MONITOR) ||
                      (wifi_scan_obj.currentScanMode == WIFI_SCAN_EAPOL) ||
                      (wifi_scan_obj.currentScanMode == WIFI_SCAN_CHAN_ANALYZER) ||
                      (wifi_scan_obj.currentScanMode == WIFI_SCAN_PACKET_RATE) ||
                      (wifi_scan_obj.currentScanMode == WIFI_SCAN_RAW_CAPTURE) ||
                      (wifi_scan_obj.currentScanMode == WIFI_SCAN_AP) ||
                      (wifi_scan_obj.currentScanMode == BT_SCAN_FLOCK) ||
                      (wifi_scan_obj.currentScanMode == WIFI_SCAN_PROBE) ||
                      (wifi_scan_obj.currentScanMode == WIFI_SCAN_DEAUTH) ||
                      (wifi_scan_obj.currentScanMode == WIFI_SCAN_SIG_STREN)) {
                #ifndef HAS_DUAL_BAND
                  if (wifi_scan_obj.set_channel < 14)
                    wifi_scan_obj.changeChannel(wifi_scan_obj.set_channel + 1);
                  else
                    wifi_scan_obj.changeChannel(1);
                #else
                  if (wifi_scan_obj.dual_band_channel_index < DUAL_BAND_CHANNELS - 1)
                    wifi_scan_obj.dual_band_channel_index++;
                  else
                    wifi_scan_obj.dual_band_channel_index = 0;

                  wifi_scan_obj.changeChannel(wifi_scan_obj.dual_band_channels[wifi_scan_obj.dual_band_channel_index]);
                #endif
              }
              else if (wifi_scan_obj.currentScanMode == WIFI_SCAN_CHAN_ACT) {
                #ifndef HAS_DUAL_BAND
                  if (wifi_scan_obj.activity_page < MAX_CHANNEL / CHAN_PER_PAGE) {
                    wifi_scan_obj.activity_page++;
                  }
                #else
                  if (wifi_scan_obj.activity_page < DUAL_BAND_CHANNELS / CHAN_PER_PAGE) {
                    wifi_scan_obj.activity_page++;
                  }
                #endif
                wifi_scan_obj.drawChannelLine();
              }
            }
        #endif
      #endif

      #if (D_BTN >= 0 || defined(MARAUDER_CARDPUTER) || defined(MARAUDER_CARDPUTER_ADV))
      #if (D_BTN >= 0)
      if (d_btn.justPressed()){
      #elif defined(MARAUDER_CARDPUTER) || defined(MARAUDER_CARDPUTER_ADV)
      if (this->isKeyPressed('.')){
      #endif
        #ifdef MARAUDER_MINI_V3
        if ((wifi_scan_obj.currentScanMode == WIFI_SCAN_OFF) ||
            (wifi_scan_obj.currentScanMode == WIFI_CONNECTED) ||
            (wifi_scan_obj.currentScanMode == OTA_UPDATE)) {
          this->navigateMiniMenu(0, 1);
        }
        else
        #endif
        if ((wifi_scan_obj.currentScanMode == WIFI_SCAN_OFF) ||
            (wifi_scan_obj.currentScanMode == WIFI_CONNECTED) ||
            (wifi_scan_obj.currentScanMode == OTA_UPDATE)) {
          if (current_menu->selected < current_menu->list->size() - 1) {
            current_menu->selected++;
            // Page down
            if (current_menu->selected - this->menu_start_index >= BUTTON_SCREEN_LIMIT) {
              this->buildButtons(current_menu, current_menu->selected + 1 - BUTTON_SCREEN_LIMIT);
              this->displayCurrentMenu(current_menu->selected + 1 - BUTTON_SCREEN_LIMIT);
            }
            else
              this->buttonSelected(current_menu->selected - this->menu_start_index, current_menu->selected);
            if (!current_menu->list->get(current_menu->selected - 1).selected || (current_menu->list->get(current_menu->selected - 1).icon == SETTINGS && current_menu->list->get(current_menu->selected - 1).color == TFTLIGHTGREY))
              this->buttonNotSelected(current_menu->selected - 1 - this->menu_start_index, current_menu->selected - 1);
          }
          // Loop to beginning
          else {
            if (current_menu->selected >= BUTTON_SCREEN_LIMIT) {
              current_menu->selected = 0;
              this->buildButtons(current_menu);
              this->displayCurrentMenu();
              this->buttonSelected(current_menu->selected);
            }
            else {
              current_menu->selected = 0;
              this->buttonSelected(current_menu->selected);
              if (!current_menu->list->get(current_menu->list->size() - 1).selected || (current_menu->list->get(current_menu->list->size() - 1).icon == SETTINGS && current_menu->list->get(current_menu->list->size() - 1).color == TFTLIGHTGREY))
                this->buttonNotSelected(current_menu->list->size() - 1);
            }
          }
        }
        else if ((wifi_scan_obj.currentScanMode == WIFI_PACKET_MONITOR) ||
                (wifi_scan_obj.currentScanMode == WIFI_SCAN_EAPOL) ||
                (wifi_scan_obj.currentScanMode == WIFI_SCAN_CHAN_ANALYZER) ||
                (wifi_scan_obj.currentScanMode == WIFI_SCAN_PACKET_RATE) ||
                (wifi_scan_obj.currentScanMode == WIFI_SCAN_RAW_CAPTURE) ||
                (wifi_scan_obj.currentScanMode == WIFI_SCAN_AP) ||
                (wifi_scan_obj.currentScanMode == BT_SCAN_FLOCK) ||
                (wifi_scan_obj.currentScanMode == WIFI_SCAN_PROBE) ||
                (wifi_scan_obj.currentScanMode == WIFI_SCAN_DEAUTH) ||
                (wifi_scan_obj.currentScanMode == WIFI_SCAN_SIG_STREN)) {
          #ifndef HAS_DUAL_BAND
            if (wifi_scan_obj.set_channel > 1)
              wifi_scan_obj.changeChannel(wifi_scan_obj.set_channel - 1);
            else
              wifi_scan_obj.changeChannel(14);
          #else
            if (wifi_scan_obj.dual_band_channel_index > 0)
              wifi_scan_obj.dual_band_channel_index--;
            else
              wifi_scan_obj.dual_band_channel_index = DUAL_BAND_CHANNELS - 1;

            wifi_scan_obj.changeChannel(wifi_scan_obj.dual_band_channels[wifi_scan_obj.dual_band_channel_index]);
          #endif
        }
        else if (wifi_scan_obj.currentScanMode == WIFI_SCAN_CHAN_ACT) {
          #ifndef HAS_DUAL_BAND
            if (wifi_scan_obj.activity_page > 1) {
              wifi_scan_obj.activity_page--;
            }
          #else
            if (wifi_scan_obj.activity_page > 0) {
              wifi_scan_obj.activity_page--;
            }
          #endif
          wifi_scan_obj.drawChannelLine();
        }
      }
      #endif

      #if (R_BTN >= 0 || defined(MARAUDER_CARDPUTER) || defined(MARAUDER_CARDPUTER_ADV))
      #if (R_BTN >= 0)
      if (r_btn.justPressed()) {
      #elif defined(MARAUDER_CARDPUTER) || defined(MARAUDER_CARDPUTER_ADV)
      if (this->isKeyPressed('/')) {
      #endif
        #ifdef MARAUDER_MINI_V3
        if ((wifi_scan_obj.currentScanMode == WIFI_SCAN_OFF) ||
            (wifi_scan_obj.currentScanMode == WIFI_CONNECTED) ||
            (wifi_scan_obj.currentScanMode == OTA_UPDATE)) {
          this->navigateMiniMenu(1, 0);
        }
        else
        #endif
        if (wifi_scan_obj.currentScanMode == WIFI_SCAN_OFF) {
          #ifndef HAS_DUAL_BAND
            if (wifi_scan_obj.set_channel < 14)
              wifi_scan_obj.changeChannel(wifi_scan_obj.set_channel + 1);
            else
              wifi_scan_obj.changeChannel(1);
          #else
            if (wifi_scan_obj.dual_band_channel_index < DUAL_BAND_CHANNELS - 1)
              wifi_scan_obj.dual_band_channel_index++;
            else
              wifi_scan_obj.dual_band_channel_index = 0;

            wifi_scan_obj.changeChannel(wifi_scan_obj.dual_band_channels[wifi_scan_obj.dual_band_channel_index]);
          #endif
        }
      }
      #endif

      #if (L_BTN >= 0 || defined(MARAUDER_CARDPUTER) || defined(MARAUDER_CARDPUTER_ADV))
      #if (L_BTN >= 0)
      if (l_btn.justPressed()) {
      #elif defined(MARAUDER_CARDPUTER) || defined(MARAUDER_CARDPUTER_ADV)
      if (this->isKeyPressed(',')) {
      #endif
        #ifdef MARAUDER_MINI_V3
        if ((wifi_scan_obj.currentScanMode == WIFI_SCAN_OFF) ||
            (wifi_scan_obj.currentScanMode == WIFI_CONNECTED) ||
            (wifi_scan_obj.currentScanMode == OTA_UPDATE)) {
          this->navigateMiniMenu(-1, 0);
        }
        else
        #endif
        if (wifi_scan_obj.currentScanMode == WIFI_SCAN_OFF) {
          #ifndef HAS_DUAL_BAND
            if (wifi_scan_obj.set_channel > 1)
              wifi_scan_obj.changeChannel(wifi_scan_obj.set_channel - 1);
            else
              wifi_scan_obj.changeChannel(14);
          #else
            if (wifi_scan_obj.dual_band_channel_index > 0)
              wifi_scan_obj.dual_band_channel_index--;
            else
              wifi_scan_obj.dual_band_channel_index = DUAL_BAND_CHANNELS - 1;

            wifi_scan_obj.changeChannel(wifi_scan_obj.dual_band_channels[wifi_scan_obj.dual_band_channel_index]);
          #endif
        }
      }
      #endif

      #if defined(MARAUDER_CARDPUTER) || defined(MARAUDER_CARDPUTER_ADV)
      if (this->isKeyPressed('`') || this->isKeyPressed(KEY_BACKSPACE)) {
        if (wifi_scan_obj.currentScanMode == WIFI_SCAN_OFF) {
          if (current_menu->parentMenu != NULL) {
            this->changeMenu(current_menu->parentMenu, true);
          }
        }
      }
      #endif

      if(c_btn_press){
        current_menu->list->get(current_menu->selected).callable();
      }

    #endif
  #endif
}

#if BATTERY_ANALOG_ON == 1
byte battery_analog_array[10];
byte battery_count = 0;
byte battery_analog_last = 101;
#define BATTERY_CHECK 50
uint16_t battery_analog = 0;
void MenuFunctions::battery(bool initial)
{
  if (BATTERY_ANALOG_ON) {
    uint8_t n = 0;
    byte battery_analog_sample[10];
    byte deviation;
    if (battery_count == BATTERY_CHECK - 5)  digitalWrite(BATTERY_PIN, HIGH);
    else if (battery_count == 5) digitalWrite(BATTERY_PIN, LOW);
    if (battery_count == 0) {
      battery_analog = 0;
      for (n = 9; n > 0; n--)battery_analog_array[n] = battery_analog_array[n - 1];
      for (n = 0; n < 10; n++) {
        battery_analog_sample[n] = map((analogRead(ANALOG_PIN) * 5), 2400, 4200, 0, 100);
        if (battery_analog_sample[n] > 100) battery_analog_sample[n] = 100;
        else if (battery_analog_sample[n] < 0) battery_analog_sample[n] = 0;
        battery_analog += battery_analog_sample[n];
      }
      battery_analog = battery_analog / 10;
      for (n = 0; n < 10; n++) {
        deviation = abs(battery_analog - battery_analog_sample[n]);
        if (deviation >= 10) battery_analog_sample[n] = battery_analog;
      }
      battery_analog = 0;
      for (n = 0; n < 10; n++) battery_analog += battery_analog_sample[n];
      battery_analog = battery_analog / 10;
      battery_analog_array[0] = battery_analog;
      if (battery_analog_array[9] > 0 ) {
        battery_analog = 0;
        for (n = 0; n < 10; n++) battery_analog += battery_analog_array[n];
        battery_analog = battery_analog / 10;
      }
      battery_count ++;
    }
    else if (battery_count < BATTERY_CHECK) battery_count++;
    else if (battery_count >= BATTERY_CHECK) battery_count = 0;

    if (battery_analog_last != battery_analog) {
      battery_analog_last = battery_analog;
      MenuFunctions::battery2();
    }
  }
}
void MenuFunctions::battery2(bool initial)
{
  uint16_t the_color;
  if ( digitalRead(CHARGING_PIN) == 1) the_color = TFT_BLUE;
  else if (battery_analog < 20) the_color = TFT_RED;
  else if (battery_analog < 40)  the_color = TFT_YELLOW;
  else the_color = TFT_GREEN;

  display_obj.tft.setTextColor(the_color, STATUSBAR_COLOR);
  display_obj.tft.fillRect(SB_TOUCH_X, 0, 50, STATUS_BAR_WIDTH, STATUSBAR_COLOR);
  display_obj.tft.drawXBitmap(SB_TOUCH_X,
                              0,
                              menu_icons[STATUS_BAT],
                              16,
                              16,
                              STATUSBAR_COLOR,
                              the_color);
  display_obj.tft.drawString((String) battery_analog + "%", SB_BAT_X, 0, 2);
}
#else
void MenuFunctions::battery(bool initial)
{
  #ifdef HAS_BATTERY
    uint16_t the_color;
    if (battery_obj.i2c_supported)
    {
      // Could use int compare maybe idk
      if (((String)battery_obj.battery_level != "25") && ((String)battery_obj.battery_level != "0"))
        the_color = TFT_GREEN;
      else
        the_color = TFT_RED;

      if ((battery_obj.battery_level != battery_obj.old_level) || (initial)) {
        battery_obj.old_level = battery_obj.battery_level;
        display_obj.tft.fillRect(204, 0, SCREEN_WIDTH, STATUS_BAR_WIDTH, STATUSBAR_COLOR);
      }

      display_obj.tft.setCursor(0, 1);
      /*if (!this->disable_touch) {
        display_obj.tft.drawXBitmap(SB_TOUCH_X,
                                    0,
                                    menu_icons[STATUS_BAT],
                                    16,
                                    16,
                                    STATUSBAR_COLOR,
                                    the_color);
      }*/
      #if defined(MARAUDER_CARDPUTER) || defined(MARAUDER_CARDPUTER_ADV)
        display_obj.tft.drawString((String)battery_obj.battery_level + "%", SB_BAT_X, 0, 1);
      #else
        display_obj.tft.drawString((String)battery_obj.battery_level + "%", SB_BAT_X, 0, 2);
      #endif
    }
  #endif
}
void MenuFunctions::battery2(bool initial)
{
  MenuFunctions::battery(initial);
}
#endif

void MenuFunctions::updateStatusBar()
{
  display_obj.tft.setTextSize(1);

  bool status_changed = false;
  
  #if defined(MARAUDER_MINI) || defined(MARAUDER_M5STICKC) || defined(MARAUDER_REV_FEATHER) || defined(MARAUDER_CARDPUTER) || defined(MARAUDER_CARDPUTER_ADV) || defined(MARAUDER_MINI_V3)
    display_obj.tft.setFreeFont(NULL);
  #endif

  uint16_t the_color; 

  #ifdef HAS_GPS
    if (this->old_gps_sat_count != gps_obj.getNumSats()) {
      this->old_gps_sat_count = gps_obj.getNumSats();
      display_obj.tft.fillRect(0, 0, SCREEN_WIDTH, STATUS_BAR_WIDTH, STATUSBAR_COLOR);
      status_changed = true;
    }
  #endif

  // GPS Stuff
  #ifdef HAS_GPS
    #ifdef MARAUDER_MINI_V3
      const bool show_gps_status = gps_obj.getFixStatus();
    #else
      const bool show_gps_status = gps_obj.getGpsModuleStatus();
    #endif
    if (show_gps_status) {
      #ifdef MARAUDER_MINI_V3
        the_color = TFT_GREEN;
      #else
        if (gps_obj.getFixStatus())
          the_color = TFT_GREEN;
        else
          the_color = TFT_RED;
      #endif
        
      #ifdef HAS_FULL_SCREEN
        display_obj.tft.drawXBitmap(4,
                                    0,
                                    menu_icons[STATUS_GPS],
                                    16,
                                    16,
                                    STATUSBAR_COLOR,
                                    the_color);
        display_obj.tft.setTextColor(TFT_WHITE, STATUSBAR_COLOR, true);

        display_obj.tft.drawString(gps_obj.getNumSatsString(), 22, 0, 2);
      #elif defined(HAS_SCREEN)
        display_obj.tft.setTextColor(the_color, STATUSBAR_COLOR, true);
        display_obj.tft.drawString("GPS", 0, 0, 1);
      #endif
    }
  #endif

  display_obj.tft.setTextColor(TFT_WHITE, STATUSBAR_COLOR, true);

  // WiFi Channel Stuff
  uint8_t primaryChannel;
  wifi_second_chan_t secondChannel;
  esp_err_t err = esp_wifi_get_channel(&primaryChannel, &secondChannel);

  uint8_t current_channel = wifi_scan_obj.set_channel;

  if (err == ESP_OK)
    current_channel = primaryChannel;

  if ((current_channel != wifi_scan_obj.old_channel) || (status_changed)) {
    wifi_scan_obj.old_channel = current_channel;
    #if defined(MARAUDER_MINI) || defined(MARAUDER_M5STICKC) || defined(MARAUDER_REV_FEATHER) || defined(MARAUDER_CARDPUTER) || defined(MARAUDER_CARDPUTER_ADV) || defined(MARAUDER_MINI_V3)
      display_obj.tft.fillRect(TFT_WIDTH/4, 0, CHAR_WIDTH * 6, STATUS_BAR_WIDTH, STATUSBAR_COLOR);
    #elif defined(HAS_DUAL_BAND)
      display_obj.tft.fillRect(50, 0, (CHAR_WIDTH / 2) * 8, STATUS_BAR_WIDTH, STATUSBAR_COLOR);
    #else
      display_obj.tft.fillRect(50, 0, (CHAR_WIDTH / 2) * 7, STATUS_BAR_WIDTH, STATUSBAR_COLOR);
    #endif
    #ifdef HAS_FULL_SCREEN
      display_obj.tft.drawString("CH: " + (String)wifi_scan_obj.old_channel, 50, 0, 2);
    #endif

    #ifdef HAS_MINI_SCREEN
      display_obj.tft.drawString("CH:" + (String)wifi_scan_obj.old_channel, TFT_WIDTH/4, 0, 1);
    #endif
  }

  // RAM Stuff
  wifi_scan_obj.free_ram = String(esp_get_free_heap_size());
  if ((wifi_scan_obj.free_ram != wifi_scan_obj.old_free_ram) || (status_changed)) {
    wifi_scan_obj.old_free_ram = wifi_scan_obj.free_ram;
    //display_obj.tft.fillRect(SB_MEM_X, 0, 60, STATUS_BAR_WIDTH, STATUSBAR_COLOR);
    #ifdef HAS_FULL_SCREEN
    #ifndef HAS_PSRAM
      display_obj.tft.drawString("D:" + String(getDRAMUsagePercent()) + "%", SB_MEM_X, 0, 2);
    #else
      display_obj.tft.drawString("D:" + String(getDRAMUsagePercent()) + "%", SB_MEM_X, 0, 1);
      display_obj.tft.drawString("P:" + String(getPSRAMUsagePercent()) + "%", SB_MEM_X, 8, 1);
    #endif
  #endif

  #ifdef HAS_MINI_SCREEN
    display_obj.tft.drawString(String(getDRAMUsagePercent()) + "%", TFT_WIDTH/1.75, 0, 1);
  #endif
  }

  // Draw battery info
  MenuFunctions::battery(false);
  display_obj.tft.fillRect(186, 0, 16, STATUS_BAR_WIDTH, STATUSBAR_COLOR);

  // Disable touch stuff
  #ifdef HAS_ILI9341
    #ifdef HAS_BUTTONS
      if (this->disable_touch) {
        display_obj.tft.setCursor(0, 1);
        display_obj.tft.drawXBitmap(SB_TOUCH_X,
                                    0,
                                    menu_icons[DISABLE_TOUCH],
                                    16,
                                    16,
                                    STATUSBAR_COLOR,
                                    TFT_RED);
      }
      else {
        display_obj.tft.setCursor(0, 1);
        display_obj.tft.drawXBitmap(SB_TOUCH_X,
                                    0,
                                    menu_icons[DISABLE_TOUCH],
                                    16,
                                    16,
                                    STATUSBAR_COLOR,
                                    TFT_DARKGREY);
      }
    #endif
  #endif

  // Draw SD info
  #ifdef HAS_SD
    if (sd_obj.supported)
      the_color = TFT_GREEN;
    else
      the_color = TFT_RED;

    #ifdef HAS_FULL_SCREEN
      display_obj.tft.drawXBitmap(SB_SD_X,
                                  0,
                                  menu_icons[STATUS_SD],
                                  16,
                                  16,
                                  STATUSBAR_COLOR,
                                  the_color);
    #endif
  #endif

  #ifdef HAS_MINI_SCREEN
    display_obj.tft.setTextColor(the_color, STATUSBAR_COLOR, true);
    display_obj.tft.drawString("SD", TFT_WIDTH - 12, 0, 1);
  #endif

  // WiFi connection status stuff
  if (wifi_scan_obj.wifi_connected) {
    #ifdef HAS_FULL_SCREEN
      display_obj.tft.drawXBitmap(SB_WIFI_X,
                                  0,
                                  menu_icons[JOINED],
                                  16,
                                  16,
                                  STATUSBAR_COLOR,
                                  TFT_GREEN);
    #endif
  } else {
    #ifdef HAS_FULL_SCREEN
      display_obj.tft.drawXBitmap(SB_WIFI_X,
                                  0,
                                  menu_icons[JOINED],
                                  16,
                                  16,
                                  STATUSBAR_COLOR,
                                  TFT_DARKGREY);
    #endif
  }

  // Force PMKID stuff
  if ((wifi_scan_obj.force_pmkid) || (wifi_scan_obj.ep_deauth)) {
    #ifdef HAS_FULL_SCREEN
      display_obj.tft.drawXBitmap(SB_FORCE_X,
                                  0,
                                  menu_icons[FORCE],
                                  16,
                                  16,
                                  STATUSBAR_COLOR,
                                  TFT_GREEN);
    #endif
  } else {
    #ifdef HAS_FULL_SCREEN
      display_obj.tft.drawXBitmap(SB_FORCE_X,
                                  0,
                                  menu_icons[FORCE],
                                  16,
                                  16,
                                  STATUSBAR_COLOR,
                                  TFT_DARKGREY);
    #endif
  }
}

void MenuFunctions::drawStatusBar()
{
  display_obj.tft.setTextSize(1);
  #ifdef HAS_MINI_SCREEN
    display_obj.tft.setFreeFont(NULL);
  #endif
  display_obj.tft.fillRect(0, 0, SCREEN_WIDTH, STATUS_BAR_WIDTH, STATUSBAR_COLOR);
  display_obj.tft.setTextColor(TFT_WHITE, STATUSBAR_COLOR);

  uint16_t the_color;

  // GPS Stuff
  #ifdef HAS_GPS
    #ifdef MARAUDER_MINI_V3
      const bool show_gps_status = gps_obj.getFixStatus();
    #else
      const bool show_gps_status = gps_obj.getGpsModuleStatus();
    #endif
    if (show_gps_status) {
      #ifdef MARAUDER_MINI_V3
        the_color = TFT_GREEN;
      #else
        if (gps_obj.getFixStatus())
          the_color = TFT_GREEN;
        else
          the_color = TFT_RED;
      #endif
        
      #ifdef HAS_FULL_SCREEN
        display_obj.tft.drawXBitmap(4,
                                    0,
                                    menu_icons[STATUS_GPS],
                                    16,
                                    16,
                                    STATUSBAR_COLOR,
                                    the_color);
        display_obj.tft.setTextColor(TFT_WHITE, STATUSBAR_COLOR);

        display_obj.tft.drawString(gps_obj.getNumSatsString(), 22, 0, 2);
      #endif
    }
  #endif

  display_obj.tft.setTextColor(TFT_WHITE, STATUSBAR_COLOR);


  // WiFi Channel Stuff
  uint8_t primaryChannel;
  wifi_second_chan_t secondChannel;
  esp_err_t err = esp_wifi_get_channel(&primaryChannel, &secondChannel);

  if (err == ESP_OK)
    wifi_scan_obj.old_channel = primaryChannel;
  else
    wifi_scan_obj.old_channel = wifi_scan_obj.set_channel;

  #ifdef HAS_MINI_SCREEN
    display_obj.tft.fillRect(43, 0, TFT_WIDTH * 0.21, STATUS_BAR_WIDTH, STATUSBAR_COLOR);
  #else
    display_obj.tft.fillRect(50, 0, TFT_WIDTH * 0.21, STATUS_BAR_WIDTH, STATUSBAR_COLOR);
  #endif
  #ifdef HAS_FULL_SCREEN
    display_obj.tft.drawString("CH: " + (String)wifi_scan_obj.old_channel, 50, 0, 2);
  #endif

  #ifdef HAS_MINI_SCREEN
    display_obj.tft.drawString("CH:" + (String)wifi_scan_obj.old_channel, TFT_WIDTH/4, 0, 1);
  #endif

  // RAM Stuff
  wifi_scan_obj.free_ram = String(esp_get_free_heap_size());
  wifi_scan_obj.old_free_ram = wifi_scan_obj.free_ram;
  display_obj.tft.fillRect(100, 0, 60, STATUS_BAR_WIDTH, STATUSBAR_COLOR);
  #ifdef HAS_FULL_SCREEN
    #ifndef HAS_PSRAM
      display_obj.tft.drawString("D:" + String(getDRAMUsagePercent()) + "%", SB_MEM_X, 0, 2);
    #else
      display_obj.tft.drawString("D:" + String(getDRAMUsagePercent()) + "%", SB_MEM_X, 0, 1);
      display_obj.tft.drawString("P:" + String(getPSRAMUsagePercent()) + "%", SB_MEM_X, 8, 1);
    #endif
  #endif

  #ifdef HAS_MINI_SCREEN
    display_obj.tft.drawString(String(getDRAMUsagePercent()) + "%", TFT_WIDTH/1.75, 0, 1);
  #endif


  MenuFunctions::battery(true);
  display_obj.tft.fillRect(186, 0, 16, STATUS_BAR_WIDTH, STATUSBAR_COLOR);


  // Disable touch stuff
  #ifdef HAS_ILI9341
    #ifdef HAS_BUTTONS
      if (this->disable_touch) {
        display_obj.tft.setCursor(0, 1);
        display_obj.tft.drawXBitmap(SB_TOUCH_X,
                                    0,
                                    menu_icons[DISABLE_TOUCH],
                                    16,
                                    16,
                                    STATUSBAR_COLOR,
                                    TFT_RED);
      }
      else {
        display_obj.tft.setCursor(0, 1);
        display_obj.tft.drawXBitmap(SB_TOUCH_X,
                                    0,
                                    menu_icons[DISABLE_TOUCH],
                                    16,
                                    16,
                                    STATUSBAR_COLOR,
                                    TFT_DARKGREY);
      }
    #endif
  #endif

  // Draw SD info
  #ifdef HAS_SD
    if (sd_obj.supported)
      the_color = TFT_GREEN;
    else
      the_color = TFT_RED;
  

    #ifdef HAS_FULL_SCREEN
      display_obj.tft.drawXBitmap(SB_SD_X,
                                  0,
                                  menu_icons[STATUS_SD],
                                  16,
                                  16,
                                  STATUSBAR_COLOR,
                                  the_color);
    #endif
  #endif

  #ifdef HAS_MINI_SCREEN
    display_obj.tft.setTextColor(the_color, STATUSBAR_COLOR);
    display_obj.tft.drawString("SD", TFT_WIDTH - 12, 0, 1);
  #endif

  // WiFi connection status stuff
  if (wifi_scan_obj.wifi_connected) {
    #ifdef HAS_FULL_SCREEN
      display_obj.tft.drawXBitmap(SB_WIFI_X,
                                  0,
                                  menu_icons[JOINED],
                                  16,
                                  16,
                                  STATUSBAR_COLOR,
                                  TFT_GREEN);
    #endif
  } else {
    #ifdef HAS_FULL_SCREEN
      display_obj.tft.drawXBitmap(SB_WIFI_X,
                                  0,
                                  menu_icons[JOINED],
                                  16,
                                  16,
                                  STATUSBAR_COLOR,
                                  TFT_DARKGREY);
    #endif
  }

  // Force PMKID stuff
  if ((wifi_scan_obj.force_pmkid) || (wifi_scan_obj.ep_deauth)) {
    #ifdef HAS_FULL_SCREEN
      display_obj.tft.drawXBitmap(SB_FORCE_X,
                                  0,
                                  menu_icons[FORCE],
                                  16,
                                  16,
                                  STATUSBAR_COLOR,
                                  TFT_GREEN);
    #endif
  } else {
    #ifdef HAS_FULL_SCREEN
      display_obj.tft.drawXBitmap(SB_FORCE_X,
                                  0,
                                  menu_icons[FORCE],
                                  16,
                                  16,
                                  STATUSBAR_COLOR,
                                  TFT_DARKGREY);
    #endif
  }
}

void MenuFunctions::orientDisplay() {
  display_obj.init();

  display_obj.tft.setRotation(SCREEN_ORIENTATION); // Portrait

  display_obj.tft.setCursor(0, 0);

  #ifdef HAS_ILI9341
    #ifndef HAS_CYD_TOUCH
      display_obj.setCalData();
    #else
      display_obj.touchscreen.setRotation(0);
    #endif
  #endif

  changeMenu(current_menu, true);
}

const char* MenuFunctions::callSetting(const char* key) {
  specSettingMenu.name = key;

  const char* setting_type = settings_obj.getSettingType(key);

  if (setting_type && strcmp(setting_type, "bool") == 0) {
    return "bool";
  }

  return "";
}

/*void MenuFunctions::displaySetting(String key, Menu* menu, int index) {
  specSettingMenu.name = key;

  bool setting_value = settings_obj.loadSetting<bool>(key);

  // Make a local copy of menu node
  MenuNode node = menu->list->get(index);

  display_obj.tft.setTextWrap(false);
  display_obj.tft.setFreeFont(NULL);
  display_obj.tft.setCursor(0, 100);
  display_obj.tft.setTextSize(1);

  // Set local copy value
  if (!setting_value) {
    display_obj.tft.setTextColor(TFT_RED);
    display_obj.tft.println(F(text_table1[4]));
    node.selected = false;
  }
  else {
    display_obj.tft.setTextColor(TFT_GREEN);
    display_obj.tft.println(F(text_table1[5]));
    node.selected = true;
  }

  // Put local copy back into menu
  menu->list->set(index, node);
    
}*/

void MenuFunctions::displaySetting(const char* key, Menu* menu, int index) {
  specSettingMenu.name = String(key);

  bool setting_value = settings_obj.loadSetting<bool>(key);

  // Make a local copy of menu node
  MenuNode node = menu->list->get(index);

  display_obj.tft.setTextWrap(false);
  display_obj.tft.setFreeFont(NULL);
  display_obj.tft.setCursor(0, 100);
  display_obj.tft.setTextSize(1);

  // Set local copy value
  if (!setting_value) {
    display_obj.tft.setTextColor(TFT_RED);
    display_obj.tft.println(F(text_table1[4]));
    node.selected = false;
  } else {
    display_obj.tft.setTextColor(TFT_GREEN);
    display_obj.tft.println(F(text_table1[5]));
    node.selected = true;
  }

  // Put local copy back into menu
  menu->list->set(index, node);
}

#if defined(MARAUDER_CARDPUTER) || defined(MARAUDER_CARDPUTER_ADV)
void MenuFunctions::updateKeyboard()
{
  M5CardputerKeyboard.updateKeyList();
  M5CardputerKeyboard.updateKeysState();
}

bool MenuFunctions::isKeyPressed(char c)
{
  bool pressed = M5CardputerKeyboard.isKeyPressed(c);

  if (pressed)
    delay(200);

  return pressed;
}
#endif

#ifdef HAS_DIRECT_UPLOAD
  void MenuFunctions::buildUploadFileMenu() {
    if (sd_obj.supported) {
      this->setupSDFileList();

      uploadLogsMenu.list->clear();
      delete uploadLogsMenu.list;
      uploadLogsMenu.list = new LinkedList<MenuNode>();
      uploadLogsMenu.name = "Logs";

      uploadLogsMenu.parentMenu = &wifiGeneralMenu;

      this->addNodes(&uploadLogsMenu, "Back", TFTLIGHTGREY, 0, [this]() {
        this->changeMenu(uploadLogsMenu.parentMenu, true);
      });

      this->addNodes(&uploadLogsMenu, "Delete Wardrive Logs", TFTORANGE, 0, [this]() {
        this->changeMenu(&deleteAllMenu, true);
      });

      this->addNodes(&uploadLogsMenu, "Upload All", TFTGREEN, 0, [this]() {
        this->changeMenu(&uploadAllMenu, true);
        
      });

      for (int i = 0; i < sd_obj.sd_files->size(); i++) {
        File current_file = sd_obj.getFile("/" + sd_obj.sd_files->get(i));
        if (sd_obj.sd_files->get(i).startsWith("wardrive_") || sd_obj.sd_files->get(i).startsWith("wigle-")) {
          if (!sd_obj.sd_files->get(i).endsWith(".wdg") && !sd_obj.sd_files->get(i).endsWith(".wigle") && !sd_obj.sd_files->get(i).endsWith(".gpx")) {
            this->addNodes(&uploadLogsMenu, sd_obj.sd_files->get(i).c_str(), TFTCYAN, 0, [this, i]() {
              sd_obj.selected_file_name = sd_obj.sd_files->get(i);
              Serial.println(sd_obj.sd_files->get(i) + " selected");
              this->changeMenu(&actionMenu, true);
            });
          }
        }
      }

      Serial.println("Built SD file menu with " + (String)sd_obj.sd_files->size() + " files");
    } else {
      Serial.println("SD Card not detected. Skipping menu creation...");
    }
  }
#endif

const char* MenuFunctions::foxSortLabel() const {
  switch (fox_sort_mode) {
    case TargetSortMode::SIGNAL_DESC: return "Signal v";
    case TargetSortMode::NAME_ASC: return "Name A-Z";
    case TargetSortMode::CHANNEL_ASC: return "Channel 1-177";
  }
  return "Signal v";
}

const char* MenuFunctions::foxFilterLabel() const {
  switch (fox_filter_mode) {
    case TargetFilterMode::ALL: return "All";
    case TargetFilterMode::RECENT_30S: return "Seen <30s";
    case TargetFilterMode::BAND_24_GHZ: return "2.4 GHz";
    case TargetFilterMode::BAND_5_GHZ: return "5 GHz";
  }
  return "All";
}

bool MenuFunctions::foxListSupportsRecent() const {
  return fox_target_list == FoxHuntListKind::AP_TARGETS ||
         fox_target_list == FoxHuntListKind::APS_WITH_STATIONS ||
         fox_target_list == FoxHuntListKind::FINDMY_TARGETS;
}

bool MenuFunctions::foxListSupportsBand() const {
  return fox_target_list == FoxHuntListKind::AP_TARGETS ||
         fox_target_list == FoxHuntListKind::APS_WITH_STATIONS ||
         fox_target_list == FoxHuntListKind::STATION_TARGETS ||
         fox_target_list == FoxHuntListKind::PINEAPPLE_TARGETS ||
         fox_target_list == FoxHuntListKind::MULTISSID_TARGETS;
}

void MenuFunctions::buildFoxSortMenu() {
  foxSortMenu.list->clear();
  foxSortMenu.parentMenu = current_menu;
  this->addNodes(&foxSortMenu, text09, TFTLIGHTGREY, 0, [this]() { this->changeMenu(foxSortMenu.parentMenu, true); });
  this->addNodes(&foxSortMenu, "Signal: Strongest", TFTCYAN, 255, [this]() {
    fox_sort_mode = TargetSortMode::SIGNAL_DESC;
    buildFoxTargetList(fox_target_list, fox_target_context_ap);
  });
  this->addNodes(&foxSortMenu, "Name/MAC: A-Z", TFTCYAN, 255, [this]() {
    fox_sort_mode = TargetSortMode::NAME_ASC;
    buildFoxTargetList(fox_target_list, fox_target_context_ap);
  });
  if (foxListSupportsBand()) {
    this->addNodes(&foxSortMenu, "Channel: Low-High", TFTCYAN, 255, [this]() {
      fox_sort_mode = TargetSortMode::CHANNEL_ASC;
      buildFoxTargetList(fox_target_list, fox_target_context_ap);
    });
  }
  this->changeMenu(&foxSortMenu, true);
}

void MenuFunctions::buildFoxFilterMenu() {
  foxFilterMenu.list->clear();
  foxFilterMenu.parentMenu = current_menu;
  this->addNodes(&foxFilterMenu, text09, TFTLIGHTGREY, 0, [this]() { this->changeMenu(foxFilterMenu.parentMenu, true); });
  this->addNodes(&foxFilterMenu, "All", TFTGREEN, 255, [this]() {
    fox_filter_mode = TargetFilterMode::ALL;
    buildFoxTargetList(fox_target_list, fox_target_context_ap);
  });
  if (foxListSupportsRecent()) {
    this->addNodes(&foxFilterMenu, "Seen in 30 Seconds", TFTGREEN, 255, [this]() {
      fox_filter_mode = TargetFilterMode::RECENT_30S;
      buildFoxTargetList(fox_target_list, fox_target_context_ap);
    });
  }
  if (foxListSupportsBand()) {
    this->addNodes(&foxFilterMenu, "2.4 GHz", TFTGREEN, 255, [this]() {
      fox_filter_mode = TargetFilterMode::BAND_24_GHZ;
      buildFoxTargetList(fox_target_list, fox_target_context_ap);
    });
    #ifdef HAS_DUAL_BAND
      this->addNodes(&foxFilterMenu, "5 GHz", TFTGREEN, 255, [this]() {
        fox_filter_mode = TargetFilterMode::BAND_5_GHZ;
        buildFoxTargetList(fox_target_list, fox_target_context_ap);
      });
    #endif
  }
  this->changeMenu(&foxFilterMenu, true);
}

void MenuFunctions::buildFoxTargetList(FoxHuntListKind type, int context_ap) {
  fox_target_list = type;
  fox_target_context_ap = context_ap;
  if (!foxListSupportsRecent() && fox_filter_mode == TargetFilterMode::RECENT_30S)
    fox_filter_mode = TargetFilterMode::ALL;
  if (!foxListSupportsBand() && (fox_filter_mode == TargetFilterMode::BAND_24_GHZ || fox_filter_mode == TargetFilterMode::BAND_5_GHZ))
    fox_filter_mode = TargetFilterMode::ALL;
  if (!foxListSupportsBand() && fox_sort_mode == TargetSortMode::CHANNEL_ASC)
    fox_sort_mode = TargetSortMode::SIGNAL_DESC;

  Menu* menu = type == FoxHuntListKind::STATION_TARGETS ? &wifiStationMenu : &wifiAPMenu;
  menu->list->clear();
  menu->parentMenu = type == FoxHuntListKind::STATION_TARGETS ? &wifiAPMenu : &foxHuntMenu;
  this->addNodes(menu, text09, TFTLIGHTGREY, 0, [this, menu]() { this->changeMenu(menu->parentMenu, true); });
  String sort_row = "Sort: " + String(foxSortLabel());
  this->addNodes(menu, sort_row.c_str(), TFTCYAN, 255, [this]() { buildFoxSortMenu(); });
  String filter_row = "Filter: " + String(foxFilterLabel());
  this->addNodes(menu, filter_row.c_str(), TFTGREEN, 255, [this]() { buildFoxFilterMenu(); });
  this->addNodes(menu, "Refresh List", TFTYELLOW, 255, [this]() { buildFoxTargetList(fox_target_list, fox_target_context_ap); });

  std::vector<TargetListItem> items;
  auto add_item = [&items](size_t index, int16_t rssi, uint8_t channel, uint32_t last_seen, const String& name) {
    TargetListItem item = {index, rssi, channel, last_seen, {}};
    strncpy(item.name, name.c_str(), sizeof(item.name) - 1);
    items.push_back(item);
  };

  if (type == FoxHuntListKind::AP_TARGETS || type == FoxHuntListKind::APS_WITH_STATIONS) {
    for (int i = 0; i < access_points->size(); i++) {
      const AccessPoint& ap = access_points->get(i);
      if (type == FoxHuntListKind::APS_WITH_STATIONS && ap.stations->size() == 0)
        continue;
      add_item(i, ap.rssi, ap.channel, ap.last_seen_ms, ap.essid.length() ? ap.essid : macToString(ap.bssid));
    }
  } else if (type == FoxHuntListKind::STATION_TARGETS && context_ap >= 0 && context_ap < access_points->size()) {
    const AccessPoint& ap = access_points->get(context_ap);
    for (int x = 0; x < ap.stations->size(); x++) {
      int station_index = ap.stations->get(x);
      add_item(station_index, -128, ap.channel, 0, macToString(stations->get(station_index).mac));
    }
  } else if (type == FoxHuntListKind::PINEAPPLE_TARGETS) {
    for (size_t i = 0; i < wifi_scan_obj.getPineScanCount(); i++)
      add_item(i, wifi_scan_obj.getPineScanRssi(i), wifi_scan_obj.getPineScanChannel(i), 0, wifi_scan_obj.getPineScanLabel(i));
  } else if (type == FoxHuntListKind::MULTISSID_TARGETS) {
    for (size_t i = 0; i < wifi_scan_obj.getMultiSSIDCount(); i++)
      add_item(i, wifi_scan_obj.getMultiSSIDRssi(i), wifi_scan_obj.getMultiSSIDChannel(i), 0, wifi_scan_obj.getMultiSSIDLabel(i));
  } else if (type == FoxHuntListKind::BLE_TARGETS) {
    for (int i = 0; i < ble_devices->size(); i++)
      add_item(i, ble_devices->get(i).rssi, 0, 0, ble_devices->get(i).name.length() ? ble_devices->get(i).name : macToString(ble_devices->get(i).mac));
  } else if (type == FoxHuntListKind::FINDMY_TARGETS) {
    for (int i = 0; i < airtags->size(); i++)
      add_item(i, airtags->get(i).rssi, 0, airtags->get(i).last_seen, airtags->get(i).mac);
  } else if (type == FoxHuntListKind::FLIPPER_TARGETS) {
    for (int i = 0; i < flippers->size(); i++)
      add_item(i, -128, 0, 0, flippers->get(i).name.length() ? flippers->get(i).name : flippers->get(i).mac);
  }

  std::vector<TargetListItem> filtered;
  uint32_t now = millis();
  for (const TargetListItem& item : items) {
    if (targetListItemMatchesFilter(item, fox_filter_mode, now))
      filtered.push_back(item);
  }
  sortTargetList(filtered, fox_sort_mode);

  for (const TargetListItem& item : filtered) {
    int index = item.source_index;
    String label;
    uint16_t color = TFTCYAN;
    if (type == FoxHuntListKind::AP_TARGETS) {
      const AccessPoint& ap = access_points->get(index);
      label = String(ap.rssi) + " " + ap.essid;
      color = rssiToMenuColor(ap.rssi);
    } else if (type == FoxHuntListKind::APS_WITH_STATIONS) {
      const AccessPoint& ap = access_points->get(index);
      label = ap.essid + " (" + String(ap.stations->size()) + ")";
      color = TFTMAGENTA;
    } else if (type == FoxHuntListKind::STATION_TARGETS) {
      label = macToString(stations->get(index).mac);
      color = TFTMAGENTA;
    } else if (type == FoxHuntListKind::PINEAPPLE_TARGETS) {
      label = wifi_scan_obj.getPineScanLabel(index);
      color = TFTYELLOW;
    } else if (type == FoxHuntListKind::MULTISSID_TARGETS) {
      label = wifi_scan_obj.getMultiSSIDLabel(index);
      color = TFTORANGE;
    } else if (type == FoxHuntListKind::BLE_TARGETS) {
      label = String(ble_devices->get(index).rssi) + " " + ble_devices->get(index).name;
      color = rssiToMenuColor(ble_devices->get(index).rssi);
    } else if (type == FoxHuntListKind::FINDMY_TARGETS) {
      label = String(airtags->get(index).rssi) + " " + airtags->get(index).mac;
      color = TFTWHITE;
    } else {
      label = flippers->get(index).name.length() ? flippers->get(index).name : flippers->get(index).mac;
      color = TFTORANGE;
    }

    this->addNodes(menu, label.c_str(), color, 255, [this, type, context_ap, index]() {
      if (type == FoxHuntListKind::APS_WITH_STATIONS) {
        buildFoxTargetList(FoxHuntListKind::STATION_TARGETS, index);
        return;
      }
      if (type == FoxHuntListKind::PINEAPPLE_TARGETS) {
        if (!wifi_scan_obj.selectPineScanFoxTarget(index)) return;
      } else if (type == FoxHuntListKind::MULTISSID_TARGETS) {
        if (!wifi_scan_obj.selectMultiSSIDFoxTarget(index)) return;
      } else if (type == FoxHuntListKind::AP_TARGETS) {
        const AccessPoint& ap = access_points->get(index);
        wifi_scan_obj.setFoxHuntTarget(ap.bssid, ap.essid, ap.rssi, ap.channel, false);
      } else if (type == FoxHuntListKind::STATION_TARGETS) {
        const AccessPoint& ap = access_points->get(context_ap);
        const Station& station = stations->get(index);
        wifi_scan_obj.setFoxHuntTarget(station.mac, macToString(station.mac), -128, ap.channel, false);
      } else if (type == FoxHuntListKind::BLE_TARGETS) {
        const BleDevice& device = ble_devices->get(index);
        wifi_scan_obj.setFoxHuntTarget(device.mac, device.name, device.rssi, 0, true, macToString(device.mac));
      } else if (type == FoxHuntListKind::FINDMY_TARGETS) {
        uint8_t mac[6];
        convertMacStringToUint8(airtags->get(index).mac, mac);
        wifi_scan_obj.setFoxHuntTarget(mac, airtags->get(index).mac, airtags->get(index).rssi, 0, true, airtags->get(index).mac);
      } else if (type == FoxHuntListKind::FLIPPER_TARGETS) {
        uint8_t mac[6];
        convertMacStringToUint8(flippers->get(index).mac, mac);
        String name = flippers->get(index).name.length() ? flippers->get(index).name : flippers->get(index).mac;
        wifi_scan_obj.setFoxHuntTarget(mac, name, -128, 0, true, flippers->get(index).mac);
      }
      display_obj.clearScreen();
      this->drawStatusBar();
      wifi_scan_obj.StartScan(type == FoxHuntListKind::BLE_TARGETS || type == FoxHuntListKind::FINDMY_TARGETS || type == FoxHuntListKind::FLIPPER_TARGETS ? BT_SCAN_FOX_HUNT : WIFI_SCAN_SIG_STREN, TFT_CYAN);
    });
  }
  this->changeMenu(menu, true);
}

#ifdef MARAUDER_MINI_V3
void MenuFunctions::buildSSIDGroupMenu(bool fox_hunt_mode,
                                       bool finder_mode) {
  ssidGroupMenu.list->clear();
  ssidGroupMenu.name = finder_mode
                           ? "SSID Finder"
                           : (fox_hunt_mode ? "Fox Hunt SSID"
                                            : "Select SSIDs");
  ssidGroupMenu.parentMenu = &wifiSnifferMenu;

  this->addNodes(&ssidGroupMenu, text09, TFTLIGHTGREY, 0,
                 [this, fox_hunt_mode]() {
    if (fox_hunt_mode) {
      for (int index = 0; index < access_points->size(); index++) {
        AccessPoint access_point = access_points->get(index);
        access_point.selected = false;
        access_points->set(index, access_point);
      }
    }
    this->changeMenu(ssidGroupMenu.parentMenu, true);
  });

  if (access_points->size() == 0) {
    this->addNodes(&ssidGroupMenu, "No APs - scan first", TFTORANGE,
                   DEVICE_INFO, []() {});
    return;
  }

  struct SSIDGroupSummary {
    String name;
    int8_t strongest_rssi;
  };

  std::vector<SSIDGroupSummary> groups;
  for (int index = 0; index < access_points->size(); index++) {
    const AccessPoint access_point = access_points->get(index);
    int group_index = -1;
    for (int candidate = 0;
         candidate < static_cast<int>(groups.size()); candidate++) {
      if (groups[candidate].name == access_point.essid) {
        group_index = candidate;
        break;
      }
    }

    if (group_index < 0)
      groups.push_back({access_point.essid, access_point.rssi});
    else if (access_point.rssi > groups[group_index].strongest_rssi)
      groups[group_index].strongest_rssi = access_point.rssi;
  }

  std::sort(groups.begin(), groups.end(),
            [](const SSIDGroupSummary& left,
               const SSIDGroupSummary& right) {
    if (left.strongest_rssi != right.strongest_rssi)
      return left.strongest_rssi > right.strongest_rssi;
    return left.name.compareTo(right.name) < 0;
  });

  for (const SSIDGroupSummary& group : groups) {
    const String group_name = group.name;
    uint16_t ap_count = 0;
    uint16_t channel_count = 0;
    uint16_t selected_count = 0;
    bool channels[256] = {};

    for (int ap_index = 0; ap_index < access_points->size(); ap_index++) {
      const AccessPoint access_point = access_points->get(ap_index);
      if (access_point.essid != group_name)
        continue;
      ap_count++;
      if (!channels[access_point.channel]) {
        channels[access_point.channel] = true;
        channel_count++;
      }
      if (access_point.selected)
        selected_count++;
    }

    const String ssid_name = group_name.length() > 0
                                 ? group_name
                                 : String("<Hidden SSID>");
    const String selection_count = finder_mode
                                       ? String(ap_count)
                                       : String(selected_count) + "/" +
                                             String(ap_count);
    const String display_name = String(group.strongest_rssi) + " " +
                                ssid_name + " [" + selection_count +
                                " AP, " + String(channel_count) + " CH]";

    this->addNodes(&ssidGroupMenu, display_name.c_str(),
                   rssiToMenuColor(group.strongest_rssi), WIFI,
                   [this, group_name, fox_hunt_mode, finder_mode]() {
      if (finder_mode) {
        if (wifi_scan_obj.prepareSSIDFinder(group_name)) {
          display_obj.clearScreen();
          wifi_scan_obj.StartScan(WIFI_SCAN_SSID_FINDER, TFT_CYAN);
        }
        else {
          Serial.println(F("[SSID Finder] No usable APs in selected SSID"));
        }
        return;
      }

      this->buildSSIDAPMenu(group_name, fox_hunt_mode);
      this->changeMenu(&ssidAPMenu, true);
    }, finder_mode ? false
                   : (fox_hunt_mode ? selected_count > 0
                                    : selected_count == ap_count));
  }
}

void MenuFunctions::buildSSIDAPMenu(const String& group_name,
                                    bool fox_hunt_mode) {
  ssidAPMenu.list->clear();
  ssidAPMenu.name = group_name.length() > 0 ? group_name : "Hidden SSID";
  ssidAPMenu.parentMenu = &ssidGroupMenu;

  this->addNodes(&ssidAPMenu, text09, TFTLIGHTGREY, 0,
                 [this, fox_hunt_mode]() {
    this->buildSSIDGroupMenu(fox_hunt_mode);
    this->changeMenu(&ssidGroupMenu, true);
  });

  std::vector<int> group_ap_indices;
  for (int index = 0; index < access_points->size(); index++) {
    if (access_points->get(index).essid == group_name)
      group_ap_indices.push_back(index);
  }
  std::sort(group_ap_indices.begin(), group_ap_indices.end(),
            [](int left_index, int right_index) {
    const AccessPoint left = access_points->get(left_index);
    const AccessPoint right = access_points->get(right_index);
    if (left.rssi != right.rssi)
      return left.rssi > right.rssi;
    return memcmp(left.bssid, right.bssid, sizeof(left.bssid)) < 0;
  });

  bool all_selected = !group_ap_indices.empty();
  int fox_hunt_target_index = -1;
  for (const int index : group_ap_indices) {
    const AccessPoint access_point = access_points->get(index);
    if (!access_point.selected)
      all_selected = false;
    else if (fox_hunt_mode)
      fox_hunt_target_index = index;

    const String display_name = String(access_point.rssi) + " dBm " +
                                macToString(access_point.bssid) +
                                " CH" + String(access_point.channel);
    this->addNodes(&ssidAPMenu, display_name.c_str(),
                   rssiToMenuColor(access_point.rssi), WIFI,
                   [this, group_name, fox_hunt_mode, index]() {
      if (index >= access_points->size() ||
          access_points->get(index).essid != group_name)
        return;

      AccessPoint access_point = access_points->get(index);
      const bool select_target = !access_point.selected;
      if (fox_hunt_mode && select_target) {
        for (int ap_index = 0; ap_index < access_points->size(); ap_index++) {
          AccessPoint candidate = access_points->get(ap_index);
          candidate.selected = false;
          access_points->set(ap_index, candidate);
        }
      }
      access_point.selected = select_target;
      access_points->set(index, access_point);
      this->buildSSIDAPMenu(group_name, fox_hunt_mode);
      this->changeMenu(&ssidAPMenu, true);
    }, access_point.selected);
  }

  if (group_ap_indices.empty()) {
    this->addNodes(&ssidAPMenu, "No APs - scan first", TFTORANGE,
                   DEVICE_INFO, []() {});
    return;
  }

  if (fox_hunt_mode) {
    if (fox_hunt_target_index >= 0) {
      this->addNodes(&ssidAPMenu, "Start Fox Hunt", TFTGREEN, SCANNERS,
                     [this, fox_hunt_target_index]() {
        if (fox_hunt_target_index < 0 ||
            fox_hunt_target_index >= access_points->size())
          return;
        const AccessPoint target = access_points->get(fox_hunt_target_index);
        if (!target.selected)
          return;
        wifi_scan_obj.setFoxHuntTarget(target.bssid, target.essid,
                                      target.rssi, target.channel, false);
        display_obj.clearScreen();
        this->drawStatusBar();
        wifi_scan_obj.StartScan(WIFI_SCAN_SIG_STREN, TFT_CYAN);
      });
    }
    return;
  }

  this->addNodes(&ssidAPMenu, "Select All", TFTGREEN, 255,
                 [this, group_name]() {
    bool whole_group_selected = true;
    bool found_group_ap = false;
    for (int index = 0; index < access_points->size(); index++) {
      const AccessPoint access_point = access_points->get(index);
      if (access_point.essid != group_name)
        continue;
      found_group_ap = true;
      if (!access_point.selected)
        whole_group_selected = false;
    }

    const bool select_group = !found_group_ap || !whole_group_selected;
    for (int index = 0; index < access_points->size(); index++) {
      AccessPoint access_point = access_points->get(index);
      if (access_point.essid == group_name) {
        access_point.selected = select_group;
        access_points->set(index, access_point);
      }
    }

    this->buildSSIDAPMenu(group_name, false);
    this->changeMenu(&ssidAPMenu, true);
  }, all_selected);
}
#endif

void MenuFunctions::buildWiFiFoxHuntMenu() {
  foxHuntMenu.list->clear();
  foxHuntMenu.parentMenu = &wifiSnifferMenu;
  this->addNodes(&foxHuntMenu, text09, TFTLIGHTGREY, 0, [this]() { this->changeMenu(foxHuntMenu.parentMenu, true); });
  this->addNodes(&foxHuntMenu, "APs", TFTLIME, WIFI, [this]() { buildFoxTargetList(FoxHuntListKind::AP_TARGETS); });
  this->addNodes(&foxHuntMenu, "Stations", TFTMAGENTA, WIFI, [this]() { buildFoxTargetList(FoxHuntListKind::APS_WITH_STATIONS); });
  this->addNodes(&foxHuntMenu, "WiFi Pineapples", TFTYELLOW, PINESCAN_SNIFF, [this]() { buildFoxTargetList(FoxHuntListKind::PINEAPPLE_TARGETS); });
  this->addNodes(&foxHuntMenu, "MultiSSID", TFTORANGE, MULTISSID_SNIFF, [this]() { buildFoxTargetList(FoxHuntListKind::MULTISSID_TARGETS); });
  this->changeMenu(&foxHuntMenu, true);
}

void MenuFunctions::buildBluetoothFoxHuntMenu() {
  foxHuntMenu.list->clear();
  foxHuntMenu.parentMenu = &bluetoothSnifferMenu;
  this->addNodes(&foxHuntMenu, text09, TFTLIGHTGREY, 0, [this]() { this->changeMenu(foxHuntMenu.parentMenu, true); });
  this->addNodes(&foxHuntMenu, "BLE Devices", TFTCYAN, BLUETOOTH, [this]() { buildFoxTargetList(FoxHuntListKind::BLE_TARGETS); });
  this->addNodes(&foxHuntMenu, "FindMy", TFTWHITE, BLUETOOTH, [this]() { buildFoxTargetList(FoxHuntListKind::FINDMY_TARGETS); });
  this->addNodes(&foxHuntMenu, "Flipper Zero", TFTORANGE, FLIPPER, [this]() { buildFoxTargetList(FoxHuntListKind::FLIPPER_TARGETS); });
  this->changeMenu(&foxHuntMenu, true);
}

// Function to build the menus
void MenuFunctions::RunSetup()
{
  extern LinkedList<AccessPoint>* access_points;
  extern LinkedList<Station>* stations;
  extern LinkedList<AirTag>* airtags;
  extern LinkedList<IPAddress>* ipList;
  extern LinkedList<ProbeReqSsid>* probe_req_ssids;
  extern LinkedList<ssid>* ssids;
  extern LinkedList<BleDevice>* ble_devices;

  #ifdef HAS_GPS
    #ifdef MARAUDER_MINI_V3
      const bool expose_gps_features = true;
    #else
      const bool expose_gps_features = gps_obj.getGpsModuleStatus();
    #endif
  #endif

  this->disable_touch = false;

  #if defined(MARAUDER_CARDPUTER) || defined(MARAUDER_CARDPUTER_ADV)
    M5CardputerKeyboard.begin();
  #endif
   
  // root menu stuff
  mainMenu.list = new LinkedList<MenuNode>(); // Get list in first menu ready

  // Main menu stuff
  wifiMenu.list = new LinkedList<MenuNode>(); // Get list in second menu ready
#ifdef HAS_BT
  bluetoothMenu.list = new LinkedList<MenuNode>(); // Get list in third menu ready
#endif
  deviceMenu.list = new LinkedList<MenuNode>();
  #ifdef HAS_GPS
    if (expose_gps_features) {
      gpsMenu.list = new LinkedList<MenuNode>();
      gpsInfoMenu.list = new LinkedList<MenuNode>();
    }
  #endif

  // Device menu stuff
  failedUpdateMenu.list = new LinkedList<MenuNode>();
  confirmMenu.list = new LinkedList<MenuNode>();
  updateMenu.list = new LinkedList<MenuNode>();
  settingsMenu.list = new LinkedList<MenuNode>();
  specSettingMenu.list = new LinkedList<MenuNode>();
  infoMenu.list = new LinkedList<MenuNode>();
  // WiFi menu stuff
  wifiSnifferMenu.list = new LinkedList<MenuNode>();
  wifiScannerMenu.list = new LinkedList<MenuNode>();
  wifiAttackMenu.list = new LinkedList<MenuNode>();
  /*#ifdef HAS_GPS
    wardrivingMenu.list = new LinkedList<MenuNode>();
  #endif*/
  wifiGeneralMenu.list = new LinkedList<MenuNode>();
  wifiAPMenu.list = new LinkedList<MenuNode>();
  ssidGroupMenu.list = new LinkedList<MenuNode>();
  ssidAPMenu.list = new LinkedList<MenuNode>();
  wifiIPMenu.list = new LinkedList<MenuNode>();
  apInfoMenu.list = new LinkedList<MenuNode>();
  setMacMenu.list = new LinkedList<MenuNode>();
  genAPMacMenu.list = new LinkedList<MenuNode>();
  wifiStationMenu.list = new LinkedList<MenuNode>();
  foxSortMenu.list = new LinkedList<MenuNode>();
  foxFilterMenu.list = new LinkedList<MenuNode>();
  selectProbeSSIDsMenu.list = new LinkedList<MenuNode>();

  // WiFi HTML menu stuff
  htmlMenu.list = new LinkedList<MenuNode>();
  miniKbMenu.list = new LinkedList<MenuNode>();
  #ifdef HAS_SD
    sdDeleteMenu.list = new LinkedList<MenuNode>();
  #endif

  // Bluetooth menu stuff
  bluetoothSnifferMenu.list = new LinkedList<MenuNode>();
  bluetoothAttackMenu.list = new LinkedList<MenuNode>();
  bleSecurityMenu.list = new LinkedList<MenuNode>();
  bleTargetMenu.list = new LinkedList<MenuNode>();
  bleConfirmMenu.list = new LinkedList<MenuNode>();

  // Settings stuff
  generateSSIDsMenu.list = new LinkedList<MenuNode>();
  clearSSIDsMenu.list = new LinkedList<MenuNode>();
  clearAPsMenu.list = new LinkedList<MenuNode>();
  saveFileMenu.list = new LinkedList<MenuNode>();

  #ifdef HAS_DIRECT_UPLOAD
    uploadLogsMenu.list = new LinkedList<MenuNode>();
    uploadAllMenu.list = new LinkedList<MenuNode>();
    deleteAllMenu.list = new LinkedList<MenuNode>();
    actionMenu.list = new LinkedList<MenuNode>();
  #endif

  saveSSIDsMenu.list = new LinkedList<MenuNode>();
  loadSSIDsMenu.list = new LinkedList<MenuNode>();
  saveAPsMenu.list = new LinkedList<MenuNode>();
  loadAPsMenu.list = new LinkedList<MenuNode>();
  saveATsMenu.list = new LinkedList<MenuNode>();
  loadATsMenu.list = new LinkedList<MenuNode>();

  evilPortalMenu.list = new LinkedList<MenuNode>();
  ssidsMenu.list = new LinkedList<MenuNode>();

  #ifdef HAS_GPS
    gpsPOIMenu.list = new LinkedList<MenuNode>();
  #endif

  foxHuntMenu.list = new LinkedList<MenuNode>();

  // Work menu names
  mainMenu.name = text_table1[6];
  wifiMenu.name = text_table1[7];
  deviceMenu.name = text_table1[9];
  failedUpdateMenu.name = text_table1[11];
  confirmMenu.name = text_table1[13];
  updateMenu.name = text_table1[15];
  infoMenu.name = text_table1[17];
  settingsMenu.name = text_table1[18];
  bluetoothMenu.name = text_table1[19];
  wifiSnifferMenu.name = text_table1[20];
  wifiScannerMenu.name = "Scanners";
  wifiAttackMenu.name = text_table1[21];
  wifiGeneralMenu.name = text_table1[22];
  ssidGroupMenu.name = "Select SSIDs";
  ssidAPMenu.name = "Access Points";
  saveFileMenu.name = "Save/Load Files";
  saveSSIDsMenu.name = "Save SSIDs";
  loadSSIDsMenu.name = "Load SSIDs";
  saveAPsMenu.name = "Save APs";
  loadAPsMenu.name = "Load APs";
  saveATsMenu.name = "Save Airtags";
  loadATsMenu.name = "Load Airtags";

  bluetoothSnifferMenu.name = text_table1[23];
  bluetoothAttackMenu.name = "Bluetooth Attacks";
  bleSecurityMenu.name = "BLE Discovery";
  bleTargetMenu.name = "BLE Targets";
  bleConfirmMenu.name = "Confirm BLE Action";
  generateSSIDsMenu.name = text_table1[27];
  clearSSIDsMenu.name = text_table1[28];
  clearAPsMenu.name = text_table1[29];
  wifiAPMenu.name = "Select";
  wifiIPMenu.name = "Active IPs";
  apInfoMenu.name = "AP Info";
  setMacMenu.name = "Set MACs";
  genAPMacMenu.name = "Generate AP MAC";
  wifiStationMenu.name = "Select Stations";

  #ifdef HAS_DIRECT_UPLOAD
    uploadLogsMenu.name = "Upload Logs";
    uploadAllMenu.name = "Upload All?";
    deleteAllMenu.name = "Delete All?";
    actionMenu.name = "Destination";
  #endif

  #ifdef HAS_GPS
    gpsMenu.name = "GPS"; 
    gpsInfoMenu.name = "GPS Data";
    //wardrivingMenu.name = "Wardriving";
  #endif  
  htmlMenu.name = "EP HTML List";
  miniKbMenu.name = "Mini Keyboard";

  #ifdef HAS_SD
    sdDeleteMenu.name = "Delete SD Files";
  #endif

  selectProbeSSIDsMenu.name = "Probe Requests";
  evilPortalMenu.name = "Evil Portal";
  ssidsMenu.name = "SSIDs";

  #ifdef HAS_GPS
    gpsPOIMenu.name = "GPS POI";
  #endif

  foxHuntMenu.name = "Fox Hunt";

  // Build Main Menu
  mainMenu.parentMenu = NULL;
  this->addNodes(&mainMenu, text_table1[7], TFTGREEN, WIFI, [this]() {
    this->changeMenu(&wifiMenu, true);
  });
  #ifdef HAS_BT
    this->addNodes(&mainMenu, text_table1[19], TFTCYAN, BLUETOOTH, [this]() {
      this->changeMenu(&bluetoothMenu, true);
    });
  #endif
  #ifdef HAS_GPS
	if (expose_gps_features) {
    	this->addNodes(&mainMenu, text1_66, TFTRED, GPS_MENU, [this]() {
      	this->changeMenu(&gpsMenu, true);
    	});
	}
  #endif
  this->addNodes(&mainMenu, text_table1[9], TFTBLUE, DEVICE, [this]() {
    this->changeMenu(&deviceMenu, true);
  });
  this->addNodes(&mainMenu, text_table1[30], TFTLIGHTGREY, REBOOT, []() {
    ESP.restart();
  });

  // Build WiFi Menu
  wifiMenu.parentMenu = &mainMenu; // Main Menu is second menu parent
  this->addNodes(&wifiMenu, text09, TFTLIGHTGREY, 0, [this]() {
    this->changeMenu(wifiMenu.parentMenu, true);
  });
  this->addNodes(&wifiMenu, text_table1[31], TFTYELLOW, SNIFFERS, [this]() {
    this->changeMenu(&wifiSnifferMenu, true);
  });
  this->addNodes(&wifiMenu, "Scanners", TFTORANGE, SCANNERS, [this]() {
    this->changeMenu(&wifiScannerMenu, true);
  });
  /*#ifdef HAS_GPS
    this->addNodes(&wifiMenu, "Wardriving", TFTGREEN, NULL, BEACON_SNIFF, [this]() {
      this->changeMenu(&wardrivingMenu, true);
    });
  #endif*/
  this->addNodes(&wifiMenu, text_table1[32], TFTRED, ATTACKS, [this]() {
    this->changeMenu(&wifiAttackMenu, true);
  });
  this->addNodes(&wifiMenu, text_table1[33], TFTPURPLE, GENERAL_APPS, [this]() {
    this->changeMenu(&wifiGeneralMenu, true);
  });

  // Build WiFi scanner Menu
  wifiScannerMenu.parentMenu = &wifiMenu; // Main Menu is second menu parent
  this->addNodes(&wifiScannerMenu, text09, TFTLIGHTGREY, 0, [this]() {
    this->changeMenu(wifiScannerMenu.parentMenu, true);
  });
  this->addNodes(&wifiScannerMenu, "Ping Scan", TFTGREEN, SCANNERS, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(WIFI_PING_SCAN, TFT_CYAN);
  });
  #ifndef HAS_DUAL_BAND
    this->addNodes(&wifiScannerMenu, "ARP Scan", TFTCYAN, SCANNERS, [this]() {
      display_obj.clearScreen();
      this->drawStatusBar();
      wifi_scan_obj.StartScan(WIFI_ARP_SCAN, TFT_CYAN);
    });
  #endif
  this->addNodes(&wifiScannerMenu, "Port Scan All", TFTMAGENTA, BEACON_LIST, [this](){
    // Add the back button
    wifiIPMenu.list->clear();
      this->addNodes(&wifiIPMenu, text09, TFTLIGHTGREY, 0, [this]() {
      this->changeMenu(wifiIPMenu.parentMenu, true);
    });

    // Populate the menu with buttons
    for (int i = 0; i < ipList->size(); i++) {
      // This is the menu node
      this->addNodes(&wifiIPMenu, ipList->get(i).toString().c_str(), TFTBLUE, 255, [this, i](){
        Serial.println("Selected: " + ipList->get(i).toString());
        wifi_scan_obj.current_scan_ip = ipList->get(i);
        display_obj.clearScreen();
        this->drawStatusBar();
        wifi_scan_obj.StartScan(WIFI_PORT_SCAN_ALL, TFT_BLUE);
      });
    }
    this->changeMenu(&wifiIPMenu, true);
  });
  this->addNodes(&wifiScannerMenu, "SSH Scan", TFTORANGE, SCANNERS, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(WIFI_SCAN_SSH, TFT_CYAN);
  });
  this->addNodes(&wifiScannerMenu, "Telnet Scan", TFTRED, SCANNERS, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(WIFI_SCAN_TELNET, TFT_CYAN);
  });
  this->addNodes(&wifiScannerMenu, "SMTP Scan", TFTWHITE, SCANNERS, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(WIFI_SCAN_SMTP, TFT_CYAN);
  });
  this->addNodes(&wifiScannerMenu, "DNS Scan", TFTLIME, SCANNERS, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(WIFI_SCAN_DNS, TFT_CYAN);
  });
  this->addNodes(&wifiScannerMenu, "HTTP Scan", TFTSKYBLUE, SCANNERS, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(WIFI_SCAN_HTTP, TFT_CYAN);
  });
  this->addNodes(&wifiScannerMenu, "HTTPS Scan", TFTYELLOW, SCANNERS, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(WIFI_SCAN_HTTPS, TFT_CYAN);
  });
  this->addNodes(&wifiScannerMenu, "RDP Scan", TFTPURPLE, SCANNERS, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(WIFI_SCAN_RDP, TFT_CYAN);
  });

  // Build WiFi sniffer Menu
  wifiSnifferMenu.parentMenu = &wifiMenu; // Main Menu is second menu parent
  this->addNodes(&wifiSnifferMenu, text09, TFTLIGHTGREY, 0, [this]() {
    this->changeMenu(wifiSnifferMenu.parentMenu, true);
  });
  #ifdef MARAUDER_MINI_V3
    this->addNodes(&wifiSnifferMenu, "Scan SSIDs", TFTGREEN, BEACON_SNIFF,
                   [this]() {
      wifi_scan_obj.prepareSSIDGroupScan();
      display_obj.clearScreen();
      this->drawStatusBar();
      wifi_scan_obj.StartScan(WIFI_SCAN_AP_STA, TFT_GREEN);
    });
    this->addNodes(&wifiSnifferMenu, "Select SSIDs", TFTGREEN,
                   KEYBOARD_ICO, [this]() {
      this->buildSSIDGroupMenu();
      this->changeMenu(&ssidGroupMenu, true);
    });
    this->addNodes(&wifiSnifferMenu, "SSID Finder", TFTCYAN,
                   SCANNERS, [this]() {
      this->buildSSIDGroupMenu(false, true);
      this->changeMenu(&ssidGroupMenu, true);
    });
  #endif
  this->addNodes(&wifiSnifferMenu, text_table1[42], TFTCYAN, PROBE_SNIFF, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(WIFI_SCAN_PROBE, TFT_CYAN);
  });
  this->addNodes(&wifiSnifferMenu, text_table1[43], TFTMAGENTA, BEACON_SNIFF, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(WIFI_SCAN_AP, TFT_MAGENTA);
  });
  this->addNodes(&wifiSnifferMenu, text_table1[44], TFTRED, DEAUTH_SNIFF, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(WIFI_SCAN_DEAUTH, TFT_RED);
  });
  this->addNodes(&wifiSnifferMenu, "Packet Count", TFTORANGE, PACKET_MONITOR, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(WIFI_SCAN_PACKET_RATE, TFT_ORANGE);
    wifi_scan_obj.renderPacketRate();
  });
  #ifdef HAS_ILI9341
    this->addNodes(&wifiSnifferMenu, text_table1[46], TFTVIOLET, EAPOL, [this]() {
      display_obj.clearScreen();
      this->drawStatusBar();
      wifi_scan_obj.StartScan(WIFI_SCAN_EAPOL, TFT_VIOLET);
    });
    this->addNodes(&wifiSnifferMenu, text_table1[45], TFTBLUE, PACKET_MONITOR, [this]() {
      wifi_scan_obj.StartScan(WIFI_PACKET_MONITOR, TFT_BLUE);
    });
  #else // No touch
    this->addNodes(&wifiSnifferMenu, text_table1[46], TFTVIOLET, EAPOL, [this]() {
      display_obj.clearScreen();
      this->drawStatusBar();
      wifi_scan_obj.StartScan(WIFI_SCAN_EAPOL, TFT_VIOLET);
    });
    this->addNodes(&wifiSnifferMenu, text_table1[45], TFTBLUE, PACKET_MONITOR, [this]() {
      display_obj.clearScreen();
      this->drawStatusBar();
      wifi_scan_obj.StartScan(WIFI_PACKET_MONITOR, TFT_BLUE);
    });
  #endif
  this->addNodes(&wifiSnifferMenu, "Channel Analyzer", TFTCYAN, PACKET_MONITOR, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    this->renderGraphUI(WIFI_SCAN_CHAN_ANALYZER);
    wifi_scan_obj.StartScan(WIFI_SCAN_CHAN_ANALYZER, TFT_CYAN);
  });
  this->addNodes(&wifiSnifferMenu, "Channel Summary", TFTORANGE, PACKET_MONITOR, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    this->renderGraphUI(WIFI_SCAN_CHAN_ACT);
    wifi_scan_obj.StartScan(WIFI_SCAN_CHAN_ACT, TFT_CYAN);
  });

  this->addNodes(&wifiSnifferMenu, text_table1[58], TFTWHITE, PACKET_MONITOR, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(WIFI_SCAN_RAW_CAPTURE, TFT_WHITE);
  });

  this->addNodes(&wifiSnifferMenu, text_table1[47], TFTRED, PWNAGOTCHI, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(WIFI_SCAN_PWN, TFT_RED);
  });
  
  this->addNodes(&wifiSnifferMenu, text_table1[63], TFTYELLOW, PINESCAN_SNIFF, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(WIFI_SCAN_PINESCAN, TFT_YELLOW);
  });

  this->addNodes(&wifiSnifferMenu, text_table1[64], TFTORANGE, MULTISSID_SNIFF, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(WIFI_SCAN_MULTISSID, TFT_ORANGE);
  });
  this->addNodes(&wifiSnifferMenu, "Scan AP/STA", TFTLIME, BEACON_SNIFF, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(WIFI_SCAN_AP_STA, 0x97e0);
  });
  /*this->addNodes(&wifiSnifferMenu, "Fox Hunt", TFTCYAN, PACKET_MONITOR, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(WIFI_SCAN_SIG_STREN, TFT_CYAN);
  });*/
  this->addNodes(&wifiSnifferMenu, "Fox Hunt", TFTCYAN, SCANNERS, [this]() {
    #ifdef MARAUDER_MINI_V3
      for (int index = 0; index < access_points->size(); index++) {
        AccessPoint access_point = access_points->get(index);
        access_point.selected = false;
        access_points->set(index, access_point);
      }
      this->buildSSIDGroupMenu(true);
      this->changeMenu(&ssidGroupMenu, true);
    #else
      this->buildWiFiFoxHuntMenu();
    #endif
  });
  this->addNodes(&wifiSnifferMenu, "MAC Monitor", TFTMAGENTA, SCANNERS, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(WIFI_SCAN_DETECT_FOLLOW, TFT_MAGENTA);
  });
  #ifdef MARAUDER_MINI_V3
    this->addNodes(&wifiSnifferMenu, "Device Scout", TFTGREEN, SCANNERS,
                   [this]() {
      WirelessDeviceScout::run();
      this->changeMenu(&wifiSnifferMenu, true);
    });
    this->addNodes(&wifiSnifferMenu, "Activity Scanner", TFTCYAN,
                   PACKET_MONITOR, [this]() {
      WirelessActivityTools::runScanner();
      this->changeMenu(&wifiSnifferMenu, true);
    });
    this->addNodes(&wifiSnifferMenu, "Jam Detector", TFTRED, SCANNERS,
                   [this]() {
      WirelessActivityTools::runJamDetector();
      this->changeMenu(&wifiSnifferMenu, true);
    });
    this->addNodes(&wifiSnifferMenu, "Camera Detect", TFTCYAN, SCANNERS,
                   [this]() {
      WiFiCameraDetector::run();
      this->changeMenu(&wifiSnifferMenu, true);
    });
    this->addNodes(&wifiSnifferMenu, "Drone Remote ID", TFTORANGE,
                   SCANNERS, [this]() {
      DroneRemoteID::run();
      this->changeMenu(&wifiSnifferMenu, true);
    });
    this->addNodes(&wifiSnifferMenu, "Detect Flock", TFTORANGE, FLOCK,
                   [this]() {
      WiFiFlockDetector::run();
      this->changeMenu(&wifiSnifferMenu, true);
    });
  #endif
  this->addNodes(&wifiSnifferMenu, "SAE Commit", TFTLIME, EAPOL, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(WIFI_SCAN_SAE_COMMIT, TFT_GREEN);
  });

  // Build Wardriving menu
  #ifdef HAS_GPS
    /*wardrivingMenu.parentMenu = &wifiMenu; // Main Menu is second menu parent
    this->addNodes(&wardrivingMenu, text09, TFTLIGHTGREY, NULL, 0, [this]() {
      this->changeMenu(wardrivingMenu.parentMenu, true);
    });*/
    if (expose_gps_features) {
      this->addNodes(&wifiSnifferMenu, "Wardrive", TFTGREEN, BEACON_SNIFF, [this]() {
        display_obj.clearScreen();
        this->drawStatusBar();
        wifi_scan_obj.StartScan(WIFI_SCAN_WAR_DRIVE, TFT_GREEN);
      });
    }
  #endif
  /*#ifdef HAS_GPS
    if (gps_obj.getGpsModuleStatus()) {
      this->addNodes(&wardrivingMenu, "Station Wardrive", TFTORANGE, NULL, PROBE_SNIFF, [this]() {
        display_obj.clearScreen();
        this->drawStatusBar();
        wifi_scan_obj.StartScan(WIFI_SCAN_STATION_WAR_DRIVE, TFT_ORANGE);
      });
    }
  #endif*/

  // Build WiFi attack menu
  wifiAttackMenu.parentMenu = &wifiMenu; // Main Menu is second menu parent
  this->addNodes(&wifiAttackMenu, text09, TFTLIGHTGREY, 0, [this]() {
    this->changeMenu(wifiAttackMenu.parentMenu, true);
  });
  this->addNodes(&wifiAttackMenu, text_table1[50], TFTRED, BEACON_LIST, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(WIFI_ATTACK_BEACON_LIST, TFT_RED);
  });
  this->addNodes(&wifiAttackMenu, text_table1[51], TFTORANGE, BEACON_SPAM, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(WIFI_ATTACK_BEACON_SPAM, TFT_ORANGE);
  });
  this->addNodes(&wifiAttackMenu, text1_67, TFTCYAN, FUNNY_BEACON, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(WIFI_ATTACK_FUNNY_BEACON, TFT_CYAN);
  });
  this->addNodes(&wifiAttackMenu, text_table1[52], TFTYELLOW, RICK_ROLL, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(WIFI_ATTACK_RICK_ROLL, TFT_YELLOW);
  });
  this->addNodes(&wifiAttackMenu, text_table1[53], TFTRED, PROBE_SNIFF, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(WIFI_ATTACK_AUTH, TFT_RED);
  });
  this->addNodes(&wifiAttackMenu, "Evil Portal", TFTORANGE, BEACON_SNIFF, [this]() {

    wifiAPMenu.list->clear();
    ssidsMenu.list->clear();

    wifiAPMenu.parentMenu = &evilPortalMenu;
    ssidsMenu.parentMenu = &evilPortalMenu;

    this->addNodes(&wifiAPMenu, text09, TFTLIGHTGREY, 0, [this]() {
      this->changeMenu(wifiAPMenu.parentMenu, true);
    });
    this->addNodes(&ssidsMenu, text09, TFTLIGHTGREY, 0, [this]() {
      this->changeMenu(ssidsMenu.parentMenu, true);
    });

    // Get AP list ready
    for (int i = 0; i < access_points->size(); i++) {
      // This is the menu node
      this->addNodes(&wifiAPMenu, access_points->get(i).essid.c_str(), TFTCYAN, 255, [this, i](){
        if (evil_portal_obj.setAP(access_points->get(i).essid)) {
          AccessPoint new_ap = access_points->get(i);
          new_ap.selected = true;
          access_points->set(i, new_ap);

          evil_portal_obj.ap_index = i;

          display_obj.clearScreen();
          this->drawStatusBar();
          wifi_scan_obj.StartScan(WIFI_SCAN_EVIL_PORTAL, TFT_ORANGE);
          wifi_scan_obj.setMac();
        }
        else
          this->changeMenu(&evilPortalMenu, true);
      });
    }

    for (int i = 0; i < ssids->size(); i++) {
      // This is the menu node
      this->addNodes(&ssidsMenu, ssids->get(i).essid.c_str(), TFTCYAN, 255, [this, i](){
        if (evil_portal_obj.setAP(ssids->get(i).essid)) {
          display_obj.clearScreen();
          this->drawStatusBar();
          wifi_scan_obj.StartScan(WIFI_SCAN_EVIL_PORTAL, TFT_ORANGE);
          wifi_scan_obj.setMac();
        }
        else
          this->changeMenu(&evilPortalMenu, true);
      });
    }
    this->changeMenu(&evilPortalMenu, true);
  });
  this->addNodes(&wifiAttackMenu, text_table1[54], TFTRED, DEAUTH_SNIFF, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(WIFI_ATTACK_DEAUTH, TFT_RED);
  });
  this->addNodes(&wifiAttackMenu, text_table1[57], TFTMAGENTA, BEACON_LIST, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(WIFI_ATTACK_AP_SPAM, TFT_MAGENTA);
  });
  this->addNodes(&wifiAttackMenu, text_table1[62], TFTRED, DEAUTH_SNIFF, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(WIFI_ATTACK_DEAUTH_TARGETED, TFT_ORANGE);
  });
  #ifdef MARAUDER_MINI_V3
    this->addNodes(&wifiAttackMenu, "SSID Beacon Clone", TFTMAGENTA,
                   BEACON_LIST, [this]() {
      display_obj.clearScreen();
      this->drawStatusBar();
      wifi_scan_obj.StartScan(WIFI_ATTACK_SSID_GROUP_CLONE, TFT_MAGENTA);
    });
    this->addNodes(&wifiAttackMenu, "SSID Group Deauth", TFTRED,
                   DEAUTH_SNIFF, [this]() {
      display_obj.clearScreen();
      this->drawStatusBar();
      wifi_scan_obj.StartScan(WIFI_ATTACK_DEAUTH, TFT_RED);
    });
    this->addNodes(&wifiAttackMenu, "Camera Deauther", TFTRED,
                   DEAUTH_SNIFF, [this]() {
      static WiFiCameraDetector::DeauthTarget target{};
      if (!WiFiCameraDetector::selectDeauthTarget(target)) {
        display_obj.init();
        this->changeMenu(&wifiAttackMenu, true);
        return;
      }
      wifi_scan_obj.setCameraDeauthTargets(target);
      display_obj.clearScreen();
      this->drawStatusBar();
      wifi_scan_obj.StartScan(WIFI_ATTACK_CAMERA_DEAUTH, TFT_RED);
    });
    this->addNodes(&wifiAttackMenu, "Drone Spoof", TFTRED, ATTACKS,
                   [this]() {
      static DroneRemoteID::CapturedDrone target{};
      if (!DroneRemoteID::selectCapturedForSpoof(target) ||
          !DroneRemoteIDSpoofer::selectTarget(target)) {
        display_obj.init();
        this->changeMenu(&wifiAttackMenu, true);
        return;
      }
      bleConfirmMenu.list->clear();
      bleConfirmMenu.name = "Authorize Drone Spoof";
      bleConfirmMenu.parentMenu = &wifiAttackMenu;
      this->addNodes(&bleConfirmMenu, "Cancel", TFTLIGHTGREY, 0,
                     [this]() {
        this->changeMenu(bleConfirmMenu.parentMenu, true);
      });
      this->addNodes(&bleConfirmMenu, "Run (Authorized)", TFTRED,
                     ATTACKS, [this]() {
        DroneRemoteIDSpoofer::run();
        display_obj.init();
        this->changeMenu(&wifiAttackMenu, true);
      });
      this->changeMenu(&bleConfirmMenu, true);
    });
  #endif

  this->addNodes(&wifiAttackMenu, "Karma", TFTORANGE, KEYBOARD_ICO, [this](){
    // Add the back button
    selectProbeSSIDsMenu.list->clear();
    this->addNodes(&selectProbeSSIDsMenu, text09, TFTLIGHTGREY, 0, [this]() {
      this->changeMenu(&wifiAttackMenu, true);
    });

    // Populate the menu with buttons
    for (int i = 0; i < probe_req_ssids->size(); i++) {
      // This is the menu node
      this->addNodes(&selectProbeSSIDsMenu, probe_req_ssids->get(i).essid.c_str(), TFTCYAN, 255, [this, i](){
        if (evil_portal_obj.setAP(probe_req_ssids->get(i).essid)) {
          display_obj.clearScreen();
          this->drawStatusBar();
          wifi_scan_obj.StartScan(WIFI_SCAN_EVIL_PORTAL, TFT_ORANGE);
          wifi_scan_obj.setMac();
        }
        else
          this->changeMenu(&wifiAttackMenu, true);
      });
    }
    this->changeMenu(&selectProbeSSIDsMenu, true);
  });

  this->addNodes(&wifiAttackMenu, "Bad Msg", TFTRED, DEAUTH_SNIFF, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(WIFI_ATTACK_BAD_MSG, TFT_RED);
  });
  this->addNodes(&wifiAttackMenu, "Bad Msg Targeted", TFTYELLOW, DEAUTH_SNIFF, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(WIFI_ATTACK_BAD_MSG_TARGETED, TFT_YELLOW);
  });
  this->addNodes(&wifiAttackMenu, "Assoc Sleep", TFTRED, DEAUTH_SNIFF, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(WIFI_ATTACK_SLEEP, TFT_RED);
  });
  this->addNodes(&wifiAttackMenu, "Assoc Sleep Targ", TFTMAGENTA, DEAUTH_SNIFF, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(WIFI_ATTACK_SLEEP_TARGETED, TFT_MAGENTA);
  });
  this->addNodes(&wifiAttackMenu, "SAE Commit Flood", TFTLIME, EAPOL, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(WIFI_ATTACK_SAE_COMMIT, TFT_GREEN);
  });
  this->addNodes(&wifiAttackMenu, "Channel Switch", TFTORANGE, BEACON_LIST, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(WIFI_ATTACK_CSA, TFT_GREEN);
  });
  this->addNodes(&wifiAttackMenu, "Quiet Time", TFTRED, BEACON_LIST, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(WIFI_ATTACK_QUIET, TFT_GREEN);
  });

  evilPortalMenu.parentMenu = &wifiAttackMenu;
  this->addNodes(&evilPortalMenu, text09, TFTLIGHTGREY, 0, [this]() {
    this->changeMenu(evilPortalMenu.parentMenu, true);
  });
  this->addNodes(&evilPortalMenu, "Access Points", TFTGREEN, BEACON_SNIFF, [this]() {
    this->changeMenu(&wifiAPMenu, true);
  });
  this->addNodes(&evilPortalMenu, "User SSIDs", TFTCYAN, PROBE_SNIFF, [this]() {
    this->changeMenu(&ssidsMenu, true);
  });

  // Build WiFi General menu
  wifiGeneralMenu.parentMenu = &wifiMenu;
  this->addNodes(&wifiGeneralMenu, text09, TFTLIGHTGREY, 0, [this]() {
    this->changeMenu(wifiGeneralMenu.parentMenu, true);
  });
  this->addNodes(&wifiGeneralMenu, text_table1[27], TFTSKYBLUE, GENERATE, [this]() {
    this->changeMenu(&generateSSIDsMenu, true);
    wifi_scan_obj.RunGenerateSSIDs();
  });

	//Add Select probe ssid
  this->addNodes(&wifiGeneralMenu, text_table1[65], TFTCYAN, KEYBOARD_ICO, [this]() {
    selectProbeSSIDsMenu.list->clear();

    // Add the back button
    this->addNodes(&selectProbeSSIDsMenu, text09, TFTLIGHTGREY, 0, [this]() {
      this->changeMenu(&wifiGeneralMenu, true);

      // TODO: TBD - Should probe_req_ssids have it´s own life and override ap.config and/or ssids -list for EP?
      // If so, then we should not add selected ssids to ssids list

      // Add selected ssid names to ssids list when clicking back button
      if (probe_req_ssids->size() > 0) {

        //TODO: TBD - Clear ssids list before adding new ones??

        for (int i = 0; i < probe_req_ssids->size(); i++) {
          ProbeReqSsid cur_probe_ssid = probe_req_ssids->get(i);
          if (cur_probe_ssid.selected) {
            bool ssidExists = false;
            for (int i = 0; i < ssids->size(); i++) {
              if (ssids->get(i).essid == cur_probe_ssid.essid) {
                ssidExists = true;
                break;
              }
            }
            if (!ssidExists) {
              wifi_scan_obj.addSSID(cur_probe_ssid.essid);
            }
          }
        }
      }
    });

    // Populate the menu with buttons
    for (int i = 0; i < probe_req_ssids->size(); i++) {
      ProbeReqSsid cur_ssid = probe_req_ssids->get(i);
      // This is the menu node
      String button_name = "[" + String(cur_ssid.requests) + "]" + cur_ssid.essid;
      this->addNodes(
        &selectProbeSSIDsMenu,
        button_name.c_str(),
        TFTCYAN,
        255,
        [this, i]() {
          ProbeReqSsid new_ssid = probe_req_ssids->get(i);
          new_ssid.selected = !probe_req_ssids->get(i).selected;

          // Change selection status of menu node
          MenuNode new_node = current_menu->list->get(i + 1);
          new_node.selected = !current_menu->list->get(i + 1).selected;
          current_menu->list->set(i + 1, new_node);

          probe_req_ssids->set(i, new_ssid);
        },
        probe_req_ssids->get(i).selected);
    }
    this->changeMenu(&selectProbeSSIDsMenu, true);
  });

  clearSSIDsMenu.parentMenu = &wifiGeneralMenu;

  #ifdef HAS_ILI9341
    this->addNodes(&wifiGeneralMenu, text_table1[1], TFTNAVY, KEYBOARD_ICO, [this](){
      char ssidBuf[64] = {0};
      bool keep_going = true;
      while (keep_going) {
        display_obj.clearScreen(); 
        if (keyboardInput(ssidBuf, sizeof(ssidBuf), "Enter SSID")) {
          if (ssidBuf[0] != 0)
            wifi_scan_obj.addSSID(String(ssidBuf));
          for (int i = 0; i < 64; i++)
            ssidBuf[i] = NULL;
        }
        else
          keep_going = false;
      }

      this->changeMenu(current_menu);
    });
  #endif
  #if (!defined(HAS_ILI9341) && defined(HAS_BUTTONS))
    this->addNodes(&wifiGeneralMenu, text_table1[1], TFTNAVY, KEYBOARD_ICO, [this](){
      this->changeMenu(&miniKbMenu, true);
      #ifdef HAS_MINI_KB
        this->miniKeyboard(&miniKbMenu);
      #endif
    });
  #endif
  this->addNodes(&wifiGeneralMenu, text_table1[28], TFTSILVER, CLEAR_ICO, [this]() {
    this->changeMenu(&clearSSIDsMenu, true);
    wifi_scan_obj.RunClearSSIDs();
  });
  this->addNodes(&wifiGeneralMenu, text_table1[29], TFTDARKGREY, CLEAR_ICO, [this]() {
    this->changeMenu(&clearAPsMenu, true);
    wifi_scan_obj.RunClearAPs();
  });
  this->addNodes(&wifiGeneralMenu, text_table1[60], TFTBLUE, CLEAR_ICO, [this]() {
    this->changeMenu(&clearAPsMenu, true);
    wifi_scan_obj.RunClearStations();
  });
  //#else // Mini EP HTML select
    this->addNodes(&wifiGeneralMenu, "Select EP HTML File", TFTCYAN, KEYBOARD_ICO, [this](){
      // Add the back button
      htmlMenu.list->clear();
        this->addNodes(&htmlMenu, text09, TFTLIGHTGREY, 0, [this]() {
        this->changeMenu(htmlMenu.parentMenu, true);
      });

      // Populate the menu with buttons
      for (int i = 0; i < evil_portal_obj.html_files->size(); i++) {
        // This is the menu node
        this->addNodes(&htmlMenu, evil_portal_obj.html_files->get(i).c_str(), TFTCYAN, 255, [this, i](){
          evil_portal_obj.selected_html_index = i;
          evil_portal_obj.target_html_name = evil_portal_obj.html_files->get(evil_portal_obj.selected_html_index);
          Serial.println("Set Evil Portal HTML as " + evil_portal_obj.target_html_name);
          evil_portal_obj.using_serial_html = false;
          this->changeMenu(htmlMenu.parentMenu, true);
          return;
        });
      }
      this->changeMenu(&htmlMenu, true);
    });

    //#if (!defined(HAS_ILI9341) && defined(HAS_BUTTONS))
      miniKbMenu.parentMenu = &wifiGeneralMenu;
      #if !defined(MARAUDER_CARDPUTER) && !defined(MARAUDER_CARDPUTER_ADV)
        this->addNodes(&miniKbMenu, "a", TFTCYAN, 0, [this]() {
          this->changeMenu(miniKbMenu.parentMenu, true);
        });
      #endif
    //#endif

    htmlMenu.parentMenu = &wifiGeneralMenu;
    this->addNodes(&htmlMenu, text09, TFTLIGHTGREY, 0, [this]() {
      this->changeMenu(htmlMenu.parentMenu, true);
    });

    // Select APs on Mini
    this->addNodes(&wifiGeneralMenu, "Select APs", TFTNAVY, KEYBOARD_ICO, [this](){
      wifiAPMenu.parentMenu = &wifiGeneralMenu;
      // Add the back button
      wifiAPMenu.list->clear();
        this->addNodes(&wifiAPMenu, text09, TFTLIGHTGREY, 0, [this]() {
        this->changeMenu(wifiAPMenu.parentMenu, true);
      });

      this->addNodes(&wifiAPMenu, "Select ALL", TFTGREEN, 255, [this](){

        for (int x = 0; x < access_points->size(); x++) {
          AccessPoint new_ap = access_points->get(x);
          new_ap.selected = !access_points->get(x).selected;
          access_points->set(x, new_ap);

          MenuNode new_node = current_menu->list->get(x + 2);
          new_node.selected = !current_menu->list->get(x + 2).selected;
          current_menu->list->set(x + 2, new_node);
        }

        this->changeMenu(current_menu, true);

      });

      // Populate the menu with buttons
      for (int i = 0; i < access_points->size(); i++) {
        // This is the menu node
        this->addNodes(&wifiAPMenu, access_points->get(i).essid.c_str(), TFTCYAN, 255, [this, i](){
        AccessPoint new_ap = access_points->get(i);
        new_ap.selected = !access_points->get(i).selected;

        // Change selection status of menu node
        MenuNode new_node = current_menu->list->get(i + 2);
        new_node.selected = !current_menu->list->get(i + 2).selected;
        current_menu->list->set(i + 2, new_node);

        access_points->set(i, new_ap);
        }, access_points->get(i).selected);
      }
      this->changeMenu(&wifiAPMenu, true);
    });

    this->addNodes(&wifiGeneralMenu, "View AP Info", TFTCYAN, KEYBOARD_ICO, [this](){
      wifiAPMenu.parentMenu = &wifiGeneralMenu;
      
      // Add the back button
      wifiAPMenu.list->clear();
        this->addNodes(&wifiAPMenu, text09, TFTLIGHTGREY, 0, [this]() {
        this->changeMenu(wifiAPMenu.parentMenu, true);
      });

      // Populate the menu with buttons
      for (int i = 0; i < access_points->size(); i++) {
        // This is the menu node
        this->addNodes(&wifiAPMenu, access_points->get(i).essid.c_str(), TFTCYAN, 255, [this, i](){
          this->changeMenu(&apInfoMenu, true);
          wifi_scan_obj.RunAPInfo(i);
        });
      }
      this->changeMenu(&wifiAPMenu, true);
    });

    apInfoMenu.parentMenu = &wifiAPMenu;
    this->addNodes(&apInfoMenu, text09, TFTLIGHTGREY, 0, [this]() {
      this->changeMenu(apInfoMenu.parentMenu, true);
    });

    wifiAPMenu.parentMenu = &wifiGeneralMenu;
    this->addNodes(&wifiAPMenu, text09, TFTLIGHTGREY, 0, [this]() {
      this->changeMenu(wifiAPMenu.parentMenu, true);
    });

    wifiIPMenu.parentMenu = &wifiScannerMenu;
    this->addNodes(&wifiIPMenu, text09, TFTLIGHTGREY, 0, [this]() {
      this->changeMenu(wifiIPMenu.parentMenu, true);
    });


    // Select Stations on Mini v2
    this->addNodes(&wifiGeneralMenu, "Select Stations", TFTCYAN, KEYBOARD_ICO, [this](){
      wifiAPMenu.parentMenu = &wifiGeneralMenu;

      wifiAPMenu.list->clear();
        this->addNodes(&wifiAPMenu, text09, TFTLIGHTGREY, 0, [this]() {
        this->changeMenu(wifiAPMenu.parentMenu, true);
      });

      int menu_limit = access_points->size();


      for (int i = 0; i < menu_limit; i++) {
        wifiStationMenu.list->clear();
        this->addNodes(&wifiAPMenu, access_points->get(i).essid.c_str(), TFTCYAN, 255, [this, i](){

          wifiStationMenu.list->clear();

          wifiStationMenu.parentMenu = &wifiAPMenu;

          // Add back button to the APs
          this->addNodes(&wifiStationMenu, text09, TFTLIGHTGREY, 0, [this]() {
            this->changeMenu(wifiStationMenu.parentMenu, true);
          });

          this->addNodes(&wifiStationMenu, "Select ALL", TFTGREEN, 255, [this, i](){

            for (int y = 0; y < access_points->get(i).stations->size(); y++) {
              int cur_ap_sta_inx = access_points->get(i).stations->get(y);
              Station new_sta = stations->get(cur_ap_sta_inx);
              new_sta.selected = !stations->get(cur_ap_sta_inx).selected;

              // Change selection status of menu node
              MenuNode new_node = current_menu->list->get(y + 2);
              new_node.selected = !current_menu->list->get(y + 2).selected;
              current_menu->list->set(y + 2, new_node);

              stations->set(cur_ap_sta_inx, new_sta);
            }

            this->changeMenu(current_menu, true);

          });

          // Add the AP's stations to the specific AP menu
          for (int x = 0; x < access_points->get(i).stations->size(); x++) {
            int cur_ap_sta = access_points->get(i).stations->get(x);

            this->addNodes(&wifiStationMenu, macToString(stations->get(cur_ap_sta)).c_str(), TFTCYAN, 255, [this, i, cur_ap_sta, x](){
            Station new_sta = stations->get(cur_ap_sta);
            new_sta.selected = !stations->get(cur_ap_sta).selected;

            // Change selection status of menu node
            MenuNode new_node = current_menu->list->get(x + 2);
            new_node.selected = !current_menu->list->get(x + 2).selected;
            current_menu->list->set(x + 2, new_node);

            stations->set(cur_ap_sta, new_sta);
            }, stations->get(cur_ap_sta).selected);
          }

          // Final change menu to the menu of Stations
          this->changeMenu(&wifiStationMenu, true);
          
        }, false);
      }
      this->changeMenu(&wifiAPMenu, true);
    });

    this->addNodes(&wifiGeneralMenu, "Join WiFi", TFTWHITE, KEYBOARD_ICO, [this](){

      wifiAPMenu.parentMenu = &wifiGeneralMenu;

      // Add the back button
      wifiAPMenu.list->clear();
        this->addNodes(&wifiAPMenu, text09, TFTLIGHTGREY, 0, [this]() {
        this->changeMenu(wifiAPMenu.parentMenu, true);
      });

      // Populate the menu with buttons
      for (int i = 0; i < access_points->size(); i++) {
        // This is the menu node
        this->addNodes(&wifiAPMenu, access_points->get(i).essid.c_str(), TFTCYAN, 255, [this, i](){
          // Join WiFi using mini keyboard
          #ifdef HAS_MINI_KB
            this->changeMenu(&miniKbMenu, true);
            String password = this->miniKeyboard(&miniKbMenu, true);
            if (password != "") {
              Serial.println("Using SSID: " + (String)access_points->get(i).essid + " Password: " + (String)password);
              wifi_scan_obj.currentScanMode = LV_JOIN_WIFI;
              wifi_scan_obj.StartScan(LV_JOIN_WIFI, TFT_YELLOW); 
              wifi_scan_obj.joinWiFi(access_points->get(i).essid, password);
              this->changeMenu(current_menu, true);
            }
          #endif

          // Join WiFi using touch screen keyboard
          #ifdef HAS_TOUCH
            char passwordBuf[64] = {0};  // or prefill with existing SSID
            if (keyboardInput(passwordBuf, sizeof(passwordBuf), "Enter Password")) {
              wifi_scan_obj.joinWiFi(access_points->get(i).essid, String(passwordBuf), true);
            }

            this->changeMenu(&wifiGeneralMenu, true);
          #endif
        });
      }
      this->changeMenu(&wifiAPMenu, true);
    });

    this->addNodes(&wifiGeneralMenu, "Join Saved WiFi", TFTWHITE, KEYBOARD_ICO, [this](){
      String ssid = settings_obj.loadSetting<String>("ClientSSID");
      String pw = settings_obj.loadSetting<String>("ClientPW");

      if ((ssid != "") && (pw != "")) {
        wifi_scan_obj.joinWiFi(ssid, pw, false);
        this->changeMenu(&wifiGeneralMenu, true);
      }
      else {
        wifiAPMenu.parentMenu = &wifiGeneralMenu;

        // Add the back button
        wifiAPMenu.list->clear();
          this->addNodes(&wifiAPMenu, text09, TFTLIGHTGREY, 0, [this]() {
          this->changeMenu(wifiAPMenu.parentMenu, true);
        });

        // Populate the menu with buttons
        for (int i = 0; i < access_points->size(); i++) {
          // This is the menu node
          this->addNodes(&wifiAPMenu, access_points->get(i).essid.c_str(), TFTCYAN, 255, [this, i](){
            // Join WiFi using mini keyboard
            #ifdef HAS_MINI_KB
              this->changeMenu(&miniKbMenu, true);
              String password = this->miniKeyboard(&miniKbMenu, true);
              if (password != "") {
                Serial.println("Using SSID: " + (String)access_points->get(i).essid + " Password: " + (String)password);
                wifi_scan_obj.currentScanMode = LV_JOIN_WIFI;
                wifi_scan_obj.StartScan(LV_JOIN_WIFI, TFT_YELLOW); 
                wifi_scan_obj.joinWiFi(access_points->get(i).essid, password);
                this->changeMenu(current_menu, true);
              }
            #endif

            // Join WiFi using touch screen keyboard
            #ifdef HAS_TOUCH
              char passwordBuf[64] = {0};  // or prefill with existing SSID
              if (keyboardInput(passwordBuf, sizeof(passwordBuf), "Enter Password")) {
                wifi_scan_obj.joinWiFi(access_points->get(i).essid, String(passwordBuf), true);
              }

              this->changeMenu(&wifiGeneralMenu, true);
            #endif
          });
        }
        this->changeMenu(&wifiAPMenu, true);
      }
    });

    this->addNodes(&wifiGeneralMenu, "Start AP", TFTGREEN, KEYBOARD_ICO, [this](){
      ssidsMenu.parentMenu = &wifiGeneralMenu;

      // Add the back button
      ssidsMenu.list->clear();
        this->addNodes(&ssidsMenu, text09, TFTLIGHTGREY, 0, [this]() {
        this->changeMenu(ssidsMenu.parentMenu, true);
      });

      // Populate the menu with buttons
      for (int i = 0; i < ssids->size(); i++) {
        // This is the menu node
        this->addNodes(&ssidsMenu, ssids->get(i).essid.c_str(), TFTCYAN, 255, [this, i](){
          // Join WiFi using mini keyboard
          #ifdef HAS_MINI_KB
            this->changeMenu(&miniKbMenu, true);
            String password = this->miniKeyboard(&miniKbMenu, true);
            if (password != "") {
              Serial.println("Using SSID: " + (String)ssids->get(i).essid + " Password: " + (String)password);
              wifi_scan_obj.currentScanMode = LV_JOIN_WIFI;
              wifi_scan_obj.StartScan(LV_JOIN_WIFI, TFT_YELLOW); 
              wifi_scan_obj.startWiFi(ssids->get(i).essid, password);
              this->changeMenu(current_menu, true);
            }
          #endif

          // Join WiFi using touch screen keyboard
          #ifdef HAS_TOUCH
            char passwordBuf[64] = {0};  // or prefill with existing SSID
            if (keyboardInput(passwordBuf, sizeof(passwordBuf), "Enter Password")) {
              Serial.println("Using SSID: " + (String)ssids->get(i).essid + " Password: " + String(passwordBuf));
              wifi_scan_obj.startWiFi(ssids->get(i).essid, String(passwordBuf));
            }

            this->changeMenu(&wifiGeneralMenu, false);
          #endif
        });
      }
      this->changeMenu(&ssidsMenu, true);
    });

    this->addNodes(&wifiGeneralMenu, "Host AP Info", TFTGREEN, BEACON_SNIFF, [this]() {
      display_obj.clearScreen();
      this->drawStatusBar();
      wifi_scan_obj.StartScan(WIFI_SCAN_DISPLAY_AP_INFO, TFT_GREEN);
    });

    wifiStationMenu.parentMenu = &ssidsMenu;
    this->addNodes(&wifiStationMenu, text09, TFTLIGHTGREY, 0, [this]() {
      this->changeMenu(wifiStationMenu.parentMenu, true);
    });

  this->addNodes(&wifiGeneralMenu, "Set MACs", TFTLIGHTGREY, 0, [this]() {
    this->changeMenu(&setMacMenu, true);
  });

  this->addNodes(&wifiGeneralMenu, "Shutdown WiFi", TFTRED, 0, [this]() {
    WiFi.softAPdisconnect(true); // Also shut down the SoftAP if it is running
	WiFi.disconnect(true);
    delay(100);
    wifi_scan_obj.StartScan(WIFI_SCAN_OFF, TFT_RED);
    this->changeMenu(current_menu, true);
  });
  
  #ifdef HAS_DIRECT_UPLOAD
    this->addNodes(&wifiGeneralMenu, "Upload Wardrive Logs", TFTGREEN, 0, [this]() {
      display_obj.clearScreen();
      display_obj.tft.setTextWrap(false);
      display_obj.tft.setCursor(0, SCREEN_HEIGHT / 3);
      display_obj.tft.setTextColor(TFT_CYAN, TFT_BLACK);
      display_obj.tft.println("Loading...");

      this->buildUploadFileMenu();

      this->changeMenu(&uploadLogsMenu, true);
    });

    uploadAllMenu.parentMenu = &uploadLogsMenu;
    this->addNodes(&uploadAllMenu, text09, TFTLIGHTGREY, 0, [this]() {
      this->changeMenu(uploadAllMenu.parentMenu, true);
    });
    this->addNodes(&uploadAllMenu, "WiGLE", TFTLIGHTGREY, 0, [this]() {
      display_obj.tft.setTextColor(TFT_CYAN, TFT_BLACK);

      String ssid = settings_obj.loadSetting<String>("ClientSSID");
      String pw = settings_obj.loadSetting<String>("ClientPW");

      if ((ssid == "") && (pw == "")) {
        display_obj.clearScreen();
        display_obj.tft.setTextWrap(true);
        display_obj.tft.setCursor(0, SCREEN_HEIGHT / 3);
        display_obj.tft.println("WiFi Credentials Empty.");
        display_obj.tft.println("Returning...");
        display_obj.tft.setTextWrap(false);
      }
      else {
        display_obj.clearScreen();
        display_obj.showCenterText(String("Connecting to " + ssid).c_str(), TFT_HEIGHT / 2, true);
        if (!wifi_scan_obj.joinWiFi(ssid, pw, false)) {
          display_obj.clearScreen();
          display_obj.tft.setTextWrap(true);
          display_obj.tft.setCursor(0, SCREEN_HEIGHT / 3);
          display_obj.tft.println("Could not connect to WiFi.");
          display_obj.tft.println("Returning...");
          display_obj.tft.setTextWrap(false);
        }
        else {
          delay(1000);
          for (int i = 0; i < sd_obj.sd_files->size(); i++) {
            if (sd_obj.sd_files->get(i).startsWith("wardrive_") || sd_obj.sd_files->get(i).startsWith("wigle-")) {
              if (!sd_obj.sd_files->get(i).endsWith(".wigle") && !sd_obj.sd_files->get(i).endsWith(".wdg") && !sd_obj.sd_files->get(i).endsWith(".gpx")) {
                Serial.println("Uploading " + sd_obj.sd_files->get(i) + "...");
                if (wifi_scan_obj.uploadFile("/" + sd_obj.sd_files->get(i), true, WIGLE_UPLOAD)) {
                  display_obj.clearScreen();
                  display_obj.showCenterText("WiGLE OK", TFT_HEIGHT / 2);
                } else {
                  display_obj.clearScreen();
                  display_obj.showCenterText("WiGLE failed", TFT_HEIGHT / 2);
                }
              }
            }
          }
          WiFi.disconnect(true);
          delay(100);
          wifi_scan_obj.StartScan(WIFI_SCAN_OFF, TFT_RED);
        }
      }

      delay(2000);

      this->changeMenu(uploadAllMenu.parentMenu, true);
    });
    this->addNodes(&uploadAllMenu, "WDGWars", TFTLIGHTGREY, 0, [this]() {
      String ssid = settings_obj.loadSetting<String>("ClientSSID");
      String pw = settings_obj.loadSetting<String>("ClientPW");

      display_obj.tft.setTextColor(TFT_CYAN, TFT_BLACK);

      if ((ssid == "") && (pw == "")) {
        display_obj.clearScreen();
        display_obj.tft.setTextWrap(true);
        display_obj.tft.setCursor(0, SCREEN_HEIGHT / 3);
        display_obj.tft.println("WiFi Credentials Empty.");
        display_obj.tft.println("Returning...");
        display_obj.tft.setTextWrap(false);
      }
      else {
        display_obj.clearScreen();
        display_obj.showCenterText(String("Connecting to " + ssid).c_str(), TFT_HEIGHT / 2, true);
        if (!wifi_scan_obj.joinWiFi(ssid, pw, false)) {
          display_obj.clearScreen();
          display_obj.tft.setTextWrap(true);
          display_obj.tft.setCursor(0, SCREEN_HEIGHT / 3);
          display_obj.tft.println("Could not connect to WiFi.");
          display_obj.tft.println("Returning...");
          display_obj.tft.setTextWrap(false);
        }
        else {
          delay(1000);
          for (int i = 0; i < sd_obj.sd_files->size(); i++) {
            if (sd_obj.sd_files->get(i).startsWith("wardrive_") || sd_obj.sd_files->get(i).startsWith("wigle-")) {
              if (!sd_obj.sd_files->get(i).endsWith(".wigle") && !sd_obj.sd_files->get(i).endsWith(".wdg") && !sd_obj.sd_files->get(i).endsWith(".gpx")) {
                Serial.println("Uploading " + sd_obj.sd_files->get(i) + "...");
                if (wifi_scan_obj.uploadFile("/" + sd_obj.sd_files->get(i), true, WDG_UPLOAD)) {
                  display_obj.clearScreen();
                  display_obj.showCenterText("WDG OK", TFT_HEIGHT / 2);
                } else {
                  display_obj.clearScreen();
                  display_obj.showCenterText("WDG failed", TFT_HEIGHT / 2);
                }
              }
            }
          }
          WiFi.disconnect(true);
          delay(100);
          wifi_scan_obj.StartScan(WIFI_SCAN_OFF, TFT_RED);
        }
      }

      delay(2000);

      this->changeMenu(uploadAllMenu.parentMenu, true);
    });
    this->addNodes(&uploadAllMenu, "Both", TFTLIGHTGREY, 0, [this]() {
      String ssid = settings_obj.loadSetting<String>("ClientSSID");
      String pw = settings_obj.loadSetting<String>("ClientPW");

      display_obj.tft.setTextColor(TFT_CYAN, TFT_BLACK);

      if ((ssid == "") && (pw == "")) {
        display_obj.clearScreen();
        display_obj.tft.setTextWrap(true);
        display_obj.tft.setCursor(0, SCREEN_HEIGHT / 3);
        display_obj.tft.println("WiFi Credentials Empty.");
        display_obj.tft.println("Returning...");
        display_obj.tft.setTextWrap(false);
      }
      else {
        display_obj.clearScreen();
        display_obj.showCenterText(String("Connecting to " + ssid).c_str(), TFT_HEIGHT / 2, true);
        if (!wifi_scan_obj.joinWiFi(ssid, pw, false)) {
          display_obj.clearScreen();
          display_obj.tft.setTextWrap(true);
          display_obj.tft.setCursor(0, SCREEN_HEIGHT / 3);
          display_obj.tft.println("Could not connect to WiFi.");
          display_obj.tft.println("Returning...");
          display_obj.tft.setTextWrap(false);
        }
        else {
          delay(1000);
          for (int i = 0; i < sd_obj.sd_files->size(); i++) {
            if (sd_obj.sd_files->get(i).startsWith("wardrive_") || sd_obj.sd_files->get(i).startsWith("wigle-")) {
              if (!sd_obj.sd_files->get(i).endsWith(".wigle") && !sd_obj.sd_files->get(i).endsWith(".wdg") && !sd_obj.sd_files->get(i).endsWith(".gpx")) {
                Serial.println("Uploading " + sd_obj.sd_files->get(i) + "...");
                if (wifi_scan_obj.uploadFile("/" + sd_obj.sd_files->get(i), true, BOTH_UPLOAD)) {
                  display_obj.clearScreen();
                  display_obj.showCenterText("Upload OK", TFT_HEIGHT / 2);
                } else {
                  display_obj.clearScreen();
                  display_obj.showCenterText("Upload failed", TFT_HEIGHT / 2);
                }
              }
            }
          }
          WiFi.disconnect(true);
          delay(100);
          wifi_scan_obj.StartScan(WIFI_SCAN_OFF, TFT_RED);
        }
      }

      delay(2000);

      this->changeMenu(uploadAllMenu.parentMenu, true);
    });

    deleteAllMenu.parentMenu = &uploadLogsMenu;
    this->addNodes(&deleteAllMenu, "No", TFTLIGHTGREY, 0, [this]() {
      this->changeMenu(deleteAllMenu.parentMenu, true);
    });
    this->addNodes(&deleteAllMenu, "Yes", TFTRED, 0, [this]() {
      display_obj.tft.setTextColor(TFT_ORANGE, TFT_BLACK);

      display_obj.clearScreen();

      display_obj.showCenterText("Deleting logs...", TFT_HEIGHT / 2, true);

      for (int i = 0; i < sd_obj.sd_files->size(); i++) {
        if (sd_obj.sd_files->get(i).startsWith("wardrive_") || sd_obj.sd_files->get(i).startsWith("wigle-")) {
          if (sd_obj.removeFile("/" + sd_obj.sd_files->get(i))) {
            Serial.println("Removed file: " + sd_obj.sd_files->get(i));
            sd_obj.removeFile("/" + sd_obj.sd_files->get(i) + ".wdg");
            sd_obj.removeFile("/" + sd_obj.sd_files->get(i) + ".wigle");
          }
          else {
            Serial.println("Could not remove file: " + sd_obj.sd_files->get(i));
          }
        }
      }
      display_obj.clearScreen();

      display_obj.showCenterText("Logs removed", TFT_HEIGHT / 2, true);

      delay(2000);

      this->buildUploadFileMenu();

      this->changeMenu(&uploadLogsMenu, true);
    });

    actionMenu.parentMenu = &uploadLogsMenu;
    this->addNodes(&actionMenu, text09, TFTLIGHTGREY, 0, [this]() {
      this->changeMenu(actionMenu.parentMenu, true);
    });
    this->addNodes(&actionMenu, "WiGLE", TFTLIGHTGREY, 0, [this]() {
      String ssid = settings_obj.loadSetting<String>("ClientSSID");
      String pw = settings_obj.loadSetting<String>("ClientPW");

      display_obj.tft.setTextColor(TFT_CYAN, TFT_BLACK);

      if ((ssid == "") && (pw == "")) {
        display_obj.clearScreen();
        display_obj.tft.setTextWrap(true);
        display_obj.tft.setCursor(0, SCREEN_HEIGHT / 3);
        display_obj.tft.println("WiFi Credentials Empty.");
        display_obj.tft.println("Returning...");
        display_obj.tft.setTextWrap(false);
      }
      else {
        display_obj.clearScreen();
        display_obj.showCenterText(String("Connecting to " + ssid).c_str(), TFT_HEIGHT / 2, true);
        if (!wifi_scan_obj.joinWiFi(ssid, pw, false)) {
          display_obj.clearScreen();
          display_obj.tft.setTextWrap(true);
          display_obj.tft.setCursor(0, SCREEN_HEIGHT / 3);
          display_obj.tft.setTextColor(TFT_CYAN, TFT_BLACK);
          display_obj.tft.println("Could not connect to WiFi.");
          display_obj.tft.println("Returning...");
          display_obj.tft.setTextWrap(false);
        }
        else {
          delay(1000);
          Serial.println("Uploading " + sd_obj.selected_file_name + "...");
          if (wifi_scan_obj.uploadFile("/" + sd_obj.selected_file_name, true, WIGLE_UPLOAD)) {
            display_obj.clearScreen();
            display_obj.showCenterText("WiGLE OK", TFT_HEIGHT / 2, true);
          } else {
            display_obj.clearScreen();
            display_obj.showCenterText("WiGLE failed", TFT_HEIGHT / 2, true);
          }

          WiFi.disconnect(true);
          delay(100);
          wifi_scan_obj.StartScan(WIFI_SCAN_OFF, TFT_RED);
        }
      }

      delay(2000);

      this->changeMenu(&actionMenu, true);
    });
    this->addNodes(&actionMenu, "WDGWars", TFTLIGHTGREY, 0, [this]() {
      String ssid = settings_obj.loadSetting<String>("ClientSSID");
      String pw = settings_obj.loadSetting<String>("ClientPW");

      display_obj.tft.setTextColor(TFT_CYAN, TFT_BLACK);

      if ((ssid == "") && (pw == "")) {
        display_obj.clearScreen();
        display_obj.tft.setTextWrap(true);
        display_obj.tft.setCursor(0, SCREEN_HEIGHT / 3);
        display_obj.tft.println("WiFi Credentials Empty.");
        display_obj.tft.println("Returning...");
        display_obj.tft.setTextWrap(false);
      }
      else {
        display_obj.clearScreen();
        display_obj.showCenterText(String("Connecting to " + ssid).c_str(), TFT_HEIGHT / 2, true);
        if (!wifi_scan_obj.joinWiFi(ssid, pw, false)) {
          display_obj.clearScreen();
          display_obj.tft.setTextWrap(true);
          display_obj.tft.setCursor(0, SCREEN_HEIGHT / 3);
          display_obj.tft.println("Could not connect to WiFi.");
          display_obj.tft.println("Returning...");
          display_obj.tft.setTextWrap(false);
        }
        else {
          delay(1000);
          Serial.println("Uploading " + sd_obj.selected_file_name + "...");
          if (wifi_scan_obj.uploadFile("/" + sd_obj.selected_file_name, true, WDG_UPLOAD)) {
            display_obj.clearScreen();
            display_obj.showCenterText("WDG OK", TFT_HEIGHT / 2, true);
          } else {
            display_obj.clearScreen();
            display_obj.showCenterText("WDG failed", TFT_HEIGHT / 2, true);
          }

          WiFi.disconnect(true);
        delay(100);
        wifi_scan_obj.StartScan(WIFI_SCAN_OFF, TFT_RED);
        }
      }

      delay(2000);

      this->changeMenu(&actionMenu, true);
    });
    this->addNodes(&actionMenu, "Both", TFTLIGHTGREY, 0, [this]() {
      String ssid = settings_obj.loadSetting<String>("ClientSSID");
      String pw = settings_obj.loadSetting<String>("ClientPW");

      display_obj.tft.setTextColor(TFT_CYAN, TFT_BLACK);

      if ((ssid == "") && (pw == "")) {
        display_obj.clearScreen();
        display_obj.tft.setTextWrap(true);
        display_obj.tft.setCursor(0, SCREEN_HEIGHT / 3);
        display_obj.tft.println("WiFi Credentials Empty.");
        display_obj.tft.println("Returning...");
        display_obj.tft.setTextWrap(false);
      }
      else {
        display_obj.clearScreen();
        display_obj.showCenterText(String("Connecting to " + ssid).c_str(), TFT_HEIGHT / 2, true);
        if (!wifi_scan_obj.joinWiFi(ssid, pw, false)) {
          display_obj.clearScreen();
          display_obj.tft.setTextWrap(true);
          display_obj.tft.setCursor(0, SCREEN_HEIGHT / 3);
          display_obj.tft.println("Could not connect to WiFi.");
          display_obj.tft.println("Returning...");
          display_obj.tft.setTextWrap(false);
        }
        else {
          delay(1000);
          Serial.println("Uploading " + sd_obj.selected_file_name + "...");
          if (wifi_scan_obj.uploadFile("/" + sd_obj.selected_file_name, true, BOTH_UPLOAD)) {
            display_obj.clearScreen();
            display_obj.showCenterText("Upload OK", TFT_HEIGHT / 2, true);
          } else {
            display_obj.clearScreen();
            display_obj.showCenterText("Upload failed", TFT_HEIGHT / 2, true);
          }

          WiFi.disconnect(true);
          delay(100);
          wifi_scan_obj.StartScan(WIFI_SCAN_OFF, TFT_RED);
        }
      }

      delay(2000);

      this->changeMenu(&actionMenu, true);
    });
  #endif


  // Menu for generating and setting MAC addrs for AP and STA
  setMacMenu.parentMenu = &wifiGeneralMenu;
  this->addNodes(&setMacMenu, text09, TFTLIGHTGREY, 0, [this]() {
    this->changeMenu(setMacMenu.parentMenu, true);
  });

  // Generate random MAC for AP
  this->addNodes(&setMacMenu, "Generate AP MAC", TFTLIME, 0, [this]() {
    this->changeMenu(&genAPMacMenu, true);
    wifi_scan_obj.RunGenerateRandomMac(true);
  });

  // Generate random MAC for AP
  this->addNodes(&setMacMenu, "Generate STA MAC", TFTCYAN, 0, [this]() {
    this->changeMenu(&genAPMacMenu, true);
    wifi_scan_obj.RunGenerateRandomMac(false);
  });

  // Clone AP MAC to ESP32 for button folks
  //#ifndef HAS_ILI9341
    this->addNodes(&setMacMenu, "Clone AP MAC", TFTRED, CLEAR_ICO, [this](){
      wifiAPMenu.parentMenu = &wifiGeneralMenu;

      // Add the back button
      wifiAPMenu.list->clear();
        this->addNodes(&wifiAPMenu, text09, TFTLIGHTGREY, 0, [this]() {
        this->changeMenu(wifiAPMenu.parentMenu, true);
      });

      // Populate the menu with buttons
      for (int i = 0; i < access_points->size(); i++) {
        // This is the menu node
        this->addNodes(&wifiAPMenu, access_points->get(i).essid.c_str(), TFTLIME, 255, [this, i](){
          this->changeMenu(&genAPMacMenu, true);
          wifi_scan_obj.RunSetMac(access_points->get(i).bssid, true);
        });
      }
      this->changeMenu(&wifiAPMenu, true);
    });

    this->addNodes(&setMacMenu, "Clone STA MAC", TFTMAGENTA, CLEAR_ICO, [this](){
      wifiAPMenu.parentMenu = &wifiGeneralMenu;

      // Add the back button
      wifiAPMenu.list->clear();
        this->addNodes(&wifiAPMenu, text09, TFTLIGHTGREY, 0, [this]() {
        this->changeMenu(wifiAPMenu.parentMenu, true);
      });

      // Populate the menu with buttons
      for (int i = 0; i < stations->size(); i++) {
        // This is the menu node
        this->addNodes(&wifiAPMenu, macToString(stations->get(i).mac).c_str(), TFTMAGENTA, 255, [this, i](){
          this->changeMenu(&genAPMacMenu, true);
          wifi_scan_obj.RunSetMac(stations->get(i).mac, false);
        });
      }
      this->changeMenu(&wifiAPMenu, true);
    });
  //#endif

  // Menu for generating and setting access point MAC (just goes bacK)
  genAPMacMenu.parentMenu = &wifiGeneralMenu;
  this->addNodes(&genAPMacMenu, text09, TFTLIGHTGREY, 0, [this]() {
    this->changeMenu(genAPMacMenu.parentMenu, true);
  });

  // Build generate ssids menu
  generateSSIDsMenu.parentMenu = &wifiGeneralMenu;
  this->addNodes(&generateSSIDsMenu, text09, TFTLIGHTGREY, 0, [this]() {
    this->changeMenu(generateSSIDsMenu.parentMenu, true);
  });

  // Build clear ssids menu
  
  this->addNodes(&clearSSIDsMenu, text09, TFTLIGHTGREY, 0, [this]() {
    this->changeMenu(clearSSIDsMenu.parentMenu, true);
  });
  clearAPsMenu.parentMenu = &wifiGeneralMenu;
  this->addNodes(&clearAPsMenu, text09, TFTLIGHTGREY, 0, [this]() {
    this->changeMenu(clearAPsMenu.parentMenu, true);
  });

#ifdef HAS_BT
  // Build Bluetooth Menu
  bluetoothMenu.parentMenu = &mainMenu; // Second Menu is third menu parent
  this->addNodes(&bluetoothMenu, text09, TFTLIGHTGREY, 0, [this]() {
    this->changeMenu(bluetoothMenu.parentMenu, true);
  });
  this->addNodes(&bluetoothMenu, text_table1[31], TFTYELLOW, SNIFFERS, [this]() {
    this->changeMenu(&bluetoothSnifferMenu, true);
  });
  this->addNodes(&bluetoothMenu, "Bluetooth Attacks", TFTRED, ATTACKS, [this]() {
    this->changeMenu(&bluetoothAttackMenu, true);
  });
  this->addNodes(&bluetoothMenu, "BLE Discovery", TFTCYAN, BLUETOOTH,
                 [this]() {
    this->changeMenu(&bleSecurityMenu, true);
  });

  bleSecurityMenu.parentMenu = &bluetoothMenu;
  this->addNodes(&bleSecurityMenu, text09, TFTLIGHTGREY, 0, [this]() {
    this->changeMenu(bleSecurityMenu.parentMenu, true);
  });
  this->addNodes(&bleSecurityMenu, "Scan Targets", TFTGREEN,
                 BLUETOOTH_SNIFF, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(BT_SCAN_ALL, TFT_GREEN);
  });
  this->addNodes(&bleSecurityMenu, "Capture Adverts to SD", TFTORANGE,
                 SD_UPDATE, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    if (!wifi_scan_obj.startBLEAdvertisementCapture()) {
      display_obj.tft.setTextColor(TFT_RED, TFT_BLACK);
      display_obj.tft.setCursor(3, 24);
      display_obj.tft.println("Mounted SD card required");
      delay(1500);
      this->changeMenu(&bleSecurityMenu, true);
    }
  });
  this->addNodes(&bleSecurityMenu, "Select Target", TFTCYAN, SCANNERS,
                 [this]() {
    bleTargetMenu.list->clear();
    bleTargetMenu.parentMenu = &bleSecurityMenu;
    this->addNodes(&bleTargetMenu, text09, TFTLIGHTGREY, 0, [this]() {
      this->changeMenu(bleTargetMenu.parentMenu, true);
    });
    const int menuLimit = min(ble_devices->size(), 40);
    for (int index = 0; index < menuLimit; index++) {
      const BleDevice device = ble_devices->get(index);
      const String label = BLESecurityTools::deviceDisplayLabel(device);
      const uint8_t color = device.connectable ? TFTGREEN : TFTDARKGREY;
      this->addNodes(&bleTargetMenu, label.c_str(), color, BLUETOOTH,
                     [this, index]() {
        if (index >= 0 && index < ble_devices->size())
          BLESecurityTools::selectTarget(ble_devices->get(index));
        this->changeMenu(&bleSecurityMenu, true);
      });
    }
    if (menuLimit == 0)
      this->addNodes(&bleTargetMenu, "Run Scan Targets first",
                     TFTDARKGREY, DEVICE_INFO, []() {});
    this->changeMenu(&bleTargetMenu, true);
  });
  this->addNodes(&bleSecurityMenu, "Advertised Info", TFTCYAN,
                 DEVICE_INFO, [this]() {
    BLESecurityTools::showAdvertisedInfo();
    display_obj.init();
    this->changeMenu(&bleSecurityMenu, true);
  });
  this->addNodes(&bleSecurityMenu, "GATT Enumeration", TFTGREEN,
                 DEVICE_INFO, [this]() {
    BLESecurityTools::inspectTarget();
    display_obj.init();
    this->changeMenu(&bleSecurityMenu, true);
  });
  this->addNodes(&bleSecurityMenu, "Device Spoof", TFTMAGENTA,
                 BLUETOOTH, [this]() {
    bleConfirmMenu.list->clear();
    bleConfirmMenu.name = "Authorize Device Spoof";
    bleConfirmMenu.parentMenu = &bleSecurityMenu;
    this->addNodes(&bleConfirmMenu, "Cancel", TFTLIGHTGREY, 0, [this]() {
      this->changeMenu(bleConfirmMenu.parentMenu, true);
    });
    this->addNodes(&bleConfirmMenu, "Run (Authorized)", TFTRED,
                   ATTACKS, [this]() {
      BLESecurityTools::runDeviceSpoof();
      display_obj.init();
      this->changeMenu(&bleSecurityMenu, true);
    });
    this->changeMenu(&bleConfirmMenu, true);
  });

  // Build bluetooth sniffer Menu
  bluetoothSnifferMenu.parentMenu = &bluetoothMenu; // Second Menu is third menu parent
  this->addNodes(&bluetoothSnifferMenu, text09, TFTLIGHTGREY, 0, [this]() {
    this->changeMenu(bluetoothSnifferMenu.parentMenu, true);
  });
  this->addNodes(&bluetoothSnifferMenu, text_table1[34], TFTGREEN, BLUETOOTH_SNIFF, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(BT_SCAN_ALL, TFT_GREEN);
  });
  this->addNodes(&bluetoothSnifferMenu, "Flipper Sniff", TFTORANGE, FLIPPER, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(BT_SCAN_FLIPPER, TFT_ORANGE);
  });
  this->addNodes(&bluetoothSnifferMenu, "FindMy Sniff", TFTWHITE, BLUETOOTH_SNIFF, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(BT_SCAN_AIRTAG, TFT_WHITE);
  });
  this->addNodes(&bluetoothSnifferMenu, "FindMy Monitor", TFTWHITE, BLUETOOTH_SNIFF, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(BT_SCAN_AIRTAG_MON, TFT_WHITE);
  });
  this->addNodes(&bluetoothSnifferMenu, text_table1[35], TFTMAGENTA, CC_SKIMMERS, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(BT_SCAN_SKIMMERS, TFT_MAGENTA);
  });
  this->addNodes(&bluetoothSnifferMenu, "Bluetooth Analyzer", TFTCYAN, PACKET_MONITOR, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    this->renderGraphUI(BT_SCAN_ANALYZER);
    wifi_scan_obj.StartScan(BT_SCAN_ANALYZER, TFT_CYAN);
  });
  this->addNodes(&bluetoothSnifferMenu, "Flock Sniff", TFTORANGE, FLOCK, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(BT_SCAN_FLOCK, TFT_ORANGE);
  });
  this->addNodes(&bluetoothSnifferMenu, "Meta Detect", TFTWHITE, BLUETOOTH_SNIFF, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(BT_SCAN_RAYBAN, TFT_CYAN);
  });
  #ifdef MARAUDER_MINI_V3
    this->addNodes(&bluetoothSnifferMenu, "Meshtastic Sniff", TFTGREEN,
                   BLUETOOTH_SNIFF, [this]() {
      BLEDeviceDetectors::run(BLEDeviceDetectors::DetectorType::Meshtastic);
      this->changeMenu(&bluetoothSnifferMenu, true);
    });
    this->addNodes(&bluetoothSnifferMenu, "MeshCore Sniff", TFTCYAN,
                   BLUETOOTH_SNIFF, [this]() {
      BLEDeviceDetectors::run(BLEDeviceDetectors::DetectorType::MeshCore);
      this->changeMenu(&bluetoothSnifferMenu, true);
    });
    this->addNodes(&bluetoothSnifferMenu, "SmartTag Sniff", TFTYELLOW,
                   BLUETOOTH_SNIFF, [this]() {
      BLEDeviceDetectors::run(BLEDeviceDetectors::DetectorType::SmartTag);
      this->changeMenu(&bluetoothSnifferMenu, true);
    });
    this->addNodes(&bluetoothSnifferMenu, "Tile Sniff", TFTORANGE,
                   BLUETOOTH_SNIFF, [this]() {
      BLEDeviceDetectors::run(BLEDeviceDetectors::DetectorType::Tile);
      this->changeMenu(&bluetoothSnifferMenu, true);
    });
    this->addNodes(&bluetoothSnifferMenu, "Axon Sniff", TFTRED,
                   BLUETOOTH_SNIFF, [this]() {
      BLEDeviceDetectors::run(BLEDeviceDetectors::DetectorType::Axon);
      this->changeMenu(&bluetoothSnifferMenu, true);
    });
    this->addNodes(&bluetoothSnifferMenu, "iBeacon Sniff", TFTPURPLE,
                   BLUETOOTH_SNIFF, [this]() {
      BLEDeviceDetectors::run(BLEDeviceDetectors::DetectorType::IBeacon);
      this->changeMenu(&bluetoothSnifferMenu, true);
    });
    this->addNodes(&bluetoothSnifferMenu, "nyanBOX Sniff", TFTMAGENTA,
                   BLUETOOTH_SNIFF, [this]() {
      BLEDeviceDetectors::run(BLEDeviceDetectors::DetectorType::NyanBox);
      this->changeMenu(&bluetoothSnifferMenu, true);
    });
  #endif
  this->addNodes(&bluetoothSnifferMenu, "Fox Hunt", TFTCYAN, SCANNERS, [this]() {
    this->buildBluetoothFoxHuntMenu();
  });

  // Bluetooth Attack menu
  bluetoothAttackMenu.parentMenu = &bluetoothMenu; // Second Menu is third menu parent
  this->addNodes(&bluetoothAttackMenu, text09, TFTLIGHTGREY, 0, [this]() {
    this->changeMenu(bluetoothAttackMenu.parentMenu, true);
  });
  this->addNodes(&bluetoothAttackMenu, "Sour Apple", TFTGREEN, DEAUTH_SNIFF, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(BT_ATTACK_SOUR_APPLE, TFT_GREEN);
  });
  this->addNodes(&bluetoothAttackMenu, "Apple Juice", TFTYELLOW, DEAUTH_SNIFF, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(BT_ATTACK_APPLE_JUICE, TFT_YELLOW);
  });
  this->addNodes(&bluetoothAttackMenu, "Swiftpair Spam", TFTCYAN, KEYBOARD_ICO, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(BT_ATTACK_SWIFTPAIR_SPAM, TFT_CYAN);
  });
  this->addNodes(&bluetoothAttackMenu, "Samsung BLE Spam", TFTRED, GENERAL_APPS, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(BT_ATTACK_SAMSUNG_SPAM, TFT_RED);
  });
  this->addNodes(&bluetoothAttackMenu, "Google BLE Spam", TFTPURPLE, LANGUAGE, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(BT_ATTACK_GOOGLE_SPAM, TFT_PURPLE);
  });
  this->addNodes(&bluetoothAttackMenu, "Flipper BLE Spam", TFTORANGE, FLIPPER, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(BT_ATTACK_FLIPPER_SPAM, TFT_ORANGE);
  });
  this->addNodes(&bluetoothAttackMenu, "BLE Spam All", TFTMAGENTA, DEAUTH_SNIFF, [this]() {
    display_obj.clearScreen();
    this->drawStatusBar();
    wifi_scan_obj.StartScan(BT_ATTACK_SPAM_ALL, TFT_MAGENTA);
  });

#endif

  //#ifndef HAS_ILI9341
    #ifdef HAS_BT
      this->addNodes(&bluetoothAttackMenu, "Spoof Airtag", TFTWHITE, ATTACKS, [this](){
          wifiAPMenu.parentMenu = &bluetoothAttackMenu;

          // Clear nodes and add back button
          wifiAPMenu.list->clear();
          this->addNodes(&wifiAPMenu, text09, TFT_LIGHTGREY, 0, [this]() {
          this->changeMenu(wifiAPMenu.parentMenu, true);
        });

        // Add buttons for all airtags
        // Find out how big our menu is going to be
        int menu_limit;
        if (airtags->size() <= BUTTON_ARRAY_LEN)
          menu_limit = airtags->size();
        else
          menu_limit = BUTTON_ARRAY_LEN;

        // Create the menu nodes for all of the list items
        for (int i = 0; i < menu_limit; i++) {
          this->addNodes(&wifiAPMenu, airtags->get(i).mac.c_str(), TFTWHITE, BLUETOOTH, [this, i](){
            AirTag new_at = airtags->get(i);
            new_at.selected = true;

            airtags->set(i, new_at);

            // Set all other airtags to "Not Selected"
            for (int x = 0; x < airtags->size(); x++) {
              if (x != i) {
                AirTag new_atx = airtags->get(x);
                new_atx.selected = false;
                airtags->set(x, new_atx);
              }
            }

            // Start the spoof
            display_obj.clearScreen();
            this->drawStatusBar();
            wifi_scan_obj.StartScan(BT_SPOOF_AIRTAG, TFT_WHITE);

          });
        }
        this->changeMenu(&wifiAPMenu, true);
      });

      #ifdef HAS_NIMBLE_2
      this->addNodes(&bluetoothAttackMenu, "FindMy Sound", TFTCYAN, ATTACKS, [this](){
          wifiAPMenu.parentMenu = &bluetoothAttackMenu;

          // Clear nodes and add back button
          wifiAPMenu.list->clear();
          this->addNodes(&wifiAPMenu, text09, TFTLIGHTGREY, 0, [this]() {
          this->changeMenu(wifiAPMenu.parentMenu, true);
        });

        /*this->addNodes(&wifiAPMenu, "Live", TFTMAGENTA, 0, [this]() {
          display_obj.clearScreen();
          this->drawStatusBar();
          wifi_scan_obj.StartScan(BT_ATTACK_FINDMY_LIVE, TFT_RED);
        });*/

        int menu_limit = airtags->size();

        // Create the menu nodes for all of the list items
        for (int i = 0; i < menu_limit; i++) {
          uint8_t node_color = rssiToMenuColor(airtags->get(i).rssi);
          String node_name = String(airtags->get(i).rssi) + " " + airtags->get(i).mac;
          this->addNodes(&wifiAPMenu, node_name.c_str(), node_color, BLUETOOTH, [this, i](){
            AirTag new_at = airtags->get(i);
            new_at.selected = true;
            new_at.connectable = true;

            airtags->set(i, new_at);

            // Set all other airtags to "Not Selected"
            for (int x = 0; x < airtags->size(); x++) {
              if (x != i) {
                AirTag new_atx = airtags->get(x);
                new_atx.selected = false;
                airtags->set(x, new_atx);
              }
            }

            // Start the spoof
            display_obj.clearScreen();
            this->drawStatusBar();
            wifi_scan_obj.executeFindMySound(true);
            delay(2000);
            this->changeMenu(&wifiAPMenu, true);
          });
        }
        this->changeMenu(&wifiAPMenu, true);
      });
      #endif

      wifiAPMenu.parentMenu = &bluetoothAttackMenu;
      this->addNodes(&wifiAPMenu, text09, TFTLIGHTGREY, 0, [this]() {
        this->changeMenu(wifiAPMenu.parentMenu, true);
      });

      wifiAPMenu.parentMenu = &bluetoothAttackMenu;
      this->addNodes(&wifiAPMenu, text09, TFTLIGHTGREY, 0, [this]() {
        this->changeMenu(wifiAPMenu.parentMenu, true);
      });
    #endif

  //#endif

  // Device menu
  deviceMenu.parentMenu = &mainMenu;
  this->addNodes(&deviceMenu, text09, TFTLIGHTGREY, 0, [this]() {
    this->changeMenu(deviceMenu.parentMenu, true);
  });

  #ifdef HAS_SD
    if (sd_obj.supported) {

      sdDeleteMenu.parentMenu = &deviceMenu;

      this->addNodes(&deviceMenu, "Update Firmware", TFTORANGE, SD_UPDATE, [this]() {
        display_obj.clearScreen();
        display_obj.tft.setTextWrap(false);
        display_obj.tft.setCursor(0, SCREEN_HEIGHT / 3);
        display_obj.tft.setTextColor(TFT_CYAN, TFT_BLACK);
        display_obj.tft.println("Loading...");

        // Clear menu and lists
        this->buildSDFileMenu(true);

        this->changeMenu(&sdDeleteMenu, true);
      });

    }
  #endif

  this->addNodes(&deviceMenu, "Save/Load Files", TFTCYAN, SD_UPDATE, [this]() {
    this->changeMenu(&saveFileMenu, true);
  });

  #if !defined(HAS_MINI_SCREEN) || defined(MARAUDER_MINI_V3)
    this->addNodes(&deviceMenu, "Brightness", TFTYELLOW, BRIGHTNESS, [this]() {
      this->brightnessMode();
    });
  #endif

  this->addNodes(&deviceMenu, text_table1[17], TFTWHITE, DEVICE_INFO, [this]() {
    wifi_scan_obj.currentScanMode = SHOW_INFO;
    this->changeMenu(&infoMenu, true);
    wifi_scan_obj.RunInfo();
  });
  this->addNodes(&deviceMenu, text08, TFTBLUE, SETTINGS, [this]() {
    this->changeMenu(&settingsMenu, true);
  });

  #ifdef HAS_SD
    if (sd_obj.supported) {

      sdDeleteMenu.parentMenu = &deviceMenu;

      this->addNodes(&deviceMenu, "Delete SD Files", TFTCYAN, SD_UPDATE, [this]() {
        display_obj.clearScreen();
        display_obj.tft.setTextWrap(false);
        display_obj.tft.setCursor(0, SCREEN_HEIGHT / 3);
        display_obj.tft.setTextColor(TFT_CYAN, TFT_BLACK);
        display_obj.tft.println("Loading...");

        // Clear menu and lists
        this->buildSDFileMenu();

        this->changeMenu(&sdDeleteMenu, true);
      });
    }
  #endif

  // Save Files Menu
  saveFileMenu.parentMenu = &deviceMenu;
  this->addNodes(&saveFileMenu, text09, TFTLIGHTGREY, 0, [this]() {
    this->changeMenu(saveFileMenu.parentMenu, true);
  });
  this->addNodes(&saveFileMenu, "Save SSIDs", TFTCYAN, SD_UPDATE, [this]() {
    this->changeMenu(&saveSSIDsMenu, true);
    wifi_scan_obj.RunSaveSSIDList(true);
  });
  this->addNodes(&saveFileMenu, "Load SSIDs", TFTSKYBLUE, SD_UPDATE, [this]() {
    this->changeMenu(&loadSSIDsMenu, true);
    wifi_scan_obj.RunLoadSSIDList();
  });
  this->addNodes(&saveFileMenu, "Save APs", TFTNAVY, SD_UPDATE, [this]() {
    this->changeMenu(&saveAPsMenu, true);
    wifi_scan_obj.RunSaveAPList();
  });
  this->addNodes(&saveFileMenu, "Load APs", TFTBLUE, SD_UPDATE, [this]() {
    this->changeMenu(&loadAPsMenu, true);
    wifi_scan_obj.RunLoadAPList();
  });
  this->addNodes(&saveFileMenu, "Save Airtags", TFTWHITE, SD_UPDATE, [this]() {
    this->changeMenu(&saveAPsMenu, true);
    wifi_scan_obj.RunSaveATList();
  });
  this->addNodes(&saveFileMenu, "Load Airtags", TFTWHITE, SD_UPDATE, [this]() {
    this->changeMenu(&loadAPsMenu, true);
    wifi_scan_obj.RunLoadATList();
  });

  saveSSIDsMenu.parentMenu = &saveFileMenu;
  this->addNodes(&saveSSIDsMenu, text09, TFTLIGHTGREY, 0, [this]() {
    this->changeMenu(saveSSIDsMenu.parentMenu, true);
  });

  loadSSIDsMenu.parentMenu = &saveFileMenu;
  this->addNodes(&loadSSIDsMenu, text09, TFTLIGHTGREY, 0, [this]() {
    this->changeMenu(loadSSIDsMenu.parentMenu, true);
  });

  saveAPsMenu.parentMenu = &saveFileMenu;
  this->addNodes(&saveAPsMenu, text09, TFTLIGHTGREY, 0, [this]() {
    this->changeMenu(saveAPsMenu.parentMenu, true);
  });

  loadAPsMenu.parentMenu = &saveFileMenu;
  this->addNodes(&loadAPsMenu, text09, TFTLIGHTGREY, 0, [this]() {
    this->changeMenu(loadAPsMenu.parentMenu, true);
  });

  saveATsMenu.parentMenu = &saveFileMenu;
  this->addNodes(&saveATsMenu, text09, TFTLIGHTGREY, 0, [this]() {
    this->changeMenu(saveATsMenu.parentMenu, true);
  });

  loadATsMenu.parentMenu = &saveFileMenu;
  this->addNodes(&loadATsMenu, text09, TFTLIGHTGREY, 0, [this]() {
    this->changeMenu(loadATsMenu.parentMenu, true);
  });

  // GPS Menu
  #ifdef HAS_GPS
    if (expose_gps_features) {
      gpsMenu.parentMenu = &mainMenu; // Main Menu is second menu parent

      this->addNodes(&gpsMenu, text09, TFTLIGHTGREY, 0, [this]() {
        this->changeMenu(gpsMenu.parentMenu, true);
      });

      this->addNodes(&gpsMenu, "GPS Data", TFTRED, GPS_MENU, [this]() {
        wifi_scan_obj.currentScanMode = WIFI_SCAN_GPS_DATA;
        this->changeMenu(&gpsInfoMenu, true);
        wifi_scan_obj.StartScan(WIFI_SCAN_GPS_DATA, TFT_CYAN);
      });

      this->addNodes(&gpsMenu, "NMEA Stream", TFTORANGE, GPS_MENU, [this]() {
        wifi_scan_obj.currentScanMode = WIFI_SCAN_GPS_NMEA;
        this->changeMenu(&gpsInfoMenu, true);
        wifi_scan_obj.StartScan(WIFI_SCAN_GPS_NMEA, TFT_ORANGE);
      });

      this->addNodes(&gpsMenu, "GPS Tracker", TFTGREEN, GPS_MENU, [this]() {
        wifi_scan_obj.currentScanMode = GPS_TRACKER;
        this->changeMenu(&gpsInfoMenu, true);
        wifi_scan_obj.StartScan(GPS_TRACKER, TFT_CYAN);
      });

      this->addNodes(&gpsMenu, "GPS POI", TFTCYAN, GPS_MENU, [this]() {
        wifi_scan_obj.StartScan(GPS_POI, TFT_CYAN);
        wifi_scan_obj.currentScanMode = WIFI_SCAN_OFF;
        this->changeMenu(&gpsPOIMenu, true);
      });

      // GPS POI Menu
      gpsPOIMenu.parentMenu = &gpsMenu;
      this->addNodes(&gpsPOIMenu, text09, TFTLIGHTGREY, 0, [this]() {
        wifi_scan_obj.currentScanMode = GPS_POI;
        wifi_scan_obj.StartScan(WIFI_SCAN_OFF);
        this->changeMenu(gpsPOIMenu.parentMenu, true);
      });
      this->addNodes(&gpsPOIMenu, "Mark POI", TFTCYAN, GPS_MENU, [this]() {
        wifi_scan_obj.currentScanMode = GPS_POI;
        display_obj.tft.setCursor(0, TFT_HEIGHT / 2);
        display_obj.clearScreen();
        if (wifi_scan_obj.RunGPSInfo(true, false, true))
          display_obj.showCenterText("POI Logged", TFT_HEIGHT / 2);
        else
          display_obj.showCenterText("POI Log Failed", TFT_HEIGHT / 2);
        wifi_scan_obj.currentScanMode = WIFI_SCAN_OFF;
        delay(2000);
        this->changeMenu(&gpsPOIMenu, true);
      });

      // GPS Info Menu
      gpsInfoMenu.parentMenu = &gpsMenu;
      this->addNodes(&gpsInfoMenu, text09, TFTLIGHTGREY, 0, [this]() {
        if(wifi_scan_obj.currentScanMode != GPS_TRACKER)
          wifi_scan_obj.currentScanMode = WIFI_SCAN_OFF;
        wifi_scan_obj.StartScan(WIFI_SCAN_OFF);
        this->changeMenu(gpsInfoMenu.parentMenu, true);
      }); 
    }
  #endif

  // Settings menu
  // Device menu
  settingsMenu.parentMenu = &deviceMenu;
  this->addNodes(&settingsMenu, text09, TFTLIGHTGREY, 0, [this]() {
    changeMenu(settingsMenu.parentMenu, true);
  });
  for (int i = 0; i < settings_obj.getNumberSettings(); i++) {
    String settingName = settings_obj.setting_index_to_name(i);
    const char* type = this->callSetting(settingName.c_str());
    if (type && strcmp(type, "bool") == 0) {
      this->addNodes(&settingsMenu, settingName.c_str(), TFTLIGHTGREY, SETTINGS, [this, i, settingName]() {
          settings_obj.toggleSetting(settingName.c_str());
          this->callSetting(settingName.c_str());
          this->changeMenu(&specSettingMenu, true);
          this->displaySetting(settingName.c_str(), &settingsMenu, i + 1);
          wifi_scan_obj.force_pmkid = settings_obj.loadSetting<bool>(text_table4[5]);
          wifi_scan_obj.force_probe = settings_obj.loadSetting<bool>(text_table4[6]);
          wifi_scan_obj.save_pcap = settings_obj.loadSetting<bool>(text_table4[7]);
          wifi_scan_obj.ep_deauth = settings_obj.loadSetting<bool>("EPDeauth");
          wifi_scan_obj.channel_hop = settings_obj.loadSetting<bool>("ChanHop");
      }, settings_obj.loadSetting<bool>(settingName.c_str()));
    }
  }

  Serial.println("Finished settings nodes");

  // Specific setting menu
  specSettingMenu.parentMenu = &settingsMenu;
  addNodes(&specSettingMenu, text09, TFTLIGHTGREY, 0, [this]() {
    this->changeMenu(specSettingMenu.parentMenu, true);
  });

  // Web Update
  updateMenu.parentMenu = &deviceMenu;

  // Failed update menu
  failedUpdateMenu.parentMenu = &deviceMenu;
  this->addNodes(&failedUpdateMenu, text09, TFTLIGHTGREY, 0, [this]() {
    wifi_scan_obj.currentScanMode = WIFI_SCAN_OFF;
    this->changeMenu(failedUpdateMenu.parentMenu, true);
  });

  // Device info menu
  infoMenu.parentMenu = &deviceMenu;
  this->addNodes(&infoMenu, text09, TFTLIGHTGREY, 0, [this]() {
    wifi_scan_obj.currentScanMode = WIFI_SCAN_OFF;
    this->changeMenu(infoMenu.parentMenu, true);
  });

  Serial.println("Changing to main menu...");

  // Set the current menu to the mainMenu
  this->changeMenu(&mainMenu, true);

  this->initTime = millis();
}

//#if (!defined(HAS_ILI9341) && defined(HAS_BUTTONS))
#ifdef HAS_MINI_KB
  String MenuFunctions::miniKeyboard(Menu * targetMenu, bool do_pass) {
    // Prepare a char array and reset temp SSID string
    extern LinkedList<ssid>* ssids;

    String ret_val = "";

    bool pressed = true;

    wifi_scan_obj.current_mini_kb_ssid = "";

    #ifdef HAS_MINI_KB
      if (c_btn.isHeld()) {
        while (!c_btn.justReleased())
          delay(1);
      }
    #endif

    int str_len = wifi_scan_obj.alfa.length() + 1; 

    char char_array[str_len];

    wifi_scan_obj.alfa.toCharArray(char_array, str_len);

    #ifdef HAS_TOUCH
      uint16_t t_x = 0, t_y = 0;

    #endif

    // Button loop until hold center button
    #ifdef HAS_BUTTONS
      //#if !(defined(MARAUDER_V6) || defined(MARAUDER_V6_1) || defined(MARAUDER_CYD_MICRO))
        while(true) {
          // Keyboard functions for switch hardware
          #ifdef HAS_MINI_KB
            // Cycle char previous
            #ifdef HAS_L
              if ((l_btn.justPressed()) || (l_btn.isHeld())) {
                pressed = true;
                if (this->mini_kb_index > 0)
                  this->mini_kb_index--;
                else
                  this->mini_kb_index = str_len - 2;

                targetMenu->list->set(0, MenuNode{String(char_array[this->mini_kb_index]).c_str(), false, TFTCYAN, 0, true, NULL});
                this->buildButtons(targetMenu);

                while (!l_btn.justReleased()) {
                  l_btn.justPressed();
                  if (!l_btn.isHeld())
                    delay(1);
                  else
                    break;
                }
              }
            #endif

            // Cycle char next
            #ifdef HAS_R
              if ((r_btn.justPressed()) || (r_btn.isHeld())) {
                pressed = true;
                if (this->mini_kb_index < str_len - 2)
                  this->mini_kb_index++;
                else
                  this->mini_kb_index = 0;

                targetMenu->list->set(0, MenuNode{String(char_array[this->mini_kb_index]).c_str(), false, TFTCYAN, 0, true, NULL});
                this->buildButtons(targetMenu, 0, &char_array[this->mini_kb_index]);
                
                while (!r_btn.justReleased()) {
                  r_btn.justPressed();
                  if (!r_btn.isHeld())
                    delay(1);
                  else
                    break;
                }
              }
            #endif

            //// 5-WAY SWITCH STUFF
            // Add character
            #if (defined(HAS_D) && defined(HAS_R))
              if (d_btn.justPressed()) {
                pressed = true;
                wifi_scan_obj.current_mini_kb_ssid.concat(String(char_array[this->mini_kb_index]).c_str());
                while (!d_btn.justReleased())
                  delay(1);
              }
            #endif

            // Remove character
            #if (defined(HAS_U) && defined(HAS_L))
              if (u_btn.justPressed()) {
                pressed = true;
                wifi_scan_obj.current_mini_kb_ssid.remove(wifi_scan_obj.current_mini_kb_ssid.length() - 1);
                while (!u_btn.justReleased())
                  delay(1);
              }
            #endif

            //// PARTIAL SWITCH STUFF
            // Advance char or add char
            #if (defined(HAS_D) && !defined(HAS_R))
              if (d_btn.justPressed()) {
                bool was_held = false;
                pressed = true;
                while(!d_btn.justReleased()) {
                  d_btn.justPressed();

                  // Add letter to string
                  if (d_btn.isHeld()) {
                    wifi_scan_obj.current_mini_kb_ssid.concat(String(char_array[this->mini_kb_index]).c_str());
                    was_held = true;
                    break;
                  }
                }
                if (!was_held) {
                  if (this->mini_kb_index < str_len - 2)
                    this->mini_kb_index++;
                  else
                    this->mini_kb_index = 0;

                  targetMenu->list->set(0, MenuNode{String(char_array[this->mini_kb_index]).c_str(), false, TFTCYAN, 0, true, NULL});
                  this->buildButtons(targetMenu, 0, &char_array[this->mini_kb_index]);
                }
              }
            #endif

            // Prev char or remove char
            #if (defined(HAS_U) && !defined(HAS_L))
              if (u_btn.justPressed()) {
                bool was_held = false;
                pressed = true;
                while(!u_btn.justReleased()) {
                  u_btn.justPressed();

                  // Remove letter from string
                  if (u_btn.isHeld()) {
                    wifi_scan_obj.current_mini_kb_ssid.remove(wifi_scan_obj.current_mini_kb_ssid.length() - 1);
                    was_held = true;
                    break;
                  }
                }
                if (!was_held) {
                  if (this->mini_kb_index > 0)
                    this->mini_kb_index--;
                  else
                    this->mini_kb_index = str_len - 2;

                  targetMenu->list->set(0, MenuNode{String(char_array[this->mini_kb_index]).c_str(), false, TFTCYAN, 0, true, NULL});
                  this->buildButtons(targetMenu);
                }
              }
            #endif

            // Add SSID
            #if defined(HAS_C) && !defined(MARAUDER_CARDPUTER) && !defined(MARAUDER_CARDPUTER_ADV)
              if (c_btn.justPressed()) {
                while (!c_btn.justReleased()) {
                  c_btn.justPressed(); // Need to continue updating button hold status. My shitty library.

                  // Exit
                  if (c_btn.isHeld()) {
                    this->changeMenu(targetMenu->parentMenu);
                    return wifi_scan_obj.current_mini_kb_ssid;
                  }
                  delay(1);
                }

                if (!do_pass) {
                // If we have a string, add it to list of SSIDs
                  if (wifi_scan_obj.current_mini_kb_ssid != "") {
                    pressed = true;
                    ssid s = {wifi_scan_obj.current_mini_kb_ssid, random(1, 12), {random(256), random(256), random(256), random(256), random(256), random(256)}, false};
                    ssids->unshift(s);
                    wifi_scan_obj.current_mini_kb_ssid = "";
                  }
                }
              }
            #endif
          #endif

          #if defined(MARAUDER_CARDPUTER) || defined(MARAUDER_CARDPUTER_ADV)
            for (int i = 0; i < 95; i++) {
              if ((M5CardputerKeyboard._ascii_list[i] != '(') &&
                  (M5CardputerKeyboard._ascii_list[i] != '`')) {
                if (this->isKeyPressed(M5CardputerKeyboard._ascii_list[i])) {
                  pressed = true;
                  wifi_scan_obj.current_mini_kb_ssid.concat(M5CardputerKeyboard._ascii_list[i]);
                }
                if (this->isKeyPressed(KEY_BACKSPACE)) {
                  pressed = true;
                  wifi_scan_obj.current_mini_kb_ssid.remove(wifi_scan_obj.current_mini_kb_ssid.length() - 1);
                }
              }
            }

            if (!do_pass) {
              if (this->isKeyPressed('`')) {
                this->changeMenu(targetMenu->parentMenu, true);
                return wifi_scan_obj.current_mini_kb_ssid;
              }

              if (this->isKeyPressed('(')) {
                if (!do_pass) {
                  if (wifi_scan_obj.current_mini_kb_ssid != "") {
                    pressed = true;
                    ssid s = {wifi_scan_obj.current_mini_kb_ssid, random(1, 12), {random(256), random(256), random(256), random(256), random(256), random(256)}, false};
                    ssids->unshift(s);
                    wifi_scan_obj.current_mini_kb_ssid = "";
                  }
                }
              }
            }
            else {
              if (this->isKeyPressed('(')) {
                this->changeMenu(targetMenu->parentMenu, true);
                return wifi_scan_obj.current_mini_kb_ssid;
              }

              if (this->isKeyPressed('`')) {
                this->changeMenu(targetMenu->parentMenu, true);
                return "";
              }
            }
            
          #endif

          // Keyboard functions for touch hardware
          #ifdef HAS_TOUCH
            bool touched = display_obj.updateTouch(&t_x, &t_y);

            uint8_t menu_button = display_obj.menuButton(&t_x, &t_y, touched);

            // Cycle char previous
            if (menu_button == UP_BUTTON) {
              pressed = true;
              if (this->mini_kb_index > 0)
                this->mini_kb_index--;
              else
                this->mini_kb_index = str_len - 2;

              targetMenu->list->set(0, MenuNode{String(char_array[this->mini_kb_index]).c_str(), false, TFTCYAN, 0, true, NULL});
              this->buildButtons(targetMenu);
              while (display_obj.updateTouch(&t_x, &t_y) > 0)
                delay(1);
              display_obj.menuButton(&t_x, &t_y, display_obj.updateTouch(&t_x, &t_y));
            }

            // Cycle char next
            if (menu_button == DOWN_BUTTON) {
              pressed = true;
              if (this->mini_kb_index < str_len - 2)
                this->mini_kb_index++;
              else
                this->mini_kb_index = 0;

              targetMenu->list->set(0, MenuNode{String(char_array[this->mini_kb_index]).c_str(), false, TFTCYAN, 0, true, NULL});
              this->buildButtons(targetMenu, 0, &char_array[this->mini_kb_index]);
              while (display_obj.updateTouch(&t_x, &t_y) > 0)
                delay(1);
              display_obj.menuButton(&t_x, &t_y, display_obj.updateTouch(&t_x, &t_y));
            }

            //// 5-WAY SWITCH STUFF
            // Add character when select button is pressed
            if (menu_button == SELECT_BUTTON) {
              pressed = true;
              wifi_scan_obj.current_mini_kb_ssid.concat(String(char_array[this->mini_kb_index]).c_str());
              while (display_obj.updateTouch(&t_x, &t_y) > 0)
                delay(1);
              display_obj.menuButton(&t_x, &t_y, display_obj.updateTouch(&t_x, &t_y));
            }

            // Remove character when select button is held
            if ((display_obj.isTouchHeld()) && (display_obj.menuButton(&t_x, &t_y, touched, true) == SELECT_BUTTON)) {
              pressed = true;
              wifi_scan_obj.current_mini_kb_ssid.remove(wifi_scan_obj.current_mini_kb_ssid.length() - 1);
              while (display_obj.menuButton(&t_x, &t_y, display_obj.updateTouch(&t_x, &t_y)) < 0)
                delay(1);
            }

            //// PARTIAL SWITCH STUFF
            // Advance char or add char
            #if (defined(HAS_D) && !defined(HAS_R))
              if (d_btn.justPressed()) {
                bool was_held = false;
                pressed = true;
                while(!d_btn.justReleased()) {
                  d_btn.justPressed();

                  // Add letter to string
                  if (d_btn.isHeld()) {
                    wifi_scan_obj.current_mini_kb_ssid.concat(String(char_array[this->mini_kb_index]).c_str());
                    was_held = true;
                    break;
                  }
                }
                if (!was_held) {
                  if (this->mini_kb_index < str_len - 2)
                    this->mini_kb_index++;
                  else
                    this->mini_kb_index = 0;

                  targetMenu->list->set(0, MenuNode{String(char_array[this->mini_kb_index]).c_str(), false, TFTCYAN, 0, true, NULL});
                  this->buildButtons(targetMenu, 0, &char_array[this->mini_kb_index]);
                }
              }
            #endif

            // Prev char or remove char
            #if (defined(HAS_U) && !defined(HAS_L))
              if (u_btn.justPressed()) {
                bool was_held = false;
                pressed = true;
                while(!u_btn.justReleased()) {
                  u_btn.justPressed();

                  // Remove letter from string
                  if (u_btn.isHeld()) {
                    wifi_scan_obj.current_mini_kb_ssid.remove(wifi_scan_obj.current_mini_kb_ssid.length() - 1);
                    was_held = true;
                    break;
                  }
                }
                if (!was_held) {
                  if (this->mini_kb_index > 0)
                    this->mini_kb_index--;
                  else
                    this->mini_kb_index = str_len - 2;

                  targetMenu->list->set(0, MenuNode{String(char_array[this->mini_kb_index]).c_str(), false, TFTCYAN, 0, true, NULL});
                  this->buildButtons(targetMenu);
                }
              }
            #endif

            // Exit if UP button is held
            if ((display_obj.isTouchHeld()) && (display_obj.menuButton(&t_x, &t_y, touched, true) == UP_BUTTON)) {
              display_obj.clearScreen();
              while (display_obj.menuButton(&t_x, &t_y, display_obj.updateTouch(&t_x, &t_y)) < 0)
                delay(1);

              // Reset the touch keys so we don't activate the keys when we go back
              display_obj.menuButton(&t_x, &t_y, display_obj.updateTouch(&t_x, &t_y));
              this->changeMenu(targetMenu->parentMenu, true);
              return wifi_scan_obj.current_mini_kb_ssid;
            }

            // If the screen is touched but none of the keys are used, don't refresh display
            if (menu_button < 0)
              pressed = false;

          #endif

          // Display info on screen
          if (pressed) {
            this->displayCurrentMenu();
            display_obj.tft.setTextWrap(false);
            display_obj.tft.fillRect(0, SCREEN_HEIGHT / 3, SCREEN_WIDTH, STATUS_BAR_WIDTH, TFT_BLACK);
            display_obj.tft.fillRect(0, SCREEN_HEIGHT / 3 + TEXT_HEIGHT * 2, SCREEN_WIDTH, STATUS_BAR_WIDTH, TFT_BLACK);
            display_obj.tft.setCursor(0, SCREEN_HEIGHT / 3);
            display_obj.tft.setTextColor(TFT_CYAN, TFT_BLACK);
            display_obj.tft.println(wifi_scan_obj.current_mini_kb_ssid + "\n");
            display_obj.tft.setTextColor(TFT_GREEN, TFT_BLACK);

            display_obj.tft.println(ssids->get(0).essid);

            display_obj.tft.setTextColor(TFT_ORANGE, TFT_BLACK);
            #ifdef HAS_MINI_KB
              #if !defined(MARAUDER_CARDPUTER) && !defined(MARAUDER_CARDPUTER_ADV)
              display_obj.tft.println("U/D - Rem/Add Char");
              display_obj.tft.println("L/R - Prev/Nxt Char");
              #endif
              if (!do_pass) {
                #if defined(MARAUDER_CARDPUTER) || defined(MARAUDER_CARDPUTER_ADV)
                  display_obj.tft.println("Enter - Save");
                  display_obj.tft.println("Esc - Exit");
                #else
                  display_obj.tft.println("C - Save");
                  display_obj.tft.println("C(Hold) - Exit");
                #endif
              }
              else {
                #if defined(MARAUDER_CARDPUTER) || defined(MARAUDER_CARDPUTER_ADV)
                  display_obj.tft.println("Enter - Enter");
                #else
                  display_obj.tft.println("C(Hold) - Enter");
                #endif
              }
            #endif

            #ifdef HAS_TOUCH
              display_obj.tft.println("U/D - Prev/Nxt Char");
              display_obj.tft.println("C - Add Char");
              display_obj.tft.println("C(Hold) - Rem Char");
              display_obj.tft.println("U(Hold) - Enter");
            #endif
            pressed = false;
          }
        }
      //#endif
    #endif
  }
#endif

void MenuFunctions::setupSDFileList(bool update) {
  sd_obj.sd_files->clear();

  delete sd_obj.sd_files;

  sd_obj.sd_files = new LinkedList<String>();

  if (!update)
    sd_obj.listDirToLinkedList(sd_obj.sd_files);
  else
    sd_obj.listDirToLinkedList(sd_obj.sd_files, "/", ".bin");
}

void MenuFunctions::buildSDFileMenu(bool update) {
  this->setupSDFileList(update);

  sdDeleteMenu.list->clear();
  delete sdDeleteMenu.list;
  sdDeleteMenu.list = new LinkedList<MenuNode>();

  if (!update)
    sdDeleteMenu.name = "SD Files";
  else
    sdDeleteMenu.name = "Bin Files";

  this->addNodes(&sdDeleteMenu, text09, TFTLIGHTGREY, 0, [this]() {
    this->changeMenu(sdDeleteMenu.parentMenu, true);
  });

  if (!update) {
    this->addNodes(&sdDeleteMenu, "Delete Selected", TFTORANGE, 0, [this]() {
      for (int x = 0; x < sd_obj.sd_files->size(); x++) {
        if (current_menu->list->get(x + 2).selected) {
          if (sd_obj.removeFile("/" + sd_obj.sd_files->get(x))) {
            Serial.println("Deleted /" + sd_obj.sd_files->get(x));
            display_obj.clearScreen();
            display_obj.tft.setTextWrap(false);
            display_obj.tft.setCursor(0, SCREEN_HEIGHT / 3);
            display_obj.tft.setTextColor(TFT_CYAN, TFT_BLACK);
            display_obj.tft.println("Deleting /" + sd_obj.sd_files->get(x) + "...");
          }
        }
      }
      this->buildSDFileMenu();
      this->changeMenu(&sdDeleteMenu, true);
    });
  }

  if (!update) {
    for (int x = 0; x < sd_obj.sd_files->size(); x++) {
      this->addNodes(&sdDeleteMenu, sd_obj.sd_files->get(x).c_str(), TFTCYAN, SD_UPDATE, [this, x]() {
        // Change selection status of menu node
        MenuNode new_node = current_menu->list->get(x + 2);
        new_node.selected = !current_menu->list->get(x + 2).selected;
        current_menu->list->set(x + 2, new_node);
      });
    }
  }
  else {
    for (int x = 0; x < sd_obj.sd_files->size(); x++) {
      this->addNodes(&sdDeleteMenu, sd_obj.sd_files->get(x).c_str(), TFTCYAN, SD_UPDATE, [this, x]() {
        wifi_scan_obj.currentScanMode = OTA_UPDATE;
        this->changeMenu(&failedUpdateMenu, true);
        sd_obj.runUpdate("/" + sd_obj.sd_files->get(x));
      });
    }
  }
}


// Function to add MenuNodes to a menu
void MenuFunctions::addNodes(Menu * menu, const char* name, uint8_t color, int place, std::function<void()> callable, bool selected)
{
  //Serial.println("Building node: " + name);
  menu->list->add(MenuNode{String(name), false, color, place, selected, callable});
}

void MenuFunctions::setGraphScale(float scale) {
  this->_graph_scale = scale;
}

float MenuFunctions::calculateGraphScale(uint8_t value) {
  if ((value * this->_graph_scale < GRAPH_VERT_LIM) && (value * this->_graph_scale > GRAPH_VERT_LIM * 0.75)) {
    return this->_graph_scale;  // No scaling needed if the value is within the limit
  }

  if (value < GRAPH_VERT_LIM)
    return 1.0;

  // Calculate the multiplier proportionally
  return (0.75 * GRAPH_VERT_LIM) / value;
}

float MenuFunctions::calculateGraphScale(int16_t value) {
  if ((value * this->_graph_scale < GRAPH_VERT_LIM) && (value * this->_graph_scale > GRAPH_VERT_LIM * 0.75)) {
    return this->_graph_scale;  // No scaling needed if the value is within the limit
  }

  if (value < GRAPH_VERT_LIM)
    return 1.0;

  // Calculate the multiplier proportionally
  return (0.75 * GRAPH_VERT_LIM) / value;
}

float MenuFunctions::graphScaleCheck(const int16_t array[SCREEN_WIDTH]) {
  int16_t maxValue = 0;

  // Iterate through the array to find the highest value
  for (int16_t i = 0; i < SCREEN_WIDTH; i++) {
    if (array[i] > maxValue) {
      maxValue = array[i];
    }
  }

  // If the highest value exceeds GRAPH_VERT_LIM, call calculateMultiplier
  if (maxValue > GRAPH_VERT_LIM) {
    return this->calculateGraphScale(maxValue);
  }

  // If the highest value does not exceed GRAPH_VERT_LIM, return 1.0
  return 1.0;
}

float MenuFunctions::graphScaleCheckSmall(const uint8_t array[CHAN_PER_PAGE]) {
  uint8_t maxValue = 0;

  // Iterate through the array to find the highest value
  for (uint8_t i = 0; i < CHAN_PER_PAGE; i++) {
    if (array[i] > maxValue) {
      maxValue = array[i];
    }
  }

  // If the highest value exceeds GRAPH_VERT_LIM, call calculateMultiplier
  if (maxValue > GRAPH_VERT_LIM) {
    return this->calculateGraphScale(maxValue);
  }

  // If the highest value does not exceed GRAPH_VERT_LIM, return 1.0
  return 1.0;
}

void MenuFunctions::drawMaxLine(int16_t value, uint16_t color) {
  display_obj.tft.drawLine(0, TFT_HEIGHT - (value * this->_graph_scale), TFT_WIDTH, TFT_HEIGHT - (value * this->_graph_scale), color);
  display_obj.tft.setCursor(0, TFT_HEIGHT - (value * this->_graph_scale));
  display_obj.tft.setTextColor(color, TFT_BLACK);
  display_obj.tft.setTextSize(1);
  display_obj.tft.println((String)(value / BASE_MULTIPLIER));
}

void MenuFunctions::drawMaxLine(uint8_t value, uint16_t color) {
  //display_obj.tft.drawLine(0, TFT_HEIGHT - (value * this->_graph_scale), TFT_WIDTH, TFT_HEIGHT - (value * this->_graph_scale), color);
  display_obj.tft.setCursor(0, TFT_HEIGHT - (value * this->_graph_scale));
  display_obj.tft.setTextColor(color, TFT_BLACK);
  display_obj.tft.setTextSize(1);
  display_obj.tft.println((String)value);
}

void MenuFunctions::drawGraphSmall(uint8_t *values) {
  uint8_t maxValue = 0;
  //(i + (CHAN_PER_PAGE * (this->activity_page - 1)))

  int bar_width = SCREEN_WIDTH / (CHAN_PER_PAGE * 2);
  //display_obj.tft.fillRect(0, TFT_HEIGHT / 2 + 1, SCREEN_WIDTH, (TFT_HEIGHT / 2) + 1, TFT_BLACK);

  #ifndef HAS_DUAL_BAND
    for (int i = 1; i < CHAN_PER_PAGE + 1; i++) {
      int targ_val = i + (CHAN_PER_PAGE * (wifi_scan_obj.activity_page - 1)) - 1;
      int x_mult = (i * 2) - 1;
      int x_coord = (SCREEN_WIDTH / (CHAN_PER_PAGE * 2)) * (x_mult - 1);

      if (values[targ_val] > maxValue) {
        maxValue = values[targ_val];
      }

      if (values[targ_val] * this->_graph_scale <= GRAPH_VERT_LIM) {
        display_obj.tft.fillRect(x_coord, SCREEN_HEIGHT / 2 + 1, bar_width, SCREEN_HEIGHT / 2 + 1, TFT_BLACK);
        display_obj.tft.fillRect(x_coord, SCREEN_HEIGHT - (values[targ_val] * this->_graph_scale), bar_width, values[targ_val] * this->_graph_scale, TFT_CYAN);
      }

      display_obj.tft.drawLine(x_coord - 2, SCREEN_HEIGHT - GRAPH_VERT_LIM - (CHAR_WIDTH * 2), x_coord - 2, SCREEN_HEIGHT, TFT_WHITE);
    }
  #else
    for (int i = 1; i < CHAN_PER_PAGE + 1; i++) {
      int targ_val = i + (CHAN_PER_PAGE * (wifi_scan_obj.activity_page - 1)) - 1;
      int x_mult = (i * 2) - 1;
      int x_coord = (SCREEN_WIDTH / (CHAN_PER_PAGE * 2)) * (x_mult - 1);

      if (values[targ_val] > maxValue) {
        maxValue = values[targ_val];
      }

      if (values[targ_val] * this->_graph_scale <= GRAPH_VERT_LIM) {
        display_obj.tft.fillRect(x_coord, SCREEN_HEIGHT / 2 + 1, bar_width + 3, SCREEN_HEIGHT / 2 + 1, TFT_BLACK);
        display_obj.tft.fillRect(x_coord, SCREEN_HEIGHT - (values[targ_val] * this->_graph_scale), bar_width, values[targ_val] * this->_graph_scale, TFT_CYAN);
      }

      display_obj.tft.drawLine(x_coord - 2, SCREEN_HEIGHT - GRAPH_VERT_LIM - (CHAR_WIDTH * 2), x_coord - 2, SCREEN_HEIGHT, TFT_WHITE);
    }
  #endif

  this->drawMaxLine(maxValue, TFT_GREEN); // Draw max
}

void MenuFunctions::drawGraph(int16_t *values) {
  #if !defined(MARAUDER_CARDPUTER) && !defined(MARAUDER_CARDPUTER_ADV)
    int width = TFT_WIDTH;
  #else
    int width = SCREEN_WIDTH;
  #endif

  int16_t maxValue = 0;
  int total = 0;
  for (int i = width - 1; i >= 0; i--) {
    if (values[i] >= 0) {
      total = total + values[i];
      if (values[i] > maxValue) {
        maxValue = values[i];
      }
      #if !defined(MARAUDER_CARDPUTER) && !defined(MARAUDER_CARDPUTER_ADV)
        display_obj.tft.drawLine(i, TFT_HEIGHT, i, TFT_HEIGHT - GRAPH_VERT_LIM, TFT_BLACK);
        display_obj.tft.drawLine(i, TFT_HEIGHT, i, TFT_HEIGHT - (values[i] * this->_graph_scale), TFT_CYAN);
      #else
        display_obj.tft.drawLine(i, TFT_WIDTH, i, TFT_WIDTH - GRAPH_VERT_LIM, TFT_BLACK);
        display_obj.tft.drawLine(i, TFT_WIDTH, i, TFT_WIDTH - (values[i] * this->_graph_scale), TFT_CYAN);
        display_obj.tft.setCursor(0, 0);
        display_obj.tft.setTextColor(TFT_WHITE, TFT_BLACK);
      #endif
    }
    else {
      int16_t ch_val = values[i] * -1;
      #if !defined(MARAUDER_CARDPUTER) && !defined(MARAUDER_CARDPUTER_ADV)
        display_obj.tft.drawLine(i, TFT_HEIGHT, i, TFT_HEIGHT - GRAPH_VERT_LIM, TFT_BLACK);
        display_obj.tft.drawLine(i, TFT_HEIGHT, i, TFT_HEIGHT - GRAPH_VERT_LIM, TFT_RED);
        display_obj.tft.setCursor(i, TFT_HEIGHT - GRAPH_VERT_LIM);
      #else
        display_obj.tft.drawLine(i, TFT_WIDTH, i, TFT_WIDTH - GRAPH_VERT_LIM, TFT_BLACK);
        display_obj.tft.drawLine(i, TFT_WIDTH, i, TFT_WIDTH - GRAPH_VERT_LIM, TFT_RED);
        display_obj.tft.setCursor(i, TFT_WIDTH - GRAPH_VERT_LIM);
      #endif
      display_obj.tft.setTextColor(TFT_BLACK, TFT_RED);
      display_obj.tft.setTextSize(1);
      display_obj.tft.println((String)ch_val);
    }
  }

  this->drawMaxLine(maxValue, TFT_GREEN); // Draw max
  this->drawMaxLine((int16_t)(total / TFT_WIDTH), TFT_ORANGE); // Draw average
}

void MenuFunctions::renderGraphUI(uint8_t scan_mode) {
  display_obj.tft.setTextColor(TFT_WHITE, TFT_BLACK);
  if (scan_mode == WIFI_SCAN_CHAN_ANALYZER)
    display_obj.tft.drawCentreString("Frames/" + (String)BANNER_TIME + "ms", SCREEN_WIDTH / 2, SCREEN_HEIGHT - GRAPH_VERT_LIM - (CHAR_WIDTH * 2), 1);
  else if (scan_mode == BT_SCAN_ANALYZER)
    display_obj.tft.drawCentreString("BLE Beacons/" + (String)BANNER_TIME + "ms", SCREEN_WIDTH / 2, SCREEN_HEIGHT - GRAPH_VERT_LIM - (CHAR_WIDTH * 2), 1);
  display_obj.tft.drawLine(0, SCREEN_HEIGHT - GRAPH_VERT_LIM - 1, SCREEN_WIDTH, SCREEN_HEIGHT - GRAPH_VERT_LIM - 1, TFT_WHITE);
  display_obj.tft.setCursor(0, SCREEN_HEIGHT - GRAPH_VERT_LIM - (CHAR_WIDTH * 8));
  display_obj.tft.setTextSize(1);
  display_obj.tft.setTextColor(TFT_GREEN, TFT_BLACK);
  display_obj.tft.println("Max");
  display_obj.tft.setTextColor(TFT_ORANGE, TFT_BLACK);
  display_obj.tft.println("Average");
  display_obj.tft.setTextColor(TFT_RED, TFT_BLACK);
  if (scan_mode != BT_SCAN_ANALYZER)
    display_obj.tft.println("Channel Marker");
}

uint16_t MenuFunctions::getColor(uint16_t color) {
  if (color == TFTWHITE) return TFT_WHITE;
  else if (color == TFTCYAN) return TFT_CYAN;
  else if (color == TFTBLUE) return TFT_BLUE;
  else if (color == TFTRED) return TFT_RED;
  else if (color == TFTGREEN) return TFT_GREEN;
  else if (color == TFTGREY) return TFT_LIGHTGREY;
  else if (color == TFTGRAY) return TFT_LIGHTGREY;
  else if (color == TFTMAGENTA) return TFT_MAGENTA;
  else if (color == TFTVIOLET) return TFT_VIOLET;
  else if (color == TFTORANGE) return TFT_ORANGE;
  else if (color == TFTYELLOW) return TFT_YELLOW;
  else if (color == TFTLIGHTGREY) return TFT_LIGHTGREY;
  else if (color == TFTPURPLE) return TFT_PURPLE;
  else if (color == TFTNAVY) return TFT_NAVY;
  else if (color == TFTSILVER) return TFT_SILVER;
  else if (color == TFTDARKGREY) return TFT_DARKGREY;
  else if (color == TFTSKYBLUE) return TFT_SKYBLUE;
  else if (color == TFTLIME) return 0x97e0;
  else return color;
}

// Function to change menu
void MenuFunctions::changeMenu(Menu* menu, bool simple_change) {
  if (!simple_change) {
    //display_obj.initScrollValues();
    //display_obj.setupScrollArea(TOP_FIXED_AREA, BOT_FIXED_AREA);
    display_obj.init();

    #ifdef HAS_ILI9341
      extern void backlightOn();
	  backlightOn();
    #endif
  }
  current_menu = menu;

  current_menu->selected = 0;

  #ifdef HAS_MINI_SCREEN
    this->resetMiniMenuMarquee();
  #endif

  buildButtons(menu);

  displayCurrentMenu();

  //#ifdef MARAUDER_V8
  //  digitalWrite(TFT_BL, HIGH);
  //#endif
}

void MenuFunctions::buildButtons(Menu *menu, int starting_index, const char* button_name) {
  if (menu->list == NULL || menu->list->size() == 0)
      return;

  if (starting_index >= menu->list->size())
    starting_index = menu->list->size() - BUTTON_SCREEN_LIMIT;
  if (starting_index < 0)
    starting_index = 0;

  this->menu_start_index = starting_index;

  uint8_t visible_buttons = min(BUTTON_SCREEN_LIMIT, menu->list->size() - starting_index);

  for (uint8_t i = 0; i < visible_buttons; i++) {
    MenuNode node = menu->list->get(starting_index + i);
    uint16_t color = (node.icon == SETTINGS && node.color == TFTLIGHTGREY) ? (node.selected ? TFT_GREEN : TFT_RED) : this->getColor(node.color);

    char buf[64];

    if (button_name != nullptr && button_name[0] != '\0') {
      strncpy(buf, button_name, sizeof(buf));
      buf[sizeof(buf) - 1] = '\0';
    } else {
      node.name.toCharArray(buf, sizeof(buf));
    }

    display_obj.key[i].initButton(&display_obj.tft,
                                  KEY_X,
                                  KEY_Y + i * (KEY_H + KEY_SPACING_Y),
                                  KEY_W,
                                  KEY_H,
                                  TFT_BLACK,
                                  TFT_BLACK,
                                  color,
                                  buf,
                                  KEY_TEXTSIZE);

    #if defined(MARAUDER_CARDPUTER) || defined(MARAUDER_CARDPUTER_ADV)
      display_obj.key[i].setLabelDatum(BUTTON_PADDING - (KEY_W / 2), 4, ML_DATUM);
    #else
      display_obj.key[i].setLabelDatum(BUTTON_PADDING - (KEY_W / 2), 2, ML_DATUM);
    #endif
  }

  for (int i = BUTTON_ARRAY_LEN; i < BUTTON_ARRAY_LEN + 3; i++) {
    uint16_t x = TFT_WIDTH / 2;
    uint16_t y = TFT_HEIGHT / 3 * (i - BUTTON_ARRAY_LEN) + ((TFT_HEIGHT / 3) / 2);
    uint16_t w = TFT_WIDTH;
    uint16_t h = TFT_HEIGHT / 3 - 1;

    display_obj.key[i].initButton(&display_obj.tft,
                                  x,
                                  y,
                                  w,
                                  h,
                                  TFT_LIGHTGREY,
                                  TFT_BLACK,
                                  TFT_BLACK,
                                  "Chicken",
                                  1);
  }
}

void MenuFunctions::displayCurrentMenu(int start_index)
{
  //Serial.println(F("Displaying current menu..."));
  display_obj.clearScreen();
  #ifdef MARAUDER_MINI_V3
    String mini_title = current_menu == &mainMenu ? "Marauder Eternal" : current_menu->name;
    mini_title.trim();
    if (mini_title.length() > 20)
      mini_title = mini_title.substring(0, 19) + "~";
    display_obj.updateBanner(mini_title);
  #else
    display_obj.updateBanner(current_menu->name);
  #endif
  display_obj.tft.setTextColor(TFT_LIGHTGREY, TFT_DARKGREY);
  this->drawStatusBar();

  if (current_menu->list != NULL)
  {
    #ifdef HAS_FULL_SCREEN
      display_obj.tft.setFreeFont(MENU_FONT);
    #endif

    #ifdef HAS_MINI_SCREEN
      display_obj.tft.setFreeFont(NULL);
      display_obj.tft.setTextSize(1);
    #endif

    for (uint16_t i = start_index; i < min(start_index + BUTTON_SCREEN_LIMIT, current_menu->list->size()); i++)
    {
      if (!current_menu || !current_menu->list || i >= current_menu->list->size())
        continue;
      uint16_t color = this->getColor(current_menu->list->get(i).color);
      #ifdef HAS_FULL_SCREEN
        bool is_setting_node = (current_menu->list->get(i).icon == SETTINGS && current_menu->list->get(i).color == TFTLIGHTGREY);
        if (is_setting_node && current_menu->selected == i) {
          uint16_t setting_color = current_menu->list->get(i).selected ? TFT_GREEN : TFT_RED;
          display_obj.key[i - start_index].initButton(&display_obj.tft, KEY_X, KEY_Y + (i - start_index) * (KEY_H + KEY_SPACING_Y), KEY_W, KEY_H, TFT_BLACK, TFT_LIGHTGREY, setting_color, (char*)"", KEY_TEXTSIZE);
          display_obj.key[i - start_index].drawButton(false, current_menu->list->get(i).name);
          display_obj.tft.drawXBitmap(0,
                                      KEY_Y + (i - start_index) * (KEY_H + KEY_SPACING_Y) - (ICON_H / 2),
                                      menu_icons[current_menu->list->get(i).icon],
                                      ICON_W,
                                      ICON_H,
                                      TFT_BLACK,
                                      TFT_LIGHTGREY);
        } else if ((!is_setting_node && current_menu->list->get(i).selected) || (current_menu->selected == i)) {
          display_obj.key[i - start_index].drawButton(true, current_menu->list->get(i).name);
          if ((current_menu->list->get(i).name != text09) && (current_menu->list->get(i).icon != 255))
            display_obj.tft.drawXBitmap(0,
                                        KEY_Y + (i - start_index) * (KEY_H + KEY_SPACING_Y) - (ICON_H / 2),
                                        menu_icons[current_menu->list->get(i).icon],
                                        ICON_W,
                                        ICON_H,
                                        TFT_BLACK,
                                        color);
        } else {
          display_obj.key[i - start_index].drawButton(false, current_menu->list->get(i).name);
          if ((current_menu->list->get(i).name != text09) && (current_menu->list->get(i).icon != 255))
            display_obj.tft.drawXBitmap(0,
                                        KEY_Y + (i - start_index) * (KEY_H + KEY_SPACING_Y) - (ICON_H / 2),
                                        menu_icons[current_menu->list->get(i).icon],
                                        ICON_W,
                                        ICON_H,
                                        TFT_BLACK,
                                        is_setting_node ? TFT_LIGHTGREY : color);
        }

      #endif

      #ifdef HAS_MINI_SCREEN
        const uint16_t text_offset = (current_menu->selected == i &&
                                      mini_marquee_menu == current_menu &&
                                      mini_marquee_index == i)
                                         ? mini_marquee_offset : 0;
        if ((current_menu->selected == i) || ((current_menu->list->get(i).icon != SETTINGS || current_menu->list->get(i).color != TFTLIGHTGREY) && current_menu->list->get(i).selected))
          this->drawMiniMenuButton(i - start_index, i, true, text_offset);
        else 
          this->drawMiniMenuButton(i - start_index, i, false);
      #endif
    }
    display_obj.tft.setFreeFont(NULL);

    #ifdef MARAUDER_MINI_V3
      if (current_menu != &mainMenu && current_menu != &wifiMenu &&
          current_menu != &bluetoothMenu && current_menu->list->size() > BUTTON_SCREEN_LIMIT) {
        constexpr int16_t track_y = 22;
        constexpr int16_t track_h = 100;
        display_obj.tft.fillRoundRect(126, track_y, 2, track_h, 1, MINI_UI_BORDER);
        const int16_t thumb_h = max((int16_t)10,
            (int16_t)((track_h * BUTTON_SCREEN_LIMIT) / current_menu->list->size()));
        const int16_t max_start = current_menu->list->size() - BUTTON_SCREEN_LIMIT;
        const int16_t thumb_y = track_y + (max_start > 0
            ? ((track_h - thumb_h) * menu_start_index) / max_start : 0);
        display_obj.tft.fillRoundRect(126, thumb_y, 2, thumb_h, 1, MINI_UI_ACCENT);
      }
    #endif
  }

  this->displayMenuButtons();
}

// ============================================================
// BRIGHTNESS ADJUSTMENT MODE
// Hold top/bottom zone 1.5s to enter. TAP TOP = brighter, TAP BOTTOM = dimmer.
// TAP MIDDLE or wait 3s = save & exit.
// ============================================================
#if !defined(HAS_MINI_SCREEN)
  void MenuFunctions::brightnessMode() {
    extern void brightnessSave(uint8_t level);
    extern void brightnessPreview(uint8_t level);
    extern uint8_t getBrightnessLevel();

    const uint8_t levels[] = {26, 51, 77, 102, 128, 153, 179, 204, 230, 255};
    const uint8_t numLevels = 10;
    uint8_t level = getBrightnessLevel();

    display_obj.tft.fillScreen(TFT_BLACK);
    display_obj.tft.setTextColor(TFT_CYAN, TFT_BLACK);
    display_obj.tft.drawCentreString("BRIGHTNESS", TFT_WIDTH/2, 30, 2);

    display_obj.tft.setTextColor(TFT_DARKGREY, TFT_BLACK);
    display_obj.tft.drawCentreString("TAP TOP = BRIGHTER", TFT_WIDTH/2, 10, 1);
    display_obj.tft.drawCentreString("TAP BOTTOM = DIMMER", TFT_WIDTH/2, TFT_HEIGHT - 20, 1);
    display_obj.tft.setTextColor(TFT_RED, TFT_BLACK);
    display_obj.tft.drawCentreString("TAP MIDDLE or WAIT 3s = SAVE", TFT_WIDTH/2, TFT_HEIGHT/2 + 50, 1);

    auto drawBar = [&]() {
      uint16_t barX = 30, barY = TFT_HEIGHT/2 - 25, barW = TFT_WIDTH - 60, barH = 30;
      display_obj.tft.drawRect(barX, barY, barW, barH, TFT_WHITE);
      uint16_t fillW = (barW - 4) * (level + 1) / numLevels;
      display_obj.tft.fillRect(barX + 2, barY + 2, barW - 4, barH - 4, TFT_BLACK);
      display_obj.tft.fillRect(barX + 2, barY + 2, fillW, barH - 4, TFT_CYAN);
      display_obj.tft.fillRect(0, barY + barH + 5, TFT_WIDTH, 20, TFT_BLACK);
      display_obj.tft.setTextColor(TFT_WHITE, TFT_BLACK);
      String pct = String(levels[level] * 100 / 255) + "%";
      display_obj.tft.drawCentreString(pct, TFT_WIDTH/2, barY + barH + 8, 2);
    };
    drawBar();

    uint16_t zoneUp = TFT_HEIGHT * 25 / 100;
    uint16_t zoneDown = TFT_HEIGHT * 75 / 100;
    uint32_t lastTouch = millis();

    while (true) {
      // Auto-save after 3s of no touch
      if (millis() - lastTouch >= 3000) {
        brightnessSave(level);
        break;
      }

      uint16_t tx, ty;
      if (display_obj.updateTouch(&tx, &ty)) {
        lastTouch = millis();
        // Wait for release
        while (display_obj.updateTouch(&tx, &ty)) delay(10);

        if (ty < zoneUp) {
          if (level < numLevels - 1) {
            level++;
            brightnessPreview(level);
            drawBar();
          }
        } else if (ty >= zoneDown) {
          if (level > 0) {
            level--;
            brightnessPreview(level);
            drawBar();
          }
        } else {
          // Middle = save now
          brightnessSave(level);
          break;
        }
        delay(150);
      }
      delay(30);
    }

    this->changeMenu(current_menu, true);
  }
#elif defined(MARAUDER_MINI_V3)
  void MenuFunctions::brightnessMode() {
    extern void brightnessPreview(uint8_t level);
    extern void brightnessSave(uint8_t level);
    extern uint8_t getBrightnessLevel();

    const uint8_t levels[] = {26, 51, 77, 102, 128,
                              153, 179, 204, 230, 255};
    const uint8_t numLevels = sizeof(levels) / sizeof(levels[0]);
    uint8_t level = min(getBrightnessLevel(),
                        static_cast<uint8_t>(numLevels - 1));
    auto buttonDown = [](Switches& button) {
      const int state = digitalRead(button.getPin());
      return button.getPullup() ? state == LOW : state == HIGH;
    };

    display_obj.tft.fillScreen(TFT_BLACK);
    display_obj.tft.setFreeFont(NULL);
    display_obj.tft.setTextSize(1);
    display_obj.tft.setTextWrap(false);
    display_obj.tft.setTextColor(TFT_CYAN, TFT_BLACK);
    display_obj.tft.drawCentreString("BRIGHTNESS", TFT_WIDTH / 2, 8, 1);

    auto drawLevel = [&]() {
      const int16_t barX = 12;
      const int16_t barY = 31;
      const int16_t barW = TFT_WIDTH - 24;
      const int16_t barH = 20;
      const int16_t fillW =
          (barW - 4) * static_cast<int16_t>(level + 1) / numLevels;

      display_obj.tft.fillRect(0, 27, TFT_WIDTH, 54, TFT_BLACK);
      display_obj.tft.drawRoundRect(barX, barY, barW, barH, 3, TFT_DARKGREY);
      display_obj.tft.fillRoundRect(barX + 2, barY + 2, fillW, barH - 4,
                                    2, TFT_CYAN);
      display_obj.tft.setTextColor(TFT_WHITE, TFT_BLACK);
      const String value = String(levels[level] * 100 / 255) + "%  " +
                           String(level + 1) + "/" + String(numLevels);
      display_obj.tft.drawCentreString(value, TFT_WIDTH / 2, 60, 2);
    };

    display_obj.tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
    display_obj.tft.drawCentreString("UP/RIGHT: BRIGHTER", TFT_WIDTH / 2, 89, 1);
    display_obj.tft.drawCentreString("DOWN/LEFT: DIMMER", TFT_WIDTH / 2, 102, 1);
    display_obj.tft.setTextColor(TFT_GREEN, TFT_BLACK);
    display_obj.tft.drawCentreString("CENTER: SAVE", TFT_WIDTH / 2, 115, 1);
    drawLevel();

    while (buttonDown(c_btn)) {
      c_btn.justPressed();
      delay(10);
    }
    c_btn.justPressed();

    while (true) {
      bool changed = false;
      if (u_btn.justPressed() || r_btn.justPressed()) {
        if (level < numLevels - 1) {
          level++;
          changed = true;
        }
      }
      else if (d_btn.justPressed() || l_btn.justPressed()) {
        if (level > 0) {
          level--;
          changed = true;
        }
      }
      else if (c_btn.justPressed()) {
        brightnessSave(level);
        while (buttonDown(c_btn)) {
          c_btn.justPressed();
          delay(10);
        }
        c_btn.justPressed();
        break;
      }

      if (changed) {
        brightnessPreview(level);
        drawLevel();
      }
      delay(20);
    }

    this->changeMenu(current_menu, true);
  }
#endif

#endif
