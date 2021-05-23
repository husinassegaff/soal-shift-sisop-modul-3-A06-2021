#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/sendfile.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include <termios.h>
#include <pthread.h>
#include <ctype.h>
#include <signal.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <fcntl.h>
#define PORT 7000

void *scan_routine(void *arg);
void *scanloop();

/* TERMIOS */
static struct termios old, current;

FILE *target_file;
char target[100];
int sock = 0;
int fini = 1;


void initTermios() {
  tcgetattr(0, &old);
  current = old;
  current.c_lflag &= ~ICANON;
  current.c_lflag &= ~ECHO;
  tcsetattr(0, TCSANOW, &current);
}

void resetTermios(void) {
  tcsetattr(0, TCSANOW, &old);
}

char getch() {
  char ch;
  initTermios();
  ch = getchar();
  resetTermios();
  return ch;
}

/* ROUTINE */

pthread_t printer;
pthread_t scanner;
pthread_t exited;
pthread_t recive;
pthread_t sendings;
pthread_t loop;

int input_mode = 0;

bool isExist(char fileName[]){
  return true;
}

void *sending(void *arg){
  // send file to server, using encodeFile()
  char c[BUFSIZ];
  char *g;
  char buffer[BUFSIZ];
  char data[100];
  char name[100];
  int fd;
  int file_size;
  int flag = 1;
  ssize_t sent_bytes = 0;
  ssize_t remain_data = 0;
  ssize_t len;
  off_t offset;
  struct stat file_stat;
  FILE *received_file;

  strcpy(c,arg);
  printf("%s\n",c);
  if(isExist(c)){
    fd = open(c,O_RDONLY);
    if (fstat(fd, &file_stat) < 0)
    {
      fprintf(stderr, "Error fstat --> %s", strerror(errno));
      exit(EXIT_FAILURE);
    }
    fprintf(stdout, "File Size: \n%ld bytes\n", file_stat.st_size);
    close(fd);
    sprintf(buffer, "%ld", file_stat.st_size);
    len = send(sock,buffer, sizeof(buffer),0);

    offset = 0;
    remain_data = file_stat.st_size;
    fd = open(c,O_RDONLY);

    while (((sent_bytes = sendfile(sock, fd, &offset, BUFSIZ)) > 0))
    {
            // fprintf(stdout, "1. Server sent %d bytes from file's data, offset is now : %ld and remaining data = %d\n", sent_bytes, offset, remain_data);
            remain_data -= sent_bytes;
            fprintf(stdout, "2. Server sent %ld bytes from file's data, offset is now : %ld and remaining data = %ld\n", sent_bytes, offset, remain_data);
    }
    close(fd);
    pthread_cancel(sendings);
  //   prompt "File to send found"
  //   FILE *source = fopen(data, "wb");
  //   size_t n;
  //   unsigned char buff[BUFSIZ];
  //   unsigned char bert[BUFSIZ];
  //   do {
  //       n = fgets(buff, BUFSIZ, source);
  //       if (n){
  //         snprintf(bert,sizeof(bert),"%zu",n);
  //         send(sock, buff, sizeof(buff), 0);
  //         // send(sock, bert, BUFSIZ, 0);
  //       }
  //       else break;
  //   } while(n > 0);

  //   while(fgets(buff, BUFSIZ, source) != NULL) {
  //   if (sendfile(sock, buff, sizeof(buff), 0) == -1) {
  //     perror("[-]Error in sending file.");
  //     exit(1);
  //   }
  //   bzero(buff, BUFSIZ);
  // }
  } 
}

void *reciveFile(void *arg){
  int sock = *(int *)arg;
  int flag = 1;
  size_t n;
  char buffer[BUFSIZ] = {0};
  char buff[BUFSIZ];
  if(recv(sock, buffer, BUFSIZ, 0) > 1)
    target_file = fopen(buffer, "wb");
  while (1) {
    memset(buffer, 0, BUFSIZ);
    if(recv(sock, buffer, BUFSIZ, 0) > 1) {
      if(flag==1){
        strcpy(buff,buffer);
      }
      else if(flag%2==0){
        n = atoi(buffer);
        fwrite(buff,1,n,target_file);
      }
      flag++;
    } else{
      break;
    }
  }
  fclose(target_file);
  pthread_cancel(recive);
}

void decodeFile(char filepath[]){
  // accept binary data from server and change it into proper file
}

void *exit_routine(void *arg) {
        printf("\e[2J\e[?25h\e[?1049l");
        resetTermios();
        pthread_cancel(scanner);
        pthread_cancel(printer);
        exit(0);
}

void *print_routine(void *arg) {
  int sock = *(int *)arg;
  char buffer[BUFSIZ] = {0};
  while (1) {
    memset(buffer, 0, BUFSIZ);
    if(recv(sock, buffer, BUFSIZ, 0) > 1) {
      char buffer2[BUFSIZ];
      strcpy(buffer2, buffer);
      char *token = strtok(buffer2, "\n");
      if (strcmp(token, "Uhuk Start") == 0) {
        input_mode = 1;
        resetTermios();
        pthread_cancel(scanner);
      } else if (strcmp(token, "Uhuk Stop") == 0) {
        input_mode = 0;
        resetTermios();
        pthread_cancel(scanner);
      } else if (strcmp(token, "File to send") == 0) {
        pthread_create(&sendings, NULL, &sending, target);
      } else if (strcmp(token, "File to download found") == 0) {
        input_mode = 0;
        resetTermios();
        pthread_cancel(scanner);
        pthread_create(&recive, NULL, &reciveFile, (void *)&sock);
      } else if (strcmp(token, "Goodbye !") == 0) {
        pthread_create(&exited, NULL, &exit_routine, (void *)&sock);
      } else {
        printf("%s", buffer);
        if(strstr(buffer,"Ekstensi")!=NULL)
          printf("\n");
      }
    }
  }
}

void *scan_routine(void *arg) {
  int sock = *(int *)arg;
  char c[BUFSIZ];
  int i = 0;
  while(1) {
    if (!input_mode) {
      do {
        char buff = getch();

        if (buff == 127 && i != 0) {
          printf("\e[D\e[K");
          c[i--] = 0;
        } else if (buff == 10 || !iscntrl(buff)) {
          c[i] = buff;
          printf("%c", c[i++]);
        }
      } while (c[i-1] != '\n' && c[i-1] != EOF);
      c[--i] = 0;
      i = 0;
      send(sock, c, BUFSIZ, 0);
    } else {
      char buff = getch();
      c[0] = buff;
      c[1] = 0;
      send(sock, c, BUFSIZ, 0);
    }
    strcpy(target,c);
  }
}

void sighandler(int signum) {
  printf("\e[2J\e[?25h\e[?1049l");
  resetTermios();
  exit(1);
}

void *scanloop(){
    pthread_create(&scanner, NULL, &scan_routine, (void *)&sock);
    while(1) {
      if (pthread_join(scanner, NULL) == 0) {
        pthread_create(&scanner, NULL, &scan_routine, (void *)&sock);
      }
  }
}

int main(int argc, char const *argv[]) {
    signal(SIGINT, sighandler);
    printf("\e[?1049h");
    struct sockaddr_in address;
    struct sockaddr_in serv_addr;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) {

        printf("\e[2J\e[?25h\e[?1049l");
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {

        printf("\e[2J\e[?25h\e[?1049l");
        printf("\nConnection Failed \n");
        return -1;
    }


    pthread_create(&printer, NULL, &print_routine, (void *)&sock);
    pthread_create(&loop,NULL,&scanloop,NULL);
    pthread_join(printer, NULL);
    pthread_join(loop,NULL);
    
    printf("\e[2J\e[?25h\e[?1049l");

}