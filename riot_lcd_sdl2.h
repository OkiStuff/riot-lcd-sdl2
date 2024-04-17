#ifndef RIOT_LCD_SDL2_H
#define RIOT_LCD_SDL2_H

#include <SDL2/SDL.h>
#include <stdint.h>

#ifndef RIOT_LCD_SDL2_EXCLUDE_HELPER_MACROS
	#define RGB565_LE(R,G,B) (uint16_t)(((R) >> 3) | (((G) & 0xFC) << 3) | (((B) >> 3) << 8)))
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

typedef struct riot_lcd_sdl2_background_color
{
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;
} riot_lcd_sdl2_background_color;

typedef struct lcd_t
{
	struct riot_lcd_sdl2_rect rectangles[RIOT_LCD_SDL2_RECTANGLES_MAX];
	riot_lcd_sdl2_background_color bkg_color;
	SDL_bool inverted;
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
	const riot_lcd_sdl2_background_color bkg_color;
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

/**
 * @brief   Invert the display colors
 *
 * @param[in]   dev     device descriptor
 */
void lcd_invert_on(lcd_t *dev);

/**
 * @brief   Disable color inversion
 *
 * @param[in]   dev     device descriptor
 */
void lcd_invert_off(lcd_t *dev);

void riot_lcd_sdl2_destroy(lcd_t* dev);
void riot_lcd_sdl2_present(lcd_t* dev);
void riot_lcd_sdl2_flush(lcd_t* dev);

// Dummy functions, are not actually implemented.

/**
 * @name    Low-level LCD API
 *
 * Low-level functions are used to acquire a device, write commands with data
 * to the device, or read data from the device and release it when it is no
 * longer needed. They are usually called by the high-level functions such
 * as @ref lcd_init, @ref lcd_fill, @ref lcd_pixmap, etc., but can also be
 * used by the application to implement low-level operations if needed.
 *
 * @{
 */
/**
 * @brief   Low-level function to acquire the device
 *
 * @param[out]  dev     device descriptor
 */
void lcd_ll_acquire(lcd_t *dev);

/**
 * @brief   Low-level function to release the device
 *
 * @param[out]  dev     device descriptor
 */
void lcd_ll_release(lcd_t *dev);

/**
 * @brief   Low-level function to write a command
 *
 * @pre The device must have already been acquired with @ref lcd_ll_acquire
 *      before this function can be called.
 *
 * @param[in]   dev     device descriptor
 * @param[in]   cmd     command code
 * @param[in]   data    command data to the device or NULL for commands without data
 * @param[in]   len     length of the command data or 0 for commands without data
 */
void lcd_ll_write_cmd(lcd_t *dev, uint8_t cmd, const uint8_t *data,
                      size_t len);

/**
 * @brief   Low-level function for read command
 *
 * @note Very often the SPI MISO signal of the serial interface or the RDX
 *       signal of the MCU 8080 parallel interface are not connected to the
 *       display. In this case the read command does not provide valid data.
 *
 * @pre The device must have already been acquired with @ref lcd_ll_acquire
 *      before this function can be called.
 * @pre len > 0
 *
 * @param[in]   dev     device descriptor
 * @param[in]   cmd     command
 * @param[out]  data    data from the device
 * @param[in]   len     length of the returned data
 */
void lcd_ll_read_cmd(lcd_t *dev, uint8_t cmd, uint8_t *data, size_t len);

/**
 * @brief   Set the LCD work area
 *
 * @param[in] dev  Pointer to the selected driver
 * @param[in] x1   x coordinate of the first corner
 * @param[in] x2   x coordinate of the opposite corner
 * @param[in] y1   y coordinate of the first corner
 * @param[in] y2   y coordinate of the opposite corner
 *
 */
void lcd_ll_set_area(lcd_t *dev, uint16_t x1, uint16_t x2, uint16_t y1, uint16_t y2);
/** @} */


/**
 * @brief   Raw write command
 *
 * @param[in]   dev     device descriptor
 * @param[in]   cmd     command code
 * @param[in]   data    command data to the device or NULL for commands without data
 * @param[in]   len     length of the command data or 0 for commands without data
 */
void lcd_write_cmd(lcd_t *dev, uint8_t cmd, const uint8_t *data,
                   size_t len);

/**
 * @brief   Raw read command
 *
 * @note Very often the SPI MISO signal of the serial interface or the RDX
 *       signal of the MCU 8080 parallel interface are not connected to the
 *       display. In this case the read command does not provide valid data.
 *
 * @pre         len > 0
 *
 * @param[in]   dev     device descriptor
 * @param[in]   cmd     command
 * @param[out]  data    data from the device
 * @param[in]   len     length of the returned data
 */
void lcd_read_cmd(lcd_t *dev, uint8_t cmd, uint8_t *data, size_t len);

#ifdef __cplusplus
}
#endif

#ifdef RIOT_LCD_SDL2_IMPLEMENTATION
	#include "riot_lcd_sdl2.c"
#endif

#endif // RIOT_LCD_SDL2_H
