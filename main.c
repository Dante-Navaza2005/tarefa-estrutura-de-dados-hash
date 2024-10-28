#include <stdio.h>
#include <stdlib.h>
#include <math.h> // Biblioteca para a função log

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

// Função para buscar um CPF específico na tabela hash e contar colisões durante a busca
void buscaCPF(HashTable *hashTable, unsigned long cpf) {
    int index, i = 0, colisoes = 0;
    do {
        // Usando a função de divisão como hash principal e dispersão dupla para tratar colisões
        index = (hashFuncDivisao(cpf, hashTable->tamanho) + i * hashFunc2(cpf, hashTable->tamanho)) % hashTable->tamanho;
        
        if (hashTable->tabela[index].ocupado == 0) {
            // Caso encontre uma posição vazia durante a busca
            printf("Posição %d está vazia.\n", index);
        } else if (hashTable->tabela[index].ocupado == 1 && hashTable->tabela[index].cpf == cpf) {
            printf("CPF %lu encontrado na posição %d com %d colisões durante a busca.\n", cpf, index, colisoes);
            return;
        }

        i++;
        colisoes++;
    } while (hashTable->tabela[index].ocupado != 0 && i < hashTable->tamanho);

    printf("CPF %lu não encontrado na tabela. Número de colisões durante a busca: %d\n", cpf, colisoes);
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
    fprintf(saida, "Chaves Inseridas,Colisoes,Log(N)\n");

    unsigned long cpf;
    int insercoes = 0;

    while (fscanf(arquivo, "%lu", &cpf) != EOF && insercoes < 1000) {
        insereCPF(hashTable, cpf); // Usando o método da Divisão com endereçamento aberto e dispersão dupla
        insercoes++;

        if (insercoes % 100 == 0) {
            int colisoes = contaColisoes(hashTable);
            double logN = log2(insercoes); // Calcula o logaritmo natural de N (ou seja, ln(N))
            // Escreve o número de inserções, de colisões e o valor de log(N) no CSV
            fprintf(saida, "%d,%d,%.3f\n", insercoes, colisoes, logN);
        }
    }

    fclose(arquivo);
    fclose(saida);

    int colisoesTotais = contaColisoes(hashTable);
    int posicoesVazias = contaPosicoesVazias(hashTable);

    printf("Número de colisões: %d\n", colisoesTotais);
    printf("Número de posições vazias: %d\n", posicoesVazias);
    printf("Fator de carga: %.3f\n", 1000.0 / TABLE_SIZE);

    // Teste de busca de um CPF específico
    printf("\nTeste de busca de CPF:\n");
    buscaCPF(hashTable, 69997097939); // Insira o CPF que deseja buscar

    // Libera memória alocada
    free(hashTable->tabela);
    free(hashTable);

    return 0;
}
