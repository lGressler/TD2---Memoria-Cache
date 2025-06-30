int main(int argc, char *argv[])
{

    int politica_escrita = atoi(argv[1]);
    int tamanho_linha = atoi(argv[2]);
    int numero_linhas = atoi(argv[3]);
    int associatividade = atoi(argv[4]);
    int tempo_hit = atoi(argv[5]);
    char *politica_substituicao = argv[6];
    int tempo_memoria = atoi(argv[7]);
    char *arquivo_entrada = argv[8];
    char *arquivo_saida = argv[9];

    simular_cache(politica_escrita, tamanho_linha, numero_linhas, associatividade,
                  tempo_hit, politica_substituicao, tempo_memoria, arquivo_entrada, arquivo_saida);

    printf("Simulação finalizada. Resultados em %s\n", arquivo_saida);
    return 0;
}