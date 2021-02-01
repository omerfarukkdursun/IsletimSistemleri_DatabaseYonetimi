#include<stdio.h> 
#include<string.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<sys/types.h> 
#include<sys/wait.h> 
#include <signal.h>

//Arguman listemizin 0. indexindeki ifadeyi,
//argüman listemisin 1. indexindeki sayi kadar tekrarlamamizi saglayan for islemi.

int main(int argc, char const *argv[]) 
{
    char sonuc;       
    char str[100]={'\0'};
    read(3,str,100);
    FILE *f = fopen("sonuc.txt","a");
    fprintf(f,"%s\n",str);
    fclose(f);
    return 0;
}
