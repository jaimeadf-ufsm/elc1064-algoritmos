//=========================================================//
// Text Box
// Controle de uma caixa de texto:
// - Cursor.
// - Seleção.
// - Inserção de texto.
// - Remoção de texto.
//==========================================================//

#ifndef __TEXT_BOX_H__
#define __TEXT_BOX_H__

#include <stdlib.h>
#include "bounds.h"

typedef struct text_box_s text_box_t;

struct text_box_s
{
    int size;
    int capacity;

    char *text;

    int selection_start;
    int selection_end;

    bool focused;
    bool dragging;

    bounds_t bounds;
};

void init_text_box(text_box_t *text_box, int capacity, bounds_t bounds)
{
    text_box->capacity = capacity;
    text_box->size = 0;

    text_box->text = (char *)malloc(capacity);

    text_box->selection_start = 0;
    text_box->selection_end = 0;

    text_box->focused = false;
    text_box->dragging = false;

    text_box->bounds = bounds;
}

void remove_segment(text_box_t *text_box, int start, int length)
{
    text_box->size -= length;

    for (int i = start; i < text_box->size; i++)
    {
        text_box->text[i] = text_box->text[i + length];
    }
}

void insert_character(text_box_t *text_box, char character, int position)
{
    for (int i = text_box->size; i > position; i--)
    {
        text_box->text[i] = text_box->text[i - 1];
    }

    text_box->text[position] = character;
    text_box->size++;
}

int constrain_position(text_box_t *text_box, int index)
{
    if (index <= 0)
    {
        return 0;
    }

    if (index > text_box->size)
    {
        return text_box->size;
    }

    return index;
}

int get_selection_min(text_box_t *text_box)
{
    return text_box->selection_start < text_box->selection_end ? text_box->selection_start : text_box->selection_end;
}

int get_selection_max(text_box_t *text_box)
{
    return text_box->selection_start > text_box->selection_end ? text_box->selection_start : text_box->selection_end;

}

bool has_selection(text_box_t *text_box)
{
    return text_box->selection_start != text_box->selection_end;
}

void move_caret_to(text_box_t *text_box, int index)
{
    text_box->selection_start = constrain_position(text_box, index);
    text_box->selection_end = text_box->selection_start;
}

void move_selection_to(text_box_t *text_box, int index)
{
    text_box->selection_end = constrain_position(text_box, index);
}

void move_caret_left(text_box_t *text_box)
{
    if (has_selection(text_box))
    {
        move_caret_to(text_box, get_selection_min(text_box));
    }
    else
    {
        move_caret_to(text_box, text_box->selection_start - 1);
    }
}

void move_caret_right(text_box_t *text_box)
{
    if (has_selection(text_box))
    {
        move_caret_to(text_box, get_selection_max(text_box));
    }
    else
    {
        move_caret_to(text_box, text_box->selection_start + 1);
    }
}

void move_selection_left(text_box_t *text_box)
{
    move_selection_to(text_box, text_box->selection_end - 1);
}

void move_selection_right(text_box_t *text_box)
{
    move_selection_to(text_box, text_box->selection_end + 1);
}

void move_to_start(text_box_t *text_box)
{
    move_caret_to(text_box, 0);
}

void move_to_end(text_box_t *text_box)
{
    move_caret_to(text_box, text_box->size);
}

void select_all(text_box_t *text_box)
{
    text_box->selection_start = 0;
    text_box->selection_end = text_box->size;
}

void erase_selection(text_box_t *text_box)
{
    int min = get_selection_min(text_box);
    int max = get_selection_max(text_box);

    remove_segment(text_box, min, max - min);
    move_caret_to(text_box, min);
}

void erase_at_caret(text_box_t *text_box)
{
    move_selection_to(text_box, text_box->selection_start + 1);
    erase_selection(text_box);
}

void erase_before_caret(text_box_t *text_box)
{
    move_selection_to(text_box, text_box->selection_start - 1);
    erase_selection(text_box);
}

void insert_at_caret(text_box_t *text_box, char character)
{
    erase_selection(text_box);

    if (text_box->size < text_box->capacity)
    {
        insert_character(text_box, character, text_box->selection_start);
        move_caret_to(text_box, text_box->selection_start + 1);
    }
}

#endif
