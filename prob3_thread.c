#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/shm.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<sys/ipc.h>
#include<pthread.h>

typedef struct{
    int low, high;
} arg_t;

int * a;

void selection_sort(int low, int high){
    int min, temp;
    for(int i=low;i<high;i++){
        min=i;
        for(int j=i+1;j<=high;j++)
            if(a[j]<a[min])
                min=j;
        if(min!=i){
            temp=a[i];
            a[i]=a[min];
            a[min]=temp;
        }
    }
    return;
}

void merge(int low, int mid, int high){
    int p1=low, p2=mid+1, i=0;
    int * temp=(int*)malloc((high-low+1)*sizeof(int));
    while(p1<=mid && p2<=high)
        if(a[p1]<=a[p2])
            temp[i++]=a[p1++];
        else
            temp[i++]=a[p2++];
    while(p1<=mid)
        temp[i++]=a[p1++];
    while(p2<=high)
        temp[i++]=a[p2++];
    for(i=0,p1=low;p1<=high;i++,p1++)
        a[p1]=temp[i];
    return;
}

void * merge_sort(void * arg){
    arg_t * nums = (arg_t *) arg;

    if(nums->high-nums->low+1<5)
        selection_sort(nums->low,nums->high);
    else{
        int mid=(nums->low+nums->high)/2;
        arg_t args1, args2;
        pthread_t tid1, tid2;

        args1.low=nums->low;
        args1.high=mid;
        args2.low=mid+1;
        args2.high=nums->high;

        pthread_create(&tid1, NULL, merge_sort, &args1);
        pthread_create(&tid2, NULL, merge_sort, &args2);
        pthread_join(tid1, NULL);
        pthread_join(tid2, NULL);

        merge(nums->low,mid,nums->high);
    }
}

int main(){
    int n;
    pthread_t tid;
    scanf("%d",&n);

    a=(int *)malloc(n*sizeof(int));
    for(int i=0;i<n;i++)
        scanf("%d",&a[i]);

    arg_t args;
    args.low=0;
    args.high=n-1;

    pthread_create(&tid, NULL, merge_sort, &args);
    pthread_join(tid, NULL);

    for(int i=0;i<n;i++)
        printf("%d\n",a[i]);

    return 0;
}
