/*********************************************************************
// NumberBox.h
// Implementação de uma caixa de entrada na qual o usuário pode digitar
// seus próprios números com o teclado. A caixa muda a cor de suas bordas
// quando o mouse está sobre ela ou quando está recebendo a entrada do
// usuário.
// *********************************************************************/

#ifndef NUMBERBOX_H
#define NUMBERBOX_H

#include "Assistant.h"

#define NB_PADDING 8

typedef struct
{
    float x, y;

    double value;
    double min, max;

    bool locked;
    char format[10];

    bool hovering, focused;
} NumberBox;

// Inicia a caixa de números
void InitializeNumberBox(NumberBox *box, double value, double min, double max, bool locked, const char *format)
{
    box->x = 0;
    box->y = 0;

    box->value = value;

    box->min = min;
    box->max = max;

    box->locked = locked;

    box->hovering = false;
    box->focused = false;

    strcpy(box->format, format);
}

// Imprime o valor da caixa de número no texto de destino
void PrintNumberBox(NumberBox *box, char *destination)
{
    sprintf(destination, box->format, box->value);
}

// Calcula a largura da caixa de número (largura do texto + bordas)
float NumberBoxWidth(NumberBox *box)
{
    char text[TEXT_BUFFER_SIZE];
    PrintNumberBox(box, text);

    return 2 * NB_PADDING + TextLength(text);
}

// Calcula a altura da caixa de número (altura do texto + bordas)
float NumberBoxHeight()
{
    return 2 * NB_PADDING + FONT_SIZE;
}

// Verifica se a coordenada está dentro da caixa de número
bool IsInsideNumberBox(NumberBox *box, int x, int y)
{
    float width = NumberBoxWidth(box);
    float height = NumberBoxHeight();

    float distanceX = x - box->x;
    float distanceY = box->y - y;

    return distanceX >= 0 && distanceX <= width && distanceY >= 0 && distanceY <= height;
}

// Desenha a caixa de número
void DrawNumberBox(NumberBox *box)
{
    char text[TEXT_BUFFER_SIZE];
    PrintNumberBox(box, text);

    float textHeight = FONT_SIZE;
    float textWidth = TextLength(text);

    float x = box->x + NB_PADDING;
    float y = box->y - NB_PADDING;

    Color8(0, 0, 0);
    CV::text(x, y, text);

    x += textWidth;

    Color8(200, 200, 200);

    if (!box->locked)
    {
        if (box->hovering)
        {
            Color8(255, 199, 128);
        }

        if (box->focused)
        {
            float caretX = x + 1;

            Color8(0, 0, 0);
            CV::rectFill(caretX, y, caretX + 1, y - textHeight);

            Color8(255, 145, 3);
        }
    }

    x += NB_PADDING;
    y -= textHeight + NB_PADDING;

    CV::rect(box->x, box->y, x, y);
}

// Processa o mouse para a caixa de número
void ProccessNumberBoxMouse(NumberBox *box, int mouseX, int mouseY, int mouseButton, int mouseState)
{
    box->hovering = IsInsideNumberBox(box, mouseX, mouseY);

    if (!box->locked && mouseButton == 0 && mouseState == 0)
    {
        box->focused = box->hovering;
    }
}

// Processa a entrada do teclado para a caixa de número
// Retorna verdadeiro caso houver alterações
bool ProccessNumberBoxInput(NumberBox *box, int key)
{
    if (box->focused)
    {
        switch (key)
        {
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '0':
        {
            int digit = key - '0';

            box->value *= 10;

            if (box->value >= 0)
                box->value += digit;
            else
                box->value -= digit;

            break;
        }
        case '-':
            if (box->min < 0)
                box->value *= -1;
            break;
        case 8:
            box->value = (int)(box->value / 10);
            break;
        }

        if (box->value > box->max)
            box->value = box->max;

        if (box->value < box->min)
            box->value = box->min;

        return true;
    }

    return false;
}

#endif