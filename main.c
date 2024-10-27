#include <stdio.h>
#include <stdlib.h>

#define TABLE_SIZE 4093 // Número primo maior para reduzir colisões

// Estrutura para lista encadeada (encadeamento exterior)
typedef struct Nodo {
    unsigned long cpf;
    struct Nodo *proximo;
} Nodo;

typedef struct {
    Nodo *lista; // Ponteiro para o início da lista de colisões
} HashEntry;

typedef struct {
    HashEntry *tabela;
    int tamanho;
} HashTable;

// Função hash aprimorada com operações de mistura de bits
int hashFunc(unsigned long cpf, int tamanho) {
    cpf = ((cpf >> 16) ^ cpf) * 0x45d9f3b;
    cpf = ((cpf >> 16) ^ cpf) * 0x45d9f3b;
    cpf = (cpf >> 16) ^ cpf;
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
    Nodo *novoNodo = (Nodo *)malloc(sizeof(Nodo));
    novoNodo->cpf = cpf;
    novoNodo->proximo = hashTable->tabela[index].lista;
    hashTable->tabela[index].lista = novoNodo;
}

int contaColisoes(HashTable *hashTable) {
    int colisoes = 0;
    for (int i = 0; i < hashTable->tamanho; i++) {
        Nodo *atual = hashTable->tabela[i].lista;
        int count = 0;
        while (atual != NULL) {
            count++;
            atual = atual->proximo;
        }
        if (count > 1) {
            colisoes += (count - 1); // Contabiliza colisões
        }
    }
    return colisoes;
}

int contaPosicoesVazias(HashTable *hashTable) {
    int vazias = 0;
    for (int i = 0; i < hashTable->tamanho; i++) {
        if (hashTable->tabela[i].lista == NULL) {
            vazias++;
        }
    }
    return vazias;
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
    int colisoesPorInsercao[10] = {0};

    while (fscanf(arquivo, "%lu", &cpf) != EOF && insercoes < 1000) {
        insereCPF(hashTable, cpf);
        insercoes++;

        if (insercoes % 100 == 0) {
            int colisoes = contaColisoes(hashTable);
            colisoesPorInsercao[insercoes / 100 - 1] = colisoes;
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
    for (int i = 0; i < hashTable->tamanho; i++) {
        Nodo *atual = hashTable->tabela[i].lista;
        while (atual != NULL) {
            Nodo *temp = atual;
            atual = atual->proximo;
            free(temp);
        }
    }
    free(hashTable->tabela);
    free(hashTable);

    return 0;
}
