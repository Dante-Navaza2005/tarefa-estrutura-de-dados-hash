#include <stdio.h>
#include <stdlib.h>

#define TABLE_SIZE 1213 // Número primo para tabela

typedef struct {
    unsigned long cpf;
    int ocupado; // 0 = vazio, 1 = ocupado, 2 = removido
} HashEntry;

typedef struct {
    HashEntry *tabela;
    int tamanho;
} HashTable;

// Função hash usando o método da divisão
int hashFuncDivisao(unsigned long cpf, int tamanho) {
    return cpf % tamanho; // Tamanho deve ser um número primo
}

// Segunda função hash para dispersão dupla (para endereçamento aberto)
int hashFunc2(unsigned long cpf, int tamanho) {
    return 1 + (cpf % (tamanho - 1)); // Deve ser co-prima ao tamanho da tabela
}

HashTable *inicializaTabela(int tamanho) {
    HashTable *hashTable = (HashTable *)malloc(sizeof(HashTable));
    hashTable->tabela = (HashEntry *)malloc(tamanho * sizeof(HashEntry));
    hashTable->tamanho = tamanho;
    for (int i = 0; i < tamanho; i++) {
        hashTable->tabela[i].ocupado = 0; // Inicializa todas as entradas como vazias
        hashTable->tabela[i].cpf = 0; // Inicializa o CPF como zero
    }
    return hashTable;
}

void insereCPF(HashTable *hashTable, unsigned long cpf) {
    int index, i = 0;
    do {
        // Usando a função de divisão como hash principal e dispersão dupla para tratar colisões
        index = (hashFuncDivisao(cpf, hashTable->tamanho) + i * hashFunc2(cpf, hashTable->tamanho)) % hashTable->tamanho;
        i++;
    } while (hashTable->tabela[index].ocupado == 1 && i < hashTable->tamanho);

    if (i < hashTable->tamanho) {
        hashTable->tabela[index].cpf = cpf;
        hashTable->tabela[index].ocupado = 1;
    } else {
        printf("Erro: Tabela cheia, não foi possível inserir o CPF %lu.\n", cpf);
    }
}

int contaColisoes(HashTable *hashTable) {
    int colisoes = 0;
    for (int i = 0; i < hashTable->tamanho; i++) {
        if (hashTable->tabela[i].ocupado == 1) {
            int index = hashFuncDivisao(hashTable->tabela[i].cpf, hashTable->tamanho);
            if (index != i) {
                colisoes++;
            }
        }
    }
    return colisoes;
}

int contaPosicoesVazias(HashTable *hashTable) {
    int vazias = 0;
    for (int i = 0; i < hashTable->tamanho; i++) {
        if (hashTable->tabela[i].ocupado == 0) {
            vazias++;
        }
    }
    return vazias;
}

int main() {
    HashTable *hashTable = inicializaTabela(TABLE_SIZE);

    FILE *arquivo = fopen("cpfs.txt", "r");
    if (arquivo == NULL) {
        perror("Erro ao abrir o arquivo");
        return 1;
    }

    FILE *saida = fopen("colisoes.csv", "w");
    if (saida == NULL) {
        perror("Erro ao criar o arquivo CSV");
        return 1;
    }

    // Escreve o cabeçalho do CSV
    fprintf(saida, "Chaves Inseridas,Colisoes\n");

    unsigned long cpf;
    int insercoes = 0;

    while (fscanf(arquivo, "%lu", &cpf) != EOF && insercoes < 1000) {
        insereCPF(hashTable, cpf); // Usando o método da Divisão com endereçamento aberto e dispersão dupla
        insercoes++;

        if (insercoes % 100 == 0) {
            int colisoes = contaColisoes(hashTable);
            // Escreve o número de inserções e de colisões no CSV
            fprintf(saida, "%d,%d\n", insercoes, colisoes);
        }
    }

    fclose(arquivo);
    fclose(saida);

    int colisoesTotais = contaColisoes(hashTable);
    int posicoesVazias = contaPosicoesVazias(hashTable);

    printf("Número de colisões: %d\n", colisoesTotais);
    printf("Número de posições vazias: %d\n", posicoesVazias);
    printf("Fator de carga: %.3f\n", 1000.0 / TABLE_SIZE);

    // Libera memória alocada
    free(hashTable->tabela);
    free(hashTable);

    return 0;
}
