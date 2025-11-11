#include <stdlib.h>
#include <time.h>
#define P 10
#define Q 20
#define R 15

int A[P][Q], B[Q][R];
int C[P][R];

void main(){
    int i,j,k;
    srand(time(NULL));
    for(i=0;i<P;i++) for(j=0;j<Q;j++) A[i][j] = rand()%100;
    for(i=0;i<Q;i++) for(j=0;j<R;j++) B[i][j] = rand()%100;
    for(i=0;i<P;i++) for(k=0;k<R;k++) for(j=0;j<Q;j++) C[i][k] += A[i][j]*B[j][k];
}