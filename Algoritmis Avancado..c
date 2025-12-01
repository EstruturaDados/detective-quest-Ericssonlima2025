#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*---------------------------------------------------------
 * Estruturas de dados
 *---------------------------------------------------------*/

// Nó da árvore da mansão
typedef struct sala {
    char nome[50];
    char pista[100];     // Pode ser vazio ""
    struct sala *esq;
    struct sala *dir;
} Sala;

// Nó da árvore de pistas (BST)
typedef struct pistaNode {
    char pista[100];
    struct pistaNode *esq;
    struct pistaNode *dir;
} PistaNode;


/*---------------------------------------------------------
 * Função criarSala()
 * Cria uma sala dinamicamente com nome e pista opcional.
 *---------------------------------------------------------*/
Sala* criarSala(char nome[], char pista[]) {
    Sala *nova = (Sala*) malloc(sizeof(Sala));
    if (!nova) {
        printf("Erro ao alocar memoria!\n");
        exit(1);
    }
    strcpy(nova->nome, nome);
    strcpy(nova->pista, pista);
    nova->esq = NULL;
    nova->dir = NULL;
    return nova;
}

/*---------------------------------------------------------
 * Função criarPistaNode()
 * Cria um nó da BST com a pista encontrada.
 *---------------------------------------------------------*/
PistaNode* criarPistaNode(char pista[]) {
    PistaNode *novo = (PistaNode*) malloc(sizeof(PistaNode));
    strcpy(novo->pista, pista);
    novo->esq = NULL;
    novo->dir = NULL;
    return novo;
}

/*---------------------------------------------------------
 * Função inserirPista()
 * Insere uma nova pista na árvore BST em ordem alfabética.
 *---------------------------------------------------------*/
PistaNode* inserirPista(PistaNode *raiz, char pista[]) {
    if (raiz == NULL) {
        return criarPistaNode(pista);
    }

    if (strcmp(pista, raiz->pista) < 0) {
        raiz->esq = inserirPista(raiz->esq, pista);
    }
    else if (strcmp(pista, raiz->pista) > 0) {
        raiz->dir = inserirPista(raiz->dir, pista);
    }
    // Igual – não insere duplicada
    return raiz;
}

/*---------------------------------------------------------
 * Função exibirPistas()
 * Mostra a árvore BST em ordem alfabética.
 *---------------------------------------------------------*/
void exibirPistas(PistaNode *raiz) {
    if (raiz == NULL) return;
    exibirPistas(raiz->esq);
    printf("- %s\n", raiz->pista);
    exibirPistas(raiz->dir);
}

/*---------------------------------------------------------
 * explorarSalasComPistas()
 * Permite navegar entre salas e coleta as pistas no caminho.
 *---------------------------------------------------------*/
void explorarSalasComPistas(Sala *atual, PistaNode **arvorePistas) {
    char opcao;

    while (atual != NULL) {
        printf("\n📍 Você está em: %s\n", atual->nome);

        // Se existir pista, coleta
        if (strlen(atual->pista) > 0) {
            printf("🔎 Pista encontrada: \"%s\"\n", atual->pista);
            *arvorePistas = inserirPista(*arvorePistas, atual->pista);
        } else {
            printf("Nenhuma pista nesta sala.\n");
        }

        // Menu de navegação
        printf("\nEscolha o próximo caminho:\n");
        printf("  (e) Ir para a esquerda\n");
        printf("  (d) Ir para a direita\n");
        printf("  (s) Sair da exploração\n");
        printf("Opção: ");
        scanf(" %c", &opcao);

        if (opcao == 'e' || opcao == 'E') {
            if (atual->esq != NULL)
                atual = atual->esq;
            else
                printf("Não há sala à esquerda.\n");
        }
        else if (opcao == 'd' || opcao == 'D') {
            if (atual->dir != NULL)
                atual = atual->dir;
            else
                printf("Não há sala à direita.\n");
        }
        else if (opcao == 's' || opcao == 'S') {
            printf("Exploração encerrada pelo detetive.\n");
            return;
        }
        else {
            printf("Opção inválida!\n");
        }
    }
}

/*---------------------------------------------------------
 * main()
 * Monta a mansão, inicia exploração e mostra pistas finais.
 *---------------------------------------------------------*/
int main() {
    // Construção do mapa da mansão (fixo)
    Sala *hall        = criarSala("Hall de Entrada", "Pegadas de sapato sujas");
    Sala *salaEstar   = criarSala("Sala de Estar", "Uma xícara quebrada");
    Sala *biblioteca  = criarSala("Biblioteca", "Um livro rasgado");
    Sala *cozinha     = criarSala("Cozinha", "Uma luva esquecida");
    Sala *jardim      = criarSala("Jardim", "");
    Sala *porao       = criarSala("Porão", "Uma lanterna caída");

    // Ligações da árvore
    hall->esq = salaEstar;
    hall->dir = biblioteca;

    salaEstar->esq = cozinha;
    salaEstar->dir = jardim;

    biblioteca->dir = porao;

    // BST de pistas inicialmente vazia
    PistaNode *arvorePistas = NULL;

    printf("=======================================\n");
    printf("   Detective Quest - Coleta de Pistas  \n");
    printf("=======================================\n");

    explorarSalasComPistas(hall, &arvorePistas);

    // Exibir todas as pistas coletadas
    printf("\n=======================================\n");
    printf("        PISTAS COLETADAS (A-Z)         \n");
    printf("=======================================\n");

    if (arvorePistas == NULL) {
        printf("Nenhuma pista coletada.\n");
    } else {
        exibirPistas(arvorePistas);
    }

    return 0;
}