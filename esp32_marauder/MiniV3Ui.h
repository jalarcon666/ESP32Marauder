#pragma once

#if defined(MARAUDER_MINI_V3) && defined(HAS_SCREEN)

#include <Arduino.h>
#include <TFT_eSPI.h>

namespace MiniV3Ui {

constexpr uint16_t kBackground = TFT_BLACK;
constexpr uint16_t kSurface = 0x18E3;
constexpr uint16_t kBorder = 0x31A6;
constexpr uint16_t kAccent = 0x733F;
constexpr uint16_t kMuted = 0xA514;
constexpr int16_t kHeaderHeight = 31;
constexpr int16_t kFooterHeight = 13;

template <typename Surface>
inline void begin(Surface& surface) {
  surface.setFreeFont(NULL);
  surface.setTextFont(1);
  surface.setTextSize(1);
  surface.setTextWrap(false);
  surface.setTextDatum(TL_DATUM);
}

template <typename Surface>
inline void header(Surface& surface, const String& title,
                   const String& subtitle = "",
                   uint16_t accent = kAccent) {
  begin(surface);
  surface.fillRect(0, 0, TFT_WIDTH, kHeaderHeight, kBackground);
  surface.fillRoundRect(2, 2, TFT_WIDTH - 4, kHeaderHeight - 4, 4, kSurface);
  surface.fillRoundRect(2, 2, 4, kHeaderHeight - 4, 2, accent);
  surface.setTextColor(TFT_WHITE, kSurface);
  surface.drawString(title, 10, subtitle.length() ? 5 : 10, 1);
  if (subtitle.length()) {
    surface.setTextColor(kMuted, kSurface);
    surface.drawString(subtitle, 10, 16, 1);
  }
}

template <typename Surface>
inline void clearContent(Surface& surface) {
  surface.fillRect(0, kHeaderHeight, TFT_WIDTH,
                   TFT_HEIGHT - kHeaderHeight, kBackground);
}

template <typename Surface>
inline void panel(Surface& surface, int16_t y, int16_t height,
                  uint16_t border = kBorder) {
  surface.fillRoundRect(2, y, TFT_WIDTH - 4, height, 4, kSurface);
  surface.drawRoundRect(2, y, TFT_WIDTH - 4, height, 4, border);
}

template <typename Surface>
inline void metric(Surface& surface, int16_t y, const String& label,
                   const String& value, uint16_t valueColor = TFT_WHITE) {
  surface.setTextColor(kMuted, kSurface);
  surface.drawString(label, 8, y, 1);
  surface.setTextDatum(TR_DATUM);
  surface.setTextColor(valueColor, kSurface);
  surface.drawString(value, TFT_WIDTH - 8, y, 1);
  surface.setTextDatum(TL_DATUM);
}

template <typename Surface>
inline void progress(Surface& surface, int16_t y, uint8_t percent,
                     uint16_t color = kAccent) {
  percent = min<uint8_t>(percent, 100);
  constexpr int16_t x = 7;
  constexpr int16_t width = TFT_WIDTH - 14;
  surface.drawRoundRect(x, y, width, 6, 3, kBorder);
  const int16_t filled = ((width - 2) * percent) / 100;
  if (filled > 0)
    surface.fillRoundRect(x + 1, y + 1, filled, 4, 2, color);
}

template <typename Surface>
inline void footer(Surface& surface, const String& controls,
                   uint16_t color = kMuted) {
  const int16_t y = TFT_HEIGHT - kFooterHeight;
  surface.fillRect(0, y, TFT_WIDTH, kFooterHeight, kBackground);
  surface.drawFastHLine(4, y, TFT_WIDTH - 8, kBorder);
  surface.setTextDatum(TC_DATUM);
  surface.setTextColor(color, kBackground);
  surface.drawString(controls, TFT_WIDTH / 2, y + 3, 1);
  surface.setTextDatum(TL_DATUM);
}

template <typename Surface>
inline void emptyState(Surface& surface, const String& title,
                       const String& detail) {
  surface.setTextDatum(TC_DATUM);
  surface.setTextColor(TFT_LIGHTGREY, kBackground);
  surface.drawString(title, TFT_WIDTH / 2, 53, 1);
  surface.setTextColor(kMuted, kBackground);
  surface.drawString(detail, TFT_WIDTH / 2, 68, 1);
  surface.setTextDatum(TL_DATUM);
}

}  // namespace MiniV3Ui

#endif
