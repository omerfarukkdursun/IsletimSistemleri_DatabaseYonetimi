#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

void clearquery(char* query,int length){
    int i=0;
    while (i<100) {
        *(query+i)='\0';
        i++;
    }
}


int main() {
    
    int fd;
    char * myfifo = "/tmp/myfifo";
    mkfifo(myfifo, 0666);
    char inputQuery[100]={"/0"};
    char query[3600]={"\0"};
    char sonuc;
    char temp[3600]={"\0"};
    
    while (1) {
        
        fd = open(myfifo, O_WRONLY);
        printf("%s","Sorgu: ");
        fgets(inputQuery, 80, stdin);
        
        write(fd, inputQuery, strlen(inputQuery));
        close(fd);
       
        
        fd = open(myfifo, O_RDONLY);
        read(fd, query, 3600);
        close(fd);
        if (!strcmp(query,"cik")) {
            break;
        }
        strcpy(temp,query);
        if(query[0] == '\0'){
            printf("NULL\n");
            
        }else{
            printf("\n-------- Sonuclar ---------\n%s\n",temp);
            printf("Sonuclar kaydedilsin mi?  e/h\nCevap: ");
            gets(&sonuc);
            if (sonuc=='e') {
                int pipefd[2];
                if(pipe(pipefd)<0){
                    exit(0);
                    
                }
                int c;
                
                int fd = fork();
                if(fd < 0) {
                    exit(0);
                }
                else if (fd == 0){
                    write(pipefd[1],temp,strlen(temp)+1);
                    c = execv("kaydet",NULL);
                    
                    
                }
                else {
                    wait(&c);
                    close(pipefd[0]);
                    close(pipefd[1]);
                    printf("SONUCLAR KAYDEDILDI!!!!\n\n");
                    
                }
            }else{
                printf("SONUCLAR KAYDEDILMEDI!!!\n\n\n");
            }
            
        }
        clearquery(temp,3600);
        clearquery(query,3600);
        clearquery(inputQuery, 100);
    }
}

