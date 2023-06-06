/*********************************************************************
// Button.h
// Implementa��o de um simples bot�o que possui uma largura e uma altura
// acompanhadas de um texto em seu centro.
// *********************************************************************/

#ifndef BUTTON_H
#define BUTTON_H

#define BTN_PADDING 16

typedef struct
{
    float x, y;
    bool hovering;

    char label[20];
} Button;

// Inicializa o bot�o
void InitializeButton(Button *button, const char *label)
{
    button->hovering = false;

    strcpy(button->label, label);
}

float ButtonWidth(Button *button)
{
    return 2 * BTN_PADDING + TextLength(button->label);
}

float ButtonHeight()
{
    return 2 * BTN_PADDING + FONT_SIZE;
}

// Verifica se a coordenada est� dentro do bot�o
bool IsInsideButton(Button *button, int x, int y)
{
    float distanceX = x - button->x;
    float distanceY = button->y - y;

    return distanceX >= 0 && distanceX <= ButtonWidth(button) &&
           distanceY >= 0 && distanceY <= ButtonHeight();
}

// Desenha o bot�o
void DrawButton(Button *button, bool selected)
{
    float x1 = button->x;
    float y1 = button->y;

    float x2 = x1 + ButtonWidth(button);
    float y2 = y1 - ButtonHeight();

    if (button->hovering)
    {
        Color8(255, 199, 128);
        CV::rectFill(x1, y1, x2, y2);
    }

    Color8(255, 145, 3);
    CV::rect(x1, y1, x2, y2);

    if (selected)
        CV::rectFill(x1, y1, x2, y2);

    Color8(0, 0, 0);
    CV::text(x1 + BTN_PADDING, y1 - BTN_PADDING, button->label);
}

// Processa o mouse para o bot�o
void ProccessButtonMouse(Button *button, int mouseX, int mouseY)
{
    button->hovering = IsInsideButton(button, mouseX, mouseY);
}

#endif
