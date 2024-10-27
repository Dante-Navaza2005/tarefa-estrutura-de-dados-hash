#include <stdio.h>
#include <stdlib.h>

#define TABLE_SIZE 2003 // Número primo para a tabela

// Estrutura de entrada da tabela hash
typedef struct {
    unsigned long cpf;
    int ocupado; // 0 para livre, 1 para ocupado
} HashEntry;

typedef struct {
    HashEntry *tabela;
    int tamanho;
} HashTable;

// Função hash usando método da divisão
int hashFunc(unsigned long cpf, int tamanho) {
    return cpf % tamanho;
}

HashTable *inicializaTabela(int tamanho) {
    HashTable *hashTable = (HashTable *)malloc(sizeof(HashTable));
    hashTable->tabela = (HashEntry *)calloc(tamanho, sizeof(HashEntry));
    hashTable->tamanho = tamanho;
    return hashTable;
}

void insereCPF(HashTable *hashTable, unsigned long cpf) {
    int index = hashFunc(cpf, hashTable->tamanho);
    int tentativas = 0;

    // Sondagem linear para encontrar um espaço vazio
    while (hashTable->tabela[index].ocupado && tentativas < hashTable->tamanho) {
        index = (index + 1) % hashTable->tamanho;
        tentativas++;
    }

    if (tentativas < hashTable->tamanho) {
        hashTable->tabela[index].cpf = cpf;
        hashTable->tabela[index].ocupado = 1;
    } else {
        printf("Tabela hash está cheia. Não foi possível inserir o CPF %lu\n", cpf);
    }
}

int contaColisoes(HashTable *hashTable) {
    int colisoes = 0;
    for (int i = 0; i < hashTable->tamanho; i++) {
        if (hashTable->tabela[i].ocupado) {
            int index = hashFunc(hashTable->tabela[i].cpf, hashTable->tamanho);
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
        if (!hashTable->tabela[i].ocupado) {
            vazias++;
        }
    }
    return vazias;
}

void liberaTabela(HashTable *hashTable) {
    free(hashTable->tabela);
    free(hashTable);
}

int main() {
    HashTable *hashTable = inicializaTabela(TABLE_SIZE);

    FILE *arquivo = fopen("cpfs_gerados.txt", "r");
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
        insereCPF(hashTable, cpf);
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

    liberaTabela(hashTable);

    return 0;
}
