/*********************************************************************
// Matrix.h
// Implementação da matriz e da lógica principal do programa. Toda matriz
// pode armazenar no máximo 9 linhas e 9 colunas. Seu determinante é
// calculado automaticamente semore que ocorrerem alterações e não possui
// limitação de tamanho.
// *********************************************************************/

#ifndef MATRIX_H
#define MATRIX_H

#include <limits.h>
#include "NumberBox.h"

#define MTX_MAX_SIZE 9

#define MTX_SPACING 8

#define MTX_BRACKET_THICKNESS 2
#define MTX_BRACKET_LENGTH 6

#define MTX_HIGHLIGHT_PADDING 4

#define MTX_DIM_SEPARATOR " x "

typedef struct
{
	char letter;
	float x, y;

	bool changed;

	double determinant;

	NumberBox rows, columns;
	NumberBox boxes[MTX_MAX_SIZE][MTX_MAX_SIZE];
} Matrix;

// Inicializa a matriz
void InitializeMatrix(Matrix* matrix, char letter, int rows, int columns, bool locked, const char *format)
{
	matrix->letter = letter;
	matrix->changed = true;

	InitializeNumberBox(&matrix->rows, rows, 0, MTX_MAX_SIZE, locked, "%.0f");
	InitializeNumberBox(&matrix->columns, columns, 0, MTX_MAX_SIZE, locked, "%.0f");

	for (int i = 0; i < MTX_MAX_SIZE; i++)
	{
		for (int j = 0; j < MTX_MAX_SIZE; j++)
		{
			InitializeNumberBox(&matrix->boxes[i][j], 0, INT_MIN, INT_MAX, locked, format);
		}
	}
}

// Retorna o valor armazenado em tal linha e em tal coluna da matriz
double MatrixValue(Matrix *matrix, int i, int j)
{
	return matrix->boxes[i][j].value;
}

// Retorna o número de linhas da matriz
int MatrixRows(Matrix *matrix)
{
	return (int)matrix->rows.value;
}

// Retorna o número de colunas da matriz
int MatrixColumns(Matrix *matrix)
{
	return (int)matrix->columns.value;
}

// Define o valor armazenado em tal linha e em tal coluna da matriz
void SetMatrixValue(Matrix *matrix, int i, int j, double value)
{
	if (MatrixValue(matrix, i, j) != value)
	{
		matrix->changed = true;
		matrix->boxes[i][j].value = value;
	}
}

// Define o número de linhas da matriz
void SetMatrixRows(Matrix *matrix, int rows)
{
	if (MatrixRows(matrix) != rows)
	{
		matrix->changed = true;
		matrix->rows.value = rows;
	}
}

// Define o número de colunas da matriz
void SetMatrixColumns(Matrix *matrix, int columns)
{
	if (MatrixColumns(matrix) != columns)
	{
		matrix->changed = true;
		matrix->columns.value = columns;
	}
}

// Define valores aleatórios de -10 até 10 para a matriz
void RandomizeMatrix(Matrix* matrix)
{
	for (int i = 0; i < MTX_MAX_SIZE; i++)
	{
		for (int j = 0; j < MTX_MAX_SIZE; j++)
		{
			SetMatrixValue(matrix, i, j, rand() % 21 - 10);
		}
	}
}

// Calcula a largura de uma coluna da matriz
float MatrixColumnWidth(Matrix *matrix, int j)
{
	float columnWidth = 0;

	for (int i = 0; i < MatrixRows(matrix); i++)
	{
		float width = NumberBoxWidth(&matrix->boxes[i][j]);

		if (width > columnWidth)
			columnWidth = width;
	}

	return columnWidth;
}

// Calcula a largura total da matriz
float MatrixWidth(Matrix *matrix)
{
	float width = MTX_SPACING;

	for (int j = 0; j < MatrixColumns(matrix); j++)
	{
		width += MatrixColumnWidth(matrix, j);
		width += MTX_SPACING;
	}

	width += 2 * MTX_SPACING;
	width += NumberBoxWidth(&matrix->rows);
	width += TextLength(MTX_DIM_SEPARATOR);
	width += NumberBoxWidth(&matrix->columns);

	return width;
}

// Calcula a altura total da matriz
float MatrixHeight(Matrix *matrix)
{
	return FONT_SIZE + (2 + MatrixRows(matrix)) * NumberBoxHeight();
}

// Verifica se a matriz possui determinant (ou seja, quadrada)
bool HasDeterminant(Matrix *matrix)
{
	return MatrixRows(matrix) == MatrixColumns(matrix);
}

// Calcula o determinante de um array bidimensional de inteiros de qualquer ordem
double CalculateMatrixDeterminant(double elements[MTX_MAX_SIZE][MTX_MAX_SIZE], int size)
{
	if (size == 1)
		return elements[0][0];

	double minor[MTX_MAX_SIZE][MTX_MAX_SIZE];

	double determinant = 0;
	double signal = 1;

	for (int expansionJ = 0; expansionJ < size; expansionJ++)
	{
		for (int i = 1; i < size; i++)
		{
			for (int j = 0; j < expansionJ; j++)
				minor[i - 1][j] = elements[i][j];

			for (int j = expansionJ + 1; j < size; j++)
				minor[i - 1][j - 1] = elements[i][j];
		}

		double cofactor = signal * elements[0][expansionJ];
		double minorDeterminant = CalculateMatrixDeterminant(minor, size - 1);

		double term = cofactor * minorDeterminant;

		determinant += term;
		signal *= -1;
	}

	return determinant;
}

// Atualiza o determinante da matriz
void UpdateMatrix(Matrix *matrix)
{
	if (!matrix->changed)
		return;

	matrix->changed = false;

	if (HasDeterminant(matrix))
	{
		double elements[MTX_MAX_SIZE][MTX_MAX_SIZE];
		int size = MatrixRows(matrix);

		for (int i = 0; i < size; i++)
		{
			for (int j = 0; j < size; j++)
			{
				elements[i][j] = MatrixValue(matrix, i, j);
			}
		}

		matrix->determinant = CalculateMatrixDeterminant(elements, size);
	}
}

// Realça uma posição até outra posição da matriz
void HighlightMatrix(Matrix *matrix, int fromI, int fromJ, int toI, int toJ)
{
	float boxHeight = NumberBoxHeight();

	float boxX = matrix->x + MTX_SPACING;
	float boxY = matrix->y - FONT_SIZE;

	float startX = boxX;
	float startY = boxY - (boxHeight + MTX_SPACING) * (MatrixRows(matrix) - fromI);

	for (int j = 0; j < fromJ; j++)
	{
		startX += MatrixColumnWidth(matrix, j);
		startX += MTX_SPACING;
	}

	float endX = startX + MatrixColumnWidth(matrix, fromJ);
	float endY = startY + boxHeight + (boxHeight + MTX_SPACING) * (toI - fromI);

	for (int j = fromJ + 1; j <= toJ; j++)
	{
		endX += MatrixColumnWidth(matrix, j);
		endX += MTX_SPACING;
	}

	CV::rectFill(
		startX - MTX_HIGHLIGHT_PADDING,
		startY - MTX_HIGHLIGHT_PADDING,
		endX + MTX_HIGHLIGHT_PADDING,
		endY + MTX_HIGHLIGHT_PADDING
	);
}

// Desenha a matriz
void DrawMatrix(Matrix *matrix)
{
	float x = matrix->x;
	float y = matrix->y;

	char determinantText[TEXT_BUFFER_SIZE];

	if (HasDeterminant(matrix))
	{
		sprintf(determinantText, "det(%c) = %.2f", matrix->letter, matrix->determinant);
	}
	else
	{
		sprintf(determinantText, "det(%c) = ERROR", matrix->letter);
	}

	Color8(0, 0, 0);
	CV::text(x, y, determinantText);
	y -= FONT_SIZE;

	float boxHeight = NumberBoxHeight();

	float boxX = x + MTX_SPACING;
	float boxY = y - MTX_SPACING;

	for (int j = 0; j < MatrixColumns(matrix); j++)
	{
		float columnWidth = MatrixColumnWidth(matrix, j);

		boxY = y - MTX_SPACING;

		for (int i = MatrixRows(matrix) - 1; i >= 0; i--)
		{
			float boxWidth = NumberBoxWidth(&matrix->boxes[i][j]);

			matrix->boxes[i][j].x = boxX + (columnWidth - boxWidth) / 2;
			matrix->boxes[i][j].y = boxY;

			DrawNumberBox(&matrix->boxes[i][j]);

			boxY -= boxHeight + MTX_SPACING;
		}

		boxX += columnWidth;
		boxX += MTX_SPACING;
	}

	Color8(0, 0, 0);

	char labelText[3];
	sprintf(labelText, "%c:", matrix->letter);

	CV::text(x, boxY - MTX_SPACING, labelText);

	CV::rectFill(x, y, x + MTX_BRACKET_THICKNESS, boxY);
	CV::rectFill(x, y, x + MTX_BRACKET_LENGTH, y - MTX_BRACKET_THICKNESS);
	CV::rectFill(x, boxY, x + MTX_BRACKET_LENGTH, boxY + MTX_BRACKET_THICKNESS);

	CV::rectFill(boxX, y, boxX - MTX_BRACKET_THICKNESS, boxY);
	CV::rectFill(boxX, y, boxX - MTX_BRACKET_LENGTH, y - MTX_BRACKET_THICKNESS);
	CV::rectFill(boxX, boxY, boxX - MTX_BRACKET_LENGTH, boxY + MTX_BRACKET_THICKNESS);

	float dimensionsX = boxX + 2 * MTX_SPACING;
	float dimensionsY = y;

	float textHeight = FONT_SIZE;

	matrix->rows.x = dimensionsX;
	matrix->rows.y = dimensionsY;

	dimensionsX += NumberBoxWidth(&matrix->rows);

	CV::text(dimensionsX, dimensionsY - (boxHeight - textHeight) / 2, MTX_DIM_SEPARATOR);

	dimensionsX += TextLength(MTX_DIM_SEPARATOR);

	matrix->columns.x = dimensionsX;
	matrix->columns.y = dimensionsY;

	DrawNumberBox(&matrix->rows);
	DrawNumberBox(&matrix->columns);
}

// Processa o mouse para a matriz
void ProccessMatrixMouse(Matrix *matrix, int mouseX, int mouseY, int mouseButton, int mouseState)
{
	for (int i = 0; i < MatrixRows(matrix); i++)
	{
		for (int j = 0; j < MatrixColumns(matrix); j++)
		{
			ProccessNumberBoxMouse(&matrix->boxes[i][j], mouseX, mouseY, mouseButton, mouseState);
		}
	}

	ProccessNumberBoxMouse(&matrix->rows, mouseX, mouseY, mouseButton, mouseState);
	ProccessNumberBoxMouse(&matrix->columns, mouseX, mouseY, mouseButton, mouseState);
}

// Processa a entrada do teclado para a matriz
void ProccessMatrixInput(Matrix *matrix, int key)
{
	for (int i = 0; i < MatrixRows(matrix); i++)
	{
		for (int j = 0; j < MatrixColumns(matrix); j++)
		{
			matrix->changed = ProccessNumberBoxInput(&matrix->boxes[i][j], key) || matrix->changed;
		}
	}

	matrix->changed = ProccessNumberBoxInput(&matrix->rows, key) || matrix->changed;
	matrix->changed = ProccessNumberBoxInput(&matrix->columns, key) || matrix->changed;
}

#endif
