#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>    //fork
#include <sys/wait.h>  //waitpid
#include <string.h>    //strcmp
#include <fcntl.h>     //open
#include <sys/types.h> //open
#include <sys/msg.h>   //mq
#include <sys/shm.h>   //shared mem
#include <sys/sem.h> //semaphor
#include <errno.h>
#include <time.h>
#include <stdbool.h>

void handler()
{
    printf("A borvetelezes elindult\n");
}

int main(int argc, char **argv)
{

    signal(SIGTERM, handler);

    pid_t krampusz;
    pid_t krampusz2;

    struct Borszallitmany
    {
        char borvidek[30];
        int liter;
    };

    // pipe //
    int mikulas_to_krampusz1[2];
    int mikulas_to_krampusz2[2];
    pipe(mikulas_to_krampusz1);
    pipe(mikulas_to_krampusz2);
    //FONTOS: close(pipefd[0]) es/vagy close(pipefd[1])

    // mq //
    key_t key = ftok(argv[0], 1);
    int mq = msgget(key, 0600 | IPC_CREAT);
    
    struct Borminosites
    {
        long mtype;
        char minosites[30];
    };
   //FONTOS: msgctl(mq, IPC_RMID, NULL);

    //shared mem //
    int sh_mem_id;
    int sh_mem_id2;
    char *liter;
    char *liter2;
    key_t sh_mem_key = ftok(argv[0], 2);
    key_t sh_mem_key2 = ftok(argv[0], 3);
    sh_mem_id = shmget(sh_mem_key, 500, IPC_CREAT | S_IRUSR | S_IWUSR);
    sh_mem_id2 = shmget(sh_mem_key2, 500, IPC_CREAT | S_IRUSR | S_IWUSR);
    liter = shmat(sh_mem_id, NULL, 0);
    liter2 = shmat(sh_mem_id2, NULL, 0);
    //FONTOS: shmdt(s); shmctl(oszt_mem_id, IPC_RMID, NULL);

    // semaphore //
    int semid;
    key_t kulcs;
    kulcs = ftok(argv[0], 1);
    semid = semget(kulcs, 1, IPC_CREAT | S_IRUSR | S_IWUSR);
    semctl(semid, 0, SETVAL, 0);

    void szemafor_muvelet(int semid, int op)
    {
        struct sembuf muvelet;

        muvelet.sem_num = 0;
        muvelet.sem_op = op; // op=1 up, op=-1 down
        muvelet.sem_flg = 0;

        if (semop(semid, &muvelet, 1) < 0) // 1 number of sem. operations
            perror("semop");
    }
    //FONTOS: semctl(semid, 0, IPC_RMID);

    krampusz = fork();
    if (krampusz > 0)
    {
        krampusz2 = fork();
        if (krampusz2 > 0) // szulo
        {
            close(mikulas_to_krampusz1[0]);
            close(mikulas_to_krampusz2[0]);
            int hanyfore = atoi(argv[1]);
            printf("Ugy tunik %i fovel kell szamolnom\n", hanyfore);
            char *borvidekek[] = {"Szekszardi" , "Badacsonyi" , "Egri" , "Tokaji", "Somlo"};
            srand(time(NULL));
            int random1 = rand() % 5;
            int random2 = rand() % 5;
            struct Borszallitmany szallitmany1;
            struct Borszallitmany szallitmany2;
            szallitmany1.liter=hanyfore;
            szallitmany2.liter=hanyfore;
            printf("Mikulas: A szukseges bor mennyisege: %il\n", szallitmany1.liter);
            strcpy(szallitmany1.borvidek, borvidekek[random1]);
            printf("Mikulas:Az elso borvidek: %s\n", szallitmany1.borvidek);
            strcpy(szallitmany2.borvidek, borvidekek[random2]);
            printf("Mikulas: A masodik borvidek: %s\n", szallitmany2.borvidek);
            write(mikulas_to_krampusz1[1], &szallitmany1, sizeof(struct Borszallitmany));
            write(mikulas_to_krampusz2[1], &szallitmany2, sizeof(struct Borszallitmany));
            printf("Mikulas var\n");
            pause();
            printf("Uzente Krampusz1 a %s teruletrol\n", szallitmany1.borvidek);
            pause();
            printf("Uzente Krampusz2 a %s teruletrol\n", szallitmany2.borvidek);
            struct Borminosites borminosites;
            msgrcv(mq, &borminosites, sizeof(struct Borminosites)-sizeof(long), 1, 0);
            printf("Az elso krampuszom minositese: %s\n", borminosites.minosites);
            msgrcv(mq, &borminosites, sizeof(struct Borminosites)-sizeof(long), 2, 0);
            printf("A masodik krampuszom minositese: %s\n", borminosites.minosites);
            msgctl(mq, IPC_RMID, NULL);
            wait(NULL);
            wait(NULL);
            szemafor_muvelet(semid, -1);
            if (strcmp(szallitmany1.borvidek, szallitmany2.borvidek)==0)
            {
                printf("Osszesen %i liter bor erkezett a %s borvidekrol\n", atoi(liter)+atoi(liter2), szallitmany1.borvidek);
            }
            else
            {
                printf("Osszesen %i liter bor erkezett a %s borvidekrol\n", atoi(liter), szallitmany1.borvidek);
                printf("Osszesen %i liter bor erkezett a %s borvidekrol\n", atoi(liter2), szallitmany2.borvidek);
            }
            szemafor_muvelet(semid, 1);
            shmdt(liter);
            shmdt(liter2);
            shmctl(sh_mem_id, IPC_RMID, NULL);
            shmctl(sh_mem_id2, IPC_RMID, NULL);
            semctl(semid, 0, IPC_RMID);
        }
        else // gyerek 2
        {
            close(mikulas_to_krampusz1[0]);
            close(mikulas_to_krampusz1[1]);
            close(mikulas_to_krampusz2[1]);
            struct Borszallitmany szallitmany;
            read(mikulas_to_krampusz2[0], &szallitmany, sizeof(struct Borszallitmany));
            printf("Krampusz2: A borvidek: %s, ahonnan %i liter bort kell hozzak\n", szallitmany.borvidek, szallitmany.liter);
            sleep(5);
            kill(getppid(), SIGTERM);
            srand(time(NULL));
            int random = rand() % 2;
            struct Borminosites borminosites;
            borminosites.mtype = 2;
            if(random == 0){
                strcpy(borminosites.minosites, "ezust");
            }
            else{
                strcpy(borminosites.minosites, "arany");
            }
            printf("Krampusz 2: A minositesem %s\n", borminosites.minosites);
            if(strcmp(borminosites.minosites, "arany")==0){
                float h = szallitmany.liter*1.5;
                sprintf(liter2, "%f", h);
                printf("A masodik krampusz inkabb %f litert hoz\n", liter2);
            }
            else if(strcmp(borminosites.minosites, "ezust")==0){
                float h = szallitmany.liter;
                sprintf(liter2, "%f", h);
                printf("A masodik krampusz %f litert hoz\n", liter2);
            }
            szemafor_muvelet(semid, 1);
            msgsnd(mq, &borminosites, sizeof(struct Borminosites)-sizeof(long), 0);
        }
    }
    else // gyerek
    {
        close(mikulas_to_krampusz2[0]);
        close(mikulas_to_krampusz2[1]);
        close(mikulas_to_krampusz1[1]);
        struct Borszallitmany szallitmany;
        read(mikulas_to_krampusz1[0], &szallitmany, sizeof(struct Borszallitmany));
        printf("Krampusz1: A borvidek: %s, ahonnan %i liter bort kell hozzak\n", szallitmany.borvidek, szallitmany.liter);
        sleep(3);
        kill(getppid(), SIGTERM);
        srand(time(NULL));
        int random = rand() % 2;
        struct Borminosites borminosites;
        borminosites.mtype = 1;
        if(random == 0){
            strcpy(borminosites.minosites, "ezust");
        }
        else{
            strcpy(borminosites.minosites, "arany");
        }
        printf("Krampusz 1: A minositesem %s\n", borminosites.minosites);
        szemafor_muvelet(semid, -1);
        if(strcmp(borminosites.minosites, "arany")==0){
                float h = szallitmany.liter*1.5;
                sprintf(liter, "%f", h);
                printf("Az elso krampuszom inkabb %s litert hoz\n", liter);
                
        }
        else if(strcmp(borminosites.minosites, "ezust")==0){
                float h = szallitmany.liter;
                sprintf(liter, "%f", h);
                printf("Az elso krampuszom %s litert hoz\n", liter);
        }
        szemafor_muvelet(semid, 1);
        msgsnd(mq, &borminosites, sizeof(struct Borminosites)-sizeof(long), 0);
    }
    return 0;
}