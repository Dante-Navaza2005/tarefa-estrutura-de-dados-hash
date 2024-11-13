#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define TAMANHO_TABELA 1231 // Número primo perto de 1200 para tabela

typedef struct {
    unsigned long cpf;
    int ocupado; // 0 = vazio, 1 = ocupado, 2 = removido
} Hash_Entrada;

typedef struct {
    Hash_Entrada *tabela;
    int tamanho;
} HashTabela;

// Função hash usando o método da divisão
int hashFuncDivisao(unsigned long cpf, int tamanho) {
    return cpf % tamanho; // Tamanho tem que ser um número primo
}

// Segunda função hash de divisao para dispersão dupla (para endereçamento aberto)
int hashFunc2(unsigned long cpf, int tamanho) {
    return 1 + (cpf % (tamanho - 1));
}

HashTabela *inicializaTabela(int tamanho) {
    HashTabela *hash_tabela = (HashTabela *)malloc(sizeof(HashTabela));
    hash_tabela->tabela = (Hash_Entrada *)malloc(tamanho * sizeof(Hash_Entrada));
    hash_tabela->tamanho = tamanho;
    for (int i = 0; i < tamanho; i++) {
        hash_tabela->tabela[i].ocupado = 0; // Inicializa todas as entradas como vazias
        hash_tabela->tabela[i].cpf = 0; // Inicializa o CPF como zero
    }
    return hash_tabela;
}

// Função para inserir um CPF na tabela hash
void insereCPF(HashTabela *hash_tabela, unsigned long cpf) {
    int tentativa = 0;
    int index = 0;

    do {
        // Calcula o índice usando hash principal e dispersão dupla
        index = (hashFuncDivisao(cpf, hash_tabela->tamanho) + tentativa * hashFunc2(cpf, hash_tabela->tamanho)) % hash_tabela->tamanho;
        
        if (hash_tabela->tabela[index].ocupado == 0) {
            // Insere o CPF na posição encontrada
            hash_tabela->tabela[index].cpf = cpf;
            hash_tabela->tabela[index].ocupado = 1;
            return;
        }
        tentativa++;
    } while (tentativa < hash_tabela->tamanho);
}

int contaColisoes(HashTabela *hash_tabela) {
    int colisoes = 0;
    for (int i = 0; i < hash_tabela->tamanho; i++) 
    {
        if (hash_tabela->tabela[i].ocupado == 1) 
        {
            int index = hashFuncDivisao(hash_tabela->tabela[i].cpf, hash_tabela->tamanho); //pega a posicao daquele cpf
            if (index != i) 
            {
                colisoes++;
            }
        }
    }
    return colisoes;
}

int contaPosicoesVazias(HashTabela *hash_tabela) 
{
    int vazias = 0;
    for (int i = 0; i < hash_tabela->tamanho; i++) 
    {
        if (hash_tabela->tabela[i].ocupado == 0) 
        {
            vazias++;
        }
    }
    return vazias;
}

int main() {
    HashTabela *hash_tabela = inicializaTabela(TAMANHO_TABELA);

    FILE *arquivo = fopen("cpfs.txt", "r");
    if (arquivo == NULL) 
    {
        printf("Erro ao abrir o arquivo");
        return 1;
    }

    FILE *saida = fopen("colisoes.csv", "w");
    if (saida == NULL) 
    {
        printf("Erro ao criar o arquivo CSV");
        return 1;
    }

    //  cabeçalho do CSV
    fprintf(saida, "Chaves Inseridas;Colisoes;Hash Acessos;Log(n)-Acessos\n");

    unsigned long cpf;
    int insercoes = 0;
    double logn_acessos_potencia = 0; //a potencia do dois para ser usada na multiplicacao do logn

    while (fscanf(arquivo, "%lu", &cpf) != EOF && insercoes < 1000) 
    {
        insereCPF(hash_tabela, cpf);
        insercoes++;

        if (insercoes % 100 == 0) {
            int colisoes = contaColisoes(hash_tabela);
            int hash_acessos = colisoes + 1000;
            double logn_acessos = pow(2,logn_acessos_potencia) * (logn_acessos_potencia + 1);
            // Salvando as colunas no CSV
            fprintf(saida, "%d;%d;%d;%d\n", insercoes, colisoes, hash_acessos, (int)logn_acessos);
            logn_acessos_potencia++;
        }
    }

    fclose(arquivo);
    fclose(saida);

    int colisoesTotais = contaColisoes(hash_tabela);
    int posicoesVazias = contaPosicoesVazias(hash_tabela);

    printf("Número de colisões apos 1000 insercoes: %d\n", colisoesTotais);
    printf("Número de posições vazias apos 1000 insercoes: %d\n", posicoesVazias);
    printf("Fator de carga: %.3f\n", 1000.0 / TAMANHO_TABELA);

    // Libera memória alocada
    free(hash_tabela->tabela);
    free(hash_tabela);

    return 0;
}
