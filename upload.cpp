//TRABALHO 2 BANCO DE DADOS
//ISABELLA ALMEIDA MACEDO DANIEL - 22250544
//LUCAS DO NASCIMENTO SILVA - 22250552

#include "./Structure/hash.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
#include <cstring>

// Função para ler o próximo campo do arquivo de entrada até encontrar um padrão específico
bool lerProximoCampo(FILE* arquivoEntrada, char campo[], const std::string& padrao) {
    unsigned int posicao = 0;
    char caractere;

    while (posicao < 1029) {
        caractere = getc(arquivoEntrada);
        if (caractere == EOF)
            return true;

        if (caractere == 0 || (caractere >= 7 && caractere <= 13) || (caractere >= 32 && caractere <= 126)) {
            if (posicao == 0 && caractere == ';') {
                strcpy(campo, "NULL");
                return false;
            }

            campo[posicao] = caractere;

            if (posicao > 0) {
                if (strncmp(campo, "NULL", 4) == 0) {
                    if (campo[posicao] == ';') {
                        campo[posicao] = '\0';
                        return false;
                    } else if (campo[posicao - 1] == '\r' && campo[posicao] == '\n') {
                        campo[posicao - 1] = '\0';
                        return false;
                    }
                }

                if (campo[posicao - 1] == padrao[0] && campo[posicao] == padrao[1]) {
                    if (padrao[0] == '"') {
                        campo[posicao - 1] = '\0';
                    } else {
                        campo[posicao - 2] = '\0';
                    }
                    for (unsigned int i = 1; i < posicao; i++) {
                        campo[i - 1] = campo[i];
                    }
                    return false;
                }
            }
            posicao++;
        }
    }
    return false;
}

int main() {
    std::string caminhoArquivoEntrada;

    // Solicita ao usuário o local do arquivo de entrada
    std::cout << "Informe o local do arquivo: ";
    std::getline(std::cin, caminhoArquivoEntrada);

    FILE* arquivoEntrada = fopen(caminhoArquivoEntrada.c_str(), "r");
    if (!arquivoEntrada) {
        std::cerr << "Não foi possível abrir o arquivo de entrada!" << std::endl;
        return -1;
    }

    fseek(arquivoEntrada, 0, SEEK_END);
    long tamanhoArquivo = ftell(arquivoEntrada);
    rewind(arquivoEntrada);

    char padrao[2], campo[1030];
    std::string caminhoArquivoDados = "dados.bin";

    std::ofstream arquivoDadosEscrita(caminhoArquivoDados, std::ios::binary | std::ios::out);
    if (!arquivoDadosEscrita) {
        std::cerr << "Erro ao criar o arquivo de dados binário!" << std::endl;
        return 1;
    }

    std::cout << "Criando tabela..." << std::endl;
    TabelaHash* tabelaHash = inicializarTabela(arquivoDadosEscrita);
    std::cout << "Tabela hash criada!" << std::endl;

    delete tabelaHash;

    std::ifstream arquivoDadosLeitura(caminhoArquivoDados, std::ios::binary | std::ios::in);
    if (!arquivoDadosLeitura) {
        std::cerr << "Erro ao abrir o arquivo de dados!" << std::endl;
        return 1;
    }

    No* raizID = nullptr;
    No* raizTitulo = nullptr;

    std::cout << "Inserindo registros e criando índices..." << std::endl;

    long posicaoAtual = 0;
    int porcentagemAnterior = -1;

    while (true) {
        int id, ano, citacoes;
        std::string atualizacao, titulo, autores, snippet;

        padrao[0] = '"';
        padrao[1] = ';';

        if (lerProximoCampo(arquivoEntrada, campo, padrao)) break;
        id = std::stoi(campo);

        if (lerProximoCampo(arquivoEntrada, campo, padrao)) break;
        titulo = campo;

        if (lerProximoCampo(arquivoEntrada, campo, padrao)) break;
        ano = std::stoi(campo);

        if (lerProximoCampo(arquivoEntrada, campo, padrao)) break;
        autores = campo;

        if (lerProximoCampo(arquivoEntrada, campo, padrao)) break;
        citacoes = std::stoi(campo);

        if (lerProximoCampo(arquivoEntrada, campo, padrao)) break;
        atualizacao = campo;

        padrao[0] = '\r';
        padrao[1] = '\n';

        if (lerProximoCampo(arquivoEntrada, campo, padrao)) break;
        snippet = campo;

        Registro registro = createRegistro(id, ano, citacoes, atualizacao, titulo, autores, snippet);
        auto raizes = inserirRegistro(nullptr, registro, arquivoDadosEscrita, arquivoDadosLeitura, raizID, raizTitulo);
        raizID = raizes.first;
        raizTitulo = raizes.second;

        posicaoAtual = ftell(arquivoEntrada);
        int porcentagemAtual = static_cast<int>((posicaoAtual * 100) / tamanhoArquivo);
        if (porcentagemAtual != porcentagemAnterior) {
            std::cout << "\rProgresso: " << porcentagemAtual << "%";
            std::cout.flush();
            porcentagemAnterior = porcentagemAtual;
        }
    }

    std::cout << "\nProcesso concluído!" << std::endl << std::endl;

    FILE* arquivoIndiceID = fopen("indiceID.bin", "wb+");
    FILE* arquivoIndiceTitulo = fopen("indiceTitulo.bin", "wb+");

    gravarArvore(raizID, -1, arquivoIndiceID);
    gravarArvore(raizTitulo, -1, arquivoIndiceTitulo);

    arquivoDadosEscrita.close();
    arquivoDadosLeitura.close();
    fclose(arquivoIndiceID);
    fclose(arquivoIndiceTitulo);
    fclose(arquivoEntrada);

    return 0;
}

