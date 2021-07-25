#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define maxchaves 4
#define NAO_ENCONTRADO 0
#define ENCONTRADO 1
#define PROMOCAO 1
#define SEM_PROMOCAO 0
#define ERRO -1
#define VAZIO 0
#define NIL -1
// FINALIZADO DIVIDE

FILE* fdtree, *fd;

typedef struct no{
    int contachaves;
    int chave[maxchaves];
    int offset[maxchaves];
    int filho[maxchaves+1];
} pagina;

typedef struct nos{
    int contachaves;
    int chave[maxchaves+1];
    int offset[maxchaves+1];
    int filho[maxchaves+2];
} paginaaux;

int RRN_novapag(){
    int NRRN,upos;
    fseek(fdtree, 0,SEEK_END); // Ando o ponteiro para o final
    upos=ftell(fdtree); // Função de captura do ultimo byte-offset
    NRRN=((upos-4)/56); //Novo RRN
    return NRRN;
}

void le_pagina(int RRN, pagina *PAG){
    fseek(fdtree, sizeof(pagina)*RRN+4, SEEK_SET);
    fread(PAG, sizeof(pagina), 1, fdtree);
}

void escreve_pagina(int RRN, pagina PAG){
    fseek(fdtree, sizeof(pagina)*RRN+4, SEEK_SET);
    fwrite(&PAG, sizeof(pagina), 1, fdtree);
}

void inicializa_pagina(pagina *PAG){
    PAG->contachaves=0;
    for(int i=0;i<=maxchaves-1;i++){
        PAG->chave[i]=-1;
        PAG->offset[i]=-1;
        PAG->filho[i]=-1;
    }
    PAG->filho[maxchaves]=-1;
}

void insere_na_pagina(int CHAVE, int FILHO_D, int OFFSET, pagina *PAG){
    int i=PAG->contachaves;
    while(i>0 && CHAVE<PAG->chave[i-1]){
        PAG->chave[i]=PAG->chave[i-1];
        PAG->offset[i]=PAG->offset[i-1];
        PAG->filho[i+1]=PAG->filho[i];
        i--;
    }
    PAG->contachaves++;
    PAG->chave[i]=CHAVE;
    PAG->filho[i+1]=FILHO_D;
    PAG->offset[i]=OFFSET;
}

int busca_na_pagina(int CHAVE, pagina PAG, int *POS){
    int i=0;
    while(i<PAG.contachaves && CHAVE>PAG.chave[i])
        i++;
    *POS=i;
    if(*POS<PAG.contachaves && CHAVE==PAG.chave[*POS]){
        return ENCONTRADO;
    }else return NAO_ENCONTRADO;
}

void divide (int CHAVE, int FILHO_D,int OFFSET, pagina *PAG,int *CHAVE_PRO,int *OFFSET_PRO, int *FILHO_D_PRO,pagina *NOVAPAG){
    paginaaux PAGAUX;
    int i=0,j=0,NRRN;
    PAGAUX.chave[i]=PAG->chave[i];
    for (i=0;PAG->chave[i]<CHAVE;i++){
        PAGAUX.chave[i]=PAG->chave[i];
        PAGAUX.offset[i]=PAG->offset[i];
        PAGAUX.filho[i]=PAG->filho[i];
    }
    PAGAUX.chave[i]=CHAVE;
    PAGAUX.offset[i]=OFFSET;
    PAGAUX.filho[i]=FILHO_D;
    for (i;i<4;i++){
        PAGAUX.chave[i+1]=PAG->chave[i];
        PAGAUX.offset[i+1]=PAG->offset[i];
        PAGAUX.filho[i+1]=PAG->filho[i];
    }

    for (i=0;i<5;i++){
        printf("PAGAUX.chave[%d]:%d\n",i,PAGAUX.chave[i]);
        printf("PAGAUX.offset[%d]:%d\n",i,PAGAUX.offset[i]);
        printf("PAGAUX.filho[%d]:%d\n",i,PAGAUX.filho[i]);
    } 
    inicializa_pagina(NOVAPAG);
    printf ("%d",*OFFSET_PRO);
    *CHAVE_PRO=PAGAUX.chave[2];
    *OFFSET_PRO=PAGAUX.offset[2];
    NRRN=RRN_novapag();
    *FILHO_D_PRO=NRRN;
    for (i=2;i<4;i++){
            PAG->chave[i]=-1;
            PAG->offset[i]=-1;
            NOVAPAG->chave[j]=PAGAUX.chave[i+1];
            NOVAPAG->offset[j]=PAGAUX.offset[i+1];
            j++;
     }*/
}

int insere(int RRN_ATUAL, int CHAVE, int OFFSET, int *FILHO_D_PRO, int *CHAVE_PRO, int *OFFSET_PRO){
    pagina PAG, NOVAPAG;
    int POS, RETORNO, RRN_PRO, CHV_PRO, OFF_PRO;
    if(RRN_ATUAL==-1){
        *CHAVE_PRO=CHAVE;
        *FILHO_D_PRO=-1;
        *OFFSET_PRO=OFFSET;
        return PROMOCAO;
    }else{
        le_pagina(RRN_ATUAL, &PAG);
        if(busca_na_pagina(CHAVE, PAG, &POS)==ENCONTRADO){
            printf("Chave duplicada!\n");
            return ERRO;
        }
    }
    RETORNO=insere(PAG.filho[POS], CHAVE, OFFSET, &RRN_PRO, &CHV_PRO, &OFF_PRO);
    if(RETORNO==SEM_PROMOCAO || RETORNO==ERRO){
        return RETORNO;
    }else{
        if(PAG.contachaves<4){
            insere_na_pagina(CHV_PRO, RRN_PRO, OFF_PRO, &PAG);
            escreve_pagina(RRN_ATUAL, PAG);
            return SEM_PROMOCAO;
        }else{
            divide(CHV_PRO, RRN_PRO, OFF_PRO, &PAG, CHAVE_PRO, FILHO_D_PRO, OFFSET_PRO, &NOVAPAG);
            escreve_pagina(RRN_ATUAL, PAG);
            escreve_pagina(*FILHO_D_PRO, NOVAPAG);
            return PROMOCAO;
        }
    }
}

void gerencia (){
    int RAIZ=0,CHAVE,i,OFFSET=4,*FILHO_D_PRO,*CHAVE_PRO,*OFFSET_PRO,RRN;
    short int tam_reg;
    char key[5],test;
    pagina NOVPAG;
    if((fdtree=fopen("btree.dat", "r+"))!=NULL){
        fread(&RAIZ, sizeof(int), 1, fdtree);
    }
    else{
        if((fdtree=fopen("btree.dat", "a+b"))==NULL){ 
            printf("Erro na abertura de btree.dat\n");
            exit(1);
        }
        inicializa_pagina(&NOVPAG);
        fwrite(&RAIZ, sizeof(int), 1, fdtree);
        escreve_pagina(RAIZ,NOVPAG);
    }
    fseek(fd,4, SEEK_SET);
    fread(&tam_reg, sizeof(short int), 1, fd); 
    while((key[i]=fgetc(fd))!='|' && feof(fd)!= 0)// função feof = 0 qr dzer q não está no final de arq
        i++;
    key[i]='\0';
    CHAVE=atoi(key);
    while(CHAVE!=-1){
        if(insere(RAIZ,CHAVE,OFFSET,FILHO_D_PRO,CHAVE_PRO,OFFSET_PRO)==PROMOCAO){
            inicializa_pagina(&NOVPAG);
            NOVPAG.chave[0]=*CHAVE_PRO;
            NOVPAG.offset[0]=*OFFSET_PRO;
            NOVPAG.filho[0]=RAIZ;
            NOVPAG.filho[1]=*FILHO_D_PRO;
            NOVPAG.contachaves++;
            RRN=RRN_novapag();
            escreve_pagina(RRN,NOVPAG);
            RAIZ=RRN;
        }
        OFFSET=OFFSET+tam_reg;
        fseek(fd,OFFSET, SEEK_SET);
        fread(&tam_reg, sizeof(short int), 1, fd);
        while((key[i]=fgetc(fd))!='|' && feof(fd)!= 0)
        i++;
        key[i]='\0';
        CHAVE=atoi(key);
    }
    fseek(fdtree,0, SEEK_SET);
    fwrite(&RAIZ, sizeof(int), 1, fdtree);
}


int busca(int RRN, int CHAVE, int *RRN_ENCONTRADO, int *POS_ENCONTRADA){
    pagina PAG;
    int i, POS=0;
    if(RRN==-1){
        return NAO_ENCONTRADO;
    }else{
        le_pagina(RRN, &PAG);
        if(busca_na_pagina(CHAVE, PAG, &POS)==ENCONTRADO){
            *RRN_ENCONTRADO=RRN;
            *POS_ENCONTRADA=POS;
            return ENCONTRADO;
        }else return(busca(PAG.filho[POS], CHAVE, RRN_ENCONTRADO, POS_ENCONTRADA));
    }
}


int main(int argc, char **argv) {
    int RAIZ=0,*FILHO_D_PRO,  *CHAVE_PRO,  *OFFSET_PRO;
    pagina pg,NOVAPAG;
    if((fd=fopen("dados.dat", "r"))==NULL){
        printf("Erro na abertura de dados.dat\n");
        exit(1);
    }
    if((fdtree=fopen("btree.dat", "w+b"))==NULL){ // estava como w, problema w destroi o arq anterior!
        printf("Erro na abertura de btree.dat\n");
        exit(1);
    }
    fwrite(&RAIZ, sizeof(int), 1, fdtree);
    inicializa_pagina(&pg);
    escreve_pagina(0,pg);
    insere(0,  23, 4, FILHO_D_PRO, CHAVE_PRO,OFFSET_PRO);
    insere(0,  98, 323, FILHO_D_PRO, CHAVE_PRO,OFFSET_PRO);
    insere(0,  33, 2342, FILHO_D_PRO, CHAVE_PRO,OFFSET_PRO);
    insere(0,  44, 2342, FILHO_D_PRO, CHAVE_PRO,OFFSET_PRO);
    le_pagina(0,&pg);
    //divide (int CHAVE, int FILHO_D,int OFFSET, pagina *PAG,int *CHAVE_PRO,int *OFFSET_PRO, int *FILHO_D_PRO,pagina *NOVAPAG)
    divide ( 12, -1, 33 ,  &pg, CHAVE_PRO, OFFSET_PRO,  FILHO_D_PRO, &NOVAPAG);
    fclose(fd);
    fclose(fdtree);
    return 0;
}
