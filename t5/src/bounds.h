//==========================================================//
// Bounds
// Define uma área retangular formada por dois pontos A e B.
//==========================================================//
#ifndef __BOUNDS_H__
#define __BOUNDS_H__

typedef struct point_s point_t;
typedef struct bounds_s bounds_t;

struct point_s
{
    int x, y;
};

struct bounds_s
{
    point_t a;
    point_t b;
};

bounds_t create_bounds(int x, int y, int width, int height)
{
    bounds_t bounds;

    bounds.a.x = x;
    bounds.a.y = y;

    bounds.b.x = x + width - 1;
    bounds.b.y = y + height - 1;

    return bounds;
}

bounds_t translate_bounds(bounds_t *bounds, int dx, int dy)
{
    bounds_t translated;

    translated.a.x = bounds->a.x + dx;
    translated.a.y = bounds->a.y + dy;

    translated.b.x = bounds->b.x + dx;
    translated.b.y = bounds->b.y + dy;

    return translated;
}

bool is_inside_bounds(bounds_t *bounds, int x, int y)
{
    return x >= bounds->a.x && x <= bounds->b.x && y >= bounds->a.y && y <= bounds->b.y;
}

#endif
