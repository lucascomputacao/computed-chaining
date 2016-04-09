#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define TAMANHO_ARQUIVO 11
#define MAXIMO_VALOR_APONTADOR 10
#define ARQUIVO "hash_cc.bin"

FILE *arquivo_hashing;

// Estrutura de registro

typedef struct REGISTRO {
    int chave;
    char nome[20];
    int idade;
    //guarda índice para o pai para acertar apontador, caso tenha que ser relocado
    int index_father;
    // apontador para salto
    int apontador;
} registro;

// Tabela Hashing
registro tabela_hash[TAMANHO_ARQUIVO], tabela_h, tabela_jump;


// Estrutura da lista encadeada

struct Node {
    int chave;
    char nome[20];
    int idade;
    // ponteiro para próximo da lista
    struct Node *prox;
};
// definindo o tipo
typedef struct Node node;

// PROTÓTIPOS
void reinserir_chave(int chave, char nome[], int idade);
void inserir_chave(int chave, char nome[], int idade);
void limpa_apontador_pai(int pai);
void limpa_registro(int indice);
void media_acessos();
void remover(int chave);
void limpa_registro(int indice);
void remove_registro(int indice);

/*
 * Função de Hashing h1
 */
int h1(int chave) {
    //    fprintf(stderr, "h1 = %d\n", chave % TAMANHO_ARQUIVO);
    return (chave % TAMANHO_ARQUIVO);
}

/*
 * Função de Hashing h2
 */
int h2(int chave) {
    //    double chao;
    //    double chao;    
    int chao = (int) (chave / TAMANHO_ARQUIVO);
    //    fprintf(stderr, "Chão: %d\n", chao);
    //    chao = floor(chave / TAMANHO_ARQUIVO);
    // max{chave/TAMANHO_ARQUIVO, 1}
    if (chao > 1)
        return chao;
    else
        return 1;
}

/**
 * Inicia lista encadeada
 * @param LISTA
 */
void inicia(node *LISTA) {
    LISTA->prox = NULL;
}

/**
 * Aloca um novo nó
 * @return node
 */
node *aloca() {
    node *novo = (node *) malloc(sizeof (node));
    if (!novo) {
        //        printf("Sem memoria disponivel!\n");
        exit(1);
    } else {
        return novo;
    }
}

/**
 *  Verifica se a lista está vazia
 * @param LISTA
 * @return 1 se vazia
 * @return 0 se NÃO vazia
 */
int vazia(node *LISTA) {
    if (LISTA->prox == NULL)
        return 1;
    else
        return 0;
}

/**
 * Insere no final da lista encadeada
 * @param LISTA
 * @param chave
 * @param nome
 * @param idade
 */
void insereFim(node *LISTA, int chave, char nome[20], int idade) {
    node *novo = aloca();

    novo->chave = chave;
    strcpy(novo->nome, nome);
    novo->idade = idade;
    novo->prox = NULL;

    if (vazia(LISTA))
        LISTA->prox = novo;
    else {
        node *tmp = LISTA->prox;
        // loop até o final da lista
        while (tmp->prox != NULL)
            tmp = tmp->prox;
        tmp->prox = novo;
    }
}

/**
 * exibe conteúdo da lista 
 * @param LISTA
 */
void exibe(node *LISTA) {
    if (vazia(LISTA)) {
        //        printf("Lista vazia!\n\n");
        return;
    }
    node *tmp;
    tmp = LISTA->prox;
    //    printf("Lista:");
    while (tmp != NULL) {
        //        printf("Chave: %d %s %d\n", tmp->chave, tmp->nome, tmp->idade);
        tmp = tmp->prox;
    }
}

/**
 * Retorna elemento da lista pesquisado pela chave
 * @param chave
 * @return 
 */
node *elem_lista(node *LISTA, int chave) {
    if (vazia(LISTA)) {
        printf("Lista vazia!\n\n");
    }
    node *tmp;
    tmp = LISTA->prox;
    //    printf("Lista:");
    while (tmp != NULL) {
        if (tmp->chave == chave)
            return tmp;
        tmp = tmp->prox;
    }
    return NULL;
}

/**
 * Reinsere os elementos da lista
 * @param LISTA
 */
void reinsere_lista(node *LISTA) {
    if (vazia(LISTA)) {
        //        printf("Lista vazia!\n\n");
    }
    node *tmp;
    tmp = LISTA->prox;
    //    printf("Lista:");
    while (tmp != NULL) {
        reinserir_chave(tmp->chave, tmp->nome, tmp->idade);
        tmp = tmp->prox;
    }
}


// Inicializa o registro

void inicializar() {
    // inicializa chave
    tabela_h.chave = -1;
    // inicializa nome
    strcpy(tabela_h.nome, "");
    //    strlcpy(tabela_h.nome, "", 20);
    tabela_h.index_father = -1;
    // inicializa idade
    tabela_h.idade = -1;
    // inicializar apontador
    tabela_h.apontador = -1;
}

// Cria o arquivo 

void criar_arquivo() {
    int i;

    // arquivo binário
    if ((arquivo_hashing = fopen(ARQUIVO, "wb")) == NULL) {
        //        fprintf(stderr, "\nErro ao abrir arquivo!");
        //        printf("\nErro ao abrir arquivo!");
        return;
    }

    //    fprintf(stderr, "arquivo criado com sucesso!\n");

    // coloca o arquivo no inicio antes de começar a inserção dos registros
    rewind(arquivo_hashing);

    // Cria tabela hashing e insere no arquivo 
    for (i = 0; i < TAMANHO_ARQUIVO; i++) {
        // acerta posição para inserção do registro no arquivo
        fseek(arquivo_hashing, ((i + 1) * sizeof (struct REGISTRO)), SEEK_SET);

        // escreve os registros no arquivo
        if (fwrite(&tabela_h, sizeof (struct REGISTRO), 1, arquivo_hashing) != 1) {
            //            fprintf(stderr, "\nErro na escrita do arquivo!");
            break;
        }
    }

    // fecha o arquivo 
    fclose(arquivo_hashing);
}

// Imprimir arquivo

void imprimir() {
    int i;
    // abre o arquivo para leitura
    if ((arquivo_hashing = fopen(ARQUIVO, "rb")) == NULL) {
        printf("\nErro ao abrir arquivo!");
        return;
    }
    // posiciona a leitura no início do arquivo
    rewind(arquivo_hashing);
    fseek(arquivo_hashing, (sizeof (struct REGISTRO)), SEEK_SET);


    for (i = 0; i < TAMANHO_ARQUIVO; i++) {
        fread(&tabela_h, sizeof (struct REGISTRO), 1, arquivo_hashing);
        if (tabela_h.chave == -1) {
            printf("%d: vazio\n", i);
        } else {
            if (tabela_h.apontador == -1)
                printf("%d: %d %s %d nulo\n", i, tabela_h.chave, tabela_h.nome, tabela_h.idade);
            else
                printf("%d: %d %s %d %d\n", i, tabela_h.chave, tabela_h.nome, tabela_h.idade, tabela_h.apontador);

//            printf("%d: %d %s %d -> %d|fater:%d\n", i, tabela_h.chave, tabela_h.nome, tabela_h.idade, tabela_h.apontador, tabela_h.index_father);

        }
    }
    // fecha o arquivo
    fclose(arquivo_hashing);
}

/**
 * Consultar existência da chave na tabela
 * @param chave
 * @param retorna_indice  retorna indice da busca
 * @return 1 se existir a chave na tabela
 * @return 0 se não existir a chave na tabela
 * @retureturn índice (caso o parâmetro retorna_indice = 1)
 */
int consultar(int chave, int retorna_indice) {
    int indice = h1(chave);
    //tem ou não sucessores | saída do loop
    bool chain, exit = false;

    if ((arquivo_hashing = fopen(ARQUIVO, "rb")) == NULL) {
        //        fprintf(stderr, "\nErro ao abrir arquivo!");
    }

    rewind(arquivo_hashing);
    // posiciona para leitura
    //    fseek(arquivo_hashing, ((indice + 1) * sizeof (struct REGISTRO)), SEEK_SET);
    fseek(arquivo_hashing, ((indice + 1) * sizeof (struct REGISTRO)), SEEK_SET);
    // le a posição na tabela hash
    fread(&tabela_h, sizeof (struct REGISTRO), 1, arquivo_hashing);

    // arquivo vazio
    if (tabela_h.chave == -1) {
        if (retorna_indice == 1)
            return -1; //não encontrou pois o arquivo está vazio - retorna o próprio índice
        else {
            fprintf(stderr, "chave nao encontrada: %d\n", chave);
            return 0;
        }
    }
    // encontrou no home adress
    if (tabela_h.chave == chave) {
        if (retorna_indice == 1)
            return indice;
        else {
            fprintf(stderr, "chave: %d\n%s\n%d\n", chave, tabela_h.nome, tabela_h.idade);
            return 1;
        }
    }// colisão
    else {
        // tem sucessores?
        if (tabela_h.apontador != -1) {
            int salto = h2(tabela_h.chave) * tabela_h.apontador;
            int go_to_index = indice + salto;
            if (go_to_index >= TAMANHO_ARQUIVO)
                go_to_index -= TAMANHO_ARQUIVO;
            //            indice = go_to_index;
            // loop para verificar todas as posições do arquivo a procura da chave
            while (go_to_index != indice && exit == false) {
                // salta no arquivo pra achar a chave em outras posições
                fseek(arquivo_hashing, ((go_to_index + 1) * sizeof (struct REGISTRO)), SEEK_SET);
                fread(&tabela_h, sizeof (struct REGISTRO), 1, arquivo_hashing);

                // encontrou 
                if (tabela_h.chave == chave) {
                    if (retorna_indice == 1)
                        return go_to_index;
                    else {
                        printf("chave: %d\n%s\n%d\n", chave, tabela_h.nome, tabela_h.idade);
                        return 1;
                    }
                } else {
                    if (tabela_h.apontador != -1) {
                        int index_aux = go_to_index; // salva o indice original
                        // tem sucessores
                        /*****************  FALTA IMPLEMENTAR **************/
                        while (tabela_h.apontador != -1 && tabela_h.chave != chave) {
                            salto = h2(tabela_h.chave) * tabela_h.apontador;
                            go_to_index = index_aux + salto;
                            index_aux = go_to_index;
                            if (go_to_index >= TAMANHO_ARQUIVO)
                                go_to_index -= TAMANHO_ARQUIVO;
                            fseek(arquivo_hashing, ((go_to_index + 1) * sizeof (struct REGISTRO)), SEEK_SET);
                            fread(&tabela_h, sizeof (struct REGISTRO), 1, arquivo_hashing);
                            index_aux = go_to_index;
                        }
                        if (tabela_h.chave == chave) {
                            // saiu do loop encontrando a chave
                            if (retorna_indice == 1)
                                return go_to_index;
                            else {
                                fprintf(stderr, "chave: %d\n%s\n%d\n", chave, tabela_h.nome, tabela_h.idade);
                                return 1;
                            }

                        } else {
                            // saiu do loop sem encontrar a chave
                            if (retorna_indice == 1)
                                return -1;
                            else {
                                fprintf(stderr, "chave nao encontrada: %d\n", chave);
                                return 0;
                            }
                        }
                    } else {
                        // não tem mais sucessore.A chave não se encontra no arquivo

                        exit = true;
                        if (retorna_indice == 1)
                            return -1;
                        else {
                            fprintf(stderr, "chave nao encontrada: %d\n", chave);
                            return 0;
                        }

                    }
                }
            }
        } else {
            //não sucessores, logo 

            if (retorna_indice == 1)
                return -1;
            else {
                fprintf(stderr, "chave nao encontrada: %d\n", chave);
                return 0;
            }

        }
    }
    fclose(arquivo_hashing);
    // existe chave
    return 1;
}

/**
 * Inserir registro na tabela hash
 * @param nome
 * @param idade
 * @param chave
 */
void inserir_chave(int chave, char nome[20], int idade) {
    int aux, indice = 0, salto, count_jumps = 0, go_to_index, index_aux, index_before_jump;
    registro register_aux, register_jump;

    indice = (int) h1(chave);
    // se já não existir, insere
    //    if (!check_key(chave)) {
    // não existe
    if (consultar(chave, 1) == -1) {
        register_aux.chave = chave;
        strcpy(register_aux.nome, nome);
        register_aux.index_father = -1;
        register_aux.idade = idade;
        register_aux.apontador = -1;

        if ((arquivo_hashing = fopen(ARQUIVO, "rb+")) == NULL) {
            printf("\nErro ao abrir arquivo!");
            return;
        }
        rewind(arquivo_hashing);

        fseek(arquivo_hashing, ((indice + 1) * sizeof (struct REGISTRO)), SEEK_SET);
        fread(&tabela_h, sizeof (struct REGISTRO), 1, arquivo_hashing);

        // SEM colisão
        if (tabela_h.chave == -1) {
            fseek(arquivo_hashing, ((indice + 1) * sizeof (struct REGISTRO)), SEEK_SET);
            fwrite(&register_aux, sizeof (struct REGISTRO), 1, arquivo_hashing);
        }// 
        else {
            // verificar se o que está na posição está no seu home adress
            if (h1(tabela_h.chave) == indice) {
                // está no homeAdress 
                if (tabela_h.apontador == -1) {
                    //NÃO tem sucessor. Salta para o sucessor
                    salto = h2(tabela_h.chave);
                    fseek(arquivo_hashing, ((salto + indice + 1) * sizeof (struct REGISTRO)), SEEK_SET);
                    fread(&tabela_jump, sizeof (struct REGISTRO), 1, arquivo_hashing);
                    index_aux = indice; //salva o índice atual
                    // CASO BASE
                    if (tabela_jump.chave == -1) {
                        // salva o novo registro
                        fseek(arquivo_hashing, ((salto + indice + 1) * sizeof (struct REGISTRO)), SEEK_SET);
                        // salva o indice do pai 
                        register_aux.index_father = indice;
                        fwrite(&register_aux, sizeof (struct REGISTRO), 1, arquivo_hashing);
                        // lê registro para salvar apontador para sucessor
                        fseek(arquivo_hashing, ((indice + 1) * sizeof (struct REGISTRO)), SEEK_SET);
                        fread(&tabela_h, sizeof (struct REGISTRO), 1, arquivo_hashing);
                        count_jumps++;
                        tabela_h.apontador = count_jumps;
                        // salva apontador para sucessor
                        fseek(arquivo_hashing, ((indice + 1) * sizeof (struct REGISTRO)), SEEK_SET);
                        fwrite(&tabela_h, sizeof (struct REGISTRO), 1, arquivo_hashing);
                    }
                    // posição ocupada - pular de h2 em h2 até achar uma disponível
                    if (tabela_jump.chave != -1) {
                        //                        index_aux++;
                        go_to_index = salto + index_aux;
                        // loop de busca de posição vazia
                        do {
                            fseek(arquivo_hashing, ((go_to_index + 1) * sizeof (struct REGISTRO)), SEEK_SET);
                            fread(&tabela_jump, sizeof (struct REGISTRO), 1, arquivo_hashing);
                            count_jumps++;
                            if (tabela_jump.chave != -1) {
                                // posição ocupada, procurar nova posição
                                //                                count_jumps++; //acerta apontador de saltos do home adress
                                //                                index_aux++;
                                index_aux = go_to_index + salto;
                                //                                go_to_index = salto + index_aux;
                                go_to_index = go_to_index + salto;
                                if (go_to_index >= TAMANHO_ARQUIVO)
                                    go_to_index -= TAMANHO_ARQUIVO;
                            } else {
                                // salva o registro
                                fseek(arquivo_hashing, ((go_to_index + 1) * sizeof (struct REGISTRO)), SEEK_SET);
                                // salva o indice do pai 
                                register_aux.index_father = indice;
                                fwrite(&register_aux, sizeof (struct REGISTRO), 1, arquivo_hashing);
                                // acertando o apontador de sucessores
                                tabela_h.apontador = count_jumps; //acerta o apontador de saltos
                                fseek(arquivo_hashing, ((indice + 1) * sizeof (struct REGISTRO)), SEEK_SET);
                                fwrite(&tabela_h, sizeof (struct REGISTRO), 1, arquivo_hashing);
                            }

                        } while (tabela_jump.chave != -1 && go_to_index != indice);
                    }
                } else {
                    // TEM SUCESSORE(S). É uma cadeia
                    salto = (h2(tabela_h.chave) * tabela_h.apontador) + indice;
                    if (salto >= TAMANHO_ARQUIVO)
                        salto -= TAMANHO_ARQUIVO;
                    // guarda o indice atual para saber onde acertar o apontador
                    int new_index = salto;

                    fseek(arquivo_hashing, ((salto + 1) * sizeof (struct REGISTRO)), SEEK_SET);
                    fread(&tabela_jump, sizeof (struct REGISTRO), 1, arquivo_hashing);
                    count_jumps++;

                    while (tabela_jump.apontador != -1 && new_index != indice) {
                        salto = (h2(tabela_jump.chave) * tabela_jump.apontador) + new_index;
                        if (salto >= TAMANHO_ARQUIVO)
                            salto -= TAMANHO_ARQUIVO;
                        new_index = salto;
                        // salva o índice anterior ao salto
                        index_before_jump = new_index;
                        count_jumps++; //salva o número de saltos p/ register_aux

                        fseek(arquivo_hashing, ((new_index + 1) * sizeof (struct REGISTRO)), SEEK_SET);
                        fread(&tabela_jump, sizeof (struct REGISTRO), 1, arquivo_hashing);
                    }
                    // saiu pq achou o último da cadeia
                    if (new_index != indice) {
                        // salta pelo h2 até achar uma vaga ou terminar o arquivo
                        index_before_jump = new_index; // salva o índice a ser atualizado
                        salto = h2(tabela_jump.chave) + new_index;
                        int index_h2 = h2(tabela_jump.chave);

                        do {
                            fseek(arquivo_hashing, ((salto + 1) * sizeof (struct REGISTRO)), SEEK_SET);
                            fread(&tabela_jump, sizeof (struct REGISTRO), 1, arquivo_hashing);
                            if (tabela_jump.chave == -1) {
                                fseek(arquivo_hashing, ((salto + 1) * sizeof (struct REGISTRO)), SEEK_SET);
                                // salva o indice do pai 
                                register_aux.index_father = index_before_jump;
                                //                                register_aux.index_father = indice;
                                fwrite(&register_aux, sizeof (struct REGISTRO), 1, arquivo_hashing);

                                // acertar o apontador
                                fseek(arquivo_hashing, ((index_before_jump + 1) * sizeof (struct REGISTRO)), SEEK_SET);
                                fread(&tabela_jump, sizeof (struct REGISTRO), 1, arquivo_hashing);
                                tabela_jump.apontador = count_jumps;
                                // salva saltos para pai
                                fseek(arquivo_hashing, ((index_before_jump + 1) * sizeof (struct REGISTRO)), SEEK_SET);
                                fwrite(&tabela_jump, sizeof (struct REGISTRO), 1, arquivo_hashing);

                                register_aux = tabela_jump;
                                fseek(arquivo_hashing, ((index_before_jump + 1) * sizeof (struct REGISTRO)), SEEK_SET);
                                //                                fwrite(&tabela_jump, sizeof (struct REGISTRO), 1, arquivo_hashing);
                                fwrite(&register_aux, sizeof (struct REGISTRO), 1, arquivo_hashing);

                                //                                fclose(arquivo_hashing);

                                // previnir loop infinito
                                return;
                            } else {
                                // saltando de h2 em h2 até encontrar uma posição 
                                new_index = salto;
                                salto += index_h2;
                                if (salto >= TAMANHO_ARQUIVO)
                                    salto -= TAMANHO_ARQUIVO;
                            }
                            count_jumps++;
                        } while (tabela_jump.apontador != -1 || salto != indice);
                    }
                }
            }// NÃO ESTÁ NO HOME ADRESS
            else {
                // retira ele e os seus sucessores e os reinsere
                int pai = tabela_h.index_father;
                // inicializa lista
                node *LISTA = (node *) malloc(sizeof (node));
                if (!LISTA) {
                    //                    fprintf(stderr, "Sem memória disponivel!\n");
                    exit(1);
                } else {
                    inicia(LISTA);
                }
                // NÃO TEM SUCESSORES - APENAS 1 A SER REINSERIDO
                if (tabela_h.apontador == -1) {
                    int key = tabela_h.chave, age = tabela_h.idade;
                    char name[20];
                    strcpy(name, tabela_h.nome);
                    // salva na lista para reinserção
                    insereFim(LISTA, tabela_h.chave, tabela_h.nome, tabela_h.idade);
                    // insere no lugar deste o registro
                    fseek(arquivo_hashing, ((indice + 1) * sizeof (struct REGISTRO)), SEEK_SET);
                    fwrite(&register_aux, sizeof (struct REGISTRO), 1, arquivo_hashing);

                    fclose(arquivo_hashing);
                    // ACERTA APONTADOR DO PAI
                    limpa_apontador_pai(pai);
                    // reinsere o elemento
                    reinserir_chave(key, name, age);
                    // REINICIA A LISTA
                    inicia(LISTA);
                    return;
                }// tem que reinserir todos até o final da cadeia
                else {
                    int jump_to;
                    // LOOP até encontrar o final da cadeia
                    while (tabela_h.apontador != -1) {
                        // salva na lista para reinserção
                        insereFim(LISTA, tabela_h.chave, tabela_h.nome, tabela_h.idade);
                        // calcula salto para o próximo para inserir
                        jump_to = (h2(tabela_h.chave) * tabela_h.apontador) + indice;
                        if (jump_to >= TAMANHO_ARQUIVO)
                            jump_to -= TAMANHO_ARQUIVO;
                        // MARCAR POSIÇÃO COMO VÁLIDA
                        limpa_registro(indice);
                        // lê o próximo da cadeia
                        fseek(arquivo_hashing, ((jump_to + 1) * sizeof (struct REGISTRO)), SEEK_SET);
                        fread(&tabela_h, sizeof (struct REGISTRO), 1, arquivo_hashing);
                    }
                    // insere o último da cadeia
                    insereFim(LISTA, tabela_h.chave, tabela_h.nome, tabela_h.idade);
                    // MARCAR POSIÇÃO COMO VÁLIDA
                    limpa_registro(jump_to);

                    // após inserir antigos na lista, insere novo 
                    fseek(arquivo_hashing, ((indice + 1) * sizeof (struct REGISTRO)), SEEK_SET);
                    fwrite(&register_aux, sizeof (struct REGISTRO), 1, arquivo_hashing);

                    fclose(arquivo_hashing);
                    // ACERTA APONTADOR DO PAI
                    limpa_apontador_pai(pai);
                    //reinsere os da lista
                    reinsere_lista(LISTA);
                    // reinicia a lista
                    inicia(LISTA);
                }
                //                exibe(LISTA);
            }
        }
        fclose(arquivo_hashing);
    } else {
        fprintf(stderr, "Chave ja existente\n");
    }
    //    imprimir()
}

void reinserir_chave(int chave, char nome[20], int idade) {
    inserir_chave(chave, nome, idade);
    //    fclose(arquivo_hashing);
}

/**
 * limpa o registro do índice passado como parâmetro
 * @param indice
 */
void limpa_registro(int indice) {
    registro register_blackout;

    register_blackout.apontador = -1;
    register_blackout.chave = -1;
    strcpy(register_blackout.nome, "");
    register_blackout.idade = -1;
    register_blackout.index_father = -1;



    fseek(arquivo_hashing, ((indice + 1) * sizeof (struct REGISTRO)), SEEK_SET);
    fwrite(&register_blackout, sizeof (struct REGISTRO), 1, arquivo_hashing);
}

void remove_registro(int indice) {
    registro register_blackout;

    register_blackout.apontador = -1;
    register_blackout.chave = -1;
    strcpy(register_blackout.nome, "");
    register_blackout.idade = -1;
    register_blackout.index_father = -1;

    if ((arquivo_hashing = fopen(ARQUIVO, "rb+")) == NULL) {
        printf("\nErro ao abrir arquivo!");
        return;
    }

    fseek(arquivo_hashing, ((indice + 1) * sizeof (struct REGISTRO)), SEEK_SET);
    fwrite(&register_blackout, sizeof (struct REGISTRO), 1, arquivo_hashing);

    fclose(arquivo_hashing);
}

/**
 * limpa referência do elemento anterior na cadeia
 * @param pai
 */
void limpa_apontador_pai(int pai) {
    registro register_aux, update_register;

    if ((arquivo_hashing = fopen(ARQUIVO, "rb+")) == NULL) {
        printf("\nErro ao abrir arquivo!");
        return;
    }
    fseek(arquivo_hashing, ((pai + 1) * sizeof (struct REGISTRO)), SEEK_SET);
    fread(&register_aux, sizeof (struct REGISTRO), 1, arquivo_hashing);
    // limpa apontador do pai 
    update_register.chave = register_aux.chave;
    strcpy(update_register.nome, register_aux.nome);
    update_register.idade = register_aux.idade;
    update_register.index_father = register_aux.index_father;
    update_register.apontador = -1;

    fseek(arquivo_hashing, ((pai + 1) * sizeof (struct REGISTRO)), SEEK_SET);
    fwrite(&update_register, sizeof (struct REGISTRO), 1, arquivo_hashing);

    fclose(arquivo_hashing);
}

/**
 * Calcula média de acessos as chaves inseridas
 */
void media_acessos() {
    float count_jumps = 0, qtd_chaves = 0;
    //    int count_jumps = 0, qtd_chaves = 0;
    node *LISTA = (node *) malloc(sizeof (node));
    if (!LISTA) {
        exit(1);
    } else {
        inicia(LISTA);
    }
    if ((arquivo_hashing = fopen(ARQUIVO, "rb+")) == NULL) {
        printf("\nErro ao abrir arquivo!");
        return;
    }
    //    int i;
    // contar as chaves inseridas, salvar em uma lista e consultar até o final da lista
    for (int i = 0; i < TAMANHO_ARQUIVO; i++) {

        fseek(arquivo_hashing, ((i + 1) * sizeof (struct REGISTRO)), SEEK_SET);
        fread(&tabela_h, sizeof (struct REGISTRO), 1, arquivo_hashing);
        // insere todas as chaves do arquivo na lista
        if (tabela_h.chave != -1) {
            insereFim(LISTA, tabela_h.chave, tabela_h.nome, tabela_h.idade);
            qtd_chaves++;
        }
    }
    if (vazia(LISTA)) {

        //        return;
    }
    node *tmp;
    tmp = LISTA->prox;

    //loop para contar os acessos
    while (tmp != NULL) {
        int indice_elemento = consultar(tmp->chave, 1);
        int exit = 0;
        //         retirar depois
        //        printf("Chave: %d %s %d\n", tmp->chave, tmp->nome, tmp->idade);


        // consultar todos os pais
        while (exit == 0) {
            fseek(arquivo_hashing, (indice_elemento + 1) * sizeof (struct REGISTRO), SEEK_SET);
            fread(&tabela_h, sizeof (struct REGISTRO), 1, arquivo_hashing);

            //            fprintf(stderr, "jumps %f\n", count_jumps);
            //saída do loop dos pais 
            if (tabela_h.index_father == -1) {
                exit = 1;
                //                fprintf(stderr, "cj %d", count_jumps);
            } else
                indice_elemento = tabela_h.index_father;
            // incrementa contador
            count_jumps++;
        }
        // seleciona próximo da lista
        tmp = tmp->prox;
    }

    if (qtd_chaves != 0) {
        float media_acessos = count_jumps / qtd_chaves;
        fprintf(stderr, "%.1f", media_acessos);
    } else {
        fprintf(stderr, "0.0");
    }


    // media = acessos / TAMANHO_ARQUIVO
    fclose(arquivo_hashing);
}

/**
 * Remove chave 
 * @param chave a ser removida
 * @return 
 */
void remover(int chave) {
    int salto, index_aux, pai_index;
    registro cleaner;
    cleaner.apontador = -1;
    cleaner.chave = -1;
    cleaner.idade = -1;
    cleaner.index_father = -1;
    strcpy(cleaner.nome, "");

    // inicializa lista
    node *LISTA = (node *) malloc(sizeof (node));
    if (!LISTA) {
        //        fprintf(stderr, "Sem memória disponivel!\n");
        exit(1);
    } else {
        inicia(LISTA);
    }
    // arquivo binário
    if ((arquivo_hashing = fopen(ARQUIVO, "rb+")) == NULL) {
        //        fprintf(stderr, "\nErro ao abrir arquivo!");
        //        printf("\nErro ao abrir arquivo!");
        return;
    }
    // consultar índice a ser removido
    int indice_remove = consultar(chave, 1);
    // pode ser 0 até TAMANHO_ARQUIVO
    if (indice_remove != -1) {
        // lê o registro da posição a ser removido
        fseek(arquivo_hashing, ((indice_remove + 1) * sizeof (struct REGISTRO)), SEEK_SET);
        fread(&tabela_h, sizeof (struct REGISTRO), 1, arquivo_hashing);
        // verifica se corresponde a chave a ser removida
        if (tabela_h.chave == chave) {
            // salva índice para pai
            pai_index = tabela_h.index_father;
            // salva índice original
            index_aux = indice_remove;
            salto = h2(tabela_h.chave) * tabela_h.apontador + index_aux;
            if (salto >= TAMANHO_ARQUIVO)
                salto -= TAMANHO_ARQUIVO;

            while (tabela_h.apontador != -1) {

                fseek(arquivo_hashing, ((salto + 1) * sizeof (struct REGISTRO)), SEEK_SET);
                fread(&tabela_h, sizeof (struct REGISTRO), 1, arquivo_hashing);

                insereFim(LISTA, tabela_h.chave, tabela_h.nome, tabela_h.idade);
                index_aux = salto;

                salto = h2(tabela_h.chave) * tabela_h.apontador + salto;
                if (salto >= TAMANHO_ARQUIVO)
                    salto -= TAMANHO_ARQUIVO;

                //                limpa_registro(index_aux);
                remove_registro(index_aux);

            }
            // remove índice
            //            limpa_registro(indice_remove);
            remove_registro(indice_remove);

            limpa_apontador_pai(pai_index);
            reinsere_lista(LISTA);
        }
    }
}

/**
 * Função principal
 * 
 * @param argc
 * @param argv
 * @return 
 */
int main(int argc, char *argv[]) {
    char nome[20], operacao;
    int idade, chave;

    // Criação de lista de reinserção de elementos
    node *LISTA = (node *) malloc(sizeof (node));
    if (!LISTA) {
        //        fprintf(stderr, "memória insuficiente para criar lista \n");
    } else {
        inicia(LISTA);
    }

    // Criação de arquivo
    if ((arquivo_hashing = fopen(ARQUIVO, "rb")) == NULL) {
        // inicializa tabela hash
        inicializar();
        // cria arquivo para inserção da tabela hash
        criar_arquivo();

    }

    // Verifica qual operação será executada
    scanf("%c", &operacao);

    do {
        switch (operacao) {
            case 'i':
                scanf("%d", &chave);
                // scanf lendo string com espaços em branco
                scanf(" %[^\n]s", &nome);
                //                fflush(stdin);
                scanf("%d", &idade);
                inserir_chave(chave, nome, idade);
                break;

            case 'c':
                //                fprintf(stderr, "CONSULTAR\n");
                scanf("%d", &chave);
                consultar(chave, 0);
                break;

            case 'r':
                //                fprintf(stderr, "REMOVER\n");
                scanf("%d", &chave);
                int result = consultar(chave, 1);
                if (result != -1) {
                    //                    fprintf(stderr, "indíce para remover: %d\n", result);
                    // remoção
                    remover(chave);
                } else {
                    fprintf(stderr, "chave nao encontrada: %d", chave);
                }
                break;

            case 'p':
                //                fprintf(stderr, "IMPRIMIR\n");
                imprimir();
                break;

            case 'm':
                //                fprintf(stderr, "MEDIA DE ACESSOS\n");
                media_acessos();
                break;

            case 'e':
                return 0;
                break; //exit

            default:
                break;

        }
        // limpa caracter do buffer antes do próximo loop
        operacao = getchar();
    } while (operacao != 'e');

    //    fclose(arquivo_hashing);
    return 0;
}

