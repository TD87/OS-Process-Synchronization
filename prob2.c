#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>

void * evm(void *);
void * voter(void *);

typedef struct{
    int arg, booth_num;
} arg_t;

typedef struct{
    int slots, val0, val1, val2, val3, val4, voters;
    pthread_mutex_t mutex;
    pthread_cond_t cond0, cond1, cond2, cond3, cond4;
} booth_t;

void booth_init(booth_t * booth){
    booth->slots=0;
    booth->val0=1;
    booth->val1=0;
    booth->val2=0;
    booth->val3=0;
    booth->val4=0;
    booth->voters=0;
    pthread_mutex_init(&(booth->mutex),NULL);
    pthread_cond_init(&(booth->cond0),NULL);
    pthread_cond_init(&(booth->cond1),NULL);
    pthread_cond_init(&(booth->cond2),NULL);
    pthread_cond_init(&(booth->cond3),NULL);
    pthread_cond_init(&(booth->cond4),NULL);
}

int polling_ready_evm(booth_t * booth, int count){
    pthread_mutex_lock(&(booth->mutex));

    //only 1 evm is active at a time per booth
    booth->val0--;
    if(booth->val0<0)
        pthread_cond_wait(&(booth->cond0), &(booth->mutex));

    //if booth is empty, exit
    if(booth->voters<=0){
        booth->val0++;
        pthread_cond_signal(&(booth->cond0));
        pthread_mutex_unlock(&(booth->mutex));
        return 1;
    }

    //if number of voters is less than number of slots
    booth->slots=count;
    if(booth->slots>booth->voters)
        booth->slots=booth->voters;

    //select voters from waiting queue
    for(int i=0;i<booth->slots;i++){
        pthread_cond_signal(&(booth->cond1));
        booth->val1++;
    }

    pthread_mutex_unlock(&(booth->mutex));
    return 0;
}

void polling_finished_evm(booth_t * booth){
    pthread_mutex_lock(&(booth->mutex));

    //signal selected voters that they can enter slots
    for(int i=0;i<booth->slots;i++){
        pthread_cond_signal(&(booth->cond2));
        booth->val2++;
    }

    //wait for voters to enter slots
    for(int i=0;i<booth->slots;i++){
        booth->val3--;
        if(booth->val3<0)
            pthread_cond_wait(&(booth->cond3), &(booth->mutex));
    }

    //wait for voters to finished voting
    for(int i=0;i<booth->slots;i++){
        booth->val4--;
        if(booth->val4<0)
            pthread_cond_wait(&(booth->cond4), &(booth->mutex));
    }

    //signal the next waiting evm
    booth->val0++;
    pthread_cond_signal(&(booth->cond0));

    pthread_mutex_unlock(&(booth->mutex));
}

void voter_wait_for_evm(booth_t * booth){
    pthread_mutex_lock(&(booth->mutex));

    //wait to be assigned evm
    booth->val1--;
    if(booth->val1<0)
        pthread_cond_wait(&(booth->cond1), &(booth->mutex));

    pthread_mutex_unlock(&(booth->mutex));
}

void voter_in_slot(booth_t * booth){
    pthread_mutex_lock(&(booth->mutex));

    //wait for all slots to be assigned
    booth->val2--;
    if(booth->val2<0)
        pthread_cond_wait(&(booth->cond2), &(booth->mutex));

    //signal in slot
    pthread_cond_signal(&(booth->cond3));
    booth->val3++;

    pthread_mutex_unlock(&(booth->mutex));
}

void voter_finished_voting(booth_t * booth){
    pthread_mutex_lock(&(booth->mutex));

    //signal voting done
    pthread_cond_signal(&(booth->cond4));
    booth->val4++;
    booth->voters--;

    pthread_mutex_unlock(&(booth->mutex));
}

booth_t * booth;

int main() {
    int a, b, c, temp;
    printf("Enter number of Booths: ");
    scanf("%d", &c);
    printf("Enter number of EVMs(>= No of booths): ");
    scanf("%d", &a);
    printf("Enter number of Voters: ");
    scanf("%d", &b);

    if(a<c)
        return 0;

    pthread_t * etid=(pthread_t*)malloc(a*sizeof(pthread_t));
    pthread_t * vtid=(pthread_t*)malloc(b*sizeof(pthread_t));
    arg_t * args1=(arg_t *)malloc(b*sizeof(arg_t));
    arg_t * args2=(arg_t *)malloc(a*sizeof(arg_t));
    booth=(booth_t*)malloc(c*sizeof(booth_t));

    for(int i=0;i<c;i++)
        booth_init(&booth[i]);

    //randomly assign booths to voters
    for(int i=0;i<b;i++){
        temp=(rand()%c)+1;
        args1[i].arg=i+1;
        args1[i].booth_num=temp;
        booth[temp-1].voters++;
    }

    //assign one evm to each booth
    for(int i=0;i<c;i++){
        args2[i].arg=i+1;
        args2[i].booth_num=i+1;
    }

    //assign remaining evms randomly
    for(int i=c;i<a;i++){
        temp=(rand()%c)+1;
        args2[i].arg=i+1;
        args2[i].booth_num=temp;
    }

    for(int i=0;i<b;i++)
        pthread_create(&vtid[i], NULL, voter, &args1[i]);
    for(int i=0;i<a;i++)
        pthread_create(&etid[i], NULL, evm, &args2[i]);

    for(int i=0;i<b;i++)
        pthread_join(vtid[i], NULL);
    for(int i=0;i<a;i++)
        pthread_join(etid[i], NULL);

    free(etid);
    free(vtid);
    free(args1);
    free(args2);
    free(booth);

    return 0;
}

void * evm(void * arg){
    arg_t * nums = (arg_t *)arg;

    while(1){
        int slots=(rand()%10)+1;

        printf("EVM %d ready to take %d voters (booth %d)\n",nums->arg, slots, nums->booth_num);
        if(polling_ready_evm(&booth[nums->booth_num-1], slots)==1)
            break;
        printf("EVM %d has taken voters (booth %d)\n",nums->arg, nums->booth_num);

        polling_finished_evm(&booth[nums->booth_num-1]);
        printf("EVM %d is freed of voters (booth %d)\n",nums->arg, nums->booth_num);
    }
}

void * voter(void * arg){
    arg_t * nums = (arg_t *)arg;

    printf("Voter %d is waiting for EVM (booth %d)\n", nums->arg, nums->booth_num);
    voter_wait_for_evm(&booth[nums->booth_num-1]);
    printf("Voter %d has been assigned EVM (booth %d)\n", nums->arg, nums->booth_num);
    sleep(1);

    voter_in_slot(&booth[nums->booth_num-1]);
    printf("Voter %d has entered slot (booth %d)\n", nums->arg, nums->booth_num);
    sleep(2);

    voter_finished_voting(&booth[nums->booth_num-1]);
    printf("Voter %d has finished voting (booth %d)\n", nums->arg, nums->booth_num);
}
