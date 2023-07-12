//==================================================//
//    Multi-Platform Console (MPC) - v4.6.2 @ 2010  //
//     UFSM - Federal University of Santa Maria     //
//     www.inf.ufsm.br/~pozzer/mpc                  //
//==================================================//

//ver 71

#ifndef __MPC_C_INTERFACE__
#define __MPC_C_INTERFACE__

#include <GL/glut.h>
#include "definitions.h"

typedef unsigned char uchar;

//inicializa o MPC
void mpcRun(int fps); // framesPerSecond
//define o tamanho da janala em caracteres
void mpcSetSize(int lin, int col); // window size
//exibe informacoes sobre o MPC
void mpcAbout(void);
void mpcVersion(void);

//funcao de desenho de caracteres
//-------------------------------------------------------------------------------
void mpcSetChar(int lin, int col, int character, int font,
   int rT, int gT, int bT, // textColor
   int rB, int gB, int bB, // backgroundColor
   double alpha);


//funcoes de cursor
//-------------------------------------------------------------------------------
void mpcSetCursorColor(int r, int g, int b); //define a cor do cursor
void mpcSetCursorPos  (int lin, int col);    //define a posicao do cursor
void mpcSetCursorVisible(bool boolean);      //indica se o cursor deve estar visivel.


//funcoes de linha (operam em pixel)
//-------------------------------------------------------------------------------
void mpcVLine(int x, int yini, int yfim,
   int r, int g, int b, double alpha); // desenha uma linha vertical
void mpcHLine(int y, int xini, int xfim,
   int r, int g, int b, double alpha); // desenha uma linha horizontal


//funcoes de callback
//-------------------------------------------------------------------------------
void mpcSetKeyboardFunc(void(*func)(int, int, bool, bool));  //callback de teclado
void mpcSetMouseFunc(void(*func)(int, int, int, int));       //callback de mouse
void mpcSetUpdateFunc(void(*func)(void));                    //callback de proposito geral



//funcoes de imagem
//-------------------------------------------------------------------------------
//define a area de recorde de imagens
void mpcSetClippingArea(int initLine, int initColumn, int numLines, int numColumns);

//retorna a altura em pixels de uma imagem
int mpcGetImgHeight(int id);
//retorna a largura em pixels de uma imagem
int mpcGetImgWidth(int id);
//retorna o numero de bytes ocupado pela imagem
int mpcGetImgSize(int id);

//carrega uma imagem bmp e associa uma cor de transparencia. Retorna o ID da imagem criada
int mpcLoadBmp(const char* path, int r, int g, int b);
//carrega uma imagem bmp. Retorna o ID da imagem criada
int mpcLoadBmp(const char* path);

//constroi um grafico em formato de barras. Retorna o ID da imagem criada
int  mpcBuildGraph(int *vet, int num, const char *titulo, const char* eixoY,
      const char*legenda[], int size);
//habilita a exibicao do ID de cada imagem
void mpcSetImgIdVisible(bool boolean);
//exibe a imagem em uma posicao na tela (dada em caracteres)
void mpcShowImg(int lin, int col, int id, double alpha);

//remove a imagem da memoria do MPC
void mpcDestroyImg(int id);
//retorna o buffer de pixels da imagem em formato RGB
unsigned char* mpcGetImg(int id);
//cria uma imagem dentro do mpc. Deve-se passar o vetor de pixels da nova imagem, e suas dimensoes
int mpcCreateImg(unsigned char *p, int w, int h);
//atualiza os pixles da imagem. O tamanho do vetor de pixels deve ser igual a imagem original
void mpcSetImg(unsigned char *p, int id);


#endif // __MPC_C_INTERFACE__
