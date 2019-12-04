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

int bz2Recursivo(char *);
int removerRecursivo(char *);

int main(int argc, char *argv[]){

    char *dir = argv[1];
    char *DESTINO = argv[2];

    char *dirAtual = (char *) malloc(4096*sizeof(char));
    dirAtual = dir;

    char buffer[PATH_MAX];

    //Entrar no diretorio
    if(chdir(dir))  perror("nao encontrou o diretorio.\n");

    //Copiar os arquivos recursivamente

    
    //Chamar o shell para fazer bzip2 (apenas em arquivos) em 4 threads (recursivamente)
    // threads
    bz2Recursivo(dir);

    //Chamar o shell para fazer o tar
    //if(chdir("..")) perror("nao encontrou o diretorio.\n");
    sprintf(buffer, "tar cf %s.tar %s", dir, dir);
    if(popen(buffer, "r") == NULL){
        perror("nao foi possivel fazer o tar do diretorio.");
    }

    //Excluir a copia A TERMINAR
    char path[PATH_MAX];
    strcpy(path, dir);
//    strcat(path, "/");
//    strcat(path, DESTINO);
//    strcat(path, ".tar");
    sleep(1);
    if(removerRecursivo(path)) perror("nao foi possivel remover o arquivo.");
    chdir("..");
    rmdir(dir);
    //A TERMINAR

    return 0;
}


int bz2Recursivo(char *dirAtual){


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
            bz2Recursivo(dirAux);
        }else if(strcmp(nameList[i]->d_name, ".") && strcmp(nameList[i]->d_name, "..")) {
            char buffer[PATH_MAX];
            chdir(dirAtual);
            sprintf(buffer, "bzip2 %s", nameList[i]->d_name);
            if (popen(buffer, "r") == NULL) perror("nao foi possivel fazer o bzip2 do arquivo.");
        }
    }
    return 0;
}

int removerRecursivo(char *dirAtual){

    Diretorio *nameList; //se o d_type == 4, entao ele e um diretorio.
    int numeroDeArquivos;

    numeroDeArquivos = scandir(dirAtual, &nameList, 0, alphasort);
    if(numeroDeArquivos < 0) perror("erro ao visualizar os arquivos.");
    for (int i = 0; i < numeroDeArquivos; ++i) {

        char dirAux[PATH_MAX];
        strcpy(dirAux, dirAtual);
        strcat(dirAux, "/");
        strcat(dirAux, nameList[i]->d_name);

        printf("%d %s\n", nameList[i]->d_type, dirAux);
        if(nameList[i]->d_type == 4 && strcmp(nameList[i]->d_name, ".") && strcmp(nameList[i]->d_name, "..")) { //se for diretorio

            removerRecursivo(dirAux);
            if(remove(dirAux)) perror("nao foi possivel remover o diretorio.");
        } else if(strcmp(nameList[i]->d_name, ".") && strcmp(nameList[i]->d_name, "..")) { //se for arquivo
            if(remove(dirAux)) perror("nao foi possivel remover o diretorio.");
        }
    }
    return 0;
}

