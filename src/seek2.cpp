//TRABALHO 2 BANCO DE DADOS
//ISABELLA ALMEIDA MACEDO DANIEL - 22250544
//LUCAS DO NASCIMENTO SILVA - 22250552

#include "./Structure/hash.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>

int main() {
    // Abre o arquivo de índice do título em modo binário para leitura
    FILE* arquivoIndice = fopen("indiceTitulo.bin", "rb+");
    if (arquivoIndice == nullptr) {
        std::cerr << "Erro ao abrir o arquivo de índice!" << std::endl;
        return 0;
    }

    std::string titulo;
    while (true) {
        std::cout << "Digite o título do registro ou 'exit' para sair: ";
        std::getline(std::cin, titulo);

        if (titulo == "exit") {
            break;
        }

        // Inicializa o contador de acessos ao índice
        unsigned int contadorAcessos = 0;

        // Calcula o hash do título fornecido
        int chaveTitulo = gerarIndiceString(titulo);

        // Busca o endereço do registro no índice usando a chave calculada
        unsigned long enderecoRegistro = buscarChave(chaveTitulo, 0, &contadorAcessos, arquivoIndice);

        if (enderecoRegistro == static_cast<unsigned long>(-1)) {
            std::cout << "Registro com título \"" << titulo << "\" não encontrado no índice." << std::endl;
        } else {
            std::cout << "Registro encontrado no índice!" << std::endl;
            std::cout << "Título: " << titulo << std::endl;
            std::cout << "Número de acessos ao índice: " << contadorAcessos << std::endl;

            // Calcula e exibe o total de blocos do arquivo de índice secundário
            fseek(arquivoIndice, 0, SEEK_END);
            unsigned long tamanhoArquivo = ftell(arquivoIndice);
            unsigned int totalBlocos = tamanhoArquivo / BLOCO_SIZE;
            std::cout << "Total de blocos no arquivo de índice secundário: " << totalBlocos << std::endl;
        }
    }

    // Fecha o arquivo de índice
    fclose(arquivoIndice);

    return 0;
}

