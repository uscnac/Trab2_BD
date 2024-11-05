//TRABALHO 2 BANCO DE DADOS
//ISABELLA ALMEIDA MACEDO DANIEL - 22250544
//LUCAS DO NASCIMENTO SILVA - 22250552

#include "./Structure/hash.hpp"
#include <iostream>
#include <fstream>
#include <string>

int main() {
    // Abre o arquivo binário de dados em modo de leitura
    std::ifstream arquivoDados("dados.bin", std::ios::binary | std::ios::in);
    if (!arquivoDados.is_open()) {
        std::cerr << "Erro ao abrir o arquivo de dados!" << std::endl;
        return 0;
    }

    std::string entrada;
    while (true) {
        std::cout << "Digite o ID do registro ou 'exit' para sair: ";
        std::getline(std::cin, entrada);

        if (entrada == "exit") {
            break;
        }

        int idRegistro;
        try {
            idRegistro = std::stoi(entrada); // Converte a entrada para inteiro
        } catch (const std::invalid_argument& e) {
            std::cout << "ID inválido. Por favor, insira um número válido ou 'exit' para sair." << std::endl;
            continue;
        }

        // Busca o registro correspondente pelo ID
        Registro* registro = buscarRegistroPorID(idRegistro, arquivoDados);

        // Verifica se o registro foi encontrado e exibe as informações
        if (registro != nullptr) {
            printRegistro(*registro);
            delete registro; // Libera a memória alocada para o registro
        } else {
            std::cout << "Registro com ID " << idRegistro << " não encontrado." << std::endl;
        }
    }

    // Fecha o arquivo de dados
    arquivoDados.close();

    return 0;
}

