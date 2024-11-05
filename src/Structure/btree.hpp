#ifndef BTREE
#define BTREE

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <queue>
#include "bloco.hpp"

// Estrutura que representa um bloco contendo um offset
struct BlocoOffset {
    int offset;
};

// Estrutura que representa um nó da árvore B+ em memória principal
struct No {
    bool ehFolha;            // Indica se o nó é uma folha
    void** ponteiros;        // Vetor de ponteiros para filhos ou registros
    int* chaves;             // Vetor de chaves armazenadas
    struct No* pai;          // Ponteiro para o nó pai
    int numChaves;           // Número de chaves armazenadas no nó
};

// Estrutura que representa um nó da árvore B+ armazenado em disco
struct NoDisco {
    bool ehFolha;                      // Indica se o nó é uma folha
    unsigned long ponteiros[2 * m + 1]; // Vetor de ponteiros para filhos ou registros
    unsigned int chaves[2 * m];        // Vetor de chaves armazenadas
    unsigned long pai;                 // Offset para o nó pai no arquivo
    unsigned int numChaves;            // Número de chaves armazenadas no nó
};

/*
    Calcula o ponto de divisão (split) de um nó na árvore B+
*/
int calcularPontoDivisao(int tamanho) {
    return (tamanho % 2 == 0) ? (tamanho / 2) : (tamanho / 2 + 1);
}

/*
    Cria um novo nó vazio para a árvore B+
*/
No* criarNo() {
    // Aloca memória para um novo nó
    No* novoNo = (No*)malloc(sizeof(No));
    if (novoNo == NULL) {
        perror("Erro ao criar novo nó\n");
        exit(EXIT_FAILURE);
    }

    // Aloca memória para o vetor de chaves
    novoNo->chaves = (int*)malloc(2 * m * sizeof(int));
    if (novoNo->chaves == NULL) {
        perror("Erro ao alocar vetor de chaves\n");
        exit(EXIT_FAILURE);
    }

    // Aloca memória para o vetor de ponteiros
    novoNo->ponteiros = (void**)malloc((2 * m + 1) * sizeof(void*));
    if (novoNo->ponteiros == NULL) {
        perror("Erro ao alocar vetor de ponteiros\n");
        exit(EXIT_FAILURE);
    }

    novoNo->ehFolha = false;    // Inicialmente, o nó não é uma folha
    novoNo->pai = NULL;         // Nó recém-criado não possui pai
    novoNo->numChaves = 0;      // Nenhuma chave armazenada inicialmente

    return novoNo;
}

/*
    Cria um novo nó folha para a árvore B+
*/
No* criarFolha() {
    No* folha = criarNo(); // Reutiliza a função criarNo()
    folha->ehFolha = true; // Define o nó como folha
    return folha;
}

/*
    Inicializa uma nova árvore B+ com uma chave e seu bloco correspondente
*/
No* inicializarArvore(int chave, BlocoOffset* bloco) {
    No* raiz = criarFolha();      // A raiz é inicialmente uma folha
    raiz->chaves[0] = chave;      // Armazena a chave na raiz
    raiz->ponteiros[0] = bloco;   // Armazena o ponteiro para o bloco
    raiz->numChaves = 1;          // Atualiza o número de chaves
    raiz->ponteiros[2 * m] = NULL; // Define o último ponteiro como NULL
    return raiz;
}

/*
    Cria um bloco para armazenar o offset de uma chave
*/
BlocoOffset* criarBlocoOffset(int offset) {
    BlocoOffset* bloco = (BlocoOffset*)malloc(sizeof(BlocoOffset));
    if (bloco == NULL) {
        perror("Erro ao criar bloco de offset!\n");
        exit(EXIT_FAILURE);
    }
    bloco->offset = offset;
    return bloco;
}

/*
    Encontra a folha onde uma chave deve ser inserida
*/
No* encontrarFolha(No* raiz, int chave) {
    No* atual = raiz;
    while (!atual->ehFolha) { // Percorre até encontrar uma folha
        int i = 0;
        while (i < atual->numChaves && chave >= atual->chaves[i]) {
            i++;
        }
        atual = (No*)atual->ponteiros[i];
    }
    return atual;
}

/*
    Insere uma chave e seu bloco em uma folha com espaço disponível
*/
No* inserirEmFolha(No* folha, int chave, BlocoOffset* bloco) {
    int i = 0;
    while (i < folha->numChaves && folha->chaves[i] < chave) {
        i++;
    }

    for (int j = folha->numChaves; j > i; j--) {
        folha->chaves[j] = folha->chaves[j - 1];
        folha->ponteiros[j] = folha->ponteiros[j - 1];
    }

    folha->chaves[i] = chave;
    folha->ponteiros[i] = bloco;
    folha->numChaves++;
    return folha;
}

/*
    Cria uma nova raiz após o split e insere a chave intermediária
*/
No* inserirNovaRaiz(No* esquerda, int chave, No* direita) {
    No* raiz = criarNo();
    raiz->chaves[0] = chave;
    raiz->ponteiros[0] = esquerda;
    raiz->ponteiros[1] = direita;
    raiz->numChaves = 1;
    raiz->pai = NULL;
    esquerda->pai = raiz;
    direita->pai = raiz;
    return raiz;
}

/*
    Obtém o índice do ponteiro esquerdo no nó pai
*/
int obterIndiceEsquerdo(No* pai, No* esquerda) {
    int indice = 0;
    while (indice <= pai->numChaves && pai->ponteiros[indice] != esquerda) {
        indice++;
    }
    return indice;
}

/*
    Insere uma chave e ponteiro em um nó interno com espaço disponível
*/
No* inserirEmNo(No* raiz, No* no, int indiceEsquerdo, int chave, No* direita) {
    for (int i = no->numChaves; i > indiceEsquerdo; i--) {
        no->ponteiros[i + 1] = no->ponteiros[i];
        no->chaves[i] = no->chaves[i - 1];
    }

    no->ponteiros[indiceEsquerdo + 1] = direita;
    no->chaves[indiceEsquerdo] = chave;
    no->numChaves++;
    return raiz;
}

No* inserirNoPai(No* raiz, No* esquerda, int chave, No* direita);

/*
    Insere uma chave em um nó interno que precisa ser dividido
*/
No* inserirEmNoAposDivisao(No* raiz, No* antigoNo, int indiceEsquerdo, int chave, No* direita) {
    int totalChaves = 2 * m;
    int* chavesTemp = (int*)malloc((totalChaves + 1) * sizeof(int));
    if (chavesTemp == NULL) {
        perror("Erro ao alocar vetor de chaves temporário!\n");
        exit(EXIT_FAILURE);
    }

    void** ponteirosTemp = (void**)malloc((totalChaves + 2) * sizeof(void*));
    if (ponteirosTemp == NULL) {
        perror("Erro ao alocar vetor de ponteiros temporário!\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0, j = 0; i < antigoNo->numChaves + 1; i++, j++) {
        if (j == indiceEsquerdo + 1) j++;
        ponteirosTemp[j] = antigoNo->ponteiros[i];
    }

    for (int i = 0, j = 0; i < antigoNo->numChaves; i++, j++) {
        if (j == indiceEsquerdo) j++;
        chavesTemp[j] = antigoNo->chaves[i];
    }

    ponteirosTemp[indiceEsquerdo + 1] = direita;
    chavesTemp[indiceEsquerdo] = chave;

    int pontoDivisao = calcularPontoDivisao(totalChaves + 1);
    No* novoNo = criarNo();
    antigoNo->numChaves = 0;

    for (int i = 0; i < pontoDivisao - 1; i++) {
        antigoNo->ponteiros[i] = ponteirosTemp[i];
        antigoNo->chaves[i] = chavesTemp[i];
        antigoNo->numChaves++;
    }
    antigoNo->ponteiros[pontoDivisao - 1] = ponteirosTemp[pontoDivisao - 1];

    int chavePromovida = chavesTemp[pontoDivisao - 1];

    for (int i = pontoDivisao, j = 0; i < totalChaves + 1; i++, j++) {
        novoNo->ponteiros[j] = ponteirosTemp[i];
        novoNo->chaves[j] = chavesTemp[i];
        novoNo->numChaves++;
    }
    novoNo->ponteiros[novoNo->numChaves] = ponteirosTemp[totalChaves + 1];

    free(chavesTemp);
    free(ponteirosTemp);

    novoNo->pai = antigoNo->pai;
    for (int i = 0; i <= novoNo->numChaves; i++) {
        No* filho = (No*)novoNo->ponteiros[i];
        filho->pai = novoNo;
    }

    return inserirNoPai(raiz, antigoNo, chavePromovida, novoNo);
}

/*
    Insere um nó após a divisão no nó pai
*/
No* inserirNoPai(No* raiz, No* esquerda, int chave, No* direita) {
    No* pai = esquerda->pai;

    if (pai == NULL) {
        return inserirNovaRaiz(esquerda, chave, direita);
    }

    int indiceEsquerdo = obterIndiceEsquerdo(pai, esquerda);

    if (pai->numChaves < 2 * m) {
        return inserirEmNo(raiz, pai, indiceEsquerdo, chave, direita);
    }

    return inserirEmNoAposDivisao(raiz, pai, indiceEsquerdo, chave, direita);
}

/*
    Insere uma chave em uma folha que precisa ser dividida
*/
No* inserirEmFolhaAposDivisao(No* raiz, No* folha, int chave, BlocoOffset* bloco) {
    int totalChaves = 2 * m;
    int* chavesTemp = (int*)malloc((totalChaves + 1) * sizeof(int));
    if (chavesTemp == NULL) {
        perror("Erro ao alocar vetor de chaves temporário!\n");
        exit(EXIT_FAILURE);
    }

    void** ponteirosTemp = (void**)malloc((totalChaves + 1) * sizeof(void*));
    if (ponteirosTemp == NULL) {
        perror("Erro ao alocar vetor de ponteiros temporário!\n");
        exit(EXIT_FAILURE);
    }

    int i = 0;
    while (i < totalChaves && chave > folha->chaves[i]) {
        i++;
    }

    for (int j = 0, k = 0; j < folha->numChaves; j++, k++) {
        if (k == i) k++;
        chavesTemp[k] = folha->chaves[j];
        ponteirosTemp[k] = folha->ponteiros[j];
    }

    chavesTemp[i] = chave;
    ponteirosTemp[i] = bloco;

    folha->numChaves = 0;

    int pontoDivisao = calcularPontoDivisao(totalChaves);

    for (int j = 0; j < pontoDivisao; j++) {
        folha->chaves[j] = chavesTemp[j];
        folha->ponteiros[j] = ponteirosTemp[j];
        folha->numChaves++;
    }

    No* novaFolha = criarFolha();
    for (int j = pontoDivisao, k = 0; j < totalChaves + 1; j++, k++) {
        novaFolha->chaves[k] = chavesTemp[j];
        novaFolha->ponteiros[k] = ponteirosTemp[j];
        novaFolha->numChaves++;
    }

    free(chavesTemp);
    free(ponteirosTemp);

    novaFolha->ponteiros[2 * m] = folha->ponteiros[2 * m];
    folha->ponteiros[2 * m] = novaFolha;

    novaFolha->pai = folha->pai;

    int novaChave = novaFolha->chaves[0];

    return inserirNoPai(raiz, folha, novaChave, novaFolha);
}

/*
    Função principal para inserir uma chave e seu offset na árvore B+
*/
No* inserir(No* raiz, int chave, int offset) {
    BlocoOffset* bloco = criarBlocoOffset(offset);
    No* folha = encontrarFolha(raiz, chave);

    if (folha->numChaves < 2 * m) {
        folha = inserirEmFolha(folha, chave, bloco);
        return raiz;
    }

    return inserirEmFolhaAposDivisao(raiz, folha, chave, bloco);
}

/*
    Calcula o nível de um nó em relação à raiz
*/
int calcularNivel(No* const raiz, No* filho) {
    int nivel = 0;
    No* atual = filho;
    while (atual != raiz) {
        atual = atual->pai;
        nivel++;
    }
    return nivel;
}

/*
    Imprime as informações de um nó armazenado em disco
*/
void imprimirNoDisco(NoDisco no) {
    std::cout << "\tPosição do pai: " << no.pai << std::endl;
    std::cout << "\tÉ folha: " << no.ehFolha << std::endl;
    std::cout << "\tNúmero de chaves: " << no.numChaves << std::endl;
    std::cout << "\tChaves: \n\t\t";
    for (unsigned int i = 0; i < 2 * m; i++) {
        std::cout << "(" << i << ":" << no.chaves[i] << "), ";
    }
    std::cout << std::endl;
    std::cout << "\tPonteiros: \n\t\t";
    for (unsigned int i = 0; i <= 2 * m; i++) {
        std::cout << "(" << i << ":" << no.ponteiros[i] << "), ";
    }
    std::cout << std::endl << std::endl;
}

/*
    Busca uma chave em uma árvore B+ armazenada em disco
*/
unsigned long buscarChave(unsigned int chave, unsigned long posicao, unsigned int* acessosDisco, FILE* arquivo) {
    NoDisco no;
    fseek(arquivo, posicao, SEEK_SET);
    fread(&no, sizeof(NoDisco), 1, arquivo);
    (*acessosDisco)++;
    imprimirNoDisco(no);

    if (!no.ehFolha) {
        for (unsigned int i = 0; i < no.numChaves; i++) {
            if (chave < no.chaves[i]) {
                return buscarChave(chave, no.ponteiros[i], acessosDisco, arquivo);
            }
        }
        return buscarChave(chave, no.ponteiros[no.numChaves], acessosDisco, arquivo);
    }

    for (unsigned int i = 0; i < no.numChaves; i++) {
        if (chave == no.chaves[i]) {
            return no.ponteiros[i];
        }
    }

    return -1;
}

/*
    Imprime todos os nós de uma árvore B+ armazenada em disco
*/
void imprimirArvoreDisco(unsigned long posicao, FILE* arquivo) {
    NoDisco no;
    fseek(arquivo, posicao, SEEK_SET);
    fread(&no, sizeof(NoDisco), 1, arquivo);
    imprimirNoDisco(no);
    if (!no.ehFolha) {
        for (unsigned int i = 0; i <= no.numChaves; i++) {
            imprimirArvoreDisco(no.ponteiros[i], arquivo);
        }
    }
}

/*
    Imprime a árvore B+ armazenada em memória
*/
void imprimirArvore(No* const raiz) {
    std::queue<No*> fila;
    No* atual = NULL;
    int nivelAtual = 0;
    int novoNivel = 0;

    fila.push(raiz);

    while (!fila.empty()) {
        atual = fila.front();
        fila.pop();

        if (atual->pai != NULL && atual == atual->pai->ponteiros[0]) {
            novoNivel = calcularNivel(raiz, atual);
            if (novoNivel != nivelAtual) {
                nivelAtual = novoNivel;
                std::cout << std::endl;
            }
        }
        for (int i = 0; i < atual->numChaves; i++) {
            std::cout << atual->chaves[i] << " ";
        }
        if (!atual->ehFolha) {
            for (int i = 0; i <= atual->numChaves; i++) {
                fila.push((No*)atual->ponteiros[i]);
            }
        }
        std::cout << "| ";
    }
    std::cout << std::endl;
}

/*
    Grava a árvore B+ em um arquivo no disco
*/
unsigned long gravarArvore(No* raiz, unsigned long paiPosicao, FILE* arquivo) {
    NoDisco temp;
    unsigned long posicaoAtual = ftell(arquivo);
    temp.ehFolha = raiz->ehFolha;
    temp.numChaves = raiz->numChaves;
    temp.pai = paiPosicao;

    for (unsigned int i = 0; i < (2 * m); i++) {
        temp.chaves[i] = (i < temp.numChaves) ? raiz->chaves[i] : -1;
    }

    for (unsigned int i = 0; i <= (2 * m); i++) {
        temp.ponteiros[i] = (i <= temp.numChaves) ? (unsigned long)raiz->ponteiros[i] : -1;
    }

    fwrite(&temp, sizeof(NoDisco), 1, arquivo);

    if (temp.ehFolha) {
        return posicaoAtual;
    }

    for (unsigned int i = 0; i <= (2 * m); i++) {
        if (i <= temp.numChaves) {
            temp.ponteiros[i] = gravarArvore((No*)raiz->ponteiros[i], posicaoAtual, arquivo);
        } else {
            temp.ponteiros[i] = -1;
        }
    }

    fseek(arquivo, posicaoAtual, SEEK_SET);
    fwrite(&temp, sizeof(NoDisco), 1, arquivo);
    fseek(arquivo, 0, SEEK_END);

    return posicaoAtual;
}

#endif // BPT_HPP

