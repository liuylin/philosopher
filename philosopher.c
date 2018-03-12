/*************************************************************************
	> File Name: philosopher.c
	> Author: 
	> Mail: 
	> Created Time: Fri 05 Jan 2018 06:50:30 PM PST
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<sys/ipc.h>
#include<sys/sem.h>
#include<string.h>
#include<unistd.h>
#include<time.h>

int semid;

union semun
{
    int val;
};

void wait_for_2fork(int no)
{
    int left = no;
    int right = (no+1)%5;
    struct sembuf sbs[2] = {
        {left,-1,0},
        {right,-1,0},
    };
    semop(semid,sbs,2);
}

void wait_for_1fork(int no)
{
    struct sembuf sbs[1] = {no,-1,0};
    semop(semid,sbs,1);
}

void put_for_2fork(int no)
{
    int left = no;
    int right = (no+1)%5;
    struct sembuf sbs[2] = {
        {left,1,0},
        {right,1,0},
    };
    semop(semid,sbs,2);
}

void philosopher(int no)
{
    while(1)
    {
        printf("%d thinking\n",no);
        sleep(rand()%5);
        printf("%d hungry\n",no);
        wait_for_1fork(no);
        sleep(rand()%2);
        wait_for_1fork((no+1)%5);
      //  wait_for_2fork(no);
        printf("%d eating\n",no);
        sleep(rand()%3);
        put_for_2fork(no);
        sleep(rand()%3);
    }
}

int main()
{
    srand(time(NULL));
    semid = semget(IPC_PRIVATE,5,IPC_PRIVATE|0666);
    if(semid == -1)
    {
        perror("semget");
        exit(1);
    }
    union semun su;
    su.val = 1;
    int i;
    for(i = 0;i<5;++i)
    {
        semctl(semid,i,SETVAL,su);
    }
    int no = 0;
    for(i = 1;i<5;++i)
    {
        pid_t pid = fork();
        if(pid == 0)
        {
            no = i;
            break;
        }
    }
    philosopher(no);
}
