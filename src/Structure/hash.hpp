#ifndef HASH
#define HASH

#include "bucket.hpp"
#include "btree.hpp"
#include <climits> // Para uso de INT_MAX
#include <string>

// Estrutura que representa a tabela hash como um array de buckets
struct TabelaHash {
    Bucket* elementos[HASH_SIZE];
};

// Função que inicializa a tabela hash e armazena seus buckets vazios no arquivo binário
TabelaHash* inicializarTabela(std::ofstream& arquivoSaida) {
    TabelaHash* tabelaHash = new TabelaHash();
    for (int i = 0; i < HASH_SIZE; i++) {
        tabelaHash->elementos[i] = criarBucket(arquivoSaida); // A função criarBucket deve estar definida em bucket.hpp
    }
    return tabelaHash;
}

// Função de hashing para calcular o índice a partir de uma chave inteira
int gerarIndice(int chave) {
    uint32_t chaveHash = static_cast<uint32_t>(chave);

    chaveHash ^= chaveHash >> 16;
    chaveHash *= 0x45d9f3b;
    chaveHash ^= chaveHash >> 15;
    chaveHash *= 0x45d9f3b;
    chaveHash ^= chaveHash >> 16;

    return static_cast<int>(chaveHash % HASH_SIZE);
}

// Função de hashing para strings, que gera um índice a partir de uma string
int gerarIndiceString(const std::string& texto) {
    unsigned long hash = 5381;

    for (char c : texto) {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }

    return static_cast<int>(hash % (INT_MAX + 1ULL));
}

// Função para inserir um registro na tabela hash
std::pair<No*, No*> inserirRegistro(TabelaHash* tabelaHash, const Registro& registro, std::ofstream& arquivoEscrita, std::ifstream& arquivoLeitura, No* raizID, No* raizTitulo) {
    int indice = gerarIndice(registro.id);
    int tentativas = 0;

    while (tentativas < BUCKET_SIZE) {
        int endereco = (tentativas * BLOCO_SIZE) + (indice * BUCKET_SIZE * BLOCO_SIZE);
        Bloco* blocoAtual = carregarBloco(endereco, arquivoLeitura);

        if (blocoAtual->header.espacoDisponivel >= (registro.tamanhoRegistro + sizeof(int))) {
            if (raizID == nullptr && raizTitulo == nullptr) {
                BlocoOffset* blocoOffsetID = criarBlocoOffset(endereco);
                raizID = inicializarArvore(registro.id, blocoOffsetID);

                BlocoOffset* blocoOffsetTitulo = criarBlocoOffset(endereco);
                raizTitulo = inicializarArvore(gerarIndiceString(registro.titulo), blocoOffsetTitulo);
            } else {
                raizID = inserir(raizID, registro.id, endereco);
                raizTitulo = inserir(raizTitulo, gerarIndiceString(registro.titulo), endereco);
            }

            escreverRegistroNoBucket(endereco, blocoAtual, registro, arquivoEscrita);
            delete blocoAtual;
            return std::make_pair(raizID, raizTitulo);
        }
        delete blocoAtual;
        tentativas++;
    }

    return std::make_pair(raizID, raizTitulo);
}

// Função para localizar um registro por ID na tabela hash
Registro* buscarRegistroPorID(int idRegistro, std::ifstream& arquivoLeitura) {
    int indice = gerarIndice(idRegistro);
    int endereco;
    Bloco* blocoAtual = nullptr;
    Registro* registroEncontrado = nullptr;

    for (int tentativa = 0; tentativa < BUCKET_SIZE; tentativa++) {
        endereco = (tentativa * BLOCO_SIZE) + (indice * BUCKET_SIZE * BLOCO_SIZE);
        blocoAtual = carregarBloco(endereco, arquivoLeitura);

        if (blocoAtual->header.quantidadeRegistros == 0) {
            delete blocoAtual;
            return nullptr;
        }

        registroEncontrado = buscarRegistroNoBloco(blocoAtual, idRegistro);

        if (registroEncontrado != nullptr) {
            std::cout << "\nBlocos verificados: " << tentativa + 1 << " para encontrar o registro!" << std::endl;
            std::cout << "Total de blocos no arquivo: " << (BUCKET_SIZE * HASH_SIZE) << std::endl;
            return registroEncontrado;
        }
        delete blocoAtual;
    }

    return nullptr;
}

#endif // HASH_HPP

