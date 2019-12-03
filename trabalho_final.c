#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>

#define PATH_MAX 4096
// pool of consumers


struct pool{
    int infd;
    int outfd;
    char filename[PATH_MAX];
};

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

int main(int argc, char *argv[]){

    char *dir = argv[1];
    char *DESTINO = argv[2];

    //Entrar no diretorio
    if(chdir(dir))  perror("nao encontrou o diretorio.\n");

    //percorrer o diretorio recursivamente

    //Copiar os arquivos recursivamente

    //Chamar o shell para fazer bzip2 (apenas em arquivos) em 4 threads (recursivamente)

    //Chamar o shell para fazer o tar
//    if(chdir("..")) perror("nao encontrou o diretorio.\n");
    char buffer[PATH_MAX];
    sprintf(buffer, "tar cf %s.tar %s", DESTINO, DESTINO);
    if(system(buffer) == -1){
        perror("chamada de sistema invalida.");
    }

    //Excluir a copia A TERMINAR
    char path[PATH_MAX];
    strcpy(path, dir);
    strcat(path, "/");
    strcat(path, DESTINO);
    strcat(path, ".tar");
    rmdir(path);
    //A TERMINAR


    return 0;
}
