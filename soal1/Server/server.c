#include <stdio.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <ctype.h>
#include <errno.h>
#include <signal.h>
#include <time.h>
#include <stdbool.h>
#define PORT 7000

/* Global variable */

int sta1;
char name[100];
bool status = true;

pthread_t process, recive;

/* Struktur Node */

typedef struct queueNode_t {
    pthread_t tid[3];
    char input[BUFSIZ];
    char name[1024];
    char action[1024];
    char file[1024];
    int idsock;
    int is_auth;
    ssize_t len;
    FILE *filename;
    struct queueNode_t *next;
} QueueNode;

/* Struktur ADT Queue */

typedef struct queue_t {
    QueueNode   *_front, 
                *_rear;
    unsigned _size;
} Queue;

/* Function prototype */

void queue_init(Queue *queue);
bool queue_isEmpty(Queue *queue);
void queue_push(Queue *queue, int value);
void queue_pop(Queue *queue);
QueueNode  *queue_front(Queue *queue);
int  queue_size(Queue *queue);
void prompt(int prompt_no);
void resetScreen();
char *get_data(char buff[]);
void *server_main_routine(void *arg);
void *server_scan_routine(void *arg);
Queue myQueue;

/* Queue Operations */

void queue_init(Queue *queue){
    queue->_size = 0;
    queue->_front = NULL;
    queue->_rear = NULL;
}

bool queue_isEmpty(Queue *queue) {
    return (queue->_front == NULL && queue->_rear == NULL);
}

void queue_push(Queue *queue, int value){
    QueueNode *newNode = (QueueNode*) malloc(sizeof(QueueNode));
    if (newNode) {
        queue->_size++;
        newNode->idsock = value;
        newNode->next = NULL;
        newNode->is_auth = 0;
        if (queue_isEmpty(queue))                 
            queue->_front = queue->_rear = newNode;
        else {
            queue->_rear->next = newNode;
            queue->_rear = newNode;
        }
    }
}

void queue_pop(Queue *queue){
    if (!queue_isEmpty(queue)) {
        QueueNode *temp = queue->_front;
        queue->_front = queue->_front->next;
        free(temp);
        
        if (queue->_front == NULL)
            queue->_rear = NULL;
        queue->_size--;
    }
}

QueueNode *queue_front(Queue *queue){
    if (!queue_isEmpty(queue)) {
        return queue->_front;
    }
    return NULL;
}

int queue_size(Queue *queue) {
    return queue->_size;
}

/* Operations */

int auth(char username[], char password[]) {
  FILE *fp = fopen("akun.txt", "r");
  int is_auth = 0;
  char buffer[BUFSIZ];
  while (fgets(buffer, BUFSIZ, fp) != NULL && is_auth == 0) {
    char f_username[BUFSIZ], f_password[BUFSIZ];
    char *token = strtok(buffer, ":");
    strcpy(f_username, token);
    token = strtok(NULL, "\n");
    strcpy(f_password, token);
    if (strcmp(username, f_username) == 0 && strcmp(password, f_password) == 0){
      is_auth = 1;
      break;
    }
  }
  fclose(fp);
  return is_auth;
}

void reg(char username[], char password[]) {
  chmod("akun.txt",S_IWOTH);
  FILE *fp = fopen("akun.txt", "a");
    if (fp == NULL) {
        perror("fopen()");
    }
  fprintf(fp, "%s:%s\n", username, password);
  fclose(fp);
}

void addFile(char data[]){
  // add file
  FILE *fp = fopen("files.tsv", "a");
  if (fp == NULL) {
    perror("fopen()");
  }
  fprintf(fp,"%s\n",data);
  fclose(fp);
}

void decodeFile(char filepath[]){
  // accept binary data from client and change it into proper file
}

bool isExist(char fileName[]){
  return true;
}

void reciveFile(){
  strcpy(queue_front(&myQueue)->action,"Tambah :");
  int file_size;
  ssize_t remain_data = 0;
  ssize_t len;
  FILE *received_file;
  char buffer[BUFSIZ] = {0};
  char g[BUFSIZ] = {0};
  char *a;
  int flag = 1;
  char publisher[BUFSIZ] = {0};
  char tahun_publikasi[BUFSIZ] = {0};
  char filepath[BUFSIZ] = {0};
  char data[1024] = {0};
  char data1[1024] = {0};

  mkdir("FILES",0777);
  chdir("FILES");

  resetScreen();
  prompt(21);
  strcpy(data,"Publisher: ");
  strcat(data,get_data(publisher));
  strcat(data,"\n");
  prompt(22);
  strcat(data,"Tahun Publikasi: ");
  strcat(data,get_data(tahun_publikasi));
  strcat(data,"\n");
  prompt(23);
  strcpy(g, get_data(filepath));
  strcat(data,"Filepath: ");
  strcat(data,g);
  
  pthread_cancel(queue_front(&myQueue)->tid[1]);
  while(flag == 1){
      a = strtok(g,"/");
      while(a!=NULL){
        strcpy(name,a);
        a = strtok(NULL,"/");
      }
      flag = 0;
  }

  strcpy(queue_front(&myQueue)->file,name);
  strcpy(g,queue_front(&myQueue)->file);

  a = strtok(g,".");
  strcpy(data1,"Nama: ");
  strcat(data1,a);
  strcat(data1,"\n");
  strcat(data1,data);
  strcat(data1,"\n");
  a = strtok(NULL,".");
  strcat(data1,"Ekstensi: ");
  strcat(data1,a);
  strcat(data1,"\n");

  received_file = fopen(name,"w");
  prompt(25);

  recv(queue_front(&myQueue)->idsock,queue_front(&myQueue)->input,BUFSIZ,0);
  printf("%s\n",queue_front(&myQueue)->input);
  file_size = atoi(queue_front(&myQueue)->input);
  fprintf(stdout,"\nFile size : %d\n", file_size);

  
  if (received_file == NULL){
    fprintf(stderr, "Failed to open file foo --> %s\n", strerror(errno));
  }

  remain_data = file_size;
  fclose(received_file);
  received_file = fopen(name, "w");

  while ((remain_data > 0) && ((queue_front(&myQueue)->len = recv(queue_front(&myQueue)->idsock, queue_front(&myQueue)->input, BUFSIZ, 0)) > 0))
  {
          fwrite(queue_front(&myQueue)->input, sizeof(char), queue_front(&myQueue)->len, received_file);
          remain_data -= queue_front(&myQueue)->len;
          fprintf(stdout, "Receive %ld bytes and we hope :- %ld bytes\n", queue_front(&myQueue)->len, remain_data);
  }
  resetScreen();
  fclose(received_file);
  chdir("..");
  addFile(data1);
}

void rmFile(char fileName[]){
  // rename file into filenameOLD.ext
}

void seeFile(){
  char buffer[BUFSIZ];
   int counter = 5;
   FILE *fp = fopen("files.tsv", "r");
  while(fgets(buffer, BUFSIZ, fp) != NULL) {
    if(buffer[0]=='\n') {
        continue;
    } else {
    send(queue_front(&myQueue)->idsock, buffer, BUFSIZ, 0);
    }
    counter--;
  }
  fclose(fp);
}

void sendFile(char data[]){
  //   send binary endoced file to requested client
  //   if(isExist(data)){
  //   prompt "File to download found"
  //   FILE *source = fopen(data, "wb");
  //   size_t n;
  //   unsigned char buff[BUFSIZ];
  //   unsigned char bert[BUFSIZ];
  //   do {
  //       n = fread(buff, 1, sizeof buff, source);
  //       if (n){
  //         snprintf(bert,sizeof(bert),"%zu",n);
  //         send(queue_front(&myQueue)->idsock, buff, BUFSIZ, 0);
  //         send(queue_front(&myQueue)->idsock, bert, BUFSIZ, 0);
  //       }
  //       else break;
  //   } while(n > 0);
  // } 
}

void encodeFile(char filepath[]){
  // encode file into binary file
}

void findFile(char filename[]){
  char buffer[BUFSIZ];
   int counter = 5;
   FILE *fp = fopen("files.tsv", "r");
  while(fgets(buffer, BUFSIZ, fp) != NULL) {
    if(strstr(buffer,filename))
      counter = 5;
    if(buffer[0]=='\n') {
        continue;
    } else {
      if(counter >0)
        send(queue_front(&myQueue)->idsock, buffer, BUFSIZ, 0);
    }
    counter--;
  }
  fclose(fp);
}

void serverLog(char activity[], char username[]){
  // add server activity to log file
}

/* Routine */

void resetScreen() {
  char buffer[BUFSIZ];
  sprintf(buffer, "\e[H\e[2J");
  send(queue_front(&myQueue)->idsock, buffer, BUFSIZ, 0);
}

void prompt(int prompt_no) {
  char buffer[BUFSIZ];
  if (prompt_no == -1) {
    sprintf(buffer, "Goodbye !\n");
  }
  if (prompt_no == 0) {
    sprintf(buffer, "\e[30;47mDatabase Buku\e[m\n");
  }
  if (prompt_no == 1) {
    sprintf(buffer,
      "1. Login\n"
      "2. Register\n"
      "3. Exit\n"
      "Choices: \e[s\n\e[u");
  }
  // Prompt Login & Register
  if (prompt_no == 11) {
    sprintf(buffer, "Username: \e[s\n\e[u");
  }
  if (prompt_no == 12) {
    sprintf(buffer, "Password: \e[s\n\e[u");
  }
  // Prompt Add file
  if (prompt_no == 21) {
    sprintf(buffer, "Publisher: \e[s\n\e[u");
  }
  if (prompt_no == 22) {
    sprintf(buffer, "Tahun Publikasi: \e[s\n\e[u");
  }
  if (prompt_no == 23) {
    sprintf(buffer, "Filepath: \e[s\n\e[u");
  }
  if (prompt_no == 25) {
    sprintf(buffer, "File to send\n");
  }
  if (prompt_no == 101) {
    sprintf(buffer, "Login Gagal... (Press Anything to Continue)\n");
  }
  if (prompt_no == 121) {
    sprintf(buffer, "Register Berhasil... (Press Anything to Continue)\n");
  }
  if (prompt_no == 20) {
    sprintf(buffer, "Username: %s\n", queue_front(&myQueue)->name);
  }
  if (prompt_no == 2) {
    sprintf(buffer,
      "1. Add\n"
      "2. Download\n"
      "3. Delete\n"
      "4. See\n"
      "5. Find\n"
      "6. Delete\n"
      "8. Logout\n"
      "Choices: \e[s\n\e[u");
  }
  if (prompt_no == 989) {
    sprintf(buffer, "\e[?25l");
  }
  if (prompt_no == 988) {
    sprintf(buffer, "\e[?25h");
  }
  if (prompt_no == 999) {
    sprintf(buffer, "Uhuk Start\n");
  }
  if (prompt_no == 998) {
    sprintf(buffer, "Uhuk Stop\n");
  }

  send(queue_front(&myQueue)->idsock, buffer, BUFSIZ, 0);

}

char *get_data(char buff[]) {
  memset(queue_front(&myQueue)->input, 0, BUFSIZ);
  while(strlen(queue_front(&myQueue)->input) == 0);
  strcpy(buff, queue_front(&myQueue)->input);
  memset(queue_front(&myQueue)->input, 0, BUFSIZ);
  printf("From socket_data[%d]: %s\n", queue_front(&myQueue)->idsock, buff);
  return buff;
}

void *server_scan_routine(void *arg) {
  int i = *(int *)arg - 1;

  char buffer[BUFSIZ];
  while(1) {
    if ((queue_front(&myQueue)->len = recv(queue_front(&myQueue)->idsock, queue_front(&myQueue)->input, BUFSIZ, 0))> 0) {
    }
  }
}

void *server_main_routine(void *arg) {
  int i = *(int *)arg - 1;

  char buffer[BUFSIZ];
  while(1) {
    resetScreen();
    if (queue_front(&myQueue)->is_auth == 0) {
      prompt(0);
      prompt(1);

      get_data(buffer);
      for (int i = 0; buffer[i]; i++) {
        buffer[i] = tolower(buffer[i]);
      }

      if (strcmp(buffer, "1") == 0 || strcmp(buffer, "login") == 0) {
        char username[BUFSIZ];
        char password[BUFSIZ];
        prompt(11);
        get_data(username);
        prompt(12);
        get_data(password);
        queue_front(&myQueue)->is_auth = auth(username, password);
        if (queue_front(&myQueue)->is_auth == 0) {
          printf("Auth Failed\n");
          prompt(999);
          prompt(101);
          get_data(buffer);
          prompt(998);
        }
        if (queue_front(&myQueue)->is_auth == 1) {
          strcpy(queue_front(&myQueue)->name, username);
          printf("Auth Success\n");
        }
      } else if (strcmp(buffer, "2") == 0 || strcmp(buffer, "register") == 0) {
        char username[BUFSIZ];
        char password[BUFSIZ];
        prompt(11);
        get_data(username);
        prompt(12);
        get_data(password);
        printf("%s:%s", username, password);
        reg(username, password);
        prompt(999);
        prompt(121);
        get_data(buffer);
        prompt(998);
      } else if (strcmp(buffer, "3") == 0 || strcmp(buffer, "exit") == 0) {
        resetScreen();
        prompt(-1);
        printf("Bye %d\n",i);
        close(queue_front(&myQueue)->idsock);
        pthread_cancel(queue_front(&myQueue)->tid[1]);
        pthread_exit(&sta1);
      }

    } else if (queue_front(&myQueue)->is_auth == 1) {
      prompt(0);
      prompt(20);
      prompt(2);

      get_data(buffer);
      for (int i = 0; buffer[i]; i++) {
        buffer[i] = tolower(buffer[i]);
      }

      if (strcmp(buffer, "1") == 0 || strcmp(buffer, "add") == 0) {
        resetScreen();
        reciveFile();
        pthread_create(&(queue_front(&myQueue)->tid[1]), NULL, &server_scan_routine, (void *)&(queue_front(&myQueue)->idsock));
      } else if (strcmp(buffer, "4") == 0 || strcmp(buffer, "see") == 0) {
        prompt(999);
        seeFile();
        get_data(buffer);
        prompt(998);
      } else if (strcmp(buffer, "5") == 0 || strstr(buffer, "find") != NULL) {
        prompt(999);
        char *a;
        strcpy(a,get_data(buffer));
        a = strtok(NULL,"find ");
        findFile(a);
        get_data(buffer);
        prompt(998);
      } else if (strcmp(buffer, "8") == 0 || strcmp(buffer, "logout") == 0) {
        queue_front(&myQueue)->is_auth = 0;
        memset(queue_front(&myQueue)->name, 0, 1000);
      }
    } 
  }
}

void *processed_thread(void *arg) {
  printf("Processing\n");
  QueueNode *temp = queue_front(&myQueue);
  pthread_create(&(temp->tid[0]), NULL, &server_main_routine, (void *)&(temp->idsock));
  pthread_create(&(temp->tid[1]), NULL, &server_scan_routine, (void *)&(temp->idsock));
  pthread_join(temp->tid[0], NULL);
  pthread_join(temp->tid[1], NULL);
  queue_pop(&myQueue);
  status = true;
  pthread_cancel(temp->tid[2]);
}

void *passer(void *arg){
  while(1){
    if(status == true && !queue_isEmpty(&myQueue)){
            status = false;
            pthread_create(&(queue_front(&myQueue)->tid[2]),NULL,&processed_thread,NULL);
    }
  }
}

int main(int argc, char const *argv[]) {

    int server_fd, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[BUFSIZ] = {0};
    int connection;

    queue_init(&myQueue);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
   
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("\e[30;47mDatabase Buku\e[m\n");
    pthread_create(&process,NULL,&passer,NULL);
    while(1){
        if((connection = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))!=0)
          queue_push(&myQueue,connection);
    }

}