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
	dev->fps_cap_ms = 1000.f / params->fps_cap;

	return 0;
}

#ifdef RIOT_LCD_SDL2_IMPLEMENTATION
	// Add a prefix to function name incase it's being included in header as to make function name less common and prevent name conflicts
	#define uint16_to_rgb565_bigendian riot_lcd_sdl2__uint16_to_rgb565_bigendian
#endif

static void uint16_to_rgb565_bigendian(uint16_t color, uint8_t* r, uint8_t* g, uint8_t* b)
{
	*r = (color >> 8) & 0xF8; // (bits 8-12)
	*g = ((color << 5) & 0xE0) | ((color >> 13) & 0x1C); // (bits 3-7 and 13-15)
	*b = (color << 3) & 0xF8; // (bits 0 -4)
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

	uint16_to_rgb565_bigendian(
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

void riot_lcd_sdl2_destroy(lcd_t* dev)
{
	SDL_DestroyRenderer(dev->renderer);
	SDL_DestroyWindow(dev->window);
	SDL_Quit();
}

// Undefine any defined macros if it's being included via header
#ifdef RIOT_LCD_SDL2_IMPLEMENTATION
	#undef ABS
	#undef uint16_to_rgb565_bigendian
	#undef PRINT_ERR
	#undef PRINT_ERR_FORMAT
#endif
