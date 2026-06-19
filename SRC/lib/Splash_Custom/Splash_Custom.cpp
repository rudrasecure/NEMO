/**
 * @file Splash_Custom.cpp
 * @brief Implementation of the custom trident boot animation.
 *
 * Renders the RUDRA trident bitmap (from image.png, see Splash_Custom.h)
 * emerging from rippling water, with a splash on breach, then a shimmer sweep,
 * then the "RUDRA" wordmark fading in beneath it.
 *
 * Phases:
 * - Phase 1: Trident rises out of an animated water surface (clipped at the
 *            waterline) with splash droplets and expanding ripple rings.
 * - Phase 2: Water recedes; a vertical shimmer band sweeps over the trident.
 * - Phase 3: RUDRA wordmark fades in row-by-row and the frame holds.
 *
 * All effects use only cheap math (sin) plus U8g2 primitives drawn into the
 * full framebuffer, well within the Teensy 4.0 / I2C OLED's budget.
 */

#include "Splash_Custom.h"
#include <math.h>

// ---------------------------------------------------------------------------
// Tunable layout / motion constants.
// ---------------------------------------------------------------------------
namespace
{
    constexpr int TRID_X = (128 - TRIDENT_W) / 2; // = 32, centered horizontally
    constexpr int WATER_BASE_Y = 50;              // resting waterline y
    constexpr float WAVE_SPEED = 0.45f;           // how fast the surface ripples

    // A single droplet thrown up by the splash.
    struct Droplet
    {
        float x, y;   // position
        float vx, vy; // velocity (vy negative = upward)
        bool active;
    };

    // An expanding ripple ring on the surface.
    struct Ripple
    {
        int cx; // center column
        int r;  // current radius
        bool active;
    };
}

int Splash_Custom::waterSurfaceY(int x, int baseY, float phase)
{
    // Two sine components of different wavelength/speed for an organic look.
    float w = sinf(x * 0.20f + phase) * 2.0f + sinf(x * 0.07f - phase * 0.6f) * 1.5f;
    return baseY + (int)lroundf(w);
}

void Splash_Custom::drawWater(U8G2_SH1106_128X64_NONAME_F_HW_I2C *u8g2, int baseY, float phase)
{
    // Draw the wavy surface line across the whole width.
    int prevY = waterSurfaceY(0, baseY, phase);
    for (int x = 1; x < 128; x++)
    {
        int y = waterSurfaceY(x, baseY, phase);
        u8g2->drawLine(x - 1, prevY, x, y);
        prevY = y;

        // Sparse "sea" texture: a dot a little below the surface every few
        // columns, offset by phase so it shimmers.
        if ((x + (int)(phase * 4)) % 9 == 0)
        {
            int dy = y + 4 + (x % 3);
            if (dy < 64)
                u8g2->drawPixel(x, dy);
        }
    }
}

void Splash_Custom::drawTridentClipped(U8G2_SH1106_128X64_NONAME_F_HW_I2C *u8g2,
                                       int topY, int baseY, float phase)
{
    // Draw the trident pixel-by-pixel from its bitmap, skipping any pixel that
    // falls below the wavy water surface so it looks like it's emerging.
    const int bytesPerRow = TRIDENT_W / 8; // = 8
    for (int r = 0; r < TRIDENT_H; r++)
    {
        int py = topY + r;
        if (py < 0 || py >= 64)
            continue;
        for (int byteIdx = 0; byteIdx < bytesPerRow; byteIdx++)
        {
            uint8_t b = pgm_read_byte(&trident_xbm[r * bytesPerRow + byteIdx]);
            if (b == 0)
                continue;
            for (int bit = 0; bit < 8; bit++)
            {
                if (b & (1 << bit))
                { // XBM packs LSB = leftmost pixel
                    int px = TRID_X + byteIdx * 8 + bit;
                    // Clip: only draw if at/above the water surface here.
                    if (py <= waterSurfaceY(px, baseY, phase))
                    {
                        u8g2->drawPixel(px, py);
                    }
                }
            }
        }
    }
}

void Splash_Custom::drawWordmark(U8G2_SH1106_128X64_NONAME_F_HW_I2C *u8g2, int startX, int startY)
{
    const int w = 16, spacing = 2, step = w + spacing;
    u8g2->drawXBMP(startX + step * 0, startY, w, 24, rudra_letter_R);
    u8g2->drawXBMP(startX + step * 1, startY, w, 24, rudra_letter_U);
    u8g2->drawXBMP(startX + step * 2, startY, w, 24, rudra_letter_D);
    u8g2->drawXBMP(startX + step * 3, startY, w, 24, rudra_letter_R);
    u8g2->drawXBMP(startX + step * 4, startY, w, 24, rudra_letter_A);
}

void Splash_Custom::show(U8G2_SH1106_128X64_NONAME_F_HW_I2C *u8g2)
{
    u8g2->begin();
    u8g2->setFont(u8g2_font_6x10_tf);

    const int tridRestY = 0; // final resting top edge of the trident
    float phase = 0.0f;      // water animation phase

    // Splash droplets (spawned at the moment of breach) and ripple rings.
    Droplet drops[10];
    for (int i = 0; i < 10; i++)
        drops[i].active = false;
    Ripple ripples[3];
    for (int i = 0; i < 3; i++)
        ripples[i].active = false;

    // Trident tip starts at the central prong; we track its top edge so we
    // know when it first breaks the surface (to trigger the splash).
    bool breached = false;

    // -----------------------------------------------------------------------
    // Phase 1: Trident rises out of the water.
    // -----------------------------------------------------------------------
    // Start with the trident below the surface, ease its top edge up to rest.
    for (int topY = 64; topY >= tridRestY; topY -= 2)
    {
        u8g2->clearBuffer();

        // The central prong tip is the highest trident pixel (~row 0..2).
        int tipY = topY;

        drawWater(u8g2, WATER_BASE_Y, phase);
        drawTridentClipped(u8g2, topY, WATER_BASE_Y, phase);

        // Trigger the splash the first frame the tip clears the surface.
        if (!breached && tipY <= waterSurfaceY(64, WATER_BASE_Y, phase))
        {
            breached = true;
            // Fling droplets outward from the breach point.
            for (int i = 0; i < 10; i++)
            {
                float dir = (i < 5) ? -1.0f : 1.0f;
                drops[i].x = 64 + dir * (2 + (i % 5));
                drops[i].y = WATER_BASE_Y - 2;
                drops[i].vx = dir * (0.6f + (i % 3) * 0.3f);
                drops[i].vy = -(2.2f + (i % 4) * 0.4f);
                drops[i].active = true;
            }
            // Start a couple of ripple rings at the breach.
            ripples[0] = {64, 2, true};
            ripples[1] = {64, 0, true};
        }

        // Update + draw droplets (simple gravity).
        for (int i = 0; i < 10; i++)
        {
            if (!drops[i].active)
                continue;
            u8g2->drawPixel((int)drops[i].x, (int)drops[i].y);
            drops[i].x += drops[i].vx;
            drops[i].y += drops[i].vy;
            drops[i].vy += 0.35f; // gravity
            // Deactivate when it falls back to the surface or leaves screen.
            if (drops[i].y >= waterSurfaceY((int)drops[i].x, WATER_BASE_Y, phase) || drops[i].x < 0 || drops[i].x > 127)
            {
                drops[i].active = false;
            }
        }

        // Update + draw ripple rings (drawn as flat ellipses on the surface).
        for (int i = 0; i < 3; i++)
        {
            if (!ripples[i].active)
                continue;
            int ry = waterSurfaceY(ripples[i].cx, WATER_BASE_Y, phase);
            u8g2->drawEllipse(ripples[i].cx, ry, ripples[i].r, 1, U8G2_DRAW_UPPER_LEFT | U8G2_DRAW_UPPER_RIGHT);
            ripples[i].r += 3;
            if (ripples[i].r > 40)
                ripples[i].active = false;
        }

        u8g2->sendBuffer();
        phase += WAVE_SPEED;
        delay(28);
    }

    // Let remaining droplets/ripples settle while the trident sits at rest.
    for (int f = 0; f < 14; f++)
    {
        u8g2->clearBuffer();
        drawWater(u8g2, WATER_BASE_Y, phase);
        drawTridentClipped(u8g2, tridRestY, WATER_BASE_Y, phase);

        for (int i = 0; i < 10; i++)
        {
            if (!drops[i].active)
                continue;
            u8g2->drawPixel((int)drops[i].x, (int)drops[i].y);
            drops[i].x += drops[i].vx;
            drops[i].y += drops[i].vy;
            drops[i].vy += 0.35f;
            if (drops[i].y >= waterSurfaceY((int)drops[i].x, WATER_BASE_Y, phase) || drops[i].x < 0 || drops[i].x > 127)
            {
                drops[i].active = false;
            }
        }
        for (int i = 0; i < 3; i++)
        {
            if (!ripples[i].active)
                continue;
            int ry = waterSurfaceY(ripples[i].cx, WATER_BASE_Y, phase);
            u8g2->drawEllipse(ripples[i].cx, ry, ripples[i].r, 1, U8G2_DRAW_UPPER_LEFT | U8G2_DRAW_UPPER_RIGHT);
            ripples[i].r += 3;
            if (ripples[i].r > 40)
                ripples[i].active = false;
        }

        u8g2->sendBuffer();
        phase += WAVE_SPEED;
        delay(28);
    }

    // -----------------------------------------------------------------------
    // Phase 1b: Water recedes (waterline sinks off the bottom) leaving the
    // full trident standing clear above the surface.
    // -----------------------------------------------------------------------
    for (int wy = WATER_BASE_Y; wy <= 72; wy += 3)
    {
        u8g2->clearBuffer();
        if (wy < 64)
            drawWater(u8g2, wy, phase);
        // Trident now drawn unclipped (water is below its base).
        u8g2->drawXBMP(TRID_X, tridRestY, TRIDENT_W, TRIDENT_H, trident_xbm);
        u8g2->sendBuffer();
        phase += WAVE_SPEED;
        delay(30);
    }

    // Settle frame: trident alone.
    u8g2->clearBuffer();
    u8g2->drawXBMP(TRID_X, tridRestY, TRIDENT_W, TRIDENT_H, trident_xbm);
    u8g2->sendBuffer();
    delay(200);

    // -----------------------------------------------------------------------
    // Phase 2: Shimmer sweep over the trident.
    // -----------------------------------------------------------------------
    for (int band = -6; band < TRIDENT_H; band += 4)
    {
        u8g2->clearBuffer();
        u8g2->drawXBMP(TRID_X, tridRestY, TRIDENT_W, TRIDENT_H, trident_xbm);

        u8g2->setDrawColor(2); // XOR mode: flips pixels under the band
        for (int by = band; by < band + 3; by++)
        {
            if (by >= 0 && by < TRIDENT_H)
            {
                u8g2->drawLine(TRID_X, tridRestY + by, TRID_X + TRIDENT_W - 1, tridRestY + by);
            }
        }
        u8g2->setDrawColor(1);

        u8g2->sendBuffer();
        delay(25);
    }

    u8g2->clearBuffer();
    u8g2->drawXBMP(TRID_X, tridRestY, TRIDENT_W, TRIDENT_H, trident_xbm);
    u8g2->sendBuffer();
    delay(150);

    // -----------------------------------------------------------------------
    // Phase 3: RUDRA wordmark fades in row-by-row beneath the trident.
    // -----------------------------------------------------------------------
    const int wordStartX = (128 - (5 * 16 + 4 * 2)) / 2; // = 20
    const int wordStartY = 42;
    const unsigned char *letters[5] = {
        rudra_letter_R, rudra_letter_U, rudra_letter_D, rudra_letter_R, rudra_letter_A};

    for (int row = 0; row < 24; row += 2)
    {
        u8g2->clearBuffer();
        u8g2->drawXBMP(TRID_X, tridRestY, TRIDENT_W, TRIDENT_H, trident_xbm);

        for (int li = 0; li < 5; li++)
        {
            int lx = wordStartX + li * (16 + 2);
            for (int r = 0; r <= row && r < 24; r++)
            {
                for (int c = 0; c < 2; c++)
                {
                    uint8_t b = pgm_read_byte(&letters[li][r * 2 + c]);
                    for (int bit = 0; bit < 8; bit++)
                    {
                        if (b & (0x80 >> bit))
                        { // letter bitmaps are MSB-first
                            u8g2->drawPixel(lx + c * 8 + bit, wordStartY + r);
                        }
                    }
                }
            }
        }

        u8g2->sendBuffer();
        delay(35);
    }

    delay(1800);

    // Hand off to the menu system with a clean screen.
    u8g2->clearBuffer();
    u8g2->sendBuffer();
    delay(100);
}