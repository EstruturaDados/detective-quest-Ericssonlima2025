#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Struct que representa cada sala da mansão
typedef struct sala {
    char nome[50];
    struct sala *esq;
    struct sala *dir;
} Sala;

/*
 * criarSala()
 * Cria dinamicamente uma sala com nome e sem filhos.
 */
Sala* criarSala(char nome[]) {
    Sala *nova = (Sala*) malloc(sizeof(Sala));
    if (!nova) {
        printf("Erro ao alocar memoria!\n");
        exit(1);
    }

    strcpy(nova->nome, nome);
    nova->esq = NULL;
    nova->dir = NULL;
    return nova;
}

/*
 * explorarSalas()
 * Permite ao jogador navegar pelo mapa da mansão interativamente.
 */
void explorarSalas(Sala *atual) {
    char opcao;

    while (atual != NULL) {
        printf("\nVocê está em: %s\n", atual->nome);

        // Sala sem caminhos → fim da exploração
        if (atual->esq == NULL && atual->dir == NULL) {
            printf("Não há mais caminhos a seguir. Exploração encerrada.\n");
            return;
        }

        printf("Escolha um caminho:\n");
        printf("  (e) Esquerda\n");
        printf("  (d) Direita\n");
        printf("  (s) Sair da exploração\n");
        printf("Opção: ");
        scanf(" %c", &opcao);

        if (opcao == 'e' || opcao == 'E') {
            if (atual->esq != NULL) {
                atual = atual->esq;
            } else {
                printf("Não há sala à esquerda!\n");
            }
        }
        else if (opcao == 'd' || opcao == 'D') {
            if (atual->dir != NULL) {
                atual = atual->dir;
            } else {
                printf("Não há sala à direita!\n");
            }
        }
        else if (opcao == 's' || opcao == 'S') {
            printf("Exploração encerrada pelo jogador.\n");
            return;
        }
        else {
            printf("Opção inválida!\n");
        }
    }
}

/*
 * main()
 * Monta a mansão de forma fixa usando criarSala()
 * e inicia a exploração.
 */
int main() {
    // Criando as salas manualmente (árvore fixa)
    Sala *hall        = criarSala("Hall de Entrada");
    Sala *salaEstar   = criarSala("Sala de Estar");
    Sala *biblioteca  = criarSala("Biblioteca");
    Sala *cozinha     = criarSala("Cozinha");
    Sala *jardim      = criarSala("Jardim");
    Sala *porao       = criarSala("Porão");

    // Montagem da árvore (mapa da mansão)
    hall->esq = salaEstar;
    hall->dir = biblioteca;

    salaEstar->esq = cozinha;
    salaEstar->dir = jardim;

    biblioteca->dir = porao;

    // Início do jogo
    printf("Bem-vindo(a) à mansão Detective Quest!\n");
    printf("Explore os caminhos e descubra seus mistérios...\n");

    explorarSalas(hall);

    return 0;
}