#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define TABLE_SIZE 1499 // Escolhido um número primo próximo

typedef struct {
    unsigned long cpf;
    int ocupado; // 0 para livre, 1 para ocupado
} HashEntry;

// Função de hash com multiplicação para melhor distribuição
int hashFunc(unsigned long cpf) {
    double A = (sqrt(5) - 1) / 2.0; // Constante irracional
    return (int)(TABLE_SIZE * fmod(cpf * A, 1.0));
}

// Função de hash auxiliar para duplo hashing
int hashFuncAux(unsigned long cpf) {
    return 1 + (cpf % (TABLE_SIZE - 1));
}

void inicializaTabela(HashEntry tabela[]) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        tabela[i].ocupado = 0; // Marca como livre
    }
}

int insereCPF(HashEntry tabela[], unsigned long cpf) {
    int index = hashFunc(cpf);
    int colisoes = 0;
    int hashAux = hashFuncAux(cpf);

    // Sondagem dupla (endereçamento aberto com duplo hashing)
    while (tabela[index].ocupado) {
        colisoes++;
        index = (index + hashAux) % TABLE_SIZE; // Avanço usando hash auxiliar
    }

    tabela[index].cpf = cpf;
    tabela[index].ocupado = 1; // Marca como ocupado

    return colisoes;
}

int contaPosicoesVazias(HashEntry tabela[]) {
    int vazias = 0;
    for (int i = 0; i < TABLE_SIZE; i++) {
        if (!tabela[i].ocupado) {
            vazias++;
        }
    }
    return vazias;
}

int main() {
    HashEntry tabela[TABLE_SIZE];
    inicializaTabela(tabela);

    FILE *arquivo = fopen("cpfs_gerados.txt", "r");

    if (arquivo == NULL) {
        perror("Erro ao abrir o arquivo");
        return 1;
    }

    unsigned long cpf;
    int colisoesTotais = 0;
    int insercoes = 0;
    int colisoesPorInsercao[10] = {0}; // Para 100, 200, 300, ..., 1000 inserções

    while (fscanf(arquivo, "%lu", &cpf) != EOF && insercoes < 1000) {
        int colisoes = insereCPF(tabela, cpf);
        colisoesTotais += colisoes;
        insercoes++;

        if (insercoes % 100 == 0) {
            colisoesPorInsercao[insercoes / 100 - 1] = colisoesTotais;
        }
    }

    fclose(arquivo);

    // Gera a tabela com o número de chaves inseridas e número de colisões
    FILE *saida = fopen("colisoes.csv", "w");
    fprintf(saida, "Chaves Inseridas,Colisões\n");
    for (int i = 0; i < 10; i++) {
        fprintf(saida, "%d,%d\n", (i + 1) * 100, colisoesPorInsercao[i]);
    }
    fclose(saida);

    int posicoesVazias = contaPosicoesVazias(tabela);
    printf("Número de colisões: %d\n", colisoesTotais);
    printf("Número de posições vazias: %d\n", posicoesVazias);

    return 0;
}
