#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>

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
int copiarArquivo(char *, char *);
int copiarRecursivo(char *, char *);
//char *completarCaminho(char *, char *);


int main(int argc, char *argv[]){

    char dir[PATH_MAX];
    strcpy(dir,argv[1]);
    char *DESTINO = strtok(argv[2], ".");
    char buffer[PATH_MAX];

    if (strcmp(strtok(argv[1], "/"), "home")) {
        char dirAtual[PATH_MAX];
        if (getcwd(dirAtual, sizeof(dirAtual)) == NULL) {
            perror("getcwd() error");
            exit(-1);
        }
        strcat(dirAtual, "/");
        strcat(dirAtual, dir);
        strcpy(dir, dirAtual);
    }


    //Copiar recursivamente
    strcat(DESTINO, ".bz2");
    mkdir(DESTINO, 0700);
    if(chdir(DESTINO))  perror("nao encontrou o diretorio.\n");
    char novo[PATH_MAX];
    if (getcwd(novo, sizeof(novo)) == NULL) {
        perror("getcwd() error");
        exit(-1);
    }
    copiarRecursivo(dir, novo);
    if(chdir(novo)) perror("erro no chdir()");

//    //Chamar o shell para fazer bzip2 (apenas em arquivos) em 4 threads (recursivamente)
    // threads
//    bz2Recursivo(novo);
//    if(chdir(novo)) perror("erro no chdir()");

    //Chamar o shell para fazer o tar
    if(chdir("..")) perror("erro no chdir()");
    sprintf(buffer, "tar cf %s.tar %s", DESTINO, DESTINO);
//    if(popen(buffer, "r") == NULL) perror("nao foi possivel fazer o tar do diretorio.");
    system(buffer);
    if(chdir(novo)) perror("erro no chdir()");


    //Excluir a copia A TERMINAR
    sleep(1);
    if(removerRecursivo(novo)) perror("nao foi possivel remover o arquivo.");
    chdir("..");
    rmdir(DESTINO);
    //A TERMINAR


    return 0;
}


int bz2Recursivo(char *dirAtual){


    Diretorio *nameList; //se o d_type == 4, entao ele e um diretorio.
    int numeroDeArquivos;

    numeroDeArquivos = scandir(dirAtual, &nameList, 0, alphasort);
    if(numeroDeArquivos < 0) perror("erro ao visualizar os arquivos.");
    for (int i = 0; i < numeroDeArquivos; ++i) {
//        printf("%d , %s\n", nameList[i]->d_type, nameList[i]->d_name);

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

//        printf("%d %s\n", nameList[i]->d_type, dirAux);
        if(nameList[i]->d_type == 4 && strcmp(nameList[i]->d_name, ".") && strcmp(nameList[i]->d_name, "..")) { //se for diretorio
            removerRecursivo(dirAux);
            if(remove(dirAux)) perror("nao foi possivel remover o diretorio.");
        } else if(strcmp(nameList[i]->d_name, ".") && strcmp(nameList[i]->d_name, "..")) { //se for arquivo
            if(remove(dirAux)) perror("nao foi possivel remover o arquivo.");
        }
    }
    return 0;
}

int copiarArquivo(char *arquivo, char *destino){
    FILE *fptr1, *fptr2;
    char novo[PATH_MAX];

    // Open one file for reading
    fptr1 = fopen(arquivo, "r");
    if (fptr1 == NULL)
    {
        printf("Cannot open file %s \n", arquivo);
        return -1;
    }

    fptr2 = fopen(destino, "w");
    if (fptr2 == NULL)
    {
        printf("Cannot open file %s \n", destino);
        return -1;
    }

    // Read contents from file
    signed c;
    c = fgetc(fptr1);
    while (c != EOF)
    {
        fputc(c, fptr2);
        c = fgetc(fptr1);
    }

//    printf("\nContents copied to %s\n", destino);

    fclose(fptr1);
    fclose(fptr2);

    return 0;
}

int copiarRecursivo(char *dirAtual, char *novo){

    Diretorio *nameList; //se o d_type == 4, entao ele e um diretorio.
    int numeroDeArquivos;

    numeroDeArquivos = scandir(dirAtual, &nameList, 0, alphasort);
    if(numeroDeArquivos < 0) perror("erro ao visualizar os arquivos.");
    for (int i = 0; i < numeroDeArquivos; ++i) {

        char dirAux[PATH_MAX];
        strcpy(dirAux, dirAtual);
        strcat(dirAux, "/");
        strcat(dirAux, nameList[i]->d_name);
        char dirNovo[PATH_MAX];
        strcpy(dirNovo, novo);
        strcat(dirNovo, "/");
        strcat(dirNovo, nameList[i]->d_name);

//        printf("%d %s\n", nameList[i]->d_type, dirAux);
        if(nameList[i]->d_type == 4 && strcmp(nameList[i]->d_name, ".") && strcmp(nameList[i]->d_name, "..")) { //se for diretorio
            if(mkdir(nameList[i]->d_name, 0700)) perror("erro no mkdir()");
            if(chdir(dirNovo)) perror("erro no chdir()");
            copiarRecursivo(dirAux, dirNovo);
            if(chdir("..")) perror("erro no chdir()");
        } else if(strcmp(nameList[i]->d_name, ".") && strcmp(nameList[i]->d_name, "..")) { //se for arquivo
            copiarArquivo(dirAux, dirNovo);
            char buffer[PATH_MAX];
            sprintf(buffer, "bzip2 %s", nameList[i]->d_name);
//            if (popen(buffer, "r") == NULL) perror("nao foi possivel fazer o bzip2 do arquivo.");
            system(buffer);
        }
    }

    return 0;
}


