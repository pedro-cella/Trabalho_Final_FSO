#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>

#define PATH_MAX 4096

///RESUMO DO CODIGO:
///cp -ax DIR_ORIGEM DESTINO.bz2
//cp -ax => -a README.md= copiar tudo do diretorio origem(recursivamente), preservando todos os atributos de arquivo; -x = no mesmo diretorio.
//DIR_ORIGEM = o diretorio onde se encontra o arquivo que queremos copiar
//DESTINO.bz2 = o nome da copia
///cd DESTINO.bz2
//DESTINO.bz2 = !!Um arquivo temporario!!
// FILE * tmpfile(void); => cria um file binario temporario
///find . -type f -exec bzip2 "{}" \;
//find . -type f => encontrar tudo que for do tipo file no diretorio atual
//find . -exec bzip2 "{}" \; => executar o comando bzip2 no diretorio atual
///cd ..
//volta para o DIR_ORIGEM
///tar cf DESTINO.bz2.tar DESTINO.bz2
//fazer um tar do arquivo temporario
///rm -rf DESTINO.bz2
//rm -rf => forca a remocao, recursivamente de tudo no arquivo temporario.

typedef struct dirent* Diretorio;

// pool of consumers
struct Pool_of_consumers{
    int infd;
    int outfd;
    char filename[PATH_MAX];
}; typedef struct Pool_of_consumers Pool;

struct ArvoreBinaria
{
    int valor;
    struct ArvoreBinaria *esquerda;
    struct ArvoreBinaria *direita;
};
typedef struct ArvoreBinaria Arvore;

int percorrerDiretorio(char *);

void loadTreeFromFile(Arvore **, char*);
int isFull(Arvore **, int, int, int);
int searchValue(Arvore **, int);
int getHeight(Arvore **, int, int);
//int removeValue(Arvore **, int);
void printInOrder(Arvore **);
void printPreOrder(Arvore **);
void printPostOrder(Arvore **);
void inserirValor(Arvore **, int);
Arvore *criarArvoreVazia();
void freeArvore(Arvore **);



int main(int argc, char *argv[]){

    char *dir = argv[1];
    char *DESTINO = argv[2];

    char *dirAtual = (char *) malloc(4096*sizeof(char));
    dirAtual = dir;

    char buffer[PATH_MAX];


    //Entrar no diretorio
    if(chdir(dir))  perror("nao encontrou o diretorio.\n");

    //percorrer o diretorio recursivamente

    percorrerDiretorio(dir);
    //Copiar os arquivos recursivamente

    //Chamar o shell para fazer bzip2 (apenas em arquivos) em 4 threads (recursivamente)

    sprintf(buffer, "bzip2 %s", DESTINO);
    if(system(buffer)) perror("nao foi possivel fazer o bzip2 do arquivo.");


    //Chamar o shell para fazer o tar
//    if(chdir("..")) perror("nao encontrou o diretorio.\n");
    sprintf(buffer, "tar cf %s.tar %s", DESTINO, DESTINO);
    if(system(buffer)){
        perror("nao foi possivel fazer o tar do diretorio.");
    }

    //Excluir a copia A TERMINAR
    char path[PATH_MAX];
    strcpy(path, dir);
    strcat(path, "/");
    strcat(path, DESTINO);
    strcat(path, ".tar");
    if(unlink(path)) perror("nao foi possivel remover o arquivo.");
    //A TERMINAR


    return 0;
}


int percorrerDiretorio(char *dirAtual){


    Diretorio *nameList; //se o d_type == 4, entao ele e um diretorio.
    int numeroDeArquivos;

    numeroDeArquivos = scandir(dirAtual, &nameList, 0, alphasort);
    if(numeroDeArquivos < 0) perror("erro ao visualizar os arquivos.");
    for (int i = 0; i < numeroDeArquivos; ++i) {
        printf("%d , %s\n", nameList[i]->d_type, nameList[i]->d_name);
        if(nameList[i]->d_type == 4 && strcmp(nameList[i]->d_name, ".") && strcmp(nameList[i]->d_name, "..")) {
            char dirAux[PATH_MAX];
            strcpy(dirAux, dirAtual);
            strcat(dirAux, "/");
            strcat(dirAux, nameList[i]->d_name);
            percorrerDiretorio(dirAux);
        }
        free(nameList[numeroDeArquivos]);
    }
//    free(nameList);
    return 0;
}

void inserirValor(Arvore **atual, int valor){
    if(*atual == NULL){
        *atual = (Arvore *) malloc(sizeof(Arvore));
        (*atual)->valor = valor;
        (*atual)->esquerda = NULL;
        (*atual)->direita = NULL;
    }else {
        if(valor <= (*atual)->valor)
            inserirValor(&(*atual)->esquerda, valor);
        else
            inserirValor(&(*atual)->direita, valor);
    }
}

Arvore *criarArvoreVazia(){
    return NULL;
}

int searchValue(Arvore **atual, int busca){
    if(*atual == NULL){
        printf("Valor nao encontrado.\n");
        return 0;
    }
    if(busca == (*atual)->valor){
        return busca;
    }else {
        if(busca < (*atual)->valor)
            return searchValue(&(*atual)->esquerda, busca);
        else
            return searchValue(&(*atual)->direita, busca);
    }
}

int isFull(Arvore **atual, int profundidade, int altura, int validacao){
    if((*atual)->esquerda != NULL ){
        profundidade++;
        validacao = isFull(&(*atual)->esquerda, profundidade, altura, validacao);
    }
    if((*atual)->direita != NULL){
        profundidade++;
        validacao = isFull(&(*atual)->direita, profundidade, altura, validacao);
    }

    if(profundidade != altura)
        return 1;
    else
        return validacao;
}

void printInOrder(Arvore **atual){
    if((*atual)->esquerda != NULL ){
        printInOrder(&(*atual)->esquerda);
    }
    printf("%d\t", (*atual)->valor);
    if((*atual)->direita != NULL){
        printInOrder(&(*atual)->direita);
    }
}

void printPreOrder(Arvore **atual){
    printf("%d\t", (*atual)->valor);
    if((*atual)->esquerda != NULL ){
        printPreOrder(&(*atual)->esquerda);
    }
    if((*atual)->direita != NULL){
        printPreOrder(&(*atual)->direita);
    }
}

void printPostOrder(Arvore **atual){
    if((*atual)->esquerda != NULL ){
        printPostOrder(&(*atual)->esquerda);
    }
    if((*atual)->direita != NULL){
        printPostOrder(&(*atual)->direita);
    }
    printf("%d\t", (*atual)->valor);
}

/*
int removeValue(Arvore **atual, int valor){
    if(*atual == NULL){
//        printf("Valor nao encontrado.\n");
        return 0;
    }
    if(valor == (*atual)->valor){
        free(*atual);
        return 1;
    }else {
        int status;
        if (valor < (*atual)->valor){
            if (removeValue(&(*atual)->esquerda, valor))
                (*atual)->esquerda = NULL;
        }else {
            if (removeValue(&(*atual)->direita, valor))
                (*atual)->direita = NULL;
        }
        return 0;
    }
}
*/


int getHeight(Arvore **atual, int alturaE, int alturaD){
    if((*atual)->esquerda != NULL ){
        alturaE++;
        alturaE = getHeight(&(*atual)->esquerda, alturaE, alturaE);
    }
    if((*atual)->direita != NULL){
        alturaD++;
        alturaD = getHeight(&(*atual)->direita, alturaD, alturaD);
    }

    if(alturaE >= alturaD)
        return alturaE;
    else
        return alturaD;
}


void freeArvore(Arvore **atual){
    if((*atual)->esquerda != NULL ){
        freeArvore(&(*atual)->esquerda);
    }
    if((*atual)->direita != NULL){
        freeArvore(&(*atual)->direita);
    }
    free(*atual);
}

