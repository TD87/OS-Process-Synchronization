#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
#include<semaphore.h>

void * player(void *);
void * organizer(void *);
void * referee(void *);

int n;
sem_t sem_2player;
sem_t sem_1referee;
sem_t sem_match_start;
sem_t sem_player_ready;
sem_t sem_referee_wait;
sem_t sem_organizer_wait;
sem_t sem_organizer_ready;
sem_t sem_organizer_confirm;

int main(){
    printf("Enter value for n: ");
    scanf("%d",&n);
    pthread_t otid;
    pthread_t * ptid=(pthread_t*)malloc(2*n*sizeof(pthread_t));
    pthread_t * rtid=(pthread_t*)malloc(n*sizeof(pthread_t));

    int * args=(int*)malloc(2*n*sizeof(int));
    for(int i=0;i<2*n;i++)
        args[i]=i+1;

    sem_init(&sem_2player, 0, 2);
    sem_init(&sem_1referee, 0, 1);
    sem_init(&sem_match_start, 0, 0);
    sem_init(&sem_player_ready, 0, 0);
    sem_init(&sem_referee_wait, 0, 0);
    sem_init(&sem_organizer_wait, 0, 0);
    sem_init(&sem_organizer_ready, 0, 0);
    sem_init(&sem_organizer_confirm, 0, 0);

    pthread_create(&otid, NULL, organizer, NULL);

    int num, p=2*n, r=n;
    for(int i=0;i<3*n;i++){
        num=rand()%(p+r);
        if(num<=r && r>0){
            pthread_create(&rtid[n-r], NULL, referee, &args[n-r]);
            r--;
        }
        else{
            pthread_create(&ptid[2*n-p], NULL, player, &args[2*n-p]);
            p--;
        }
        sleep(rand()%3);
    }

    for(int i=0;i<2*n;i++)
        pthread_join(ptid[i], NULL);
    for(int i=0;i<n;i++)
        pthread_join(rtid[i], NULL);
    pthread_join(otid, NULL);

    free(ptid);
    free(rtid);

    return 0;
}

void player_enterAcademy(int num){
    printf("Player %d entered academy\n",num);

    sem_wait(&sem_2player);
    printf("Player %d gets chance to meet organizer\n",num);
}

void player_meetOrganizer(int num){
    sem_post(&sem_organizer_wait);
    printf("Player %d meets organizer\n",num);
}

void player_enterCourt(int num){
    sem_wait(&sem_organizer_confirm);
    printf("Player %d enters court\n",num);
}

void player_warmUp(int num){
    sleep(1);
    sem_post(&sem_player_ready);
    printf("Player %d finished warmup\n",num);

    sem_wait(&sem_match_start);
    printf("Player %d starts match\n",num);

    sem_post(&sem_2player);
}

void * player(void * arg){
    int num=*((int*)arg);

    player_enterAcademy(num);

    player_meetOrganizer(num);

    player_enterCourt(num);

    player_warmUp(num);
}

void referee_enterAcademy(int num){
    printf("Referee %d entered academy\n",num);

    sem_wait(&sem_1referee);
    printf("Referee %d gets chance to meet organizer\n",num);
}

void referee_meetOrganizer(int num){
    sem_post(&sem_referee_wait);
    printf("Referee %d meets organizer\n",num);
}

void referee_enterCourt(int num){
    sem_wait(&sem_organizer_confirm);
    printf("Referee %d enters court\n",num);
}

void referee_adjustEquipment(int num){
    sleep(0.5);
    printf("Referee %d finished adjusting equipment\n",num);
}

void referee_startGame(int num){
    sem_wait(&sem_player_ready);
    printf("Referee %d sees first player is ready\n",num);

    sem_wait(&sem_player_ready);
    printf("Referee %d sees second player is ready\n",num);

    sem_post(&sem_match_start);
    sem_post(&sem_match_start);
    sem_post(&sem_match_start);
    printf("Referee %d starts the match\n",num);

    sem_post(&sem_1referee);
}

void * referee(void * arg){
    int num=*((int*)arg);

    referee_enterAcademy(num);

    referee_meetOrganizer(num);

    referee_enterCourt(num);

    referee_adjustEquipment(num);

    referee_startGame(num);
}

void * organizer(void * arg){
    for(int i=0;i<n;i++){
        sem_wait(&sem_organizer_wait);
        printf("Organizer meets first player\n");

        sem_wait(&sem_organizer_wait);
        printf("Organizer meets second player\n");

        sem_wait(&sem_referee_wait);
        printf("Organizer meets referee\n");

        sem_post(&sem_organizer_confirm);
        sem_post(&sem_organizer_confirm);
        sem_post(&sem_organizer_confirm);
        printf("Organizer leads party to court\n");

        sem_wait(&sem_match_start);
        printf("Organizer leaves court\n");
    }
}
