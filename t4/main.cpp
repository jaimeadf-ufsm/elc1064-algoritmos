/*********************************************************************
// Canvas para desenho, criada sobre a API OpenGL. Nao eh necessario conhecimentos de OpenGL para usar.
//  Autor: Cesar Tadeu Pozzer
//         05/2020
//
//  Pode ser utilizada para fazer desenhos, animacoes, e jogos simples.
//  Tem tratamento de mouse e teclado
//  Estude o OpenGL antes de tentar compreender o arquivo gl_canvas.cpp

//  Todos os arquivos do projeto devem ser .cpp
//
//  Versao 2.0
//
// *********************************************************************/

#include <GL/glut.h>
#include <GL/freeglut_ext.h> //callback da wheel do mouse.

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "gl_canvas2d.h"

#define EMPTY -1
#define WHITE 0
#define BLACK 1

#define START 3

#define SELECTING_FROM 0
#define SELECTING_TO 1

#define BOARD_X 0
#define BOARD_Y 0

#define SIDEBAR_WIDTH 200

typedef struct {
    int row;
    int column;
} Position;

typedef struct {
    int size;
    int turn;

    int *pieces;
} Board;

typedef struct {
    int state;

    Position from;
    Position to;
} Selection;

typedef struct {
    int x, y;
    int value;
} Button;

Board board;
Selection selection;

int screenWidth = 1000, screenHeight = 800;

void Color8(unsigned char r, unsigned char g, unsigned char b)
{
    CV::color(r / 255.0f, g / 255.0f, b / 255.0f);
}

void InitBoard(int size)
{
    board.size = size;
    board.pieces = (int*)malloc(size * size * sizeof(int));
}

bool IsDarkSquare(int row, int column)
{
    return (row + column) % 2 == 1;
}

int GetBoardDimensions()
{
    int maxWidth = screenWidth - SIDEBAR_WIDTH;
    int maxHeight = screenHeight;

    return maxWidth < maxHeight ? maxWidth : maxHeight;
}

int GetCellDimensions()
{
    return GetBoardDimensions() / board.size;
}

int GetBoardIndex(int row, int column)
{
    return row * board.size + column;
}

void ResizeBoard(int size)
{
    free(board.pieces);
    board.size = size;
    board.pieces = (int*)malloc(size * size * sizeof(int));
}

void ResetBoard()
{
    board.turn = WHITE;

    for (int i = 0; i < board.size; i++)
    {
        for (int j = 0; j < board.size; j++)
        {
            board.pieces[GetBoardIndex(i, j)] = EMPTY;
        }
    }

    for (int i = 0; i < START; i++)
    {
        for (int j = 0; j < board.size; j++)
        {
            int oppositeI = board.size - i - 1;

            if (IsDarkSquare(i, j))
            {
                board.pieces[GetBoardIndex(i, j)] = BLACK;
            }

            if (IsDarkSquare(oppositeI, j))
            {
                board.pieces[GetBoardIndex(oppositeI, j)] = WHITE;
            }
        }
    }
}

bool IsInsideBoard(int x, int y)
{
    int dimensions = GetBoardDimensions();

    return x >= BOARD_X && x <= BOARD_X + dimensions && y >= BOARD_Y && y <= BOARD_Y + dimensions;
}

void RenderPiece(int piece, int x, int y)
{
    int cell = GetCellDimensions();

    if (piece == EMPTY)
    {
        return;
    }

    switch (piece)
    {
    case WHITE:
        Color8(255, 255, 255);
        break;
    case BLACK:
        Color8(0, 0, 0);
        break;
    }

    CV::circleFill(x, y, cell * 0.3, 128);

}

void RenderBoard()
{
    int cell = GetCellDimensions();

    for (int i = 0; i < board.size; i++)
    {
        for (int j = 0; j < board.size; j++)
        {
            int index = GetBoardIndex(i, j);

            float x = BOARD_X + j * cell;
            float y = BOARD_Y + i * cell;

            float center_x = x + cell / 2;
            float center_y = y + cell / 2;

            int piece = board.pieces[index];

            if (IsDarkSquare(i, j))
            {
                Color8(181, 135, 99);
            }
            else
            {
                Color8(240, 218, 181);
            }

            CV::rectFill(x, y, x + cell, y + cell);

            RenderPiece(piece, center_x, center_y);
        }
    }
}

void InitSelection()
{
    selection.state = SELECTING_FROM;

    selection.from.row = 0;
    selection.from.column = 0;

    selection.to.row = 0;
    selection.to.column = 0;
}

void RestartSelection()
{
    selection.state = SELECTING_FROM;
}

void RenderSelection()
{
    int cell = GetCellDimensions();

    int fromX = BOARD_X + selection.from.column * cell;
    int fromY = BOARD_Y + selection.from.row * cell;

    Color8(0, 0, 255);
    CV::rect(fromX, fromY, fromX + cell, fromY + cell);

    if (selection.state == SELECTING_TO)
    {
        printf("here2\n");

        int toX = BOARD_X + selection.to.column * cell;
        int toY = BOARD_Y + selection.to.row * cell;

        Color8(255, 0, 0);
        CV::rect(toX, toY, toX + cell, toY + cell);
    }
}

Position *GetSelectingPosition()
{
    return selection.state == SELECTING_FROM ? &selection.from : &selection.to;
}

void MoveSelectionLeft()
{
    Position *position = GetSelectingPosition();
    position->column = (position->column + board.size - 1) % board.size;
}

void MoveSelectionUp()
{
    Position *position = GetSelectingPosition();
    position->row = (position->row + board.size - 1) % board.size;
}

void MoveSelectionRight()
{
    Position *position = GetSelectingPosition();
    position->column = (position->column + 1) % board.size;
}

void MoveSelectionDown()
{
    Position *position = GetSelectingPosition();
    position->row = (position->row + 1) % board.size;
}

void SelectOrigin()
{
    int piece = board.pieces[GetBoardIndex(selection.from.row, selection.from.column)];

    if (piece == board.turn)
    {
        selection.to.row = selection.from.row;
        selection.to.column = selection.from.column;

        selection.state = SELECTING_TO;
    }
}

void FinishSelection()
{
    int fromIndex = GetBoardIndex(selection.from.row, selection.from.column);
    int toIndex = GetBoardIndex(selection.to.row, selection.to.column);

    int piece = board.pieces[fromIndex];

    board.pieces[fromIndex] = EMPTY;
    board.pieces[toIndex] = piece;

    board.turn = (board.turn + 1) % 2;

    RestartSelection();
}

void SubmitSelection()
{
    if (selection.state == SELECTING_FROM)
    {
        SelectOrigin();
    }
    else
    {
        FinishSelection();
    }
}

void render()
{
    RenderBoard();
    RenderSelection();
}

void keyboard(int key)
{
    Position *position = GetSelectingPosition();

    switch (key)
    {
    // ESCAPE
    case 27:
        RestartSelection();
        break;
    // SPACE
    case 32:
        SubmitSelection();
        break;
    // LEFT
    case 200:
        MoveSelectionLeft();
        break;
    // UP
    case 201:
        MoveSelectionUp();
        break;
    // RIGHT
    case 202:
        MoveSelectionRight();
        break;
    // DOWN
    case 203:
        MoveSelectionDown();
        break;
    }
}


void keyboardUp(int key)
{
   printf("\nLiberou tecla: %d" , key);
}



void mouse(int button, int state, int wheel, int direction, int x, int y)
{
    Position *position = GetSelectingPosition();

    if (IsInsideBoard(x, y))
    {
        position->column = x / GetCellDimensions();
        position->row = y / GetCellDimensions();

        if (button == 0 && state == 0)
        {
            SubmitSelection();
        }
    }

    printf("\nmouse %d %d %d %d %d %d", button, state, wheel, direction,  x, y);
}

int main(void)
{
    InitBoard(8);
    InitSelection();

    ResetBoard();

    CV::init(&screenWidth, &screenHeight, "Damas");
    CV::run();
}
