#include "tumble/color.h"

static float clamp01(float v) { return v < 0 ? 0 : (v > 1 ? 1 : v); }

tmb_col tmb_col_rgb(float r, float g, float b)
{
    tmb_col c = {r, g, b, 1.0f};
    return c;
}

tmb_col tmb_col_rgba(float r, float g, float b, float a)
{
    tmb_col c = {r, g, b, a};
    return c;
}

tmb_col tmb_col_hsv(float h, float s, float v)
{
    while (h < 0)
        h += 360.0f;
    while (h >= 360.0f)
        h -= 360.0f;
    float c = v * s;
    float hp = h / 60.0f;
    float frac = hp - 2.0f * (float)((int)(hp / 2)) - 1.0f; /* in [-1,1] */
    float x = c * (1.0f - (frac < 0 ? -frac : frac));
    float r = 0, g = 0, b = 0;
    int seg = (int)hp;
    switch (seg) {
    case 0: r = c; g = x; break;
    case 1: r = x; g = c; break;
    case 2: g = c; b = x; break;
    case 3: g = x; b = c; break;
    case 4: r = x; b = c; break;
    default: r = c; b = x; break;
    }
    float m = v - c;
    return tmb_col_rgb(r + m, g + m, b + m);
}

tmb_col tmb_col_lerp(tmb_col a, tmb_col b, float t)
{
    tmb_col c;
    c.r = a.r + (b.r - a.r) * t;
    c.g = a.g + (b.g - a.g) * t;
    c.b = a.b + (b.b - a.b) * t;
    c.a = a.a + (b.a - a.a) * t;
    return c;
}

tmb_col tmb_col_scale(tmb_col c, float s)
{
    return tmb_col_rgba(c.r * s, c.g * s, c.b * s, c.a);
}

uint32_t tmb_col_to_rgba8(tmb_col c)
{
    uint32_t r = (uint32_t)(clamp01(c.r) * 255.0f + 0.5f);
    uint32_t g = (uint32_t)(clamp01(c.g) * 255.0f + 0.5f);
    uint32_t b = (uint32_t)(clamp01(c.b) * 255.0f + 0.5f);
    uint32_t a = (uint32_t)(clamp01(c.a) * 255.0f + 0.5f);
    return (r << 24) | (g << 16) | (b << 8) | a;
}

tmb_col tmb_col_from_rgba8(uint32_t v)
{
    return tmb_col_rgba(((v >> 24) & 0xff) / 255.0f, ((v >> 16) & 0xff) / 255.0f,
                        ((v >> 8) & 0xff) / 255.0f, (v & 0xff) / 255.0f);
}

tmb_col tmb_palette(int index, int count)
{
    if (count < 1)
        count = 1;
    float h = 360.0f * (float)(index % count) / (float)count;
    return tmb_col_hsv(h, 0.65f, 0.95f);
}
