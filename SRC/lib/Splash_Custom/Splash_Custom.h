/**
 * @file Splash_Custom.h
 * @brief Custom trident splash screen / boot animation for the NEMO device.
 *
 * This module provides an alternative boot animation to the default
 * Splash_Screen. It renders the RUDRA trident mark (converted from the
 * source artwork image.png) rising from the bottom of the display, followed
 * by a shimmer sweep and the "RUDRA" wordmark fading in beneath it.
 *
 * Like Splash_Screen, it draws to the SH1106 128x64 OLED using the U8g2
 * graphics library in full-framebuffer mode. The display is monochrome, so
 * the teal trident from the source artwork is rendered as a white silhouette
 * on black.
 *
 * The trident is an XBM bitmap generated from image.png by img2xbm.py
 * (threshold + dilate + downscale to 64x48). The RUDRA wordmark uses the
 * XBM letter bitmaps defined below.
 *
 * Usage (drop-in replacement for Splash_Screen::show):
 * @code
 *   #include <Splash_Custom.h>
 *   Splash_Custom::show(&u8g2);
 * @endcode
 */

#ifndef SPLASH_CUSTOM_H
#define SPLASH_CUSTOM_H

#include <Arduino.h>
#include <U8g2lib.h>

/**
 * @class Splash_Custom
 * @brief Handles the custom trident boot animation for the NEMO device.
 *
 * All methods are static: the class carries no per-instance state and is
 * used purely as a namespace grouping the animation routines, mirroring
 * the design of the original Splash_Screen class.
 */
class Splash_Custom
{
public:
    /**
     * @brief Display the complete custom splash animation sequence.
     *
     * Runs the full boot animation:
     * - Phase 1: Trident rises from the bottom of the screen into place.
     * - Phase 2: A horizontal shimmer/glow sweep passes over the trident.
     * - Phase 3: The "RUDRA" wordmark fades in beneath the trident and holds.
     *
     * Uses U8g2 graphics mode for smooth full-framebuffer rendering.
     *
     * @param u8g2 Pointer to the U8G2 display driver instance.
     *             Must be initialized (begin()) by this method.
     */
    static void show(U8G2_SH1106_128X64_NONAME_F_HW_I2C *u8g2);

private:
    /**
     * @brief Draw the "RUDRA" wordmark using the shared XBM letter bitmaps.
     *
     * @param u8g2   Pointer to the U8G2 display driver instance.
     * @param startX Left edge of the first letter.
     * @param startY Top edge of the letters.
     */
    static void drawWordmark(U8G2_SH1106_128X64_NONAME_F_HW_I2C *u8g2, int startX, int startY);

    /**
     * @brief Compute the y of the animated water surface at a given column.
     *
     * The surface is a sum of two sine waves so the ripple looks organic
     * rather than a single clean wave. `phase` advances each frame to make
     * the water move.
     *
     * @param x       Column (0..127).
     * @param baseY   Resting y of the waterline.
     * @param phase   Animation phase (advances each frame).
     * @return The y coordinate of the surface at column x.
     */
    static int waterSurfaceY(int x, int baseY, float phase);

    /**
     * @brief Draw the rippling water surface and the dark region below it.
     *
     * Renders the wavy surface line plus a few sparse "sea" texture dots
     * beneath it, across the full screen width.
     *
     * @param u8g2   Pointer to the U8G2 display driver instance.
     * @param baseY  Resting y of the waterline.
     * @param phase  Animation phase (advances each frame).
     */
    static void drawWater(U8G2_SH1106_128X64_NONAME_F_HW_I2C *u8g2, int baseY, float phase);

    /**
     * @brief Draw the trident bitmap clipped to only the part above a surface.
     *
     * Pixels of the trident that fall below the (wavy) water surface are not
     * drawn, so the mark appears to emerge from the water as it rises.
     *
     * @param u8g2   Pointer to the U8G2 display driver instance.
     * @param topY   Top edge (y) of the trident bitmap.
     * @param baseY  Resting y of the waterline.
     * @param phase  Animation phase (for the wavy clip edge).
     */
    static void drawTridentClipped(U8G2_SH1106_128X64_NONAME_F_HW_I2C *u8g2,
                                   int topY, int baseY, float phase);
};

/*
 * Trident bitmap generated from image.png by img2xbm.py: 64 x 48 px (384 bytes).
 * XBM format (LSB = leftmost pixel), suitable for u8g2->drawXBMP().
 */
#define TRIDENT_W 64
#define TRIDENT_H 48
static const unsigned char trident_xbm[] PROGMEM = {
    0x00,
    0x00,
    0x00,
    0x60,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x60,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x60,
    0x60,
    0x60,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x60,
    0xF0,
    0x60,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x70,
    0xF0,
    0x60,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x70,
    0xF0,
    0xE0,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x70,
    0xF0,
    0xE0,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x78,
    0xF0,
    0xE0,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x38,
    0xF0,
    0xE0,
    0x01,
    0x00,
    0x00,
    0x00,
    0x00,
    0x3C,
    0xF8,
    0xC1,
    0x01,
    0x00,
    0x00,
    0x00,
    0x00,
    0x3C,
    0xF8,
    0xC1,
    0x03,
    0x00,
    0x00,
    0x00,
    0x00,
    0x3C,
    0xF8,
    0xC1,
    0x03,
    0x00,
    0x00,
    0x00,
    0x00,
    0x3E,
    0xF8,
    0xC1,
    0x03,
    0x00,
    0x00,
    0x00,
    0x00,
    0x1E,
    0xF8,
    0xC1,
    0x07,
    0x00,
    0x00,
    0x00,
    0x00,
    0x1E,
    0xFC,
    0xC3,
    0x07,
    0x00,
    0x00,
    0x00,
    0x00,
    0x1F,
    0xFC,
    0xC3,
    0x07,
    0x00,
    0x00,
    0x00,
    0x00,
    0x3F,
    0xFC,
    0xC3,
    0x0F,
    0x00,
    0x00,
    0x00,
    0x00,
    0x7F,
    0xFC,
    0xE3,
    0x0F,
    0x00,
    0x00,
    0x00,
    0x00,
    0xFF,
    0xFC,
    0xFF,
    0x07,
    0x00,
    0x00,
    0x00,
    0x00,
    0xFE,
    0xFF,
    0xFF,
    0x03,
    0x00,
    0x00,
    0x00,
    0x00,
    0xF8,
    0xFF,
    0xFF,
    0x01,
    0x00,
    0x00,
    0x00,
    0x00,
    0xF0,
    0xFF,
    0xFF,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0xE0,
    0xFF,
    0x3F,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x80,
    0xFF,
    0x1F,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0xFF,
    0x0F,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0xF0,
    0x01,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0xF0,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0xF0,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0xF0,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0xF0,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0xF0,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0xF0,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0xF0,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0xF0,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x70,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x70,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x70,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x60,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x60,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x60,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x60,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x60,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x60,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x60,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x60,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x60,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x60,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
};

/*
 * RUDRA wordmark letter bitmaps.
 *
 * Each letter is 16 px wide x 24 px tall, encoded as XBM-style rows of two
 * bytes each (MSB = leftmost pixel), matching the format used by the original
 * Splash_Screen letters. The ASCII art on each row shows the rendered glyph
 * ('O' = pixel on, '.' = off).
 */

/** @brief Letter 'R'. 16x24 px (48 bytes). */
static const unsigned char rudra_letter_R[] PROGMEM = {
    0xFF, 0xC0, // OOOOOOOOOO......
    0xFF, 0xF0, // OOOOOOOOOOOO....
    0xFF, 0xF8, // OOOOOOOOOOOOO...
    0xC0, 0x38, // OO........OOO...
    0xC0, 0x1C, // OO.........OOO..
    0xC0, 0x1C, // OO.........OOO..
    0xC0, 0x1C, // OO.........OOO..
    0xC0, 0x38, // OO........OOO...
    0xFF, 0xF8, // OOOOOOOOOOOOO...
    0xFF, 0xF0, // OOOOOOOOOOOO....
    0xFF, 0xC0, // OOOOOOOOOO......
    0xC7, 0x80, // OO...OOOO.......
    0xC3, 0x80, // OO....OOO.......
    0xC1, 0xC0, // OO.....OOO......
    0xC1, 0xC0, // OO.....OOO......
    0xC0, 0xE0, // OO......OOO.....
    0xC0, 0xE0, // OO......OOO.....
    0xC0, 0x70, // OO.......OOO....
    0xC0, 0x70, // OO.......OOO....
    0xC0, 0x38, // OO........OOO...
    0xC0, 0x38, // OO........OOO...
    0xC0, 0x1C, // OO.........OOO..
    0xC0, 0x1C, // OO.........OOO..
    0xC0, 0x1C  // OO.........OOO..
};

/** @brief Letter 'U'. 16x24 px (48 bytes). */
static const unsigned char rudra_letter_U[] PROGMEM = {
    0xE0, 0x1C, // OOO........OOO..
    0xE0, 0x1C, // OOO........OOO..
    0xE0, 0x1C, // OOO........OOO..
    0xE0, 0x1C, // OOO........OOO..
    0xE0, 0x1C, // OOO........OOO..
    0xE0, 0x1C, // OOO........OOO..
    0xE0, 0x1C, // OOO........OOO..
    0xE0, 0x1C, // OOO........OOO..
    0xE0, 0x1C, // OOO........OOO..
    0xE0, 0x1C, // OOO........OOO..
    0xE0, 0x1C, // OOO........OOO..
    0xE0, 0x1C, // OOO........OOO..
    0xE0, 0x1C, // OOO........OOO..
    0xE0, 0x1C, // OOO........OOO..
    0xE0, 0x1C, // OOO........OOO..
    0xE0, 0x1C, // OOO........OOO..
    0xE0, 0x1C, // OOO........OOO..
    0x70, 0x38, // .OOO......OOO...
    0x78, 0x78, // .OOOO....OOOO...
    0x3F, 0xF0, // ..OOOOOOOOOO....
    0x1F, 0xE0, // ...OOOOOOOO.....
    0x0F, 0xC0, // ....OOOOOO......
    0x00, 0x00, // ................
    0x00, 0x00  // ................
};

/** @brief Letter 'D'. 16x24 px (48 bytes). */
static const unsigned char rudra_letter_D[] PROGMEM = {
    0xFF, 0x00, // OOOOOOOO........
    0xFF, 0xC0, // OOOOOOOOOO......
    0xFF, 0xE0, // OOOOOOOOOOO.....
    0xC0, 0xF0, // OO......OOOO....
    0xC0, 0x78, // OO.......OOOO...
    0xC0, 0x38, // OO........OOO...
    0xC0, 0x1C, // OO.........OOO..
    0xC0, 0x1C, // OO.........OOO..
    0xC0, 0x1C, // OO.........OOO..
    0xC0, 0x1C, // OO.........OOO..
    0xC0, 0x1C, // OO.........OOO..
    0xC0, 0x1C, // OO.........OOO..
    0xC0, 0x1C, // OO.........OOO..
    0xC0, 0x1C, // OO.........OOO..
    0xC0, 0x1C, // OO.........OOO..
    0xC0, 0x1C, // OO.........OOO..
    0xC0, 0x38, // OO........OOO...
    0xC0, 0x78, // OO.......OOOO...
    0xC0, 0xF0, // OO......OOOO....
    0xFF, 0xE0, // OOOOOOOOOOO.....
    0xFF, 0xC0, // OOOOOOOOOO......
    0xFF, 0x00, // OOOOOOOO........
    0x00, 0x00, // ................
    0x00, 0x00  // ................
};

/** @brief Letter 'A'. 16x24 px (48 bytes). */
static const unsigned char rudra_letter_A[] PROGMEM = {
    0x07, 0x00, // .....OOO........
    0x0F, 0x80, // ....OOOOO.......
    0x0F, 0x80, // ....OOOOO.......
    0x1F, 0xC0, // ...OOOOOOO......
    0x1D, 0xC0, // ...OOO.OOO......
    0x38, 0xE0, // ..OOO...OOO.....
    0x38, 0xE0, // ..OOO...OOO.....
    0x70, 0x70, // .OOO.....OOO....
    0x70, 0x70, // .OOO.....OOO....
    0xE0, 0x38, // OOO.......OOO...
    0xE0, 0x38, // OOO.......OOO...
    0xFF, 0xF8, // OOOOOOOOOOOOO...
    0xFF, 0xF8, // OOOOOOOOOOOOO...
    0xFF, 0xF8, // OOOOOOOOOOOOO...
    0xE0, 0x38, // OOO.......OOO...
    0xC0, 0x18, // OO.........OO...
    0xC0, 0x1C, // OO.........OOO..
    0xC0, 0x1C, // OO.........OOO..
    0xC0, 0x1C, // OO.........OOO..
    0xC0, 0x1C, // OO.........OOO..
    0xC0, 0x1C, // OO.........OOO..
    0xC0, 0x1C, // OO.........OOO..
    0x00, 0x00, // ................
    0x00, 0x00  // ................
};

#endif // SPLASH_CUSTOM_H