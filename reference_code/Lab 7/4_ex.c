#include <stdio.h>
#include <pthread.h>
int arr[100];

typedef struct ar_pointer{
        int size;
        int offset; //element that the subarr starts with

}p;


void* fill_array(void* array){
        int i;
        int sum=0;
        p * subarray= (p*)array;

        //assign a subarray
        for(i=subarray->offset;i<subarray->size;i++){
                arr[i]=i;
                sum+=arr[i];
        } 

        //print the subarray
        for(i=subarray->offset;i<subarray->size;i++){
                printf("%d - " ,arr[i]);
        } 

        printf("\nSum of subarray: %d\n", sum);
        printf("\n\n");
}


int main(){

        pthread_t th1,th2,th3, th4;

        p p1,p2,p3,p4;
        p1.size=25;
        p1.offset=0;
        p2.size=50;
        p2.offset=25;
        p3.size=75;
        p3.offset=50;
        p4.size=100;
        p4.offset=75;
        pthread_create(&th1,NULL,fill_array,&p1);
        pthread_create(&th2,NULL,fill_array,&p2);
        pthread_create(&th3,NULL,fill_array,&p3);
        pthread_create(&th4,NULL,fill_array,&p4);

        pthread_join(th1,NULL);
        pthread_join(th2,NULL);
        pthread_join(th3,NULL);
        pthread_join(th4,NULL);

        int i;  
        for(i=0;i<100;i++){
                printf("%d ",arr[i]);
        }
        printf("\n");
}
