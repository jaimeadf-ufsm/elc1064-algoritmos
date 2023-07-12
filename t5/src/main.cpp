//==========================================================//
// Manipulação de Texto
// T5 - Jaime Antonio Daniel Filho
//
// Implementação de caixas de texto
// A caixa de texto chamada ENTRADA ao ser clicada permite
// que o usuário digite letras e espaços.
//
// As caixas abaixo são atualizadas de acordo com o conteúdo
// da ENTRADA:
// - DUPLICADO: Texto com cada letra duplicada.
// - MAIÚSCULO INVERTIDO: Texto em carecteres maiúsculo e
//   de trás para frente.
// - CONTAGEM: Número de carecteres digitados.
//
// Funcionalidades:
// - Limite de 20 carecteres na entrada.
// - Cursor no qual o texto é inserido.
// - Navegação utilizando as setas do teclado ou o mouse.
// - Seleção utilizando SHIFT junto às setas do teclado ou
//   o mouse.
// - HOME (cursor para o início).
// - END (cursor para o final).
// - DELETE (remove o caractere sobre o cursor).
// - BACKSPACE (remove o caractere atrás do cursor).
// - CTRL + A (seleçào de todo o texto).
//
// - Em seleções:
//   * DELETE e BACKSPACE apagam toda a seleção.
//   * Novos carecteres ocupam o lugar da seleção.
//   * Seta à direita move o cursor para o início da seleção.
//   * Seta à esquerda move o cursor para o final de seleção.
//==========================================================//
#include <stdio.h>
#include <ctype.h>

#include "mpc_c.h"
#include "text_box.h"

#define SELECTION_BACKGROUND 80, 80, 80

const int APP_LINES = 35;
const int APP_COLUMNS = 100;

const int APP_FPS = 60;

const int INPUT_CAPACITY = 20;

const int INPUT_WIDTH = 43;
const int INPUT_HEIGHT = 3;

const int INPUT_OFFSET = (INPUT_HEIGHT + 0);

const int KEY_SELECT_ALL = 1;
const int KEY_BACKSPACE = 8;
const int KEY_DELETE = 127;
const int KEY_ARROW_LEFT = 100;
const int KEY_ARROW_RIGHT = 102;
const int KEY_HOME = 106;
const int KEY_END = 107;

text_box_t g_input;
text_box_t g_duplicated;
text_box_t g_uppercase;
text_box_t g_count;

void display_bounds(bounds_t *bounds, const char *label)
{
    for (int x = bounds->a.x + 1; x < bounds->b.x; x++)
    {
        mpcSetChar(bounds->a.y, x, 9, F_STD, WHITE, BLACK, 1.0);
        mpcSetChar(bounds->b.y, x, 9, F_STD, WHITE, BLACK, 1.0);
    }

    for (int y = bounds->a.y + 1; y < bounds->b.y; y++)
    {
        mpcSetChar(y, bounds->a.x, 10, F_STD, WHITE, BLACK, 1.0);
        mpcSetChar(y, bounds->b.x, 10, F_STD, WHITE, BLACK, 1.0);
    }

    for (int i = 0; i < strlen(label); i++)
    {
        mpcSetChar(bounds->a.y, bounds->a.x + 1 + i, label[i], F_STD, WHITE, BLACK, 1.0);
    }

    mpcSetChar(bounds->a.y, bounds->a.x, 11, F_STD, WHITE, BLACK, 1.0);
    mpcSetChar(bounds->a.y, bounds->b.x, 12, F_STD, WHITE, BLACK, 1.0);
    mpcSetChar(bounds->b.y, bounds->a.x, 13, F_STD, WHITE, BLACK, 1.0);
    mpcSetChar(bounds->b.y, bounds->b.x, 14, F_STD, WHITE, BLACK, 1.0);
}

void display_text_box(text_box_t *text_box, const char *label)
{
    int x = text_box->bounds.a.x + 1;
    int y = (text_box->bounds.a.y + text_box->bounds.b.y) / 2;

    int min = get_selection_min(text_box);
    int max = get_selection_max(text_box);

    if (text_box->focused)
    {
        if (!has_selection(text_box))
        {
            mpcSetCursorVisible(true);
            mpcSetCursorPos(y, x + min);
        }
    }

    display_bounds(&text_box->bounds, label);

    for (int i = 0; i < text_box->size; i++)
    {
        if (i >= min && i < max)
        {
            mpcSetChar(y, x + i, text_box->text[i], F_STD, WHITE, SELECTION_BACKGROUND, 1.0);
        }
        else
        {
            mpcSetChar(y, x + i, text_box->text[i], F_STD, WHITE, BLACK, 1.0);
        }
    }
}

void handle_text_box_keyboard(text_box_t *text_box, int key, int modifier, bool special, bool up)
{
    if (!text_box->focused)
    {
        return;
    }

    if (up)
    {
        return;
    }

    if (special)
    {
        bool select = modifier & ACTIVE_SHIFT;

        switch (key)
        {
            case KEY_ARROW_LEFT:
                if (select)
                {
                    move_selection_left(text_box);
                }
                else
                {
                    move_caret_left(text_box);
                }
                break;
            case KEY_ARROW_RIGHT:
                if (select)
                {
                    move_selection_right(text_box);
                }
                else
                {
                    move_caret_right(text_box);
                }
                break;
            case KEY_HOME:
                move_to_start(text_box);
                break;
            case KEY_END:
                move_to_end(text_box);
                break;
        }

        return;
    }

    switch (key)
    {
        case KEY_SELECT_ALL:
            select_all(text_box);
            break;
        case KEY_BACKSPACE:
            if (has_selection(text_box))
            {
                erase_selection(text_box);
            }
            else
            {
                erase_before_caret(text_box);
            }
            break;
        case KEY_DELETE:
            if (has_selection(text_box))
            {
                erase_selection(text_box);
            }
            else
            {
                erase_at_caret(text_box);
            }
            break;
        default:
            if (isalpha(key) || key == ' ')
            {
                insert_at_caret(text_box, key);
            }
            break;
    }
}

void handle_text_box_mouse(text_box_t *text_box, int line, int column, int button, int state)
{
    switch (button)
    {
        case -1:
            if (text_box->dragging)
            {
                move_selection_to(text_box, column);
            }
            break;
        case 0:
            text_box->dragging = false;

            if (state == 0)
            {
                if (is_inside_bounds(&text_box->bounds, column, line))
                {
                    text_box->focused = true;
                    text_box->dragging = true;

                    move_caret_to(text_box, column - text_box->bounds.a.x - 1);
                }
                else
                {
                    text_box->focused = false;
                }
            }
            break;
    }
}

void cbKeyboard(int key, int modifier, bool special, bool up)
{
    handle_text_box_keyboard(&g_input, key, modifier, special, up);

}

void cbMouse(int line, int column, int button, int state)
{
    handle_text_box_mouse(&g_input, line, column, button, state);
}

void fill_text_boxes()
{
    sprintf(g_count.text, "%d", g_input.size);

    g_duplicated.size = 2 * g_input.size;
    g_uppercase.size = g_input.size;
    g_count.size = strlen(g_count.text);

    for (int i = 0; i < g_input.size; i++)
    {
        g_duplicated.text[2 * i] = g_input.text[i];
        g_duplicated.text[2 * i + 1] = g_input.text[i];

        g_uppercase.text[g_input.size - i - 1] = toupper(g_input.text[i]);
    }
}

void cbUpdate()
{
    fill_text_boxes();

    mpcSetCursorVisible(false);

    display_text_box(&g_input, "entrada");
    display_text_box(&g_duplicated, "duplicado");
    display_text_box(&g_uppercase, "maiusculo invertido");
    display_text_box(&g_count, "contagem");
}

int main(void)
{
    init_text_box(&g_input, INPUT_CAPACITY, create_bounds(0, 0, INPUT_WIDTH, INPUT_HEIGHT));
    init_text_box(&g_duplicated, 2 * INPUT_CAPACITY, translate_bounds(&g_input.bounds, 0, INPUT_OFFSET));
    init_text_box(&g_uppercase, INPUT_CAPACITY, translate_bounds(&g_duplicated.bounds, 0, INPUT_OFFSET));
    init_text_box(&g_count, INPUT_CAPACITY, translate_bounds(&g_uppercase.bounds, 0, INPUT_OFFSET));

    mpcSetSize(APP_LINES, APP_COLUMNS);

    mpcSetKeyboardFunc(cbKeyboard);
    mpcSetMouseFunc(cbMouse);
    mpcSetUpdateFunc(cbUpdate);

    mpcAbout();
    mpcVersion();

    mpcSetCursorColor(WHITE);

    mpcRun(APP_FPS);

    return 0;
}
