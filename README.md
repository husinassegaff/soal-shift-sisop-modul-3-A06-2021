# soal-shift-sisop-modul-3-A06-2021

Jawaban soal praktikum Sistem Operasi modul 3.

**Anggota kelompok**:

```
- Richard Asmarakandi       05111940000017
- Muhammad Rafki Mardi      05111940000054
- Husin Muhammad Assegaff   05111940000127
```

---

## Tabel Konten

- [soal-shift-sisop-modul-3-A06-2021](#soal-shift-sisop-modul-3-a06-2021)
  - [Tabel Konten](#tabel-konten)
  - [Soal 1](#soal-1)
    - [Soal Client](#soal-client)
    - [Soal Server](#soal-server)
  - [Soal 2](#soal-2)
    - [Soal 2.a](#soal-2a)
    - [Soal 2.b](#soal-2b)
    - [Soal 2.c](#soal-2c)
  - [Soal 3](#soal-3)

  - [Tabel Konten](#tabel-konten)
  - [Soal 1](#soal-1)
    - [Soal Client](#soal-client)
    - [Soal Server](#soal-server)
  - [Soal 2](#soal-2)
    - [Soal 2.a](#soal-2a)
    - [Soal 2.b](#soal-2b)
    - [Soal 2.c](#soal-2c)
  - [Soal 3](#soal-3)

- [soal-shift-sisop-modul-3-A06-2021](#soal-shift-sisop-modul-3-a06-2021)
  - [Tabel Konten](#tabel-konten)
  - [Soal 1](#soal-1)
    - [Soal Client](#soal-client)
    - [Soal Server](#soal-server)
  - [Soal 2](#soal-2)
    - [Soal 2.a](#soal-2a)
    - [Soal 2.b](#soal-2b)
    - [Soal 2.c](#soal-2c)
  - [Soal 3](#soal-3)

## Soal 1

**[Source Code Soal 1 - Client](https://github.com/husinassegaff/soal-shift-sisop-modul-3-A06-2021/blob/main/soal1/Client/client.c)**<br />
**[Source Code Soal 1 - Server](https://github.com/husinassegaff/soal-shift-sisop-modul-3-A06-2021/blob/main/soal1/Server/server.c)**

**Deskripsi:**\
Keverk adalah orang yang cukup ambisius dan terkenal di angkatannya. Sebelum dia menjadi ketua departemen di HMTC, dia pernah mengerjakan suatu proyek dimana keverk tersebut meminta untuk membuat server database buku. Proyek ini diminta agar dapat digunakan oleh pemilik aplikasi dan diharapkan bantuannya dari pengguna aplikasi ini.

### Soal Client

Daftar library client yang digunakan:

```c
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <termios.h>
#include <pthread.h>
#include <ctype.h>
#include <signal.h>
#define PORT 8080
```

- `#include <stdio.h>` Library fungsi input output (e.g. printf(), scanf())
- `#include <sys/socket.h>` Library untuk membuat socket
- `#include <sys/types.h>` Library tipe data khusus (e.g. pid_t)
- `#include <stdlib.h>` Library untuk fungsi umum (e.g. exit())
- `#include <netinet/in.h>` Library untuk melakukan koneksi socket
- `#include <string.h>` Library untuk manipulasi string (e.g. strcpy())
- `#include <arpa/inet.h>` Library untuk melakukan koneksi socket
- `#include <unistd.h>` Library untuk melakukan system call kepada kernel linux (e.g. fork())
- `#include <termios.h>` Library untuk mengatur terminal (e.g. tcsetattr())
- `#include <pthread.h>` Library untuk operasi thread (e.g. pthread_create(), pthread_exit())
- `#include <ctype.h>` Library untuk fungsi tipe (e.g. tolower())
- `#include <signal.h>` Library untuk mengatur signal yang datang (e.g. signal())
- `#define PORT 7000` Pendefinisian port dari socket

---

**Ide Awal:**

- Disini fungsi termios() sebagai input guna `Press anything to continue` agar tidak perlu menunggu hingga `newline`.
- Pada client digunakan 3 buah thread utama, pertama untuk scanloop (memanggil thread scan), thread scan, dan thread print(guna mencetak data dari dari server)

**_Termios_**

**Global Variable & Fungsi _initTermios_, _resetTermios_, dan _getch_**

```c
static struct termios old, current;

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
```

- Variable `static struct termios old`menyimpan pengaturan awal terminal
- Variable `static struct termios current` sebagai setting terminal
- `initTermios()`
  - Fungsi `initTermios()` mengambil dan menyimpan pengaturan awal terminal ke `old` menggunakan fungsi `tcgetattr()`.
  - Pengaturan disimpan juga ke `current`.
  - Lalu mengubah mode input `current` menjadi **Tidak Kanonikal** (Input yang langsung dibaca tanpa menunggu newline). Dan juga mode pada `current` menjadi **Tanpa Echo** (Input tidak ditampilkan dalam STDOUT).
  - Lalu settingan `current` di set pada terminal menggunakan `tcsetattr()`.
- Fungsi `resetTermios()` akan mengubah settingan terminal kembali menjadi normal.
- `getch()`
  - Pada fungsi `getch()`, nanti akan melakukan `initTermios()` agar input modenya berubah. Lalu menggunakan fungsi `getchar()` untuk mendapatkan 1 karakter yang diinputkan. Dan terakhir akan dijalankan `resetTermios` untuk mengembalikan settingan terminal.

**_Thread Scanloop_**

```c
void *scanloop(){
    pthread_create(&scanner, NULL, &scan_routine, (void *)&sock);
    while(1) {
      if (pthread_join(scanner, NULL) == 0) {
        pthread_create(&scanner, NULL, &scan_routine, (void *)&sock);
      }
  }
}
```

- Mengingat bahwa scan nantinya akan diberhentikan guna input dengan jalur berbeda scanloop berfungsi sebagai switch dari `Scan Thread`

**_Thread Print Routine_**

```c
void *print_routine(void *arg) {
  int sock = *(int *)arg;
  char buffer[BUFSIZ] = {0};
  while (1) {
    memset(buffer, 0, BUFSIZ);
    if(recv(sock, buffer, BUFSIZ, 0) > 1) {
      char buffer2[BUFSIZ];
      strcpy(buffer2, buffer);
      char *token = strtok(buffer2, "\n");
      if (strcmp(token, "Start") == 0) {
        input_mode = 1;
        resetTermios();
        pthread_cancel(scanner);
      } else if (strcmp(token, "Stop") == 0) {
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
```

- guna menerima data dari socket server dan mencetaknya ke terminal client apabila bukan trigger.
- trigger berfungsi agar program client berubah mode menjadi mode yang diinginkan.
- mode `Start` sebagai triger mode input `kanonikal` sedangkan mode `Stop` guna mode normal yang dimanipulasi(dijelaskan berikutnya) trigger dibantu oleh variabel `input_mode`.
- `strstr` guna membantu mencetak `newline` ketika client melakukan perintah `see`(dijelaskan nanit).
- terdapat thread baru berupa `sendings` sebagai thread yang mengirimkan file ke server (dijelaskan nanit).

**_Thread Scan Routine_**

```c
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
```

- Untuk `scan_routine()`, `arg` yang diinputkan sama dengan `print_routine` yaitu `socket` dari program utama.
- Lalu looping untuk melakukan scanning terhadap user input.
- Jika `input_mode == 0`, maka karakter akan diinputkan sampai newline. Disini kami memanipulasi agar input seolah-olah terlihat normal dengan mengatur input yang diterima oleh user. Contoh jika input yang didapat `127` **(BACKSPACE)**, maka proses yang akan kita lakukan yaitu menggeser kursor kekiri, lalu menghapus semua karakter yang didepannya, dan juga pada buffer `c` akan dihapus satu input terakhir. Dan jika input karakter biasa atau `10` **(ENTER)**, maka akan dimasukkan kedalam buffer `c` dan diprint biasa. Dan terakhir buffer `c` akan dikirimkan ke server menggunakan `send()` dengan socket `sock`.
- Jika `input_mode == 1`, maka karakter yang diinputkan didapat menggunakan fungsi `getch()` yang telah didefinisikan. Dan karakter tersebut akan langsung dikirim ke server.
  **_Fungsi Sending_**

```c

void *sending(void *arg){
  char c[BUFSIZ];
  char buffer[BUFSIZ];
  char name[100];
  int fd;
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
            remain_data -= sent_bytes;
            fprintf(stdout, "Server sent %ld bytes from file's data, offset is now : %ld and remaining data = %ld\n", sent_bytes, offset, remain_data);
    }
    close(fd);
    pthread_cancel(sendings);
  }
}
```

- `BUFSIZ` sebagai konstanta bernilai 4096 bit
- `Buffer` menyimpan data yang diterima dari server
- struct `file_stat` guna menarik angka byte ukuran file.
- `fd` menyimpan ukuran dari file yang akan dikirim
- `c[]` tempat menyimpan alamat file
- `ssize_t` sebagai data byte (sisa, terkirim, total)
- `offset` sebagai ukuran data yang terkirim
- `*received_file` guna membuka file yang akan dikirim
- menggunakan fungsi `open()` guna membuka file yang akan dikirim
- `fstat()` berfungsi untuk mengetahui besar file yang akan dikirim dalam byte
- kemudian mengirim ukuran total file ke server guna server mempersiapkan diri
- `sendfile()` mengirimkan data dan ukurannya yang dalam prosesnya diatur oleh kernel.
- `fprintf` guna memantau proses pengiriman file ke server
- jika file sudah terkirim maka file yang di`open` sebelumnya di`close` dan thread `sendings` pun berakhir.

**_Exit Routine_**

```c
void *exit_routine(void *arg) {
        printf("\e[2J\e[?25h\e[?1049l");
        resetTermios();
        pthread_cancel(scanner);
        pthread_cancel(printer);
        exit(0);
}
```

- Saat client ingin keluar dari server maka terminal akan dikondisikan kesedia kala dengan `printf` dan `resetTermios()`.
- `pthread_cancel()` guna menghentikan thread yang berjalan.
- `exit()` menutup program.

**_SignHandler_**

```c
void sighandler(int signum) {
  printf("\e[2J\e[?25h\e[?1049l");
  resetTermios();
  exit(1);
}
```

- dipanggil diawalan `main` guna mengubah kondisi terminal menjadi normal dengan `signal()` yang dipanggil oleh main (dijelaskan nanti).
  - `\e[2J` merupakan escape sequence untuk menghapus isi terminal dan mengembalikan posisi kursor ke posisi 1,1 (pojok kiri atas).
  - `\e[?25h` merupakan escape sequence untuk memunculkan posisi kursor.
  - `\e[?1049l` merupakan escape sequence untuk mengembalikan terminal menjadi buffer utamanya.

**_Main Program_**

```c
int main(int argc, char const *argv[]) {
    signal(SIGINT, sighandler);
    printf("\e[?1049h");
    ...
}
```

- fungsi `signal()` untuk menghandle signal `SIGINT` (oleh `sighandler()`). lalu akan di di-print kedalam terminal escape sequence `"\e[?1049h"` untuk mengubah buffer terminal menjadi **_alternate-buffer_**.

```c
struct sockaddr_in address;
int sock = 0, valread;
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
```

- Operasi untuk membuat program utama terkoneksi dengan socket.
- Socket file descriptor akan dimasukkan kedalam variable `sock` ketika proses `connect()` berhasil berjalan.

### Soal Server

**_Struktur Data Queue_**

```c
void queue_init(Queue *queue);
bool queue_isEmpty(Queue *queue);
void queue_push(Queue *queue, int value);
void queue_pop(Queue *queue);
QueueNode  *queue_front(Queue *queue);
int  queue_size(Queue *queue);
```

- Guna queue multi koneksi dari server

**_Socket struct_**

```c
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
```

- menyimpan data seperti "user siapa ini", data yang masuk, aksi yang dilakukan, nilai soketnya, status kredensial.

**_1.A Autentikasi_**

```c
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
```

- membuat `akun.txt` jika belum ada.
- inisiasi status kredensial
- saat menerima data parameter, maka file `akun.txt` akan dipindai apakah terdapat string yang cocok dengan input parameter.

**_1.A Registrasi_**

```c
void reg(char username[], char password[]) {
  FILE *fp = fopen("akun.txt", "r");
  fclose(fp);
  chmod("akun.txt",S_IWOTH);
  FILE *fp = fopen("akun.txt", "a");
    if (fp == NULL) {
        perror("fopen()");
    }
  fprintf(fp, "%s:%s\n", username, password);
  fclose(fp);
}
```

- membuat `akun.txt` jika belum ada.
- input dari client akan ditarik menggunakan `get_data()` (dijelaskan kemudian) dan ditampung guna dijadikan argumen dari paramenter `reg()` nantinya argumen tersebut akan di print ke file `akun.txt`;

**_1.B & 1.C Add File_**

```c
void addFile(char data[]){
  // add file
  FILE *fp = fopen("files.tsv", "a");
  if (fp == NULL) {
    perror("fopen()");
  }
  fprintf(fp,"%s\n",data);
  fclose(fp);
}
```

- Membuat `files.tsv` jika belum ada
- Nantinya ketika file sudah diterima, fungsi `recieveFile` (dijelaskan kemudian) akan memanggil `addFile()` guna mencetak info file ke `file.tsv`

**_1.C Recieve File_**

```c
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
```

- Sebagai pintu masuk file yang dikirim oleh client.
- Membuat direktori `FILES` dan membuka direktori tersebut.
- Akan terjadi komunikasi antar fungsi ini dengan client dimana fungsi ini akan memberikan `prompt` seputar apa saja yang diminta guna mengirim file ke server.
- Thread `scan` server akan dimatikan saat file dikirim ke server agar tidak terjadi deadlock.
- Kemudian kebalikan dari `sendFile()` di client, server akan menerima pertama nama file, kedua ukuran file yang akan diterima sebagai parameter loop, dan terakhir looping byte byte file.
- setelah selesai, terminal client akan di reset oleh `resetScreen()`
- Kemudian kembali ke direktori sebelumnya, lalu mengirimkan data info file ke `files.tsv`
- setelah selesai `thread scan` akan dipanggil kembali

**_Reset Screen_**

```c
void resetScreen() {
  char buffer[BUFSIZ];
  sprintf(buffer, "\e[H\e[2J");
  send(queue_front(&myQueue)->idsock, buffer, BUFSIZ, 0);
}
```

- Guna mengosongkan tampilan terminal client ke tampilan menu utama

**_1.F See File_**

```c
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
```

- Mencetak seluruh file yang berada di database
- membuka file `files.tsv`
- melakukan looping isi file, mengakali `newline` dengan `continue` jika `buffer[0]=='\n'`

**_Server Promt_**

```c
void prompt(int prompt_no)
```

- berisikan data tampilan yang akan dikirim ke client

**_Server Main Rutine_**

```c
void *server_scan_routine(void *arg)
```

- loop routine(menu utama) dari client.

**_Server Scan Rutine_**

```c
void *server_scan_routine(void *arg) {
  int i = *(int *)arg - 1;

  char buffer[BUFSIZ];
  while(1) {
    if ((queue_front(&myQueue)->len = recv(queue_front(&myQueue)->idsock, queue_front(&myQueue)->input, BUFSIZ, 0))> 0) {
    }
  }
```

- pintu utama tempat data dari client masuk, data yang masuk akan langsung ke struct atribut dari `queue` di `input`.
- `len` menyimpan ukuran byte dari data yang masuk.

**_1.A Passer dan Processor_**

```c
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
```

- `passer` Sebagai pengatur dari antrian koneksi, jika dipop queue, maka koneksi yang baru akan muncul alhasil thread koneksi baru terbuat dengan pemanggilan thread `processed thread`.

**Kendala :**\
Sulit mengatur program flow.

**Soal yang belum terselesaikan (Kendala) :**

- 1.D (Sulit mengatur program flow.)
- 1.E (Sulit mengatur program flow.)
- 1.G (Sulit mengatur program flow.)
- 1.H (Sulit mengatur program flow.)

**Bukti :**

---

## Soal 2

**[Source Code Soal 2](https://github.com/husinassegaff/soal-shift-sisop-modul-2-A06-2021/blob/main/soal2)**

**Deskripsi:**\
Soal akan diperjelas pada tiap subsoal.

### Soal 2.a

**Deskripsi:**\
Membuat program perkalian matriks 4x3 dengan matriks 3x6, lalu menampilkan hasilnya

**Pembahasan:**

```c
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
```

- Kita buat 2 matriks global sebagai input dan 1 matriks local sebagai output dengan tipe data int
- Looping pertama dan kedua digunakan untuk menginput data matriks
- Looping ketiga digunakan untuk mengisi matriks hasil dengan hasil operasi perkalian dua matriks input.
- Operasi perkalian dibuat dalam fungsi baru bernama kaliMatriks(int x, int y) dengan parameter x sebagai baris dan y sebagai kolom.

```
Input :
1 2 3
3 2 1
1 2 3
3 2 1
1 2 3 3 2 1
1 2 3 2 3 1
1 3 2 3 1 2

Output :
6 15 15 16 11 9
6 13 17 16 13 7
6 15 15 16 11 9
6 13 17 16 13 7
```

**Bukti :**

![Bukti2A](soal2/Bukti2A.png)

**Kendala :**\
Belum ditemukan kendala dalam mengerjakan soal ini

### Soal 2.b

**Deskripsi:**\
deskripsi.

**Pembahasan:**

```c
//kode
```

-
-
-

```
hasil
```

**Bukti :**

![Bukti2B](soal2/Bukti2B.png)

**Kendala :**
kendala.

### Soal 2.c

**Deskripsi:**\
deskripsi.

**Pembahasan:**

```c
//kode
```

-
-
-

```
hasil
```

**Bukti :**

![Bukti2C](soal2/Bukti2c.png)

**Kendala :**\
Kendala.

---

## Soal 3

**[Source Code Soal 3](https://https://github.com/husinassegaff/soal-shift-sisop-modul-2-A06-2021/blob/main/soal3/soal3.c)**

**Deskripsi:**\
Pada soal ini diminta untuk merapikan file dengan dikategorikan berdasarkan ekstensinya, yakni file dipindahkan ke folder yang memiliki nama sama dengan ekstensi file tersebut. Contohnya **file1.jpg** akan dipindah ke folder **jpg**

Adapun terdapat beberapa opsi,
1. opsi -f digunakan untuk menambahkan argumen file yang bisa dikategorikan sebanyak yang diinginkan pengguna. Output yang dikeluarkan adalah,
```
<!-- jika berhasil --> Berhasil Dikategorikan
<!-- jika gagal --> Sad, gagal :(
```
2. opsi -d digunakan untuk mengkategorikan directory. Namun, user hanya bisa memasukkan input sebanyak satu directory saja. Output yang dikeluarkan adalah,
```
<!-- jika berhasil --> Direktori sukses disimpan!
<!-- jika gagal --> Yah, gagal disimpan :(
```
3. opsi * akan mengkategorikan seluruh file yang ada di working directory ketika menjalankan program C tersebut. Adapun pada file yang tidak memiliki ekstensi, maka file tersebut akan disimpan di folder **Unknown**. Begitu juga dengan file hidden, akan masuk ke folder **Hidden**


**Pembahasan:**

```c
#include<stdio.h>
#include<ctype.h>
#include<string.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>
#include<dirent.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<sys/wait.h>

pthread_t tid[1000000];
pid_t child;
char cwd[1000];
char folder[1000];
char deepfolder[1000];
char deepfolder2[1000];
int err;

void *move(void *arg) {
	char *filepath = (char *)arg;
	char *extension = NULL;
	extension = strchr(filepath, '.');
	char ext[1000];
	memset(ext, '\0', sizeof(ext));
	if((extension-filepath-strlen(cwd)+1)==2 || (extension-filepath+1)==1) strcpy(ext, "Hidden");
	else if(extension) {
		extension++;
        for(int i=0; i<strlen(extension); i++) {
               	ext[i] = tolower(extension[i]);
    	}
	}
	else strcpy(ext, "Unknown");
    char *filename = NULL;
    filename = strrchr(filepath, '/');
    if(filename) filename++;
	else filename = filepath;

	char folderpath[1000];
	strcpy(folderpath, cwd);
	strcat(folderpath, "/");
	strcat(folderpath, ext);
	mkdir(folderpath, S_IRWXU);
	if(strlen(deepfolder2)>1){
		char fullname[1000];
		strcpy(fullname, deepfolder2);
		strcat(fullname, "/");
		strcat(fullname, filename);
		strcat(folderpath, "/");
        strcat(folderpath, filename);
        rename(fullname, folderpath);
	}
	else if(strlen(folder) > 1) {
		char fullname[1000];
		strcpy(fullname, folder);
		strcat(fullname, "/");
		strcat(fullname, filename);
		strcat(folderpath, "/");
        strcat(folderpath, filename);
        rename(fullname, folderpath);
	}
	else {					
	    strcat(folderpath, "/");
        strcat(folderpath, filename);
		rename(filepath, folderpath);
	}
	return NULL;
}

void minf(int argc, char *argv[]){
  for(int i=2; i<argc; i++) {
		err = pthread_create(&tid[i], NULL, move, (void *)argv[i]);
		if(err != 0) printf("File %d : Sad, gagal :(\n", i-1);
        else printf("File %d : Berhasil Dikategorikan\n", i-1);
	}
	for(int j=2; j<argc; j++) pthread_join(tid[j], NULL);
    return;
}

void deep(){
	DIR *dir;
	struct dirent *tmp;
	int i=0;
	memset(deepfolder2, '\0', sizeof(deepfolder2));
	strcpy(deepfolder2,deepfolder);
	printf("%s\n", deepfolder2);
	dir = opendir(deepfolder2);
	while((dir!=NULL) && (tmp=readdir(dir))) {
		if(tmp->d_type==DT_DIR && strcmp(tmp->d_name, ".")!=0 && strcmp(tmp->d_name, "..")!=0){
			strcat(deepfolder2,tmp->d_name);
			strcat(deepfolder2,"/");
			deep();
		}
        if(strcmp(tmp->d_name, ".")==0 || strcmp(tmp->d_name, "..")==0 || strcmp(tmp->d_name, "soal3.c")==0 || strcmp(tmp->d_name, "soal3")==0 || tmp->d_type==DT_DIR) 
		continue;
        err = pthread_create(&tid[i], NULL, move, tmp->d_name);
        if(err != 0) printf("Yah, gagal disimpan :(\n");
        else printf("Direktori sukses disimpan!\n");
        i++;
    }
    for(int j=0; j<i; j++) pthread_join(tid[j], NULL);
    closedir(dir);
    return;
}

void other(int argc, char *argv[]){
  DIR *dir;
	struct dirent *tmp;
	int i=0;
	if(strcmp(argv[1], "-d") == 0) {
    	dir = opendir(argv[2]);
		strcpy(folder, argv[2]);
	}
	else if((argv[1][0]=='*') && (strlen(argv[1])==1)) {
		dir = opendir(cwd);
		strcpy(folder, cwd);
	}
	else {
		printf("Butuh Argumen -f atau -d atau \\*\n");
        return;
	}
	while((dir!=NULL) && (tmp=readdir(dir))) {
		if(tmp->d_type==DT_DIR && strcmp(tmp->d_name, ".")!=0 && strcmp(tmp->d_name, "..")!=0){
			strcpy(deepfolder,folder);
			strcat(deepfolder,"/");
			strcat(deepfolder,tmp->d_name);
			strcat(deepfolder,"/");
			deep();
		}
        if(strcmp(tmp->d_name, ".")==0 || strcmp(tmp->d_name, "..")==0 || strcmp(tmp->d_name, "soal3.c")==0 || strcmp(tmp->d_name, "soal3")==0 || tmp->d_type==DT_DIR) 
		continue;
        err = pthread_create(&tid[i], NULL, move, tmp->d_name);
        if(err != 0) printf("Yah, gagal disimpan :(\n");
        else printf("Direktori sukses disimpan!\n");
        i++;
    }
    for(int j=0; j<i; j++) pthread_join(tid[j], NULL);
    closedir(dir);
    return;
}

int main(int argc, char *argv[]) {
	getcwd(cwd, sizeof(cwd));
	memset(folder, '\0', sizeof(folder));
	memset(deepfolder, '\0', sizeof(deepfolder));
	memset(deepfolder2, '\0', sizeof(deepfolder2));
	if(argc < 2) {
		printf("Butuh Argumen -f atau -d atau \\*\n");
		return 0;
	}
	if(strcmp(argv[1], "-f") == 0) {
		minf(argc,argv);
	}
	else {
		other(argc,argv);
	}
	return 0;
}
```

- Untuk opsi **-f** dapat diselesaikan dengan membuat thread pada tiap file yang ingin dikategorikan serta melakukan thread join agar bisa berjalan secara bersamaan. Berikut kode yang menjalankan hal tersebut

```
for(int i=2; i<argc; i++) {
  err = pthread_create(&tid[i], NULL, move, (void *)argv[i]);
  if(err != 0) printf("File %d : Sad, gagal :(\n", i-1);
  else printf("File %d : Berhasil Dikategorikan\n", i-1);
}
for(int j=2; j<argc; j++) pthread_join(tid[j], NULL);
``` 
- Adapun untuk opsi **-d** dan * pada dasarnya cara kerjanya sama, hanya saja berbeda pada directory yang dibuka. Untuk **-d** membuka directory yang disebutkan di argument. Maka digunakan kode sebagai berikut untuk menunjuk directory tersebut,
```
if(strcmp(argv[1], "-d") == 0) {
  dir = opendir(argv[2]);
  strcpy(folder, argv[2]);
}
```
- Sedangkan pada opsi * yang ditunjuk adalah directory saat ini. berikut kode yang menjalankan perintah tersebut,
```
else if((argv[1][0]=='*') && (strlen(argv[1])==1)) {
  dir = opendir(cwd);
  strcpy(folder, cwd);
}
```
- Kemudian, setiap file yang berada di directory yang ditunjuk opsi **-d** ataupun directory saat ini jika menggunakan * akan diperiksa satu per satu. Apabila yang diperiksa benar merupakan file, maka akan dibuat thread baru untuk kategorisasi file tersebut ke dalam folder dan juga menggunakan thread join supaya dapat berjalan secara bersamaan. Berikut bagian implementasi kodenya,
```
while((dir!=NULL) && (tmp=readdir(dir))) {
  if(tmp->d_type==DT_DIR && strcmp(tmp->d_name, ".")!=0 && strcmp(tmp->d_name, "..")!=0){
    strcpy(deepfolder,folder);
    strcat(deepfolder,"/");
    strcat(deepfolder,tmp->d_name);
    strcat(deepfolder,"/");
    deep();
  }

  if(strcmp(tmp->d_name, ".")==0 || strcmp(tmp->d_name, "..")==0 || strcmp(tmp->d_name, "soal3.c")==0 || strcmp(tmp->d_name, "soal3")==0 || tmp->d_type==DT_DIR) continue;

  err = pthread_create(&tid[i], NULL, move, tmp->d_name);

  if(err != 0) printf("Yah, gagal disimpan :(\n");
  else printf("Direktori sukses disimpan!\n");
  i++;
}
for(int j=0; j<i; j++) pthread_join(tid[j], NULL);
closedir(dir);
```

- Adapun untuk memindahkan file dan dikategorikan perlu diambil ekstensi pada file tersebut sebagai penamaan folder dengan menggunakan `strchr` berupa pemisah "."
- Apabila ektensi file tidak ada, maka file tersebut akan dimasukkan ke folder **Unknown**
- Adapun implementasi kodenya berupa mengubah nama filenya untuk dipindahkan


```
void *move(void *arg) {
	char *filepath = (char *)arg;
	char *extension = NULL;
	extension = strchr(filepath, '.');
	char ext[1000];
	memset(ext, '\0', sizeof(ext));
	if((extension-filepath-strlen(cwd)+1)==2 || (extension-filepath+1)==1) strcpy(ext, "Hidden");
	else if(extension) {
		extension++;
        for(int i=0; i<strlen(extension); i++) {
               	ext[i] = tolower(extension[i]);
    	}
	}
	else strcpy(ext, "Unknown");
    char *filename = NULL;
    filename = strrchr(filepath, '/');
    if(filename) filename++;
	else filename = filepath;

	char folderpath[1000];
	strcpy(folderpath, cwd);
	strcat(folderpath, "/");
	strcat(folderpath, ext);
	mkdir(folderpath, S_IRWXU);
	if(strlen(deepfolder2)>1){
		char fullname[1000];
		strcpy(fullname, deepfolder2);
		strcat(fullname, "/");
		strcat(fullname, filename);
		strcat(folderpath, "/");
        strcat(folderpath, filename);
        rename(fullname, folderpath);
	}
	else if(strlen(folder) > 1) {
		char fullname[1000];
		strcpy(fullname, folder);
		strcat(fullname, "/");
		strcat(fullname, filename);
		strcat(folderpath, "/");
        strcat(folderpath, filename);
        rename(fullname, folderpath);
	}
	else {					
	    strcat(folderpath, "/");
        strcat(folderpath, filename);
		rename(filepath, folderpath);
	}
	return NULL;
}
```

**Bukti :**
- Bukti penerapan opsi **-f**
  - Mengkategorikan file **Archive.zip**
![Bukti3(1)](soal3/Bukti3(1).PNG)
  - Hasil kategori
![Bukti3(2)](soal3/Bukti3(2).PNG)

- Bukti penggunaan opsi **-d**
  - Mengkatogerikan file ekstensi JPG yang berada di directory **/home/husin/kategorijpg**
![Bukti3(3)](soal3/Bukti3(3).PNG)
  - Hasilnya berupa empat folder dengan nama file masing-masing
![Bukti3(4)](soal3/Bukti3(4).PNG)

- Bukti penggunaan opsi *
  - Mengkategorikan file yang berada di directory **/home/husin/soal3**
![Bukti3(5)](soal3/Bukti3(5).PNG)
  - Hasil kategori
  ```
  Direktori sukses disimpan!
  Direktori sukses disimpan!
  Direktori sukses disimpan!
  Direktori sukses disimpan!
  Direktori sukses disimpan!
  Direktori sukses disimpan!
  Direktori sukses disimpan!
  Direktori sukses disimpan!
  Direktori sukses disimpan!
  Direktori sukses disimpan!
  Direktori sukses disimpan!
  Direktori sukses disimpan!
  Direktori sukses disimpan!
  Direktori sukses disimpan!
  Direktori sukses disimpan!
  /home/husin/soal3/hasduhasudha.jpg/
  Direktori sukses disimpan!
  Direktori sukses disimpan!
  Direktori sukses disimpan!
  Direktori sukses disimpan!
  Direktori sukses disimpan!
  /home/husin/soal3/adjaisd/
  Direktori sukses disimpan!
  /home/husin/soal3/sdauh kasdks.ZIP/
  Direktori sukses disimpan!
  Direktori sukses disimpan!
  Direktori sukses disimpan!
  Direktori sukses disimpan!
  Direktori sukses disimpan!
  Direktori sukses disimpan!
  Direktori sukses disimpan!
  Direktori sukses disimpan!
  Direktori sukses disimpan!
  Direktori sukses disimpan!
  Direktori sukses disimpan!
  Direktori sukses disimpan!
  Direktori sukses disimpan!
  Direktori sukses disimpan!
  Direktori sukses disimpan!
  Direktori sukses disimpan!
  Direktori sukses disimpan!
  /home/husin/soal3/folderku/
  Direktori sukses disimpan!
  Direktori sukses disimpan!
  Direktori sukses disimpan!
  Direktori sukses disimpan!
  /home/husin/soal3/ashduashd.txt/
  Direktori sukses disimpan!
  Direktori sukses disimpan!
  Direktori sukses disimpan!
  Direktori sukses disimpan!
  Segmentation fault
  ```
  ![Bukti3(5)](soal3/Bukti3(5).PNG)

**Kendala :**\
Pada pengkategorian file masih terdapat kesalahan dalam penamaan folder dan rekursifnya yang belum berjalan dengan sempurna
