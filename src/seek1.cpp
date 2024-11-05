//TRABALHO 2 BANCO DE DADOS
//ISABELLA ALMEIDA MACEDO DANIEL - 22250544
//LUCAS DO NASCIMENTO SILVA - 22250552

#include "./Structure/hash.hpp"
#include <iostream>
#include <fstream>
#include <string>

int main() {
    // Abre o arquivo de índice em modo binário para leitura e escrita
    FILE* arquivoIndice = fopen("indiceID.bin", "rb+");
    if (arquivoIndice == nullptr) {
        std::cerr << "Erro: não foi possível abrir o arquivo de índice!" << std::endl;
        return 0;
    }

    std::string entrada;
    while (true) {
        std::cout << "Digite o ID do registro ou 'exit' para sair: ";
        std::getline(std::cin, entrada);

        if (entrada == "exit") {
            break;
        }

        int idBusca;
        try {
            idBusca = std::stoi(entrada); // Converte a entrada para inteiro
        } catch (const std::invalid_argument& e) {
            std::cout << "ID inválido. Por favor, insira um número válido ou 'exit' para sair." << std::endl;
            continue;
        }

        // Inicializa o contador de acessos ao índice
        unsigned int contadorAcessos = 0;

        // Busca o endereço do registro no índice utilizando a função atualizada
        unsigned long enderecoRegistro = buscarChave(idBusca, 0, &contadorAcessos, arquivoIndice);

        // Verifica se o registro foi encontrado no índice
        if (enderecoRegistro == static_cast<unsigned long>(-1)) {
            std::cout << "Registro com ID " << idBusca << " não encontrado no índice." << std::endl;
        } else {
            std::cout << "Registro encontrado no índice!" << std::endl;
            std::cout << "ID: " << idBusca << std::endl;
            std::cout << "Número de acessos ao índice: " << contadorAcessos << std::endl;
            // Exibe o total de blocos no arquivo de índice
            fseek(arquivoIndice, 0, SEEK_END);
            unsigned long tamanhoArquivo = ftell(arquivoIndice);
            unsigned int totalBlocos = tamanhoArquivo / BLOCO_SIZE;
            std::cout << "Total de blocos no arquivo de índice primário: " << totalBlocos << std::endl;
        }
    }

    // Fecha o arquivo de índice
    fclose(arquivoIndice);

    return 0;
}

