#include <stdio.h>
#include <stdlib.h>
#include "cache.h"

int main()
{

    int politica_escrita = 0;             // 0 = write-through, 1 = write-back
    int tamanho_linha = 64;               // em bytes
    int numero_linhas = 4096;             // número total de linhas da cache
    int associatividade = 2;              // número de linhas por conjunto
    int tempo_hit = 5;                    // tempo de acesso à cache (ns)
    char politica_substituicao[] = "LRU"; // "LRU" ou "RAND"
    int tempo_memoria = 70;               // tempo de acesso à memória principal (ns)

    char arquivo_entrada[] = "teste.cache";
    char arquivo_saida[] = "resultado.txt";

    simular_cache(politica_escrita, tamanho_linha, numero_linhas, associatividade,
                  tempo_hit, politica_substituicao, tempo_memoria, arquivo_entrada, arquivo_saida);

    printf("Simulação finalizada. Resultados em %s\n", arquivo_saida);
    return 0;
}
