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
- [Soal 1](#soal-1)
  - [Soal Client](#soal-client)
  - [Soal Server](#soal-server)
  
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
deskripsi.

### Soal Client

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

**Bukti   :**
![BuktiClient1](soal1/Client/BuktiClient1.PNG)

**Kendala :**\
kendala.

### Soal Server
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
**Bukti   :**
![BuktiServer1](soal1/Server/BuktiServer1.PNG)

**Kendala :**\
kendala.

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
**Bukti   :**

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
**Bukti   :**

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

![Bukti3](soal3/Bukti3.PNG)

**Kendala :**\
Kendala.