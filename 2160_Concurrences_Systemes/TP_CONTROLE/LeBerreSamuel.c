/**
 * autheur Samuel LE BERRE
 */
#include <unistd.h> // appel systeme fork
#include <fcntl.h>  // appel system unix ES
#include <stdio.h> // librairie standard C
#include <stdlib.h> // exit
#include <sched.h>  // sche_yield
#include <sys/types.h>
#include <sys/sem.h> // semaphore IPC
#include <sys/ipc.h> // services IPC
#include <sys/wait.h> // wait
#include <time.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <sched.h>
#include <sys/ipc.h> // services IPC
#include <sys/resource.h>
#include <sys/shm.h>
#include <sys/sem.h> // semaphore IPC
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MUTEX 0
#define ESCLAVE 1
#define ELEMENT_TAB 2
#define NB_ELEMENT 10

key_t cle; /* cle ipc */


int nb_esclave;

int semid; /* nom local de l'ensemble des semaphores */

int shmid;

int * tab;

struct sembuf op;

void maitre(){
    char c;
    int rep;
    int delai;
    while ( scanf(" %c%u%u",&c, &rep,&delai) == 3){
        
        //printf("P(&MUTEX)\n");
        op.sem_num=MUTEX;op.sem_op=-1;op.sem_flg=0;
        semop(semid,&op,1);

        //printf("V(&ELEMENT_TAB)\n");
        op.sem_num=ELEMENT_TAB;op.sem_op=-1;op.sem_flg=0;
        semop(semid,&op,1);
        
        if(*(tab  = (int*) shmat(shmid,NULL,0)) == -1){
            perror("probleme shmat");
            exit(4);
        }

        int pos = 2 + tab[0]*3;
        tab[0] = (tab[0] +1) % NB_ELEMENT;

        tab[pos] = (int)c;
        tab[pos+1] = rep;
        tab[pos+2] = delai;

        if(shmdt(tab) == -1){
            perror("probleme sur shmdt");
            exit(4);
        }

        

        //printf("V(&ESCLAVE)\n");
        op.sem_num=ESCLAVE;op.sem_op=1;op.sem_flg=0;
        semop(semid,&op,1);

        //printf("V(&MUTEX)\n");
        op.sem_num=MUTEX;op.sem_op=1;op.sem_flg=0;
        semop(semid,&op,1);
    }
}

void affichage(char letter, int rep, int delai){
    int x;
    for(x=0; x<rep; x++){
        write(1,&letter,1);
        printf("\n");
        //sched_yield();
        sleep(delai);
    }
}

void esclave(){

    while(1){

        printf("pid : %d P(&ESCLAVE)\n", getpid());
        op.sem_num=ESCLAVE;op.sem_op=-1;op.sem_flg=0;
        semop(semid,&op,1);

        //printf("P(&MUTEX)\n");
        op.sem_num=MUTEX;op.sem_op=-1;op.sem_flg=0;
        semop(semid,&op,1);

        if(*(tab  = (int*) shmat(shmid,NULL,0)) == -1){
            perror("probleme shmat");
            exit(4);
        }

        int posLect = 2 + tab[1] * 3;

        tab[1] = (tab[1] +1) % NB_ELEMENT;


        char letter = tab[posLect];
        int rep = tab[posLect+1];
        int delai = tab[posLect+2];
        

        if(shmdt(tab) == -1){
            perror("probleme sur shmdt");
            exit(4);
        }

        //printf("V(&MUTEX)\n");
        op.sem_num=MUTEX;op.sem_op=1;op.sem_flg=0;
        semop(semid,&op,1);

        //printf("V(&ELEMENT_TAB)\n");
        op.sem_num=ELEMENT_TAB;op.sem_op=1;op.sem_flg=0;
        semop(semid,&op,1);

        affichage(letter,rep,delai);
    }
}

int main(int argc, char * argv[]){

    nb_esclave = atoi(argv[1]);

    printf("nb esclave : %d", nb_esclave);
    //init sem
    ushort init_sem[3]={1,0,NB_ELEMENT};

    // creation d'une cle IPC en fonction du nom du programme
    if ((cle=ftok(argv[0],'0')) == -1 ) {
        fprintf(stderr,"Probleme sur ftoks\n");
        exit(1);
    }

    // demande un ensemble de semaphore (ici deux mutex)
    if ((semid=semget(cle,3,IPC_CREAT|0666))==-1) {
        fprintf(stderr,"Probleme sur semget\n");
        exit(2);
    }

    // initialise l'ensemble
    if (semctl(semid,3,SETALL,init_sem)==-1) {
        fprintf(stderr,"Probleme sur semctl SETALL\n");
        exit(3);
    }

    key_t cle2;
    if ((cle2=ftok(argv[0],'1')) == -1 ) {
        fprintf(stderr,"Probleme sur ftoks\n");
        exit(1);
    }
    //Allocation espace necessaire tab
    if((shmid = shmget(cle2, 4096, IPC_CREAT | 0666)) == -1){
        perror("shmget");
        exit(1);
    }

    if(*(tab  = (int*) shmat(shmid,NULL,0)) == -1){
        perror("probleme shmat");
        exit(4);
    }
    
    int x;
    for(x=0; x< ((NB_ELEMENT*3)+2); x++){
        tab[x] = 0;
    }
    printf("1\n");
    if(shmdt(tab) == -1){
        perror("probleme sur shmdt");
        exit(4);
    }


    int i =0;
    int pid;
    if((pid = fork()) == 0){
        while(i<nb_esclave){
            if((pid = fork()) == 0){
                esclave();
                exit(0);
            }
            i++;
        }
    }else{
        maitre();
    }
    int nb;
    for(nb=0; nb < nb_esclave; nb++){
        
        wait(NULL);
    }



}