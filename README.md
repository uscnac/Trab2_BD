# TRABALHO PRÁTICO 2 - BANCO DE DADOS I (ICC200)
### Contribuidores
-Isabella Almeida Macedo Daniel 22250544 \
-Lucas do Nascimento Silva 22250552


## Objetivo
Este trabalho consiste na implementação de programas para armazenamento e consulta em  dados armazenados em memória secundária utilizando as estruturas de arquivo de dados e índice estudadas nas aulas. Os programas devem fornecer suporte para a inserção, assim como diferentes formas de busca, seguindo as técnicas apresentadas nas aulas de organização e indexação de arquivos.


## Ambiente de desenvolvimento

O trabalho deve ser implementado na linguagem C++ utilizando as bibliotecas padrão de chamadas de sistema disponíveis no Linux.

## Arquivos

**tp1_3.3.py:** Script main, implementado um menu interativo para que o usuário possa escolher as opções desejadas de consulta no banco de dados.

**tp1_3.2.py:** Script main, realiza as extrações dos dados do arquivo, conexão com o SGBD, criação das tabelas do banco e inserção dos dados. Possui as definições de classes de modelos relacionadas às entidades do sistema, como Product, Category, Review e SimilarProducts. Cada classe mapeia as tabelas do banco de dados e possui métodos para converter os dados em tuplas adequadas para inserção no banco. 

**tp1_3.1.PDF:** Arquivo PDF com o esquema, dicionario e diagrama do banco de dados.

**infobase.ini:** Arquivo de configuração que contém as informações de conexão com o SGBD

**db_create.sql:** Arquivo que contém as instruções SQL para criar as tabelas do banco de dados necessárias para o sistema. Ele define as estruturas de dados para entidades como products, categories, reviews, entre outras.


## Arquivo de Entrada

Os dados utiliazado para os programas implementados estão disponíveis “[neste arquivo](https://drive.google.com/file/d/1EVoP0d9Wwzj1O6eoFIkel9I3cpe43Gbv/view?usp=sharing)”  em formato CSV que serve como entrada de dados.


## Scripty do código

Faça a clonagem do repositorio 
  
```sh
git clone https://github.com/micalevisk/BD1-2017-1](https://github.com/bd1-icomp-ufam/trabalho-pr-tico-2-bancos-de-dados-2024-2-tp1-isabella-lucas_nascimento
```

Após baixar os arquivos disponibilizados no github, entre no diretorio que foram baixados e execute o script upload.cpp para extração dos metadados e criação e inserção do banco de dados
```sh
g++ upload.cpp -o upload
```
O script solicitará o local do arquivo que se encontra os metadados\
ex: /home/NomeUsuario/Downloads/artigo.csv

Espere a criação das tabelas e a inserção dos dados, após terminarem, inicialize os script findrex, seek1 e seek2 para realizar as buscas.
```sh
g++ findrec.cpp -o findrec
g++ seek1.cpp -o seek1
g++ seek2.cpp -o seek1
```
Compile os execultaveis desses arquivos de acordo com o tipo de busca:

findrec - 

```sh
g++ findrec.cpp -o findrec
```

Seek1 

```sh
g++ seek1.cpp -o seek1 
```

```sh
g++ seek2.cpp -o seek1
```


Cada um deles solicitará o ID para procura do registro
[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/qqQZEoeb)
