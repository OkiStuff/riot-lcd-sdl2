#include "riot_lcd_sdl2.h"
#include <stdio.h>
#define PRINT_ERR(s) printf("riot_lcd_sdl2 ERROR: " s "\n")
#define PRINT_ERR_FORMAT(s, ...) printf("riot_lcd_sdl2 ERROR: " s "\n", __VA_ARGS__)

int lcd_init(lcd_t* dev, const lcd_params_t* params)
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		PRINT_ERR("Failed to initialize SDL2");
		return -1;
	}

	SDL_assert(dev != NULL);

	dev->window = SDL_CreateWindow(
		params->window_title,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		params->width,
		params->height,
		0
	);

	if (dev->window == NULL)
	{
		PRINT_ERR("Failed to create a window");
		SDL_Quit();

		return -1;
	}

	dev->renderer = SDL_CreateRenderer(dev->window, -1, SDL_RENDERER_ACCELERATED);
	
	if (dev->renderer == NULL)
	{
		PRINT_ERR("Failed to create SDL2 renderer");
		SDL_DestroyWindow(dev->window);
		SDL_Quit();
		
		return -1;
	}

	dev->rect_index = 0;
	dev->fps_cap_ms = 1000.f / ((params->fps_cap < 0) ? 25.f : params->fps_cap);
	dev->bkg_color = params->bkg_color;
	dev->inverted = SDL_FALSE;

	return 0;
}

#ifdef RIOT_LCD_SDL2_IMPLEMENTATION
	// Add a prefix to function name incase it's being included in header as to make function name less common and prevent name conflicts
	#define uint16_to_rgb565_little_endian riot_lcd_sdl2__uint16_to_rgb565_little_endian
#endif

static void uint16_to_rgb565_little_endian(uint16_t color, uint8_t* r, uint8_t* g, uint8_t* b)
{
	*r = (color >> 3) & 0xF8; // (bits 11-15)
	*g = ((color << 3) & 0xFC) | ((color >> 9) & 0x03); // (bits 5-10)
	*b = (color << 8) & 0xF8; // (bits 0-4)
}

#define ABS(x) ((x) < 0 ? -(x) : (x))

void lcd_fill(lcd_t* dev, uint16_t x1, uint16_t x2, uint16_t y1, uint16_t y2, uint16_t color)
{
	if (dev->rect_index + 1 > RIOT_LCD_SDL2_RECTANGLES_MAX)
	{
		riot_lcd_sdl2_present(dev);
		riot_lcd_sdl2_flush(dev);
	}

	dev->rectangles[dev->rect_index] = (struct riot_lcd_sdl2_rect)
	{
		.rect = (SDL_Rect)
		{
			x1, y2, // First Corner
			ABS(x2 - x1), ABS(y2 - y1) // Second Corner
		}
	};

	uint16_to_rgb565_little_endian(
	    color,
	    &dev->rectangles[dev->rect_index].red,
	    &dev->rectangles[dev->rect_index].green,
	    &dev->rectangles[dev->rect_index].blue
	);

	dev->rect_index++;
}

void lcd_pixmap(lcd_t* dev, uint16_t x1, uint16_t x2, uint16_t y1, uint16_t y2, const uint16_t* color)
{
	// WARNING: Don't want to modify the original function signature so we are just going to assume
	// the size of `color` via the corner coordinates. If `color` isn't that large too bad :(
	// this isn't made for production anyways

	const uint16_t width = ABS(x2 - x1);

	for (int i = 0; i < (x2 - x1 + 1) * (y2 - y1 + 1); i++)
	{
		uint16_t x = i % width;
		uint16_t y = i / width;
		
		lcd_fill(dev, x, y, x, y, color[i]);
	}
}

void lcd_invert_on(lcd_t *dev)
{
	dev->inverted = SDL_TRUE;
}

void lcd_invert_off(lcd_t *dev)
{
	dev->inverted = SDL_FALSE;
}

void riot_lcd_sdl2_destroy(lcd_t* dev)
{
	SDL_DestroyRenderer(dev->renderer);
	SDL_DestroyWindow(dev->window);
	SDL_Quit();
}

void riot_lcd_sdl2_present(lcd_t* dev)
{
	uint64_t frame_start = SDL_GetPerformanceCounter();

	for (int i = 0; i < dev->rect_index + 1; i++)
	{
		uint8_t red = dev->rectangles[i].red;
		uint8_t green = dev->rectangles[i].green;
		uint8_t blue = dev->rectangles[i].blue;

		if (dev->inverted)
		{
			red -= 255;
			green -= 255;
			blue -= 255;
		}
		
		SDL_SetRenderDrawColor(dev->renderer, red, green, blue, 255);
		SDL_RenderDrawRect(dev->renderer, &dev->rectangles[i].rect);
	}

	SDL_RenderPresent(dev->renderer);

	float delta = (SDL_GetPerformanceCounter() - frame_start)/((float)(SDL_GetPerformanceFrequency() * 1000.0f));

	if (delta < dev->fps_cap_ms)
	{
		SDL_Delay(dev->fps_cap_ms - delta);
	}
}

void riot_lcd_sdl2_flush(lcd_t* dev)
{
	SDL_SetRenderDrawColor(dev->renderer, dev->bkg_color.r, dev->bkg_color.g, dev->bkg_color.b, dev->bkg_color.a);
	SDL_RenderClear(dev->renderer);
	
	dev->rect_index = 0;
}

// Dumy functions
#ifndef RIOT_LCD_SDL2_IGNORE_UNIMPLEMENTED_WARNINGS

// @param ... function signature
#define DEFINE_DUMMY_FUNCTION(...) __VA_ARGS__\
{\
 printf("%s is not implemented by riot_lcd_sdl2\n", __func__); \
}

#else

// @param ... function signature
#define DEFINE_DUMMY_FUNCTION(...) __VA_ARGS__ {}

#endif // RIOT_LCD_SDL2_IGNORE_UNIMPLEMENTED_WARNINGS

DEFINE_DUMMY_FUNCTION(void lcd_ll_acquire(lcd_t* dev));
DEFINE_DUMMY_FUNCTION(void lcd_ll_release(lcd_t* dev));
DEFINE_DUMMY_FUNCTION(void lcd_ll_write_cmd (lcd_t* dev, uint8_t cmd, const uint8_t *data, size_t len));
DEFINE_DUMMY_FUNCTION(void lcd_ll_read_cmd (lcd_t* dev, uint8_t cmd, uint8_t *data, size_t len));
DEFINE_DUMMY_FUNCTION(void lcd_ll_set_area (lcd_t* dev, uint16_t x1, uint16_t x2, uint16_t y1, uint16_t y2));
DEFINE_DUMMY_FUNCTION(void lcd_write_cmd (lcd_t* dev, uint8_t cmd, const uint8_t *data, size_t len));
DEFINE_DUMMY_FUNCTION(void lcd_read_cmd (lcd_t* dev, uint8_t cmd, uint8_t *data, size_t len));

// Undefine any defined macros if it's being included via header
#ifdef RIOT_LCD_SDL2_IMPLEMENTATION
	#undef ABS
	#undef uint16_to_rgb565_little_endian
	#undef PRINT_ERR
	#undef PRINT_ERR_FORMAT
	#undef DEFINE_DUMMY_FUNCTION
#endif
