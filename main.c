/*
*Arquivo principal do programa contendo o menu de interação com o usuário
*/
#include <stdio.h>
#include <stdlib.h>
#include "380229_587320_ED2_T01.h"


int main() {
    char equipeVencedora[40], mvp[40];
    RegistroPrim chaveBuscada;
    RegistroPrim *indicePrim = NULL;
    RegistroWin *indiceWin = NULL;
    RegistroMvp *indiceMvp = NULL;
    char opcaoChar;
    int opt;
    long idx;

    idx = inicializarIdx(&indicePrim, &indiceWin, &indiceMvp);
    //Para garatir que não haveria lixo no buffer, as opções são recebidas como uma string e então depois são transformadas em números.
    scanf("%[^\n]%*c", &opcaoChar);
    opt = strtol(&opcaoChar, NULL, 10);
    while(opt != 7){
        switch(opt){
            case 1:
                idx = inserirPartida(&indicePrim, &indiceWin, &indiceMvp, idx);
                break;
            case 2:
                scanf("%[^\n]%*c", chaveBuscada.codigo);
                alterarDuracao(&indicePrim, &chaveBuscada, idx);
                chaveBuscada.codigo[0] = '\0';
                break;
            case 3:
                scanf("%[^\n]%*c", chaveBuscada.codigo);
                removerPartida(&indicePrim, &chaveBuscada, idx);
                chaveBuscada.codigo[0] = '\0';
                break;
            case 4:
                scanf("%[^\n]%*c", &opcaoChar);
                opt = strtol(&opcaoChar, NULL, 10);
                switch(opt){
                    case 1:
                        scanf("%[^\n]%*c", chaveBuscada.codigo);
                        buscarPrim(&indicePrim, &chaveBuscada, idx);
                        chaveBuscada.codigo[0] = '\0';
                        break;
                    case 2:
                        scanf("%[^\n]%*c", equipeVencedora);
                        buscarWin(&indicePrim, &indiceWin, equipeVencedora, idx);
                        break;
                    case 3:
                        scanf("%[^\n]%*c", mvp);
                        buscarMvp(&indicePrim, &indiceMvp, mvp, idx);
                        break;
                }
                break;
            case 5:
                scanf("%[^\n]%*c", &opcaoChar);
                opt = strtol(&opcaoChar, NULL, 10);
                switch(opt){
                    case 1:
                        listarPrim(&indicePrim, idx);
                        break;
                    case 2:
                        listarWin(&indicePrim, &indiceWin, idx);
                        break;
                    case 3:
                        listarMvp(&indicePrim, &indiceMvp, idx);
                        break;
                }
                break;
            case 6:
                idx = liberar(&indicePrim, &indiceWin, &indiceMvp, idx);
                break;
        }
        scanf("%[^\n]%*c", &opcaoChar);
        opt = strtol(&opcaoChar, NULL, 10);
    }

    gravarIdx(&indicePrim, &indiceWin, &indiceMvp, idx);

    free(indicePrim);
    free(indiceWin);
    free(indiceMvp);
    return (0);
}
