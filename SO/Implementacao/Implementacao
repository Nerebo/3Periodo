#include <stdio.h>
#include <malloc.h>

#ifdef EDF_FLAG
#define ESCOLHA 'E'
#elif RATE_FLAG
#define ESCOLHA 'R'
#else
#define ESCOLHA 'D'
#endif

struct Processo{

    char nome[50];
    int periodo;
    int periodoa;
    int numburst;
    int burstt;
    int tempou;
    int tempochegada;
    int hasfinished;
    int prioridade;
    int lost;
    int killed;
    int finalized;

};

int tempot;
int tempoa = 0;
int i = 0;
int idle = 0;
int x = 0;
int previ = -1;

char alg = ESCOLHA;

// funções gerais
void processoinit(struct Processo vetor[], int n);
void killed(struct Processo vetor[], int n, int tempoa, int tempot, int previ, FILE *fp);
void lost(struct Processo vetor[],int n, int tempoa, int tempot, FILE *fp);
void finish(struct Processo vetor[], int n, int tempoa, int tempot, FILE *fp);
void hold(struct Processo vetor[], int n, int tempoa, int tempot, int previ, FILE *fp);
void reset(struct Processo vetor[], int n, int tempoa, int tempot);
void lerArquivo(FILE *dados, struct Processo vetor[], int n);
int qtdprocessos(FILE *dados);

// EDF
int prioedf(struct Processo vetor[], int n, int tempoa);
int edf(struct Processo vetor[], int n, int tempoa, int tempot);

// RATE
int priorate(struct Processo vetor[], int n, int tempoa);
int rate(struct Processo vetor[], int n, int tempoa, int tempot);



int main(int argc, char *argv[]) {

    FILE *dados = fopen(argv[1], "r");
    int n = qtdprocessos(dados)+1;

    fseek(dados, 0, 0);
    struct Processo *vetor = (struct Processo *)malloc(n * sizeof(struct Processo));

    int primeiraLinha = 1, i = 0, tempo, periodo;
    while (!feof(dados)) {
        if (primeiraLinha) {
            fscanf(dados, "%d", &tempo);
            tempot = tempo;
            primeiraLinha = 0;
        } else {
            i++;
            fscanf(dados, "%s %d %d", vetor[i].nome, &periodo, &vetor[i].numburst);
            vetor[i].periodo = periodo;
        }
    }
    fclose(dados);

    processoinit(vetor, n);


    if(alg == 'R') {
        rate(vetor, n, tempoa, tempot);
    }else if(alg == 'E') {
        edf(vetor, n, tempoa, tempot);
    }

    rate(vetor, n, tempoa, tempot);

    return 0;
}

void lerArquivo(FILE *dados, struct Processo vetor[], int n) {
    int primeiraLinha = 1, i = 0, tempo, periodo;
    while (!feof(dados)) {
        if (primeiraLinha) {
            fscanf(dados, "%d", &tempo);
            tempot = tempo;
            primeiraLinha = 0;
        } else {
            i++;
            fscanf(dados, "%s %d %d", vetor[i].nome, &periodo, &vetor[i].numburst);
            vetor[i].periodo = periodo;
        }
    }
    fclose(dados);
}

void processoinit(struct Processo vetor[], int n){
    for (int j = 0; j < n; j++) {
        vetor[j].periodoa = vetor[j].periodo;
        vetor[j].burstt = vetor[j].numburst;
        vetor[j].finalized = 0;
        vetor[j].tempou = 0;
        vetor[j].lost = 0;
        vetor[j].killed = 0;
        vetor[j].hasfinished = 0;
        vetor[j].tempochegada = 0;
    }

}

void killed(struct Processo vetor[], int n, int tempoa, int tempot, int previ, FILE *fp){
    for (int j = 0; j < n; ++j) {
        if(vetor[j].numburst >= 0 && tempoa == tempot && vetor[j].tempou != 0) {
            vetor[j].killed += 1;
            if(previ == j && j!=0){
                fprintf(fp, "[%s] for %d units - K\n", vetor[j].nome, vetor[j].tempou);

            }else{
                fprintf(fp, "idle for %d units", vetor[j].tempou);
            }
        }else if(vetor[j].periodoa == tempoa){
            vetor[j].killed += 1;
        }
    }
}


void lost(struct Processo vetor[],int n, int tempoa, int tempot, FILE *fp){
    for (int j = 0; j < n; ++j) {
        if(tempoa !=0){
            if(tempoa == vetor[j].periodoa && vetor[j].numburst>0) {
                vetor[j].lost += 1;
                if(vetor[j].tempou != 0) {
                    fprintf(fp, "[%s] for %d units - L\n", vetor[j].nome, vetor[j].tempou);
                }
                vetor[j].tempou = 0;
            }
        }
    }
}

/*void lost(Processo *perdido, Processo *processoatual){
    perdido->lost++;
    if(strcmp(perdido->nome,  processoatual->nome)){
        printf("faz o L")
    }
}*/

void finish(struct Processo vetor[], int n, int tempoa, int tempot, FILE *fp){
    for (int j = 0; j < n; ++j) {
        if(tempoa != 0) {
            if (vetor[j].numburst == 0 && vetor[j].hasfinished == 0) {
                vetor[j].finalized += 1;
                vetor[j].hasfinished = 1;
                if(vetor[j].tempou > 0) {
                    fprintf(fp, "[%s] for %d units - F\n", vetor[j].nome, vetor[j].tempou);
                }
                vetor[j].tempou = 0;
            }
        }
    }
}

void hold(struct Processo vetor[], int n, int tempoa, int tempot, int previ, FILE *fp) {
    for (int j = 0; j < n; ++j) {
        if (tempoa != 0) {
            if (vetor[j].numburst > 0 && previ == j && vetor[j].tempou != 0) {
                struct Processo debug = vetor[j];
                if(j!=0) {
                    fprintf(fp, "[%s] for %d units - H\n", vetor[j].nome, vetor[j].tempou);
                }else{
                    fprintf(fp, "idle for %d units\n", vetor[j].tempou);
                }
                vetor[j].tempou = 0;
            }
        }
    }
}

void reset(struct Processo vetor[], int n, int tempoa, int tempot){
    for (int j = 0; j < n; ++j) {
        struct Processo *debug = &vetor[j];
        if(tempoa !=0) {
            //tempoa % vetor[j].periodo == 0
            if (tempoa == vetor[j].periodoa) {
                vetor[j].numburst = vetor[j].burstt;
                vetor[j].periodoa += vetor[j].periodo;
                vetor[j].hasfinished = 0;
            }
        }
    }
}

int priorate(struct Processo vetor[], int n, int tempoa){
    int aux=-1;
    int priority;
    int flag=1;

    for (int j = 0; j < n; ++j) {
        vetor[j].prioridade = 0;
    }

    for (int j = 0; j < n; ++j) {
        if(vetor[j].numburst > 0){
            if(flag) {
                priority = vetor[j].periodo;
                flag = 0;
                aux=j;
            }else{
                if (vetor[j].periodo< priority) {
                    priority = vetor[j].periodo;
                    aux = j;
                }
            }
        }
    }vetor[aux].prioridade = 1;
    return 0;
}


int rate(struct Processo vetor[], int n, int tempoa, int tempot){
    processoinit(vetor, n);
    priorate(vetor, n, tempoa);

    FILE *fp = fopen("rate.out", "w");

    fprintf(fp, "EXECUTION BY RATE\n");

    while (tempoa < tempot) {
        struct Processo *processo1 = &vetor[0];
        struct Processo *processo2 = &vetor[1];
        i = 0;

        lost(vetor, n, tempoa, tempot, fp);
        reset(vetor, n, tempoa, tempot);
        priorate(vetor, n, tempoa);

        while (i < n) {
            struct Processo *debug = &vetor[i];
            if (vetor[i].prioridade == 1 && vetor[i].numburst > 0){

                if(previ != i) {
                    hold(vetor, n, tempoa, tempot, previ, fp);
                }
                vetor[i].numburst -=1;
                vetor[i].tempou += 1;
                previ = i;
                x = 1;
            }
            finish(vetor, n, tempoa, tempot, fp);
            i++;
        }

        if(x == 0){
            idle++;
        }
        if(x == 1 && idle > 0){
            fprintf(fp, "idle for %d units\n", idle);
            idle = 0;
        }

        x = 0;
        tempoa++;

        if(tempoa == tempot && idle > 0){
            fprintf(fp, "idle for %d units\n", idle);
            idle = 0;
        }

    }

    lost(vetor, n, tempoa, tempot, fp);
    killed(vetor, n, tempoa, tempot, previ, fp);

    fprintf(fp, "\nLOST DEADLINES\n");
    for(int j = 1; j < n; ++j){
        fprintf(fp, "[%s] %d\n", vetor[j].nome ,vetor[j].lost);
    }
    fprintf(fp, "\n");


    fprintf(fp, "COMPLETE EXECUTION\n");
    for(int j = 1; j < n; ++j){
        fprintf(fp, "[%s] %d\n", vetor[j].nome ,vetor[j].finalized);
    }
    fprintf(fp, "\n");

    fprintf(fp, "KILLED\n");
    for(int j = 1; j < n; ++j){
        fprintf(fp, "[%s] %d", vetor[j].nome ,vetor[j].killed);
        if(j<n-1){
            fprintf(fp, "\n");
        }
    }

    return 0;
}

int qtdprocessos(FILE *dados) {
    int linhas = 0;
    char ch;

    while ((ch = fgetc(dados)) != EOF) {
        if (ch == '\n') {
            linhas++;
        }
    }

    return linhas;
}


int prioedf(struct Processo vetor[], int n, int tempoa){
    int aux=-1;
    int priority;
    int flag=1;

    for (int j = 0; j < n; ++j) {
        vetor[j].prioridade = 0;
    }

    for (int j = 0; j < n; ++j) {
        if(vetor[j].numburst > 0){
            if(flag) {
                priority = vetor[j].periodoa;
                flag = 0;
                aux=j;
            }else{
                if (vetor[j].periodoa < priority) {
                    priority = vetor[j].periodoa;
                    aux = j;
                }
            }
        }
    }vetor[aux].prioridade = 1;
    return 0;
}

int edf(struct Processo vetor[], int n, int tempoa, int tempot){
    processoinit(vetor, n);
    prioedf(vetor, n, tempoa);

    FILE *fp = fopen("edf.out", "w");

    fprintf(fp, "EXECUTION BY EDF\n");

    while (tempoa < tempot) {
        struct Processo *processo1 = &vetor[0];
        struct Processo *processo2 = &vetor[1];
        i = 0;

        lost(vetor, n, tempoa, tempot, fp);
        reset(vetor, n, tempoa, tempot);
        prioedf(vetor, n, tempoa);

        while (i < n) {
            struct Processo *debug = &vetor[i];
            if (vetor[i].prioridade == 1 && vetor[i].numburst > 0){

                if(previ != i) {
                    hold(vetor, n, tempoa, tempot, previ, fp);
                }

                vetor[i].numburst -=1;
                vetor[i].tempou += 1;
                previ = i;
            }
            finish(vetor, n, tempoa, tempot, fp);
            i++;
        }

        if(x == 0){
            idle++;
        }
        if(x == 1 && idle > 0){
            fprintf(fp, "idle for %d units\n", idle);
            idle = 0;
        }

        x = 0;
        tempoa++;
    }

    lost(vetor, n, tempoa, tempot, fp);
    killed(vetor, n, tempoa, tempot, previ, fp);

    fprintf(fp, "\nLOST DEADLINES\n");
    for(int j = 1; j < n; ++j){
        fprintf(fp, "[%s] %d\n", vetor[j].nome ,vetor[j].lost);
    }
    fprintf(fp, "\n");


    fprintf(fp, "COMPLETE EXECUTION\n");
    for(int j = 1; j < n; ++j){
        fprintf(fp, "[%s] %d\n", vetor[j].nome ,vetor[j].finalized);
    }
    fprintf(fp, "\n");

    fprintf(fp, "KILLED\n");
    for(int j = 1; j < n; ++j){
        fprintf(fp, "[%s] %d", vetor[j].nome ,vetor[j].killed);
        if(j<n-1){
            fprintf(fp, "\n");
        }
    }

    return 0;
}
