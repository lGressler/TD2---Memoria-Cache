#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

typedef struct
{
    int rotulo;
    int sujo;
    int valido;
    int contador_lru;
} LinhaCache;

typedef struct
{
    LinhaCache *linhas;
} Conjunto;

int hex_para_inteiro(char *hex)
{
    int valor;
    sscanf(hex, "%x", &valor);
    return valor;
}

int encontrar_lru(Conjunto conjunto, int associatividade)
{
    int indice = 0;
    for (int i = 1; i < associatividade; i++)
    {
        if (conjunto.linhas[i].contador_lru < conjunto.linhas[indice].contador_lru)
        {
            indice = i;
        }
    }
    return indice;
}

int escolher_aleatorio(int associatividade)
{
    return rand() % associatividade;
}

void simular_cache(int politica_escrita, int tamanho_linha, int numero_linhas,
                   int associatividade, int tempo_hit, char *politica_substituicao,
                   int tempo_memoria, char *arquivo_entrada, char *arquivo_saida)
{

    srand(time(NULL));
    FILE *entrada = fopen(arquivo_entrada, "r");
    if (!entrada)
    {
        perror("Erro ao abrir arquivo de entrada");
        exit(1);
    }

    FILE *saida = fopen(arquivo_saida, "w");
    if (!saida)
    {
        perror("Erro ao criar arquivo de saída");
        exit(1);
    }

    int numero_conjuntos = numero_linhas / associatividade;
    Conjunto *cache = malloc(numero_conjuntos * sizeof(Conjunto));
    for (int i = 0; i < numero_conjuntos; i++)
    {
        cache[i].linhas = malloc(associatividade * sizeof(LinhaCache));
        for (int j = 0; j < associatividade; j++)
        {
            cache[i].linhas[j].valido = 0;
            cache[i].linhas[j].sujo = 0;
            cache[i].linhas[j].contador_lru = 0;
        }
    }

    int leituras = 0, escritas = 0;
    int acertos_leitura = 0, acertos_escrita = 0;
    int acessos_memoria_leitura = 0, acessos_memoria_escrita = 0;
    int tempo_total = 0;
    int tempo_global = 0;

    char linha[100];
    while (fgets(linha, sizeof(linha), entrada))
    {
        char endereco_hex[20], operacao;
        sscanf(linha, "%s %c", endereco_hex, &operacao);

        int endereco = hex_para_inteiro(endereco_hex);
        int bits_offset = (int)log2(tamanho_linha);
        int bits_indice = (int)log2(numero_conjuntos);
        int indice = (endereco >> bits_offset) & ((1 << bits_indice) - 1);
        int rotulo = endereco >> (bits_offset + bits_indice);

        Conjunto *conjunto = &cache[indice];
        int houve_acerto = 0, linha_vazia = -1;

        for (int i = 0; i < associatividade; i++)
        {
            if (conjunto->linhas[i].valido && conjunto->linhas[i].rotulo == rotulo)
            {
                houve_acerto = 1;
                if (strcmp(politica_substituicao, "LRU") == 0)
                    conjunto->linhas[i].contador_lru = ++tempo_global;

                if (operacao == 'W')
                {
                    if (politica_escrita == 1)
                    {
                        conjunto->linhas[i].sujo = 1;
                    }
                    else
                    {
                        acessos_memoria_escrita++;
                    }
                    acertos_escrita++;
                }
                else
                {
                    acertos_leitura++;
                }

                tempo_total += tempo_hit;
                break;
            }
            else if (!conjunto->linhas[i].valido && linha_vazia == -1)
            {
                linha_vazia = i;
            }
        }

        if (!houve_acerto)
        {
            int indice_substituir = (linha_vazia != -1) ? linha_vazia : (strcmp(politica_substituicao, "LRU") == 0) ? encontrar_lru(*conjunto, associatividade)
                                                                                                                    : escolher_aleatorio(associatividade);

            LinhaCache *linha_cache = &conjunto->linhas[indice_substituir];

            if (politica_escrita == 1 && linha_cache->valido && linha_cache->sujo)
            {
                acessos_memoria_escrita++;
            }

            linha_cache->rotulo = rotulo;
            linha_cache->valido = 1;
            linha_cache->contador_lru = ++tempo_global;
            linha_cache->sujo = 0;

            acessos_memoria_leitura++;
            tempo_total += tempo_hit + tempo_memoria;

            if (operacao == 'W')
            {
                if (politica_escrita == 1)
                {
                    linha_cache->sujo = 1;
                }
                else
                {
                    acessos_memoria_escrita++;
                }
                escritas++;
            }
            else
            {
                leituras++;
            }
        }
        else
        {
            if (operacao == 'W')
                escritas++;
            else
                leituras++;
        }
    }

    int acessos_totais = leituras + escritas;
    int acertos_totais = acertos_leitura + acertos_escrita;
    double taxa_leitura = (double)acertos_leitura / leituras;
    double taxa_escrita = (double)acertos_escrita / escritas;
    double taxa_total = (double)acertos_totais / acessos_totais;
    double tempo_medio = (double)tempo_total / acessos_totais;

    // Saída de resultados
    fprintf(saida, "Parâmetros da Simulação:\n");
    fprintf(saida, "Política de Escrita: %s\n", politica_escrita == 0 ? "write-through" : "write-back");
    fprintf(saida, "Tamanho do Bloco: %d bytes\n", tamanho_linha);
    fprintf(saida, "Número de Linhas: %d\n", numero_linhas);
    fprintf(saida, "Associatividade: %d blocos por conjunto\n", associatividade);
    fprintf(saida, "Tempo de Acesso (hit): %d ns\n", tempo_hit);
    fprintf(saida, "Política de Substituição: %s\n", politica_substituicao);
    fprintf(saida, "Tempo de Acesso à Memória Principal: %d ns\n\n", tempo_memoria);

    fprintf(saida, "Total de Leituras: %d\n", leituras);
    fprintf(saida, "Total de Escritas: %d\n", escritas);
    fprintf(saida, "Total de Acessos: %d\n", acessos_totais);
    fprintf(saida, "Leituras na Memória Principal: %d\n", acessos_memoria_leitura);
    fprintf(saida, "Escritas na Memória Principal: %d\n", acessos_memoria_escrita);

    fprintf(saida, "Taxa de Acerto (Leitura): %.4f (%d)\n", taxa_leitura, acertos_leitura);
    fprintf(saida, "Taxa de Acerto (Escrita): %.4f (%d)\n", taxa_escrita, acertos_escrita);
    fprintf(saida, "Taxa de Acerto Total: %.4f (%d)\n", taxa_total, acertos_totais);
    fprintf(saida, "Tempo Médio de Acesso: %.4f ns\n", tempo_medio);

    for (int i = 0; i < numero_conjuntos; i++)
    {
        free(cache[i].linhas);
    }
    free(cache);
    fclose(entrada);
    fclose(saida);
}
