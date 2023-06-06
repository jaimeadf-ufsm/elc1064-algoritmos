/*********************************************************************
// The Matrix
// Um programa para a multiplição, soma, subtração e redução de matrizes.
//
// A expressão sendo calculada é exibida no centro da janela, sendo as
// matrizes X e Y para entrada e a matriz Z para o resultado. O tamanho
// máximo das matrizes é 9.
//
// No canto superior esquerdo, encontram-se 4 botões:
// - Os botões X, +, - e Gauss Jordan servem para selecionar a operação a ser realizada
// nas matrizes.
// - O botão ? gera valores aleatórios e também um tamanho aleatório.
//
// Os valores dos elementos das matrizes X e Y podem ser alterados com o teclado
// ao clicar dentro de sua caixa. Isso também se aplica as suas dimensões.
//
// O determinante é calculado para cada matriz sempre que ocorrer alguma alteração
// e, baseado no contexto, ele pode assumir:
// - ERROR: Caso não exista determinante para a determinada matriz (ou seja,
//   a matriz não é quadrada).
//
// Ao passar o mouse sobre os elementos da matriz de resultado, os elementos
// da matriz X e da matriz Y que resultaram naquele valor serão realçados.
// *********************************************************************/

#include <GL/glut.h>
#include <GL/freeglut_ext.h> //callback da wheel do mouse.

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "gl_canvas2d.h"
#include "Matrix.h"
#include "Button.h"

#define OPERATION_NUM 4

#define OPERATION_MULTIPLY 0
#define OPERATION_ADD 1
#define OPERATION_SUBTRACT 2
#define OPERATION_GAUSS_JORDAN 3

#define ELEMENT_SPACING 16

#define ZERO_THRESHOLD 0.000001

// variaveis globais
int windowWidth = 1280, windowHeight = 720;

int operation = OPERATION_MULTIPLY;

bool success = false;
char error[100];

Matrix matrixX;
Matrix matrixY;
Matrix matrixZ;

Button operationButtons[OPERATION_NUM];
Button randomizeButton;

// Gera tamanhos e elementos aleatórios para as matrizes
void Randomize()
{
    int size = rand() % MTX_MAX_SIZE + 1;

    SetMatrixRows(&matrixX, size);
    SetMatrixColumns(&matrixX, size);

    SetMatrixRows(&matrixY, size);
    SetMatrixColumns(&matrixY, size);

    RandomizeMatrix(&matrixX);
    RandomizeMatrix(&matrixY);
}

// Multiplica a matrix X e a matriz Y
void Multiply()
{
    if (MatrixColumns(&matrixX) != MatrixRows(&matrixY))
    {
        success = false;
        strcpy(error, "colunas X diferente de linhas Y");

        return;
    }

    int size = MatrixColumns(&matrixX);

    int rows = MatrixRows(&matrixX);
    int columns = MatrixColumns(&matrixY);

    SetMatrixRows(&matrixZ, rows);
    SetMatrixColumns(&matrixZ, columns);

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < columns; j++)
        {
            double result = 0;

            for (int k = 0; k < size; k++)
            {
                result += MatrixValue(&matrixX, i, k) * MatrixValue(&matrixY, k, j);
            }

            SetMatrixValue(&matrixZ, i, j, result);
        }
    }

    success = true;
}

// Adiciona a matrix X e a matriz Y
void Add()
{
    int rows = MatrixRows(&matrixX);
    int columns = MatrixColumns(&matrixX);

    if (rows != MatrixRows(&matrixY) || columns != MatrixColumns(&matrixY))
    {
        success = false;
        strcpy(error, "tamanho X diferente de tamanho Y");

        return;
    }

    SetMatrixRows(&matrixZ, rows);
    SetMatrixColumns(&matrixZ, columns);

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < columns; j++)
        {
            SetMatrixValue(&matrixZ, i, j, MatrixValue(&matrixX, i, j) + MatrixValue(&matrixY, i, j));
        }
    }

    success = true;
}

// Subtrai a matrix X e a matriz Y
void Subtract()
{
    int rows = MatrixRows(&matrixX);
    int columns = MatrixColumns(&matrixX);

    if (rows != MatrixRows(&matrixY) || columns != MatrixColumns(&matrixY))
    {
        success = false;
        strcpy(error, "tamanho X diferente de tamanho Y");

        return;
    }

    SetMatrixRows(&matrixZ, rows);
    SetMatrixColumns(&matrixZ, columns);

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < columns; j++)
        {
            SetMatrixValue(&matrixZ, i, j, MatrixValue(&matrixX, i, j) - MatrixValue(&matrixY, i, j));
        }
    }

    success = true;
}

// Troca os elementos de duas linhas da matriz Z
void SwapRows(int i1, int i2)
{
    for (int j = 0; j < MatrixRows(&matrixZ); j++)
    {
        double temp = MatrixValue(&matrixZ, i1, j);

        SetMatrixValue(&matrixZ, i1, j, MatrixValue(&matrixZ, i2, j));
        SetMatrixValue(&matrixZ, i2, j, temp);
    }
}

// Encontra o pivo de uma linha da matriz Z
// (isto é, o primeiro elemento da direita para a esquerda diferente de zero)
int SelectGaussianPivot(int i)
{
    for (int j = 0; j < MatrixRows(&matrixZ) && j < MatrixColumns(&matrixZ); j++)
    {
        if (fabs(MatrixValue(&matrixZ, i, j)) > ZERO_THRESHOLD)
        {
            return j;
        }
    }

    return -1;
}

// Elimina o pivo de todas as outras linhas da matriz Z
void EliminateGaussianPivot(int row, int pivot)
{
    for (int i = 0; i < MatrixRows(&matrixZ); i++)
    {
        if (i != row)
        {
            double coefficient = MatrixValue(&matrixZ, i, pivot) / MatrixValue(&matrixZ, row, pivot);

            for (int j = 0; j < MatrixColumns(&matrixZ); j++)
            {
                SetMatrixValue(&matrixZ, i, j, MatrixValue(&matrixZ, i, j) - coefficient * MatrixValue(&matrixZ, row, j));
            }
        }
    }
}

// Redução da matriz X pelo método de Gauss Jordan
void GaussJordan()
{
    int rows = MatrixRows(&matrixX);
    int columns = MatrixColumns(&matrixX);

    SetMatrixRows(&matrixZ, rows);
    SetMatrixColumns(&matrixZ, columns);

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < columns; j++)
        {
            SetMatrixValue(&matrixZ, i, j, MatrixValue(&matrixX, i, j));
        }
    }

    for (int i = 0; i < rows; i++)
    {
        int pivot = SelectGaussianPivot(i);

        if (pivot != -1)
        {
            EliminateGaussianPivot(i, pivot);
            SwapRows(i, pivot);
        }
    }

    for (int k = 0; k < rows; k++)
    {
        double coefficient = MatrixValue(&matrixZ, k, k);

        if (fabs(coefficient) > ZERO_THRESHOLD)
        {
            for (int j = 0; j < columns; j++)
            {
                SetMatrixValue(&matrixZ, k, j, MatrixValue(&matrixZ, k, j) / coefficient);
            }
        }
    }

    success = true;
}

// Calcula o resultado baseado na operação selecionada
void CalculateResult()
{
    switch (operation)
    {
    case OPERATION_MULTIPLY:
        Multiply();
        break;
    case OPERATION_ADD:
        Add();
        break;
    case OPERATION_SUBTRACT:
        Subtract();
        break;
    case OPERATION_GAUSS_JORDAN:
        GaussJordan();
        break;
    }
}

// Realça as posições que resultaram no elemento sobre o qual está o mouse
void HighlightResult()
{
    if (!success)
        return;

    for (int i = 0; i < MatrixRows(&matrixZ); i++)
    {
        for (int j = 0; j < MatrixColumns(&matrixZ); j++)
        {
            if (matrixZ.boxes[i][j].hovering)
            {
                Color8(203, 194, 255);
                HighlightMatrix(&matrixZ, i, j, i, j);

                switch (operation)
                {
                case OPERATION_MULTIPLY:
                    Color8(184, 223, 220);
                    HighlightMatrix(&matrixX, i, 0, i, MatrixColumns(&matrixX) - 1);

                    Color8(155, 205, 255);
                    HighlightMatrix(&matrixY, 0, j, MatrixRows(&matrixY) - 1, j);
                    break;
                case OPERATION_ADD:
                case OPERATION_SUBTRACT:
                    Color8(184, 223, 220);
                    HighlightMatrix(&matrixX, i, j, i, j);

                    Color8(155, 205, 255);
                    HighlightMatrix(&matrixY, i, j, i, j);
                    break;
                }
            }
        }
    }
}

// Desenha os botões
void DrawButtons()
{
    float x = 2 * ELEMENT_SPACING;
    float y = ELEMENT_SPACING + ButtonHeight();

    for (int i = 0; i < OPERATION_NUM; i++)
    {
        operationButtons[i].x = x;
        operationButtons[i].y = y;

        x += ELEMENT_SPACING;
        x += ButtonWidth(&operationButtons[i]);

        DrawButton(&operationButtons[i], operation == i);
    }

    x += 3 * ELEMENT_SPACING;

    randomizeButton.y = y;
    randomizeButton.x = x;

    DrawButton(&randomizeButton, false);
}

// Desenha a expressão
void DrawExpression()
{
    float x = 2 * ELEMENT_SPACING;
    float y = windowHeight / 2.0f;

    matrixX.x = x;
    matrixX.y = y + MatrixHeight(&matrixX) / 2;

    x += MatrixWidth(&matrixX);
    x += ELEMENT_SPACING;

    Color8(0, 0, 0);
    CV::text(x, y + FONT_SIZE / 2, operationButtons[operation].label);
    x += TextLength(operationButtons[operation].label);
    x += ELEMENT_SPACING;

    if (operation != OPERATION_GAUSS_JORDAN)
    {
        matrixY.x = x;
        matrixY.y = y + MatrixHeight(&matrixY) / 2;

        x += MatrixWidth(&matrixY);
        x += ELEMENT_SPACING;
    }

    Color8(0, 0, 0);
    CV::text(x, y + FONT_SIZE / 2, "=");
    x += TextLength("=");
    x += ELEMENT_SPACING;

    matrixZ.x = x;
    matrixZ.y = y + MatrixHeight(&matrixZ) / 2;

    HighlightResult();

    DrawMatrix(&matrixX);

    if (operation != OPERATION_GAUSS_JORDAN)
    {
        DrawMatrix(&matrixY);
    }

    if (success)
    {
        DrawMatrix(&matrixZ);
    }
    else
    {
        Color8(255, 0, 0);
        CV::text(x, y + FONT_SIZE / 2, error);
    }
}

// funcao chamada continuamente. Deve-se controlar o que desenhar por meio de variaveis
// globais que podem ser setadas pelo metodo keyboard()
void render()
{
    if (matrixX.changed || matrixY.changed)
    {
        CalculateResult();
    }

    UpdateMatrix(&matrixX);
    UpdateMatrix(&matrixY);
    UpdateMatrix(&matrixZ);

    DrawButtons();
    DrawExpression();
}

// funcao chamada toda vez que uma tecla for pressionada
void keyboard(int key)
{
    ProccessMatrixInput(&matrixX, key);
    ProccessMatrixInput(&matrixY, key);
}

// funcao chamada toda vez que uma tecla for liberada
void keyboardUp(int key)
{
    printf("\nLiberou tecla: %d", key);
}

// funcao para tratamento de mouse: cliques, movimentos e arrastos
void mouse(int button, int state, int wheel, int direction, int x, int y)
{
    printf("\nmouse %d %d %d %d %d %d", button, state, wheel, direction, x, y);

    ProccessMatrixMouse(&matrixX, x, y, button, state);
    ProccessMatrixMouse(&matrixY, x, y, button, state);
    ProccessMatrixMouse(&matrixZ, x, y, button, state);

    for (int i = 0; i < OPERATION_NUM; i++)
    {
        ProccessButtonMouse(&operationButtons[i], x, y);
    }

    ProccessButtonMouse(&randomizeButton, x, y);

    if (button == 0 && state == 0)
    {
        for (int i = 0; i < OPERATION_NUM; i++)
        {
            if (operationButtons[i].hovering)
            {
                operation = i;
                CalculateResult();
            }
        }

        if (randomizeButton.hovering)
        {
            Randomize();
        }
    }
}

int main(void)
{
    srand(time(NULL));

    InitializeMatrix(&matrixX, 'x', 4, 4, false, "%.0f");
    InitializeMatrix(&matrixY, 'y', 4, 4, false, "%.0f");
    InitializeMatrix(&matrixZ, 'z', 0, 0, true, "%.2f");

    RandomizeMatrix(&matrixX);
    RandomizeMatrix(&matrixY);

    InitializeButton(&operationButtons[OPERATION_MULTIPLY], "X");
    InitializeButton(&operationButtons[OPERATION_ADD], "+");
    InitializeButton(&operationButtons[OPERATION_SUBTRACT], "-");
    InitializeButton(&operationButtons[OPERATION_GAUSS_JORDAN], "Gauss Jordan");

    InitializeButton(&randomizeButton, "?");

    CV::init(&windowWidth, &windowHeight, "The Matrix");
    CV::run();
}
