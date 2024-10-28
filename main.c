#include <stdio.h>
#include <stdlib.h>

#define TABLE_SIZE 2003 // Número primo para tabela

typedef struct {
    unsigned long cpf;
    int ocupado; // 0 = vazio, 1 = ocupado, 2 = removido
} HashEntry;

typedef struct {
    HashEntry *tabela;
    int tamanho;
} HashTable;

// Função hash usando método da multiplicação por uma constante
int hashFunc(unsigned long cpf, int tamanho) {
    unsigned long k = 2654435761; // Constante para dispersão uniforme
    return (cpf * k) % tamanho;
}

// Função para tentativa quadrática
int hashFuncQuadratica(unsigned long cpf, int i, int tamanho) {
    return (hashFunc(cpf, tamanho) + i * i) % tamanho;
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
        index = hashFuncQuadratica(cpf, i, hashTable->tamanho);
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
        if (hashTable->tabela[i].ocupado == 0) {
            vazias++;
        }
    }
    return vazias;
}

int validaCPF(unsigned long cpf) {
    // Valida o comprimento do CPF (no caso, assume 11 dígitos)
    return (cpf >= 10000000000 && cpf <= 99999999999);
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
    fprintf(saida, "CPFs inseridos, Colisoes\n");

    unsigned long cpf;
    int insercoes = 0;

    while (fscanf(arquivo, "%lu", &cpf) != EOF && insercoes < 1000) {
        if (!validaCPF(cpf)) {
            printf("Aviso: CPF inválido ignorado: %lu\n", cpf);
            continue;
        }
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

    // Libera memória alocada
    free(hashTable->tabela);
    free(hashTable);

    return 0;
}
