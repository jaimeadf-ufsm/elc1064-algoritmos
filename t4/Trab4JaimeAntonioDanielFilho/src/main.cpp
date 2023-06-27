/*********************************************************************
// Jogo de Damas
// Autor: Jaime Antonio Daniel Filho
//
// -> Troca de turno
// -> Movimento com o teclado
// -> Movimento com o mouse
// -> Usando as flechinas, o cursor move para o outro lado do tabuleiro
//    nas bordas
// -> Apenas movimentos validos sao aceitos
// -> Detecção de captura como no jogo original
// -> Registro do placar de capturas de cada jogador
// -> Tamanho do tabuleiro alocado dinamicamente e selecionado por
      botões
// -> Responsivo de acordo com o tamanho de tela
// -> Apertar X para alternar entre a tela de instruções
// *********************************************************************/

#include <GL/glut.h>
#include <GL/freeglut_ext.h> //callback da wheel do mouse.

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "gl_canvas2d.h"

#include "auxiliar.h"

#define EMPTY -1
#define WHITE 0
#define BLACK 1

#define SELECTING_FROM 0
#define SELECTING_TO 1
#define SELECTION_READY 2

#define INITIAL_DISTANCE 3
#define INITIAL_BOARD_SIZE 8

#define PIECE_SCALE 0.3

#define SCORE_MARGIN 50

#define TEXT_HEIGHT 8

#define SELECTION_THICKNESS 4

#define TURN_WIDTH 100
#define TURN_HEIGHT 100

#define BUTTON_WIDTH 100
#define BUTTON_HEIGHT 40

#define BUTTON_MARGIN 4

#define SIDEBAR_WIDTH 200

#define HELP_MARGIN 100
#define HELP_LINE 24

#define CIRCLE_DIVIDERS 128

#define BUTTONS_NUM 3

// Guard uma posição no tabuleiro
typedef struct
{
    int row;
    int column;
} Position;

// Guarda o tamanho, jogador e a matriz do tabuleiro
typedef struct
{
    int size;
    int turn;

    int white_captures;
    int black_captures;

    int *cells;
} Board;

// Guarda informações relacionadas a um lance
typedef struct
{
    Position from;
    Position to;
} Move;

// Guarda informações relacionadas a seleção atual para o movimento
typedef struct
{
    int state;

    Move move;
    Position *current;
} Selection;

// Guarda a posição de um botão e o seu valo
typedef struct
{
    int x, y;
    int value;
} Button;

Board board;
Selection selection;

bool help = false;

Button buttons[BUTTONS_NUM];

int screenWidth = 1000, screenHeight = 800; // largura e altura inicial da tela. Alteram com o redimensionamento de tela.

// Define a cor a partir de valores entres 0 e 255
void Color8(unsigned char r, unsigned char g, unsigned char b)
{
    return CV::color(r / 255.0f, g / 255.0f, b / 255.0f);
}

// Desenha um texto centralizado na coordenada
void TextCenter(int x, int y, char *text)
{
    int length = strlen(text) * 10;

    CV::text(x - length / 2, y + TEXT_HEIGHT / 2, text);
}

// Desenha um inteiro centralizado na coordenada
void NumberCenter(int x, int y, int number)
{
    char text[20];
    sprintf(text, "%d", number);

    TextCenter(x, y, text);
}

// Retorna a direcao para frente do jogador
int GetForwardDirection(int color)
{
    if (color == WHITE)
    {
        return -1;
    }
    else
    {
        return 1;
    }
}

// Retorna a cor do adversário
int GetOpponentColor(int color)
{
    if (color == WHITE)
    {
        return BLACK;
    }
    else
    {
        return WHITE;
    }
}

// Inicializa o tabuleiro
void InitBoard()
{
    board.size = INITIAL_BOARD_SIZE;
    board.cells = (int *)malloc(board.size * board.size * sizeof(int));
}

// Verifica se é um quadrado escuro
bool IsDarkSquare(Position *position)
{
    return (position->row + position->column) % 2 == 1;
}

// Retorna o índice no vetor de células correspondente à posição
int GetBoardIndex(Position *position)
{
    return position->row * board.size + position->column;
}

// Retorna o valor armazenado corresponde à posição
int GetBoardCell(Position *position)
{
    return board.cells[GetBoardIndex(position)];
}

// Atribui o valor armazenado à posição correspondente
void SetBoardCell(Position *position, int value)
{
    board.cells[GetBoardIndex(position)] = value;
}

// Altera o tamanho do tabuleiro
void ResizeBoard(int size)
{
    free(board.cells);
    board.size = size;
    board.cells = (int *)malloc(size * size * sizeof(int));
}

// Volta o jogo para o estado inicial
void ResetBoard()
{
    board.turn = WHITE;

    board.white_captures = 0;
    board.black_captures = 0;

    for (int i = 0; i < board.size; i++)
    {
        for (int j = 0; j < board.size; j++)
        {
            Position position = {i, j};

            SetBoardCell(&position, EMPTY);
        }
    }

    for (int d = 0; d < INITIAL_DISTANCE; d++)
    {
        for (int j = 0; j < board.size; j++)
        {
            Position top = {d, j};
            Position bottom = {board.size - d - 1, j};

            if (IsDarkSquare(&top))
            {
                SetBoardCell(&top, BLACK);
            }

            if (IsDarkSquare(&bottom))
            {
                SetBoardCell(&bottom, WHITE);
            }
        }
    }
}

// Incrementa o número de capturas para determinado jogador
void IncrementCaptures(int color)
{
    if (board.turn == WHITE)
    {
        board.white_captures++;
    }
    else
    {
        board.black_captures++;
    }
}

// Alterna o turno de jogadores
void ToggleTurn()
{
    board.turn = GetOpponentColor(board.turn);
}

// Verifica se a direção entre a posição de partida e a posição de destino é frente para o jogador
bool IsForwardDirection(int color, Position *from, Position *to)
{
    return (to->row - from->row) * GetForwardDirection(color) > 0;
}

// Verifica se a posição de partida e a posição de destino formam uma diagonal
bool IsDiagonal(Position *from, Position *to)
{
    int deltaRow = to->row - from->row;
    int deltaColumn = to->column - from->column;

    return abs(deltaRow) == abs(deltaColumn);
}

// Verifica se é um movimento de captura
bool IsCaptureMove(Move *move)
{
    return abs(move->to.column - move->from.column) == 2;
}

// Verifica se é um movimento de deslocamento para frente
bool IsForwardMove(Move *move)
{
    return abs(move->to.column - move->from.column) == 1;
}

// Retorna a posição intermediária entre duas posições
Position GetMiddlePosition(Position *from, Position *to)
{
    return {(from->row + to->row) / 2, (from->column + to->column) / 2};
}

// Verifica se o movimento é válido
bool IsMoveValid(Move *move)
{
    if (GetBoardCell(&move->from) != board.turn)
    {
        return false;
    }

    if (GetBoardCell(&move->to) != EMPTY)
    {
        return false;
    }

    if (!IsDiagonal(&move->from, &move->to))
    {
        return false;
    }

    if (!IsForwardDirection(board.turn, &move->from, &move->to))
    {
        return false;
    }

    if (IsCaptureMove(move))
    {
        Position middle = GetMiddlePosition(&move->from, &move->to);

        return GetBoardCell(&middle) == GetOpponentColor(board.turn);
    }

    return IsForwardMove(move);
}

// Realiza um lance
void MakeMove(Move *move)
{
    if (!IsMoveValid(move))
    {
        return;
    }

    SetBoardCell(&move->from, EMPTY);
    SetBoardCell(&move->to, board.turn);

    if (IsCaptureMove(move))
    {
        Position middle = GetMiddlePosition(&move->from, &move->to);

        SetBoardCell(&middle, EMPTY);
        IncrementCaptures(board.turn);
    }

    ToggleTurn();
}

// Retorna a dimensão do tabuleiro em pixels
int GetBoardLength()
{
    int maxWidth = screenWidth - SIDEBAR_WIDTH;
    int maxHeight = screenHeight;

    return maxWidth < maxHeight ? maxWidth : maxHeight;
}

// Retorna a dimensão de uma célula do tabuleiro em pixels
int GetBoardCellLength()
{
    return GetBoardLength() / board.size;
}

// Verifica se a posição se encontra dentro do tabuleiro
bool IsInsideBoard(int px, int py)
{
    int length = GetBoardLength();

    return px >= 0 && px < length && py >= 0 && py < length;
}

// Renderiza uma peça
void RenderPiece(int color, int centerX, int centerY, int radius)
{
    if (color == WHITE)
    {
        Color8(255, 255, 255);
    }
    else
    {
        Color8(0, 0, 0);
    }

    CV::circleFill(centerX, centerY, radius, CIRCLE_DIVIDERS);
}

// Renderiza o tabuleiro
void RenderBoard()
{
    int cellLength = GetBoardCellLength();

    for (int i = 0; i < board.size; i++)
    {
        for (int j = 0; j < board.size; j++)
        {
            Position position = {i, j};

            int cell = GetBoardCell(&position);

            int x = cellLength * j;
            int y = cellLength * i;

            int centerX = x + cellLength / 2;
            int centerY = y + cellLength / 2;

            if (IsDarkSquare(&position))
            {
                Color8(181, 135, 99);
            }
            else
            {
                Color8(240, 218, 181);
            }

            CV::rectFill(x, y, x + cellLength, y + cellLength);

            if (cell != EMPTY)
            {
                RenderPiece(cell, centerX, centerY, cellLength * PIECE_SCALE);
            }
        }
    }
}

// Volta a seleção para o estado inicial
void ResetSelection()
{
    selection.state = SELECTING_FROM;

    selection.move.from = {0, 0};
    selection.move.to = {0, 0};

    selection.current = &selection.move.from;
}

// Muda a posição sendo selecionada
void ChangeSelectionState(int state)
{
    Position last = *selection.current;

    selection.state = state;

    switch (selection.state)
    {
    case SELECTING_FROM:
        selection.current = &selection.move.from;
        break;
    case SELECTING_TO:
        selection.current = &selection.move.to;
        break;
    }

    *selection.current = last;
}

// Define as coordenadas da posição sendo selecionada
void SetCurrentSelection(Position *position)
{
    *selection.current = *position;
}

// Reinicia a seleção
void RestartSelection()
{
    ChangeSelectionState(SELECTING_FROM);
}

// Move a seleção uma posição para esquerda
void MoveSelectionLeft()
{
    selection.current->column = (board.size + selection.current->column - 1) % board.size;
}

// Move a seleção uma posição para cima
void MoveSelectionUp()
{
    selection.current->row = (board.size + selection.current->row - 1) % board.size;
}

// Move a seleção uma posição para direita
void MoveSelectionRight()
{
    selection.current->column = (selection.current->column + 1) % board.size;
}

// Move a seleção uma posição para baixo
void MoveSelectionDown()
{
    selection.current->row = (selection.current->row + 1) % board.size;
}

// Confirma a seleção da posição
void SubmitSelection()
{
    switch (selection.state)
    {
    case SELECTING_FROM:
        if (GetBoardCell(&selection.move.from) == board.turn)
        {
            ChangeSelectionState(SELECTING_TO);
        }
        break;
    case SELECTING_TO:
        MakeMove(&selection.move);
        RestartSelection();
        break;
    }
}

// Renderiza o quadrado ao redor da posição selecionada
void RenderSelectionFrame(Position *position)
{
    int cellLength = GetBoardCellLength();

    int startX = position->column * cellLength;
    int startY = position->row * cellLength;

    int endX = startX + cellLength;
    int endY = startY + cellLength;

    CV::rectFill(startX, startY, endX, startY + SELECTION_THICKNESS);
    CV::rectFill(startX, endY, endX, endY - SELECTION_THICKNESS);
    CV::rectFill(startX, startY, startX + SELECTION_THICKNESS, endY);
    CV::rectFill(endX, startY, endX - SELECTION_THICKNESS, endY);
}

// Renderiza as posições selecionadas
void RenderSelection()
{
    Color8(0, 0, 255);
    RenderSelectionFrame(&selection.move.from);

    if (selection.state >= SELECTING_TO)
    {
        Color8(255, 0, 0);
        RenderSelectionFrame(&selection.move.to);
    }
}

// Inicializa o botão
void InitButton(Button *button, int value)
{
    button->x = 0;
    button->y = 0;
    button->value = value;
}

// Verifica se a posição se encontra dentro do botão
bool IsInsideButton(Button *button, int px, int py)
{
    return px >= button->x && px < button->x + BUTTON_WIDTH && py >= button->y && py < button->y + BUTTON_HEIGHT;
}

// Renderiza o botão
void RenderButton(Button *button)
{
    int centerX = button->x + BUTTON_WIDTH / 2;
    int centerY = button->y + BUTTON_HEIGHT / 2;

    char text[20];
    sprintf(text, "%d x %d", button->value, button->value);

    if (button->value == board.size)
    {
        Color8(203, 227, 245);
    }
    else
    {
        Color8(240, 240, 240);
    }

    CV::rectFill(button->x, button->y, button->x + BUTTON_WIDTH, button->y + BUTTON_HEIGHT);

    Color8(0, 0, 0);
    TextCenter(centerX, centerY, text);
}

// Renderiza a barra lateral
void RenderSidebar()
{
    int sidebarHeight = GetBoardLength();
    int buttonHeight = BUTTON_HEIGHT + BUTTON_MARGIN;

    int startX = screenWidth - SIDEBAR_WIDTH;
    int startY = 0;

    int endY = startY + sidebarHeight;

    int centerX = startX + SIDEBAR_WIDTH / 2;
    int centerY = startY + sidebarHeight / 2;

    int helpX = centerX;
    int helpY = centerY - sidebarHeight / 4;

    int buttonsX = centerX - BUTTON_WIDTH / 2;
    int buttonsY = centerY + sidebarHeight / 4 - BUTTONS_NUM * buttonHeight / 2;

    Color8(0, 0, 0);
    TextCenter(helpX, helpY, "tecle X para ajuda");

    NumberCenter(centerX, startY + SCORE_MARGIN, board.black_captures);
    NumberCenter(centerX, endY - SCORE_MARGIN, board.white_captures);

    Color8(197, 230, 200);
    CV::rectFill(centerX - TURN_WIDTH / 2, centerY - TURN_HEIGHT / 2, centerX + TURN_WIDTH / 2, centerY + TURN_HEIGHT / 2);

    RenderPiece(board.turn, centerX, centerY, TURN_WIDTH * PIECE_SCALE);

    for (int i = 0; i < BUTTONS_NUM; i++)
    {
        buttons[i].x = buttonsX;
        buttons[i].y = buttonsY + i * buttonHeight;

        RenderButton(&buttons[i]);
    }
}

// Alterna a tela de ajuda
void ToggleHelp()
{
    help = !help;
}

// Renderiza a tela de ajuda
void RenderHelp()
{
    char lines[][200] = {
        "jogo de damas",
        "-> uma peca apenas anda para frente, uma casa de cada vez, sobre as suas diagonais",
        "-> uma peca pode capturar uma adversaria em suas diagonais adjacentes quando movida",
        "   para uma casa livre diagonalmente logo apos a adversaria (como no jogo original)",
        "-> os jogadores se alternam a cada lance",
        "",
        "-> o jogador atual pode ser visualizado na direita",
        "-> a contagem de pecas capturadas encontra-se a direita ao lado do respectivo jogador",
        "",
        "-> para executar um movimento:",
        "   - mova o cursor utilizando as setinhas do teclado ou o mouse",
        "   - selecione a peca de origem apertando espaco ou o botao esquerdo",
        "   - selecione a posicao de destino apertando espaco ou o botao esquerdo",
        "   - aperte esc para cancelar a selecao",
        "   - OBS: durante uma captura deve ser selecionada a posicao final e nao a peca",
        "     adversaria",
        "",
        "tecle X para voltar"
    };

    Color8(0, 0, 0);

    for (int i = 0; i < sizeof(lines) / sizeof(lines[0]); i++)
    {
        CV::text(HELP_MARGIN, HELP_MARGIN + HELP_LINE * i, lines[i]);
    }

}

// funcao chamada continuamente. Deve-se controlar o que desenhar por meio de variaveis
// globais que podem ser setadas pelo metodo keyboard()
void render()
{
    if (help)
    {
        RenderHelp();
    }
    else
    {
        RenderBoard();
        RenderSidebar();
        RenderSelection();
    }
}

// funcao chamada toda vez que uma tecla for pressionada
void keyboard(int key)
{
    printf("\nTecla: %d", key);

    switch (key)
    {
    case 27: // esc
        RestartSelection();
        break;
    case 32: // espaco
        SubmitSelection();
        break;
    case 120: // x
        ToggleHelp();
        break;
    case 200: // seta para esquerda
        MoveSelectionLeft();
        break;
    case 201: // seta para cima
        MoveSelectionUp();
        break;
    case 202: // seta para direita
        MoveSelectionRight();
        break;
    case 203: // seta para a baixo
        MoveSelectionDown();
        break;
    }
}
// funcao chamada toda vez que uma tecla for liberada
void keyboardUp(int key)
{
}

// funcao para tratamento de mouse: cliques, movimentos e arrastos
void mouse(int button, int state, int wheel, int direction, int x, int y)
{
    int cellLength = GetBoardCellLength();

    if (IsInsideBoard(x, y))
    {
        Position position = {y / cellLength, x / cellLength};

        SetCurrentSelection(&position);

        if (button == 0 && state == 0)
        {
            SubmitSelection();
        }
    }

    if (button == 0 && state == 0)
    {
        for (int i = 0; i < BUTTONS_NUM; i++)
        {
            if (IsInsideButton(&buttons[i], x, y))
            {
                ResizeBoard(buttons[i].value);
                ResetBoard();
                ResetSelection();
            }
        }
    }
}

int main(void)
{
    InitBoard();

    InitButton(&buttons[0], 8);
    InitButton(&buttons[1], 10);
    InitButton(&buttons[2], 12);

    ResetSelection();
    ResetBoard();

    CV::init(&screenWidth, &screenHeight, "Damas");
    CV::run();
}
