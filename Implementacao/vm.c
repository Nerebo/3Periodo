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
    int lastused;
    struct PageTableEntry *prox;
}PageTableEntry;

typedef struct TLB{
    char pagenum[9];
    char frame;
    int frame_number;
    PageTableEntry *pagetable;
    struct TLB *prox;
}TLB;

char* toBinary(int numero);
void getOffset(Node *head);
void getPage(Node *head);
void setVirtualAddress(Node *head, int Virtual_Address, int indice);
void incluir(Node **head, char bin[16]);
int binarioParaInteiro(char *binario);
int readBackStore(Node *head);
int qtdendereco(FILE *dados) ;

//Pagetable
void pageTableinit(PageTableEntry **head, PageTableEntry **tail);
PageTableEntry *pageTable(Node *head, PageTableEntry *pthead,PageTableEntry *pttail,int *pagetable, int *indice, int validacao, int tempo);
PageTableEntry *pageTroca(Node *head, PageTableEntry *page_table, int indice, int validacao, int tempo);
void pageAtt(TLB *head, PageTableEntry *phead, int tempo);

//TLB
void TLBinit(TLB **head, TLB **tail);
TLB * TLBexec(Node *head, TLB *tlhead, TLB *tltail,PageTableEntry *phead, int *indice, int *tlb, int tempo);
TLB *TLBtroca(Node *head, TLB *tlhead, int indice);

int i = 0; int j = 0; int k = 0;
int tlb, pagetable;
int fifo_ou_lru;
int *numtlb = &tlb; int *numpagetable = &pagetable;
int *x = &j; int *y = &k;

int main(int argc, char *argv[]){
    FILE *fptr = fopen(argv[1], "r");
    FILE *fcor = fopen("correctmeu.txt", "w");
    int linhas = qtdendereco(fptr);
    fseek(fptr, 0, 0);

    if(strcmp(argv[2], "fifo") == 0){
        fifo_ou_lru = 1;
    }

    int virtual_address;
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
    while(i <= linhas){
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
        ttemp = TLBexec(head, tlhead, tltail, pthead, y, numtlb, i);
        ptemp = pageTable(head, pthead, pttail,numpagetable, x, fifo_ou_lru, i);
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
    return 0;
}

char* toBinary(int numero) {
    char* bin = (char*)malloc(17 * sizeof(char));
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

int binarioParaInteiro(char *binario) {
    int tamanho = strlen(binario);
    int inteiro = 0;
    int potencia = 1;

    for (int i = tamanho - 1; i >= 0; i--) {

        if (binario[i] == '1') {

            inteiro += potencia;
        }
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
            novo->lastused = -129;
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
PageTableEntry *pageTable(Node *head, PageTableEntry *pthead, PageTableEntry *pttail,int *pagetable, int *indice, int validacao, int tempo) {
    PageTableEntry *ptemp = pthead;
    while(ptemp != NULL){
        if(strcmp(ptemp->page, head->page) != 0){
            ptemp = ptemp->prox;
        }else{
            (*indice)--;
            ptemp->lastused = tempo;
            return ptemp;
        }
    }
    (*pagetable)++;
    pageTroca(head, pthead, *indice, validacao, tempo);
}

PageTableEntry *pageTroca(Node *head, PageTableEntry *page_table, int indice, int validacao, int tempo) {
int i = 0;
    if (validacao != 1) {
        indice = i;
        PageTableEntry *temp = page_table;
        PageTableEntry *menorEntry = page_table;
        int menorTempo = temp->lastused;

        while (temp != NULL) {
            if (temp->lastused < menorEntry->lastused) {
                menorTempo = temp->lastused;
                menorEntry = temp;
                indice = i;
            }
            i++;
            temp = temp->prox;
        }

        if(indice != 0 && indice < 128){
        for (int l = 0; l < indice; ++l) {
            page_table = page_table->prox;
        }
        }

        strcpy(page_table->page, head->page);
        (*page_table).lastused = tempo;
        return page_table;

    }else if (validacao == 1) {
        int i = 0;
        PageTableEntry *temp = page_table;
        while (i < indice && temp != NULL) {
            temp = temp->prox;
            i++;
        }

        strcpy(temp->page, head->page);
        return temp;
    }

    return NULL;
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

TLB * TLBexec(Node *head, TLB *tlhead, TLB *tltail,PageTableEntry *phead, int *indice, int *tlb, int tempo){
    TLB *ttemp = tlhead;
    while(ttemp != NULL){
        if(strcmp(ttemp->pagenum, head->page) != 0){
            ttemp = ttemp->prox;
        }else{
            (*indice)--;
            (*tlb)++;
            pageAtt(ttemp, phead, tempo);
            return ttemp;
        }
    }TLBtroca(head, tlhead, *indice);
}

TLB *TLBtroca(Node *head, TLB *tlhead, int indice){
    int i = 0;
    while(i < indice && tlhead != NULL){
        tlhead = tlhead->prox;
        i++;
    }strcpy(tlhead->pagenum, head->page);
    return tlhead;
}

int qtdendereco(FILE *dados) {
    int linhas = 0;
    char ch;

    while ((ch = fgetc(dados)) != EOF) {
        if (ch == '\n') {
            linhas++;
        }
    }
    linhas--;
    return linhas;
}

void pageAtt(TLB *head, PageTableEntry *phead, int tempo){
    int ptp, tlbp;
   tlbp = binarioParaInteiro(head->pagenum);
    while(phead != NULL){
        ptp = binarioParaInteiro(phead->page);
        if(ptp == tlbp){
            (*phead).lastused = tempo;
        }
        phead = phead->prox;
    }
}
