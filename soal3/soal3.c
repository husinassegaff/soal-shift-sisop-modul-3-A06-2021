#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <syslog.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <dirent.h>
#include <string.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>
#include <ctype.h>
#include <unistd.h>

char cwd[100];
pthread_t tid[3];
pid_t child;
char *checkext[4],*checkext2[20],namafile[100],dummyinput[100];
int n = 0, m = 0;

void* movinfile(void *arg){
    strcpy(dummyinput,arg);
    char *token,*token1;

	unsigned long i=0;
	pthread_t id = pthread_self();

    token1 = strtok(dummyinput, "/");
    while( token1 != NULL ) {
        checkext2[m] = token1;
        m++;
        token1 = strtok(NULL, "/");
    }
    strcpy(namafile,checkext2[m-1]);

    token = strtok(checkext2[m-1], ".");
    while( token != NULL ) {
        checkext[n] = token;
        n++;
        token = strtok(NULL, ".");
    }
	
    char ext[100];
    strcpy(ext,checkext[n-1]);
    for(int i = 0; ext[i]; i++){
       ext[i] = tolower(ext[i]);
    }
    
    DIR *folder, *folderopen;
    struct dirent *entry;
    char tujuanfile[100];
    folder = opendir(cwd);
    int available = 0;

    if( n > 1 ){
        if(folder == NULL)printf("error\n");
        while( (entry=readdir(folder)) ){
            if(strcmp(entry->d_name,ext) == 0 && entry->d_type == 4){
                available = 1;
                break;
            }
        }

    if(available == 0){
            strcpy(tujuanfile,cwd);
            strcat(tujuanfile,"/");
            strcat(tujuanfile,ext);
            mkdir(tujuanfile, 0777);
        }
    }
}
	return NULL;
}

int main(int argc, char *argv[]) {
    int err;
    pthread_t tid[1000];

    if (strcmp(argv[1],"-f") == 0) {
        for(int j = 2 ; j < argc ; j++ ){
            err = pthread_create(&tid[j],NULL,movinfile,(void *)argv[j]);
            
            if(err != 0){
                printf("Can't create thread! : [%s]\n",strerror(err));
            }
        }

        for(int j = 2; j < argc; j++){
            pthread_join(tid[j], NULL);
        }
    }
return 0; 
}