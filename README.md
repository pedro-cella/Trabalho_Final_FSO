# Trabalho_Final_FSO
O objetivo do trabalho é escrever um programa que dado um diretório origem faz a compactação recursiva de todos os arquivos pertecentes ao diretório origem e seus sub-diretórios. O algoritmo de compactação deve ser o utilizado pelo bzip2, você pode utilizar as funções da biblioteca libbz2. O que você não pode fazer é chamar o comando bzip2 diretamente.

Supondo que executamos o comando: bz2tar DIR DESTINO

O programa deve ter resultado equivalente a seguinte sequência de comandos:
```

cp -ax DIR_ORIGEM DESTINO.bz2
cd DESTINO.bz2
find . -type f -exec bzip2 "{}" \;
cd ..
tar cf DESTINO.bz2.tar DESTINO.bz2
rm -rf DESTINO.bz2
```
Verificando a corretude
```

cd DIR_ORIGEM
find . -type f -exec md5sum "{}" \; > /tmp/checksum
cd ..
tar xf DESTINO.bz2.tar
cd DESTINO.bz2
find . -type f -exec bunzip2 "{}" \;
md5sum -c /tmp/checksum
```
OBSERVAÇÃO: Isto não é a mesma coisa de um tar.bz2

Onde DIR_ORIGEM é o diretório de origem dos dados, podendo conter um número indefinido de sub-diretórios e DESTINO.bz2.tar é o arquivo que vai conter o backup com arquivos compactado de DIR_ORIGEM.
