#ifndef BLOCO
#define BLOCO

#include <iostream>
#include <vector>
#include <cstring>

// Constantes que definem tamanhos e limites
const int BLOCO_SIZE = 4096; // Tamanho do bloco em bytes
const int BUCKET_SIZE = 20;  // Tamanho do bucket em blocos
const int HASH_SIZE = 15000; // Tamanho da tabela hash (quantidade de buckets)
const int m = 1;             // Ordem mínima da árvore B+

// Estrutura que representa um registro
typedef struct Registro {
    int id;                  // Identificador único do registro

    // Cabeçalho do Registro
    int tamanhoRegistro;     // Tamanho total do registro em bytes

    // Dados do Registro
    int ano;                 // Ano de publicação
    int citacoes;            // Número de citações
    std::string atualizacao; // Data de atualização (tamanho fixo)
    std::string titulo;      // Título do artigo
    std::string autores;     // Lista de autores
    std::string snippet;     // Resumo ou trecho do artigo
} Registro;

// Calcula o tamanho do registro e atualiza o campo tamanhoRegistro
void sizeRegistro(Registro* registro) {
    int size = 0;
    size += sizeof(int); // id
    size += sizeof(int); // tamanhoRegistro
    size += sizeof(int); // ano
    size += sizeof(int); // citacoes
    // Nas strings, somamos o tamanho da string + 1 (para o '\0')
    size += registro->atualizacao.size() + 1;
    size += registro->titulo.size() + 1;
    size += registro->autores.size() + 1;
    size += registro->snippet.size() + 1;
    registro->tamanhoRegistro = size;
}

// Imprime o Registro em um formato específico
void printRegistro(const Registro& registro) {
    std::cout << "\n-----------------------------------------------\n";
    std::cout << "ID: " << registro.id << "\n";
    std::cout << "Título: " << registro.titulo << "\n";
    std::cout << "Ano: " << registro.ano << "\n";
    std::cout << "Autores: " << registro.autores << "\n";
    std::cout << "Citações: " << registro.citacoes << "\n";
    std::cout << "Atualização: " << registro.atualizacao << "\n";
    std::cout << "Snippet: " << registro.snippet << "\n";
    std::cout << "--------------------------------------------------\n\n";
}

// Cria um novo Registro
Registro createRegistro(int id = 0, int ano = 0, int citacoes = 0,
                        std::string atualizacao = "NULL", std::string titulo = "NULL",
                        std::string autores = "NULL", std::string snippet = "NULL") {
    Registro registro;
    registro.id = id;
    registro.ano = ano;
    registro.citacoes = citacoes;
    registro.atualizacao = atualizacao;
    registro.titulo = titulo;
    registro.autores = autores;
    registro.snippet = snippet;
    sizeRegistro(&registro); // Atualiza tamanhoRegistro
    return registro;
}

// Função adicional: compara dois registros pelo ID
bool compareRegistroById(const Registro& a, const Registro& b) {
    return a.id < b.id;
}

// Estrutura que representa o cabeçalho de um bloco, contendo metadados
struct HeaderBlock {
    int espacoDisponivel;             // Espaço livre no bloco (em bytes)
    int quantidadeRegistros;          // Número de registros armazenados
    std::vector<int> offsetsRegistros; // Lista de offsets dos registros dentro do bloco
};

// Estrutura que representa um bloco de dados
struct Bloco {
    HeaderBlock header;                     // Cabeçalho do bloco
    unsigned char dadosBloco[BLOCO_SIZE];   // Conteúdo do bloco em bytes
};

// Função para criar e inicializar um novo bloco vazio
Bloco criarBloco() {
    Bloco bloco;
    bloco.header.espacoDisponivel = BLOCO_SIZE - 2 * sizeof(int); // Espaço livre inicial, descontando o tamanho do header
    bloco.header.quantidadeRegistros = 0;

    // Inicializa todos os bytes do bloco com zero
    std::memset(bloco.dadosBloco, 0, BLOCO_SIZE);
    return bloco;
}

// Função para extrair o header de um bloco a partir de um array de bytes
Bloco extrairHeader(const char* bytesBloco) {
    Bloco bloco;
    int posicao = 0;

    // Copia o espaço disponível e a quantidade de registros do array de bytes para o header do bloco
    std::memcpy(&bloco.header.espacoDisponivel, &bytesBloco[posicao], sizeof(int));
    posicao += sizeof(int);
    std::memcpy(&bloco.header.quantidadeRegistros, &bytesBloco[posicao], sizeof(int));
    posicao += sizeof(int);

    // Lê os offsets de cada registro armazenado no bloco
    for (int i = 0; i < bloco.header.quantidadeRegistros; ++i) {
        int offset;
        std::memcpy(&offset, &bytesBloco[posicao], sizeof(int));
        posicao += sizeof(int);
        bloco.header.offsetsRegistros.push_back(offset);
    }

    // Copia todo o conteúdo do bloco para a estrutura Bloco
    std::memcpy(bloco.dadosBloco, bytesBloco, BLOCO_SIZE);
    return bloco;
}

// Função para imprimir os metadados do bloco
void imprimirBloco(const Bloco* bloco) {
    std::cout << "Espaço Disponível: " << bloco->header.espacoDisponivel << " bytes" << std::endl;
    std::cout << "Quantidade de Registros: " << bloco->header.quantidadeRegistros << std::endl;
    std::cout << "Offsets dos Registros: ";
    for (int offset : bloco->header.offsetsRegistros) {
        std::cout << offset << " ";
    }
    std::cout << std::endl;
}

// Função para inserir um registro no bloco
bool inserirRegistroNoBloco(Bloco* bloco, const Registro& registro) {

    // Verifica se há espaço suficiente no bloco para o novo registro
    if (bloco->header.espacoDisponivel < registro.tamanhoRegistro + sizeof(int)) {
        return false; // Não há espaço suficiente
    }

    // Determina o endereço onde o registro será inserido
    int enderecoInsercao;

    // Se o bloco estiver vazio, insere no final; caso contrário, insere antes do último registro
    if (bloco->header.quantidadeRegistros == 0) {
        enderecoInsercao = BLOCO_SIZE - registro.tamanhoRegistro;
    } else {
        enderecoInsercao = bloco->header.offsetsRegistros.back() - registro.tamanhoRegistro;
    }

    // Armazena o offset do novo registro
    int novoOffset = enderecoInsercao;

    // Copia os dados do registro para o bloco
    int posicao = enderecoInsercao;
    std::memcpy(&bloco->dadosBloco[posicao], &registro.id, sizeof(int));
    posicao += sizeof(int);

    std::memcpy(&bloco->dadosBloco[posicao], &registro.tamanhoRegistro, sizeof(int));
    posicao += sizeof(int);

    std::memcpy(&bloco->dadosBloco[posicao], &registro.ano, sizeof(int));
    posicao += sizeof(int);

    std::memcpy(&bloco->dadosBloco[posicao], &registro.citacoes, sizeof(int));
    posicao += sizeof(int);

    // Copia as strings, incluindo o caractere nulo '\0'
    std::memcpy(&bloco->dadosBloco[posicao], registro.atualizacao.c_str(), registro.atualizacao.size() + 1);
    posicao += registro.atualizacao.size() + 1;

    std::memcpy(&bloco->dadosBloco[posicao], registro.titulo.c_str(), registro.titulo.size() + 1);
    posicao += registro.titulo.size() + 1;

    std::memcpy(&bloco->dadosBloco[posicao], registro.autores.c_str(), registro.autores.size() + 1);
    posicao += registro.autores.size() + 1;

    std::memcpy(&bloco->dadosBloco[posicao], registro.snippet.c_str(), registro.snippet.size() + 1);
    posicao += registro.snippet.size() + 1;

    // Atualiza o header do bloco com o novo espaço disponível e quantidade de registros
    bloco->header.espacoDisponivel -= (registro.tamanhoRegistro + sizeof(int));
    bloco->header.quantidadeRegistros++;

    // Adiciona o offset do novo registro à lista de offsets
    bloco->header.offsetsRegistros.push_back(novoOffset);

    // Reinicia a posição para atualizar o header no array de bytes do bloco
    posicao = 0;

    // Copia os metadados atualizados para o array de bytes do bloco
    std::memcpy(&bloco->dadosBloco[posicao], &bloco->header.espacoDisponivel, sizeof(int));
    posicao += sizeof(int);

    std::memcpy(&bloco->dadosBloco[posicao], &bloco->header.quantidadeRegistros, sizeof(int));
    posicao += sizeof(int);

    // Copia os offsets atualizados para o array de bytes do bloco
    for (int offset : bloco->header.offsetsRegistros) {
        std::memcpy(&bloco->dadosBloco[posicao], &offset, sizeof(int));
        posicao += sizeof(int);
    }

    return true; // Registro inserido com sucesso
}

// Função para buscar um registro no bloco pelo ID
Registro* buscarRegistroNoBloco(const Bloco* bloco, int idRegistro) {
    // Cria um novo registro para armazenar os dados encontrados
    Registro* registroEncontrado = new Registro();

    // Percorre todos os offsets dos registros armazenados no bloco
    for (int offset : bloco->header.offsetsRegistros) {
        int posicao = offset;

        // Lê o ID do registro na posição atual
        std::memcpy(&registroEncontrado->id, &bloco->dadosBloco[posicao], sizeof(int));

        // Verifica se o ID corresponde ao registro buscado
        if (registroEncontrado->id == idRegistro) {
            posicao += sizeof(int);

            // Lê os demais campos do registro
            std::memcpy(&registroEncontrado->tamanhoRegistro, &bloco->dadosBloco[posicao], sizeof(int));
            posicao += sizeof(int);

            std::memcpy(&registroEncontrado->ano, &bloco->dadosBloco[posicao], sizeof(int));
            posicao += sizeof(int);

            std::memcpy(&registroEncontrado->citacoes, &bloco->dadosBloco[posicao], sizeof(int));
            posicao += sizeof(int);

            registroEncontrado->atualizacao = std::string(reinterpret_cast<const char*>(&bloco->dadosBloco[posicao]));
            posicao += registroEncontrado->atualizacao.size() + 1;

            registroEncontrado->titulo = std::string(reinterpret_cast<const char*>(&bloco->dadosBloco[posicao]));
            posicao += registroEncontrado->titulo.size() + 1;

            registroEncontrado->autores = std::string(reinterpret_cast<const char*>(&bloco->dadosBloco[posicao]));
            posicao += registroEncontrado->autores.size() + 1;

            registroEncontrado->snippet = std::string(reinterpret_cast<const char*>(&bloco->dadosBloco[posicao]));

            // Retorna o registro encontrado
            return registroEncontrado;
        }
    }

    // Se o registro não for encontrado, libera a memória e retorna nulo
    delete registroEncontrado;
    return nullptr;
}

#endif // BLOCO_HPP

