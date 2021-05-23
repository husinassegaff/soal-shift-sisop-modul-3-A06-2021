#include<stdio.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<unistd.h>
int matriksA[4][3];
int matriksB[3][6];

int kaliMatriks(int x,int y)
{
    int hasil=0;
    for (int a=0;a<3;a++)
    {
        hasil= hasil + (matriksA[x][a] * matriksB[a][y]);
    }
    return hasil;
}

int main ()
{
    int matriksC[4][6];
    for (int a=0;a<4;a++)
    {
        for (int b=0;b<3;b++)
        {
            scanf("%d",&matriksA[a][b]);
        }
    }
    for (int a=0;a<3;a++)
    {
        for (int b=0;b<6;b++)
        {
            scanf("%d",&matriksB[a][b]);
        }
    }
    for (int a=0;a<4;a++)
    {
        for (int b=0;b<6;b++)
        {
            matriksC[a][b] = kaliMatriks(a,b);
        }
    }
    printf("\n");
    for (int a=0;a<4;a++)
    {
        for (int b=0;b<6;b++)
        {
            printf("%d ",matriksC[a][b]);
        }
        printf("\n");
    }
}
