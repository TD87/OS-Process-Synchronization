#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/shm.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<sys/ipc.h>

int * a;
key_t key = IPC_PRIVATE;

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

void merge_sort(int low, int high){
    if(high-low+1<5)
        selection_sort(low,high);
    else{
        int pid1, pid2, mid=(low+high)/2;

        if((pid1=fork())<0){
    		perror("fork error");
    		exit(1);
    	}

        if(pid1==0)
            merge_sort(low,mid);
        else{
            if((pid2=fork())<0){
        		perror("fork error");
        		exit(1);
        	}

            if(pid2==0)
                merge_sort(mid+1,high);
            else{
                waitpid(pid1, NULL, 0);
                waitpid(pid2, NULL, 0);
                merge(low,mid,high);
            }
        }
    }
    if(shmdt(a)==-1)
        perror("shmdt error");
    exit(0);
}

int main(){
    int n, shmid, pid;
    scanf("%d",&n);

    if((shmid=shmget(key, sizeof(int)*n, IPC_CREAT | 0666))==-1){
		perror("shmget error");
		exit(1);
	}

    if((a=shmat(shmid, 0, 0))==(void *)-1){
		perror("shmat error");
		exit(1);
	}

    for(int i=0;i<n;i++)
        scanf("%d",&a[i]);

    if((pid=fork())<0){
		perror("fork error");
		exit(1);
	}

    if(pid==0)
        merge_sort(0,n-1);
    else{
        waitpid(pid, NULL, 0);

        for(int i=0;i<n;i++)
            printf("%d\n",a[i]);
    }
    return 0;
}
