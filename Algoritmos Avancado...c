#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_NOME 64
#define MAX_PISTA 128
#define HASH_SIZE 101   // tamanho da tabela hash (primo razoável)

/* ---------------------------
   Estruturas principais
   ---------------------------*/

// Nó da árvore que representa uma sala da mansão
typedef struct Sala {
    char nome[MAX_NOME];
    char pista[MAX_PISTA]; // pista estática; fica vazia após coletada
    struct Sala *esq;
    struct Sala *dir;
} Sala;

// Nó da BST que guarda pistas coletadas
typedef struct PistaNode {
    char pista[MAX_PISTA];
    struct PistaNode *esq;
    struct PistaNode *dir;
} PistaNode;

// Nó para lista encadeada usada na tabela hash
typedef struct HashNode {
    char *pista;         // chave (string alocada)
    char *suspeito;      // valor (string alocada)
    struct HashNode *prox;
} HashNode;

/* ---------------------------
   Protótipos
   ---------------------------*/
Sala* criarSala(const char *nome, const char *pista);
void explorarSalas(Sala *raiz, PistaNode **bstRoot);
PistaNode* inserirPista(PistaNode *raiz, const char *pista);
void exibirPistasInOrder(PistaNode *raiz);
unsigned long hash_djb2(const char *str);
void inserirNaHash(HashNode *tabela[], const char *pista, const char *suspeito);
const char* encontrarSuspeito(HashNode *tabela[], const char *pista);
int verificarSuspeitoFinal(PistaNode *raiz, HashNode *tabela[], const char *acusado);
void liberarBST(PistaNode *raiz);
void liberarHash(HashNode *tabela[]);

/* ---------------------------
   Implementações
   ---------------------------*/

/*
 * criarSala()
 * Cria dinamicamente um cômodo (Sala) com nome e pista.
 */
Sala* criarSala(const char *nome, const char *pista) {
    Sala *s = (Sala*) malloc(sizeof(Sala));
    if (!s) {
        fprintf(stderr, "Erro: falha de alocacao em criarSala\n");
        exit(1);
    }
    strncpy(s->nome, nome, MAX_NOME-1); s->nome[MAX_NOME-1] = '\0';
    if (pista) {
        strncpy(s->pista, pista, MAX_PISTA-1); s->pista[MAX_PISTA-1] = '\0';
    } else {
        s->pista[0] = '\0';
    }
    s->esq = s->dir = NULL;
    return s;
}

/*
 * inserirPista()
 * Insere uma pista na BST de forma ordenada (alfabética).
 * Duplicações são permitidas (colocadas à direita).
 */
PistaNode* inserirPista(PistaNode *raiz, const char *pista) {
    if (pista == NULL || pista[0] == '\0') return raiz;
    if (raiz == NULL) {
        PistaNode *no = (PistaNode*) malloc(sizeof(PistaNode));
        if (!no) { fprintf(stderr, "Erro alocacao inserirPista\n"); exit(1); }
        strncpy(no->pista, pista, MAX_PISTA-1); no->pista[MAX_PISTA-1] = '\0';
        no->esq = no->dir = NULL;
        return no;
    }
    int cmp = strcmp(pista, raiz->pista);
    if (cmp < 0) raiz->esq = inserirPista(raiz->esq, pista);
    else raiz->dir = inserirPista(raiz->dir, pista); // cmp >= 0 -> direita (permitir igual)
    return raiz;
}

/*
 * exibirPistasInOrder()
 * Imprime a árvore BST em ordem alfabética (A-Z).
 */
void exibirPistasInOrder(PistaNode *raiz) {
    if (!raiz) return;
    exibirPistasInOrder(raiz->esq);
    printf(" - %s\n", raiz->pista);
    exibirPistasInOrder(raiz->dir);
}

/*
 * hash_djb2()
 * Função de hash djb2 retornando índice para a tabela.
 */
unsigned long hash_djb2(const char *str) {
    unsigned long hash = 5381;
    int c;
    while ((c = (unsigned char)*str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    return hash % HASH_SIZE;
}

/*
 * inserirNaHash()
 * Insere a associação pista -> suspeito na tabela hash.
 * Faz strdup das strings para armazenamento.
 */
void inserirNaHash(HashNode *tabela[], const char *pista, const char *suspeito) {
    if (!pista || pista[0] == '\0' || !suspeito) return;
    unsigned long idx = hash_djb2(pista);
    // Evitar duplicar chave: se já existir, substitui o suspeito
    HashNode *cur = tabela[idx];
    while (cur) {
        if (strcmp(cur->pista, pista) == 0) {
            // substitui valor
            free(cur->suspeito);
            cur->suspeito = strdup(suspeito);
            return;
        }
        cur = cur->prox;
    }
    // cria novo nó
    HashNode *novo = (HashNode*) malloc(sizeof(HashNode));
    if (!novo) { fprintf(stderr,"Erro alocar inserirNaHash\n"); exit(1); }
    novo->pista = strdup(pista);
    novo->suspeito = strdup(suspeito);
    novo->prox = tabela[idx];
    tabela[idx] = novo;
}

/*
 * encontrarSuspeito()
 * Retorna o nome do suspeito associado à pista, ou NULL se não houver.
 * (não libera nada; ponteiro pertence à tabela)
 */
const char* encontrarSuspeito(HashNode *tabela[], const char *pista) {
    if (!pista || pista[0] == '\0') return NULL;
    unsigned long idx = hash_djb2(pista);
    HashNode *cur = tabela[idx];
    while (cur) {
        if (strcmp(cur->pista, pista) == 0) return cur->suspeito;
        cur = cur->prox;
    }
    return NULL;
}

/*
 * explorarSalas()
 * Navega pela árvore (interativo) a partir da raiz;
 * ao visitar uma sala coleta sua pista (se existir), adicionando-a à BST.
 * Após coletar, a pista da sala é 'removida' (string vazia) para evitar recolha duplicada.
 */
void explorarSalas(Sala *raiz, PistaNode **bstRoot) {
    Sala *atual = raiz;
    char opcao;
    while (atual) {
        printf("\nVocê entrou em: %s\n", atual->nome);
        if (atual->pista[0]) {
            printf("Pista encontrada: \"%s\"\n", atual->pista);
            *bstRoot = inserirPista(*bstRoot, atual->pista);
            // evita coletar novamente se o jogador voltar aqui
            atual->pista[0] = '\0';
        } else {
            printf("Nenhuma pista nesta sala (ou já coletada).\n");
        }

        // Opções de navegação
        printf("\nEscolha: (e) esquerda  (d) direita  (s) sair\nOpção: ");
        if (scanf(" %c", &opcao) != 1) opcao = 's';
        // consumir restante da linha (se houver)
        while (getchar() != '\n');

        if (opcao == 'e' || opcao == 'E') {
            if (atual->esq) atual = atual->esq;
            else printf("Não há caminho à esquerda.\n");
        } else if (opcao == 'd' || opcao == 'D') {
            if (atual->dir) atual = atual->dir;
            else printf("Não há caminho à direita.\n");
        } else if (opcao == 's' || opcao == 'S') {
            printf("Você encerrou a exploração.\n");
            return;
        } else {
            printf("Opção inválida.\n");
        }
    }
}

/*
 * verificarSuspeitoFinal()
 * Percorre a BST de pistas coletadas e conta quantas pistas apontam para 'acusado';
 * retorna o número de pistas que ligam ao suspeito.
 * Se count >= 2 -> suficientes.
 */
int contarPistasParaSuspeito(PistaNode *raiz, HashNode *tabela[], const char *acusado) {
    if (!raiz) return 0;
    int count = 0;
    const char *sus = encontrarSuspeito(tabela, raiz->pista);
    if (sus && strcmp(sus, acusado) == 0) count = 1;
    count += contarPistasParaSuspeito(raiz->esq, tabela, acusado);
    count += contarPistasParaSuspeito(raiz->dir, tabela, acusado);
    return count;
}

int verificarSuspeitoFinal(PistaNode *raiz, HashNode *tabela[], const char *acusado) {
    if (!acusado || acusado[0] == '\0') return 0;
    return contarPistasParaSuspeito(raiz, tabela, acusado);
}

/* ---------------------------
   Funções utilitárias de liberação
   ---------------------------*/
void liberarBST(PistaNode *raiz) {
    if (!raiz) return;
    liberarBST(raiz->esq);
    liberarBST(raiz->dir);
    free(raiz);
}

void liberarHash(HashNode *tabela[]) {
    for (int i = 0; i < HASH_SIZE; ++i) {
        HashNode *cur = tabela[i];
        while (cur) {
            HashNode *tmp = cur;
            cur = cur->prox;
            free(tmp->pista);
            free(tmp->suspeito);
            free(tmp);
        }
        tabela[i] = NULL;
    }
}

/* ---------------------------
   Main: monta mapa, tabela de suspeitos/pistas e executa jogo
   ---------------------------*/
int main() {
    // --- Montar a mansão (árvore de salas) ---
    // Exemplo de mapa fixo:
    //                    Hall
    //                   /    \
    //              SalaEstar  Biblioteca
    //               /    \         \
    //          Cozinha  Jardim    Porão
    //
    Sala *hall = criarSala("Hall de Entrada", "Pegadas sujas no tapete");
    Sala *salaEstar = criarSala("Sala de Estar", "Uma xícara quebrada com monograma X");
    Sala *biblioteca = criarSala("Biblioteca", "Página arrancada do diário");
    Sala *cozinha = criarSala("Cozinha", "Luvas com manchas vermelhas");
    Sala *jardim = criarSala("Jardim", "Pegadas que saem para o portão");
    Sala *porao = criarSala("Porão", "Lanterna caída com poeira recente");

    // ligações
    hall->esq = salaEstar;
    hall->dir = biblioteca;
    salaEstar->esq = cozinha;
    salaEstar->dir = jardim;
    biblioteca->dir = porao;

    // --- Preparar tabela hash pista -> suspeito ---
    HashNode *tabela[HASH_SIZE];
    for (int i = 0; i < HASH_SIZE; ++i) tabela[i] = NULL;

    // Associações codificadas (regras fixas)
    inserirNaHash(tabela, "Pegadas sujas no tapete", "Sr. Verdes");
    inserirNaHash(tabela, "Uma xícara quebrada com monograma X", "Sra. Xavier");
    inserirNaHash(tabela, "Página arrancada do diário", "Sra. Xavier");
    inserirNaHash(tabela, "Luvas com manchas vermelhas", "Sr. Vermelho");
    inserirNaHash(tabela, "Pegadas que saem para o portão", "Sr. Verdes");
    inserirNaHash(tabela, "Lanterna caída com poeira recente", "Sr. Cinza");
    // (pode-se adicionar mais mapeamentos aqui)

    // --- BST vazio para armazenar pistas coletadas ---
    PistaNode *arvorePistas = NULL;

    printf("=============================================\n");
    printf(" Detective Quest - Modo Mestre (Acusacao)\n");
    printf("=============================================\n");
    printf("Iniciando exploração a partir do Hall de Entrada.\n");

    // explorar a mansão
    explorarSalas(hall, &arvorePistas);

    // Exibir pistas coletadas
    printf("\n=============================================\n");
    printf(" PISTAS COLETADAS (ordenadas alfabeticamente)\n");
    printf("=============================================\n");
    if (!arvorePistas) {
        printf("Nenhuma pista coletada.\n");
    } else {
        exibirPistasInOrder(arvorePistas);
    }

    // Solicitar ao jogador que acuse um suspeito
    char acusado[MAX_NOME];
    printf("\nDigite o nome do suspeito que você deseja acusar (ex: 'Sra. Xavier'): ");
    if (!fgets(acusado, sizeof(acusado), stdin)) acusado[0] = '\0';
    // remover newline
    acusado[strcspn(acusado, "\r\n")] = '\0';
    // trim espaços iniciais/finais (simples)
    char *start = acusado;
    while (*start && isspace((unsigned char)*start)) start++;
    char *end = acusado + strlen(acusado) - 1;
    while (end >= start && isspace((unsigned char)*end)) { *end = '\0'; end--; }

    if (start[0] == '\0') {
        printf("Nenhum suspeito informado. Encerrando.\n");
    } else {
        // contar pistas que apontam para o acusado
        int cont = verificarSuspeitoFinal(arvorePistas, tabela, start);
        printf("\nO jogador acusou: \"%s\"\n", start);
        printf("Pistas que apontam para %s: %d\n", start, cont);
        if (cont >= 2) {
            printf("\n>>> Veredito: EVIDENCIAS SUFICIENTES. %s é considerado(a) CULPADO(A).\n", start);
        } else {
            printf("\n>>> Veredito: EVIDENCIAS INSUFICIENTES. %s NÃO PODE SER CONDENADO(A).\n", start);
        }
    }

    // liberar memoria
    liberarBST(arvorePistas);
    liberarHash(tabela);
    // liberar salas (não estritamente necessário ao final, mas bom estilo)
    free(hall); free(salaEstar); free(biblioteca); free(cozinha); free(jardim); free(porao);

    printf("\nObrigado por jogar Detective Quest (modo mestre)!\n");
    return 0;
}