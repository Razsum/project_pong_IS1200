#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "objects.h"

/**
 * @brief Framebuffer base address
 *
 * Defines the starting address of the framebuffer in memory
 * The framebuffer allows direct pixel drawing by writing to display memory
 */
#define FB_BASE 0x08000000u


/**
 * @brief Max width and height of our window in pixels
 *
 */
#define WIDTH         320
#define HEIGHT        240

/**
 * @brief Our defined colors 
 * 
 * We are using the RGB 3-3-2 8-bit pixel map, which means we have 256 
 * colors to define our graphics with
 * @author Razmus Nilsson
 */
enum
{
  COL_BG = 0x00,  // Background (Black)
  COL_NET = 0x80, // Center net (Dark red)
  COL_FG = 0xFF,  // Paddles (White)
  COL_BALL = 0xC0, // Ball (Red)
  COL_GOLD = 0xFC  // Start and victory text (Yellow)
};


/**
 * @brief Bitmaps used to define our numbers and characters
 *
 * Characters are displayed in a 5x7 pixel width*height matrix.
 * Each element in the array represents one row of pixels.
 * Pixels set to 1 are drawn, 0 are left blank.
 * Font design by user "maneirinho" on fontstruct
 * https://fontstruct.com/fontstructions/show/2126742/5x7-font-10
 * @author Razmus Nilsson
 */
extern const uint8_t digits[10][7];
extern const uint8_t letters[26][7];

/**
 * @brief 8-bit framebuffer pointer
 *
 * Pointer to the start of framebuffer memory.
 * Each pixel is represented as a single byte.
 * @author Razmus Nilsson
 */
extern volatile uint8_t *const fb;

/**
 * @brief Set a single pixel on the screen
 *
 * Writes a pixel color to the framebuffer at coordinates (x, y)
 *
 * @param x Horizontal coordinate
 * @param y Vertical coordinate
 * @param c Color value (8-bit)
 * @author Razmus Nilsson
 */
extern inline void pset8(int x, int y, uint8_t c);

/**
 * @brief Draw a filled rectangle
 *
 * Checks so the drawing doesn't exceed the screen boundaries 
 * Draws a filled rectangle of width (w) and height (h) at position (x, y)
 * using the specified color. 
 *
 * @param x Top-left x coordinate
 * @param y Top-left y coordinate
 * @param w Width of rectangle
 * @param h Height of rectangle
 * @param c Color value 
 * @author Razmus Nilsson
 */
extern void rect_fill8(int x,int y,int w,int h,uint8_t c);

/**
 * @brief Draw a single digit
 *
 * Draws a 0–9 digit at a given screen position using the 5x7 bitmap.
 *
 * @param x Top-left x coordinate
 * @param y Top-left y coordinate
 * @param digit Digit to draw (0–9)
 * @param color Color value
 * @author Razmus Nilsson
 */
extern void draw_digit(int x, int y, int digit, uint8_t color);

/**
 * @brief Draw a two-digit number
 *
 * @param x Top-left x coordinate
 * @param y Top-left y coordinate
 * @param score Number to draw (0–99)
 * @param color Color value
 * @author Razmus Nilsson
 */
extern void draw_score(int x, int y, int score, uint8_t color);

/**
 * @brief Draw a single character
 *
 * Draws a character (A-Z, a-z, 0-9) at the given position
 *
 * @param x Top-left x coordinate
 * @param y Top-left y coordinate
 * @param c Character to draw
 * @param color Color value
 * @author Razmus Nilsson
 */
extern void draw_char(int x, int y, char c, uint8_t color);

/**
 * @brief Draw a string of text
 *
 * Draws a string starting at the given coordinates.
 * Characters are spaced 6 pixels apart (5x7 bitmap + 1 pixel spacing)
 *
 * @param x Top-left x coordinate
 * @param y Top-left y coordinate
 * @param text Null-terminated string to draw
 * @param color Color value 
 * @author Razmus Nilsson
 */
extern void draw_text(int x, int y, const char *text, uint8_t color);

/**
 * @brief Clears the framebuffer to a single color. *
 *
 * @param c Color value
 * @author Razmus Nilsson
 */
void clear_screen8(uint8_t c);

/**
 * @brief Redraws all visible game objects (paddles, ball, net, and scores)
 *
 * @param p1_score Player 1 score
 * @param p2_score Player 2 score
 * @author Razmus Nilsson
 */
void draw_all(int p1_score, int p2_score);

#endif