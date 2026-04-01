/*
 * Sistema de Cadastro de Multas de Trânsito
 * Linguagem: C
 * Funcionalidades: cadastrar, listar, buscar, excluir e salvar multas
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_MULTAS 100
#define ARQUIVO "multas.dat"

/* ===== ESTRUTURA DE DADOS ===== */

typedef struct {
    int    id;
    char   nome[100];
    char   cpf[15];
    char   cnh[12];
    char   placa[8];
    char   codigo[10];
    char   descricao[200];
    char   data[11];        /* formato: DD/MM/AAAA */
    char   local[150];
    float  valor;
    int    pontos;
    char   status[20];      /* "Pendente", "Paga", "Recorrida" */
} Multa;

/* ===== VARIÁVEIS GLOBAIS ===== */

Multa multas[MAX_MULTAS];
int   total = 0;
int   proximo_id = 1;

/* ===== PROTÓTIPOS ===== */

void  pausar();
void  menu_principal();
void  cadastrar_multa();
void  listar_multas();
void  buscar_por_placa();
void  buscar_por_cpf();
void  excluir_multa();
void  alterar_status();
void  salvar_arquivo();
void  carregar_arquivo();
void  exibir_multa(Multa m);
int   validar_cpf(const char *cpf);
int   validar_placa(const char *placa);

/* ===== FUNÇÕES UTILITÁRIAS ===== */

void pausar() {
    printf("\nPressione ENTER para continuar...");
    getchar();
    getchar();
}

void exibir_multa(Multa m) {
    printf("+-------------------------------------------------+\n");
    printf("| ID: %-5d                                       |\n", m.id);
    printf("+-------------------------------------------------+\n");
    printf("  Nome   : %s\n", m.nome);
    printf("  CPF    : %s\n", m.cpf);
    printf("  CNH    : %s\n", m.cnh);
    printf("  Placa  : %s\n", m.placa);
    printf("  Codigo : %s\n", m.codigo);
    printf("  Data   : %s\n", m.data);
    printf("  Local  : %s\n", m.local);
    printf("  Descr. : %s\n", m.descricao);
    printf("  Valor  : R$ %.2f\n", m.valor);
    printf("  Pontos : %d\n", m.pontos);
    printf("  Status : %s\n", m.status);
    printf("+-------------------------------------------------+\n");
}

/* Validação simples de CPF (apenas formato) */
int validar_cpf(const char *cpf) {
    int i, digitos = 0;
    for (i = 0; cpf[i] != '\0'; i++) {
        if (cpf[i] >= '0' && cpf[i] <= '9') digitos++;
        else if (cpf[i] != '.' && cpf[i] != '-') return 0;
    }
    return (digitos == 11);
}

/* Validação simples de placa (formato antigo: ABC1234 ou Mercosul: ABC1D23) */
int validar_placa(const char *placa) {
    if (strlen(placa) != 7) return 0;
    int i;
    for (i = 0; i < 3; i++)
        if (placa[i] < 'A' || placa[i] > 'Z') return 0;
    if (placa[3] < '0' || placa[3] > '9') return 0;
    /* aceita digito ou letra na 5a posicao (Mercosul) */
    if (placa[5] < '0' || placa[5] > '9') return 0;
    if (placa[6] < '0' || placa[6] > '9') return 0;
    return 1;
}

/* ===== ARQUIVO ===== */

void salvar_arquivo() {
    FILE *fp = fopen(ARQUIVO, "wb");
    if (!fp) {
        printf("ERRO: Nao foi possivel salvar o arquivo!\n");
        return;
    }
    fwrite(&total,      sizeof(int),   1,     fp);
    fwrite(&proximo_id, sizeof(int),   1,     fp);
    fwrite(multas,      sizeof(Multa), total, fp);
    fclose(fp);
    printf("Dados salvos com sucesso!\n");
}

void carregar_arquivo() {
    FILE *fp = fopen(ARQUIVO, "rb");
    if (!fp) {
        printf("Nenhum arquivo encontrado. Iniciando com banco vazio.\n");
        return;
    }
    fread(&total,      sizeof(int),   1,     fp);
    fread(&proximo_id, sizeof(int),   1,     fp);
    fread(multas,      sizeof(Multa), total, fp);
    fclose(fp);
    printf("Dados carregados: %d multa(s) encontrada(s).\n", total);
}

/* ===== CADASTRO ===== */

void cadastrar_multa() {

    printf("=== CADASTRAR NOVA MULTA ===\n\n");

    if (total >= MAX_MULTAS) {
        printf("ERRO: Limite maximo de %d multas atingido!\n", MAX_MULTAS);
        pausar();
        return;
    }

    Multa nova;
    nova.id = proximo_id;

    printf("Nome do infrator : ");
    scanf(" %[^\n]", nova.nome);

    do {
        printf("CPF (somente numeros ou com . e -) : ");
        scanf(" %s", nova.cpf);
        if (!validar_cpf(nova.cpf))
            printf("CPF invalido! Tente novamente.\n");
    } while (!validar_cpf(nova.cpf));

    printf("Numero da CNH   : ");
    scanf(" %s", nova.cnh);

    do {
        printf("Placa do veiculo (sem espacos, ex: ABC1234) : ");
        scanf(" %s", nova.placa);
        /* converte para maiusculo */
        int k;
        for (k = 0; nova.placa[k]; k++)
            if (nova.placa[k] >= 'a' && nova.placa[k] <= 'z')
                nova.placa[k] -= 32;
        if (!validar_placa(nova.placa))
            printf("Placa invalida! Use o formato ABC1234.\n");
    } while (!validar_placa(nova.placa));

    printf("Codigo da infracao : ");
    scanf(" %s", nova.codigo);

    printf("Descricao        : ");
    scanf(" %[^\n]", nova.descricao);

    printf("Data (DD/MM/AAAA): ");
    scanf(" %s", nova.data);

    printf("Local da infracao: ");
    scanf(" %[^\n]", nova.local);

    printf("Valor da multa (R$): ");
    scanf(" %f", &nova.valor);

    printf("Pontuacao na CNH : ");
    scanf(" %d", &nova.pontos);

    strcpy(nova.status, "Pendente");

    multas[total] = nova;
    total++;
    proximo_id++;

    salvar_arquivo();
    printf("\nMulta cadastrada com sucesso! (ID: %d)\n", nova.id);
    pausar();
}

/* ===== LISTAGEM ===== */

void listar_multas() {

    printf("=== LISTA DE MULTAS ===\n\n");

    if (total == 0) {
        printf("Nenhuma multa cadastrada.\n");
        pausar();
        return;
    }

    int i;
    for (i = 0; i < total; i++) {
        exibir_multa(multas[i]);
        printf("\n");
    }

    printf("Total: %d multa(s)\n", total);
    pausar();
}

/* ===== BUSCA POR PLACA ===== */

void buscar_por_placa() {
    
    printf("=== BUSCAR POR PLACA ===\n\n");

    char placa[8];
    printf("Digite a placa: ");
    scanf(" %s", placa);

    /* converte para maiusculo */
    int k;
    for (k = 0; placa[k]; k++)
        if (placa[k] >= 'a' && placa[k] <= 'z') placa[k] -= 32;

    int encontrado = 0, i;
    for (i = 0; i < total; i++) {
        if (strcmp(multas[i].placa, placa) == 0) {
            exibir_multa(multas[i]);
            printf("\n");
            encontrado = 1;
        }
    }

    if (!encontrado)
        printf("Nenhuma multa encontrada para a placa '%s'.\n", placa);

    pausar();
}

/* ===== BUSCA POR CPF ===== */

void buscar_por_cpf() {
    
    printf("=== BUSCAR POR CPF ===\n\n");

    char cpf[15];
    printf("Digite o CPF: ");
    scanf(" %s", cpf);

    int encontrado = 0, i;
    for (i = 0; i < total; i++) {
        if (strcmp(multas[i].cpf, cpf) == 0) {
            exibir_multa(multas[i]);
            printf("\n");
            encontrado = 1;
        }
    }

    if (!encontrado)
        printf("Nenhuma multa encontrada para o CPF '%s'.\n", cpf);

    pausar();
}

/* ===== EXCLUSAO ===== */

void excluir_multa() {
    
    printf("=== EXCLUIR MULTA ===\n\n");

    if (total == 0) {
        printf("Nenhuma multa cadastrada.\n");
        pausar();
        return;
    }

    int id;
    printf("Digite o ID da multa a excluir: ");
    scanf(" %d", &id);

    int i, pos = -1;
    for (i = 0; i < total; i++) {
        if (multas[i].id == id) { pos = i; break; }
    }

    if (pos == -1) {
        printf("Multa com ID %d nao encontrada.\n", id);
        pausar();
        return;
    }

    exibir_multa(multas[pos]);
    printf("Confirma exclusao? (s/n): ");
    char resp;
    scanf(" %c", &resp);

    if (resp == 's' || resp == 'S') {
        /* desloca os registros para preencher o buraco */
        for (i = pos; i < total - 1; i++)
            multas[i] = multas[i + 1];
        total--;
        salvar_arquivo();
        printf("Multa excluida com sucesso!\n");
    } else {
        printf("Exclusao cancelada.\n");
    }

    pausar();
}

/* ===== ALTERAR STATUS ===== */

void alterar_status() {
    
    printf("=== ALTERAR STATUS ===\n\n");

    int id;
    printf("Digite o ID da multa: ");
    scanf(" %d", &id);

    int i, pos = -1;
    for (i = 0; i < total; i++) {
        if (multas[i].id == id) { pos = i; break; }
    }

    if (pos == -1) {
        printf("Multa com ID %d nao encontrada.\n", id);
        pausar();
        return;
    }

    exibir_multa(multas[pos]);
    printf("\nNovo status:\n");
    printf("  1 - Pendente\n");
    printf("  2 - Paga\n");
    printf("  3 - Recorrida\n");
    printf("Opcao: ");
    int op;
    scanf(" %d", &op);

    switch (op) {
        case 1: strcpy(multas[pos].status, "Pendente");  break;
        case 2: strcpy(multas[pos].status, "Paga");      break;
        case 3: strcpy(multas[pos].status, "Recorrida"); break;
        default: printf("Opcao invalida.\n"); pausar(); return;
    }

    salvar_arquivo();
    printf("Status atualizado para '%s'.\n", multas[pos].status);
    pausar();
}

/* ===== MENU PRINCIPAL ===== */

void menu_principal() {
    int opcao;
    do {

        printf("========================================\n");
        printf("   SISTEMA DE CADASTRO DE MULTAS v1.0  \n");
        printf("========================================\n\n");
        printf("  1. Cadastrar nova multa\n");
        printf("  2. Listar todas as multas\n");
        printf("  3. Buscar por placa\n");
        printf("  4. Buscar por CPF\n");
        printf("  5. Excluir multa\n");
        printf("  6. Alterar status de pagamento\n");
        printf("  0. Sair\n\n");
        printf("Opcao: ");
        scanf(" %d", &opcao);

        switch (opcao) {
            case 1: cadastrar_multa();  break;
            case 2: listar_multas();    break;
            case 3: buscar_por_placa(); break;
            case 4: buscar_por_cpf();   break;
            case 5: excluir_multa();    break;
            case 6: alterar_status();   break;
            case 0: printf("\nSaindo... Ate logo!\n"); break;
            default: printf("\nOpcao invalida!\n"); pausar();
        }
    } while (opcao != 0);
}

/* ===== FUNÇÃO PRINCIPAL ===== */

int main() {
    printf("Carregando dados...\n");
    carregar_arquivo();
    pausar();
    menu_principal();
    return 0;
}
