#ifndef BUCKET
#define BUCKET

#include "bloco.hpp"
#include <fstream>
#include <cstring>

// Estrutura que representa um bucket, composto por um array de índices de blocos
struct Bucket {
    unsigned int indicesBlocos[BUCKET_SIZE];
};

// Função para criar buckets e inicializá-los vazios no arquivo binário
Bucket* criarBucket(std::ofstream& arquivoBinario) {
    Bucket* bucket = new Bucket();

    // Buffer auxiliar para escrita no arquivo
    char buffer[BLOCO_SIZE];

    // Cria um bloco vazio utilizando a função do arquivo 'bloco.hpp'
    Bloco bloco = criarBloco();

    // Itera sobre cada bloco dentro do bucket
    for (int i = 0; i < BUCKET_SIZE; ++i) {
        bucket->indicesBlocos[i] = i;

        // Copia o header do bloco para o buffer
        std::memcpy(buffer, &bloco.header, sizeof(HeaderBlock));
        // Copia os dados do bloco para o buffer, após o header
        std::memcpy(buffer + sizeof(HeaderBlock), bloco.dadosBloco, BLOCO_SIZE - sizeof(HeaderBlock));

        // Escreve o buffer completo no arquivo binário
        arquivoBinario.write(buffer, BLOCO_SIZE);
    }
    return bucket;
}

// Função para carregar um bloco específico do arquivo para a memória
Bloco* carregarBloco(int enderecoBloco, std::ifstream& arquivoBinario) {
    Bloco* bloco = new Bloco();

    // Move o ponteiro do arquivo para a posição desejada
    arquivoBinario.seekg(enderecoBloco);
    // Lê os dados do bloco diretamente para 'dadosBloco'
    arquivoBinario.read(reinterpret_cast<char*>(bloco->dadosBloco), BLOCO_SIZE);

    // Extrai o header dos dados lidos e atualiza o header do bloco
    Bloco tempBloco = extrairHeader(reinterpret_cast<char*>(bloco->dadosBloco));
    bloco->header = tempBloco.header;

    return bloco;
}

// Função para escrever um registro no bucket correspondente, baseado em uma função hash
bool escreverRegistroNoBucket(int enderecoBloco, Bloco* bloco, const Registro& registro, std::ofstream& arquivoEscrita) {
    // Insere o registro no bloco utilizando a função atualizada
    inserirRegistroNoBloco(bloco, registro);

    // Move o ponteiro do arquivo para a posição do bloco onde o registro será escrito
    arquivoEscrita.seekp(enderecoBloco);
    // Escreve o bloco atualizado de volta no arquivo
    arquivoEscrita.write(reinterpret_cast<char*>(bloco->dadosBloco), BLOCO_SIZE);
    return true;
}

#endif // BUCKET_HPP

