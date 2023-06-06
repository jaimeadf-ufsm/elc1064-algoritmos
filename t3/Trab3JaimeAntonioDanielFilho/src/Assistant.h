/*********************************************************************
// Assistant.h
// Defini��o de uma s�rie de fun��es e constantes auxiliares.
// *********************************************************************/

#ifndef ASSISTANT_H
#define ASSISTANT_H

#include "gl_canvas2d.h"

#define FONT_SIZE 12
#define TEXT_BUFFER_SIZE 400

// Define a cor baseado em valores de 8 bits (0 a 255)
void Color8(int r, int g, int b)
{
    CV::color(r / 255.0f, g / 255.0f, b / 255.0f);
}

// Calcula a largura ocupado por um texto
float TextLength(const char *text)
{
    return strlen(text) * 8;
}

#endif
