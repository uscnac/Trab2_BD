# TRABALHO PRÁTICO 2 - BANCO DE DADOS I (ICC200)
### Contribuidores
-Isabella Almeida Macedo Daniel 22250544 \
-Lucas do Nascimento Silva 22250552


## Objetivo
Este trabalho consiste na implementação de programas para armazenamento e consulta em  dados armazenados em memória secundária utilizando as estruturas de arquivo de dados e índice estudadas nas aulas. Os programas devem fornecer suporte para a inserção, assim como diferentes formas de busca, seguindo as técnicas apresentadas nas aulas de organização e indexação de arquivos.


## Ambiente de desenvolvimento

O trabalho deve ser implementado na linguagem C++ utilizando as bibliotecas padrão de chamadas de sistema disponíveis no Linux.

## Arquivos

upload.cpp
findrec.cpp
seek1.cpp
seek2.cpp

```sh
src
└─── upload.cpp - Carrega registros de um arquivo de entrada, armazena-os em uma tabela hash e cria índices B+.
└─── findrec.cpp - Busca e exibir um registro específico no arquivo de dados binário, com base no ID do registro.
└─── seek1.cpp - Busca registros pelo ID usando o índice primário, exibindo o registro e o número de blocos lidos.
└─── seek2.cpp - Busca registros pelo título usando o índice secundário e exibe o registro encontrado junto com informações de acesso.
└─── Structure 
    └─── hash.hpp -  Define a estrutura de tabela hash e funções para gerar índices e inserir registros.
    └─── bucket.hpp - Implementa a estrutura de bucket, com funções para manipular blocos de dados em buckets e armazená-los em um arquivo binário.
    └─── btree.hpp -  Implementação de uma árvore B+ para indexação, facilitando a busca e organização de registros em disco.
    └─── bloco.hpp - Define estruturas de blocos de dados, incluindo funções para criar e gerenciar blocos que armazenam registros
```


## Arquivo de Entrada

Os dados utiliazado para os programas implementados estão disponíveis “[neste arquivo](https://drive.google.com/file/d/1EVoP0d9Wwzj1O6eoFIkel9I3cpe43Gbv/view?usp=sharing)”  em formato CSV que serve como entrada de dados.


## Scripty do código

Faça a clonagem do repositorio 
  
```sh
git clone https://github.com/bd1-icomp-ufam/trabalho-pr-tico-2-bancos-de-dados-2024-2-tp1-isabella-lucas_nascimento/src
```

Após baixar os arquivos disponibilizados no github, entre no diretorio que foram baixados e execute o script upload.cpp.
```sh
g++ upload.cpp -o upload
```
Compile o execultavel:

```sh
./upload
```

O script solicitará o local do arquivo de dados
ex: /home/NomeUsuario/Downloads/artigo.csv

Espere a criação das tabelas e a inserção dos registros e criação dos indices, após terminarem, inicialize os script findrex, seek1 e seek2 para realizar as buscas.
```sh
g++ findrec.cpp -o findrec
g++ seek1.cpp -o seek1
g++ seek2.cpp -o seek2
```
Compile os execultaveis desses arquivos de acordo com o tipo de busca:

findrec - busca no arquivo de dado <ID>

```sh
./findrec
```

Seek1 - Busca pelo arquivo de indice primário <ID>

```sh
./seek1 
```

Seek2 - Busca pelo arquivo de indice secundário <TItulo>
```sh
./seek2
```


[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/qqQZEoeb)
