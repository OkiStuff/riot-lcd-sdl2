#ifndef RIOT_LCD_SDL2_H
#define RIOT_LCD_SDL2_H

#include <SDL2/SDL.h>
#include <stdint.h>

#ifndef RIOT_LCD_SDL2_EXCLUDE_HELPER_MACROS
	#define RGB565_BE(R, G, B) (((R & 0xF8) << 8) | ((G & 0xFC) << 3) | (B >> 3))
#endif

#ifndef RIOT_LCD_SDL2_RECTANGLES_MAX
	#define RIOT_LCD_SDL2_RECTANGLES_MAX 512
#endif

#ifdef __cplusplus
extern "C"
{
#endif

struct riot_lcd_sdl2_rect
{
	SDL_Rect rect;
	uint8_t red;
	uint8_t green;
	uint8_t blue;
};

typedef struct lcd_t
{
	// Set the following to null / 0
	struct riot_lcd_sdl2_rect rectangles[RIOT_LCD_SDL2_RECTANGLES_MAX];
	SDL_Window* window;
	SDL_Renderer* renderer;
	size_t rect_index;
	int width;
	int height;
	float fps_cap_ms;
} lcd_t;

typedef struct lcd_params_t
{
	const char* window_title;
	const int width;
	const int height;
	const float fps_cap; // Set to 0 for default (25.f)
} lcd_params_t;

/**
 * @brief   Setup an LCD display device
 *
 * @param[out]  dev     device descriptor, slightly different to actual lcd.h as it's an out pointer
 * @param[in]   params  parameters for device initialization
 * @return      int     -1 in the event of an error, 0 in the event of success
 */
int lcd_init(lcd_t* dev, const lcd_params_t* params);

/**
 * @brief   Fill a rectangular area with a single pixel color
 *
 * the rectangular area is defined as x1 being the first column of pixels and
 * x2 being the last column of pixels to fill. similar to that, y1 is the first
 * row to fill and y2 is the last row to fill.
 *
 * @param[in]   dev     device descriptor
 * @param[in]   x1      x coordinate of the first corner
 * @param[in]   x2      x coordinate of the opposite corner
 * @param[in]   y1      y coordinate of the first corner
 * @param[in]   y2      y coordinate of the opposite corner
 * @param[in]   color   single color to fill the area with
 */
void lcd_fill(lcd_t* dev, uint16_t x1, uint16_t x2, uint16_t y1, uint16_t y2, uint16_t color);

/**
 * @brief   Fill a rectangular area with an array of pixels
 *
 * the rectangular area is defined as x1 being the first column of pixels and
 * x2 being the last column of pixels to fill. similar to that, y1 is the first
 * row to fill and y2 is the last row to fill.
 *
 * @note @p color must have a length equal to `(x2 - x1 + 1) * (y2 - y1 + 1)`
 *
 * @param[in]   dev     device descriptor
 * @param[in]   x1      x coordinate of the first corner
 * @param[in]   x2      x coordinate of the opposite corner
 * @param[in]   y1      y coordinate of the first corner
 * @param[in]   y2      y coordinate of the opposite corner
 * @param[in]   color   array of colors to fill the area with
 */
void lcd_pixmap(lcd_t* dev, uint16_t x1, uint16_t x2, uint16_t y1, uint16_t y2, const uint16_t* color);

void riot_lcd_sdl2_destroy(lcd_t* dev);
void riot_lcd_sdl2_present(lcd_t* dev);
void riot_lcd_sdl2_flush(lcd_t* dev);

#ifdef __cplusplus
}
#endif

#ifdef RIOT_LCD_SDL2_IMPLEMENTATION
	// ...
#endif

#endif // RIOT_LCD_SDL2_H
