#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PAGE_SIZE 256
#define NUM_PAGES 128
#define FRAME_SIZE 256
#define NUM_FRAMES 128
#define TLB_SIZE 16

typedef struct Node{
    int virtual_address;
    char bin[17];
    char offset[9];
    char page[9];
    struct Node *prox;
}Node;

typedef struct PageTableEntry{
    int frame_number;
    int valid;
    char page[9];
    struct PageTableEntry *prox;
}PageTableEntry;

typedef struct TLB{
    char pagenum[9];
    char frame;
    int frame_number;
    struct TLB *prox;
}TLB;

char* toBinary(int numero);
void getOffset(Node *head);
void getPage(Node *head);
void setVirtualAddress(Node *head, int Virtual_Address, int indice);
void incluir(Node **head, char bin[16]);
int binarioParaInteiro(char *binario);
int readBackStore(Node *head);

//Pagetable
void pageTableinit(PageTableEntry **head, PageTableEntry **tail);
PageTableEntry *pageTable(Node *head, PageTableEntry *pthead,PageTableEntry *pttail, int *indice);
PageTableEntry *pageTroca(Node *head, PageTableEntry *page_table, int indice, int *pagetable);
void print(PageTableEntry *head);

//TLB
void TLBinit(TLB **head, TLB **tail);
TLB * TLBexec(Node *head, TLB *tlhead, TLB *tltail, int *indice, int *tlb);
TLB *TLBtroca(Node *head, TLB *tlhead, int indice);
void printTLB(TLB *head);

int i = 0; int j = 0; int k = 0;
int tlb, pagetable;
int *numtlb = &tlb; int *numpagetable = &pagetable;
int *x = &j; int *y = &k;

int main(int argc, char *argv[]){
    FILE *fptr = fopen(argv[1], "r");
    FILE *fcor = fopen("correctmeu.txt", "w");

    int virtual_address, teste2;
    PageTableEntry *pthead = NULL;
    PageTableEntry *pttail = NULL;
    PageTableEntry *ptemp = NULL;
    pageTableinit(&pthead, &pttail);

    TLB *tlhead = NULL;
    TLB *tltail = NULL;
    TLB *ttemp = NULL;
    TLBinit(&tlhead, &tltail);

    Node *head = NULL;
    char enderecobin[16];
    while(!(feof(fptr))){
        if(j == NUM_PAGES){
            j = 0;
        }
        if(k == TLB_SIZE){
            k = 0;
        }

        fscanf(fptr, "%d", &virtual_address);

        incluir(&head, toBinary(virtual_address));
        setVirtualAddress(head, virtual_address, 0);
        getOffset(head);
        getPage(head);
        int offset = binarioParaInteiro(head->offset);
        int page = binarioParaInteiro(head->page);
        int instruct = readBackStore(head);
        ptemp = pageTable(head, pthead, pttail, x);
        ttemp = TLBexec(head, tlhead, tltail, y, numtlb);
        fprintf(fcor, "Virtual address: %d TLB: %d Physical address: %d Value: %d\n", head->virtual_address, ttemp->frame,(offset + (256 * ptemp->frame_number)), instruct);
        j++;
        k++;
        i++;
        free(head);
        head = NULL;
    }
    fprintf(fcor, "Number of Translated Addresses = %d\n", i);
    fprintf(fcor, "Page Faults = %d\n", *numpagetable);
    float faultrate =  *numpagetable/(float)i;
    fprintf(fcor, "Page Fault Rate = %.3f\n", faultrate);
    fprintf(fcor, "TLB Hits = %d\n", *numtlb);
    float tlbrate = *numtlb/(float)i;
    fprintf(fcor, "TLB Hit Rate = %.3f\n", tlbrate);
    //for (int j = 0; j < 128; ++j) {

    //}
    //readBackStore(head);
    //print(pthead, pttail);
    return 0;
}

char* toBinary(int numero) {
    char* bin = (char*)malloc(17 * sizeof(char)); // 16 bits + 1 para o caractere nulo
    int tamanho_binario = 0;
    int temp = numero;

    while (temp > 0) {
        tamanho_binario++;
        temp = temp/2;
    }

    int zero = 16 - tamanho_binario;
    if (zero < 0) {
        zero = 0;
    }

    bin[16] = '\0';

    for (int j = 0; j < zero; j++) {
        bin[j] = '0';
    }

    for (int j = 15; j >= zero; j--) {
        bin[j] = (numero & 1) + '0';
        numero >>= 1;
    }

    return bin;
}

void getPage(Node *head){
    if(head != NULL){
        while(head!=NULL){
            strncpy(head->page, head->bin, 8);
            head->page[8] = '\0';
            head = head->prox;
        }
    }
}

void getOffset(Node *head){
    if(head != NULL){
        while(head!=NULL){
            strncpy(head->offset, head->bin + 8, 8);
            head->offset[8] = '\0';
            head = head->prox;
        }
    }
}

void setVirtualAddress(Node *head, int Virtual_Address, int indice){
    for (int i = 0; i < indice; ++i) {
        if (head != NULL) {
            head = head->prox;
        }
    }
    head->virtual_address = Virtual_Address;
}

void incluir(Node **head, char bin[17]){
    Node *novo = (Node *)malloc(sizeof(Node));
    Node *temp = *head;

    if(novo != NULL) {
        strcpy(novo->bin, bin);
        novo->prox = NULL;
        if (*head == NULL) {
            *head = novo;
        }else {
            novo->prox = *head;
            *head = novo;
        }
    }
}

void print(PageTableEntry *head){
    FILE *pt = fopen("pagetable.txt", "a");
    while(head!=NULL){
        fprintf(pt, "Numpage: %d|Page: %s\n", head->frame_number , head->page);
        head = head->prox;
    }fclose(pt);
}

int binarioParaInteiro(char *binario) {
    int tamanho = strlen(binario);
    int inteiro = 0;
    int potencia = 1;

    // Começando da direita para a esquerda
    for (int i = tamanho - 1; i >= 0; i--) {
        // Verificando se o caractere é '1' (dígito 1)
        if (binario[i] == '1') {
            // Adicionando a potência de 2 correspondente
            inteiro += potencia;
        }
        // Dobrando a potência para o próximo dígito binário
        potencia *= 2;
    }

    return inteiro;
}

int readBackStore(Node *head){
    FILE *fbin = fopen("BACKING_STORE.bin", "rb");
    char value;
    int offset = binarioParaInteiro(head->offset);
    int page = binarioParaInteiro(head->page);
    fseek(fbin, (page * 256) + offset, SEEK_SET);
    fread(&value, sizeof(value), 1, fbin);
    //printf("Page: %d, Offset %d\n", page, offset);
    fclose(fbin);
    return value;
}

void pageTableinit(PageTableEntry **head, PageTableEntry **tail) {
    int i = 0;
    while (i < 128) {
        PageTableEntry *novo = (PageTableEntry *) malloc(sizeof(PageTableEntry));
        if (novo != NULL) {
            novo->valid = 0;
            novo->frame_number = i;
            strcpy(novo->page, "x");
            novo->prox = NULL;
            if (*head == NULL) {
                *head = novo;
                *tail = novo;
            } else {
                (*tail)->prox = novo;
                *tail = novo;
            }
            i++;
        }
    }
}
PageTableEntry *pageTable(Node *head, PageTableEntry *pthead, PageTableEntry *pttail, int *indice) {
    PageTableEntry *ptemp = pthead;
    while(ptemp != NULL){
        if(strcmp(ptemp->page, head->page) != 0){
            //printf("PTP: %s, HP: %s - NAO ENCONTRADA\n", ptemp->page, head->page);
            ptemp = ptemp->prox;
            //printf("Indice: %d\n", *indice);
        }else{
            //printf("PTP: %s, HP: %s - ENCONTRADA\n", ptemp->page, head->page);
            (*indice)--;
            return ptemp;
            ptemp = ptemp->prox;
            //printf("Indice: %d A\n", *indice);
        }
    }
    pageTroca(head, pthead, *indice, numpagetable);
}

PageTableEntry *pageTroca(Node *head, PageTableEntry *page_table, int indice, int *pagetable){
    int i = 0;
    (*pagetable)++;
    print(page_table);
    while(i < indice && page_table != NULL){
        page_table = page_table->prox;
        i++;
    }
    strcpy(page_table->page, head->page);
    return page_table;
}

void TLBinit(TLB **head, TLB **tail){
    int i = 0;
    while (i < TLB_SIZE) {
        TLB *novo = (TLB *) malloc(sizeof(TLB));
        if (novo != NULL) {
            strcpy(novo->pagenum, "X");
            novo->frame = i;
            novo->prox = NULL;
            if (*head == NULL) {
                *head = novo;
                *tail = novo;
            } else {
                (*tail)->prox = novo;
                *tail = novo;
            }
            i++;
        }
    }
}

TLB * TLBexec(Node *head, TLB *tlhead, TLB *tltail, int *indice, int *tlb){
    TLB *ttemp = tlhead;
    while(ttemp != NULL){
        if(strcmp(ttemp->pagenum, head->page) != 0){
            //printf("PTP: %s, HP: %s - NAO ENCONTRADA\n", ptemp->page, head->page);
            ttemp = ttemp->prox;
            //printf("Indice: %d\n", indice);
        }else{
            //printf("PTP: %s, HP: %s - ENCONTRADA\n", ptemp->page, head->page);
            (*indice)--;
            (*tlb)++;
            return ttemp;
            //printf("Indice: %d A\n", indice);
        }
    }TLBtroca(head, tlhead, *indice);
}

TLB *TLBtroca(Node *head, TLB *tlhead, int indice){
    int i = 0;
    printTLB(tlhead);
    while(i < indice && tlhead != NULL){
        tlhead = tlhead->prox;
        i++;
    }strcpy(tlhead->pagenum, head->page);
    return tlhead;
}

void printTLB(TLB *head){
    FILE *pt = fopen("TLB.txt", "a");
    while(head!=NULL){
        fprintf(pt, "Frame: %d|Page: %s\n", head->frame , head->pagenum);
        head = head->prox;
    }fclose(pt);
}
