#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

#define MAX_QUERY_SIZE 7
#define MAX_QUERY_ELEMENT_LENGTH 15
#define NAME_LENGTH 15
#define NUMBER_LENGTH 3

char** reserved (int row,int colum){ //verilen satır ve sütuna göre heapten yer ayırıyorum.
    char** base = calloc(row, sizeof(char*));
    int i;
    for (i=0; i<row; i++) {
        *(base+i)=(char*)calloc(colum,1);
    }
    return base;
}

char** get_query_element(char* searched_query, int size_of_elements){ //sorgunun elemanlarını bir tane diziye atarak diziyi döndürüyorum.
    char** base = reserved (MAX_QUERY_SIZE ,MAX_QUERY_ELEMENT_LENGTH);
    char* element;
    int i=0;
    element= strtok(searched_query, "\n =");
    while (element!=NULL) {
        strcpy(*(base+i),element);
        element= strtok(NULL, "\n =");
        i++;
    }
    
    return base;
}
int get_row_count(FILE* file){//dosyadaki satır saysını buluyorum.
    int row_count=1;
    char c;
    while ((c=getc(file))!=EOF) {
        if (c=='\n') {
            row_count++;
        }
    }
    rewind(file);
    return row_count;
}

void heap_free(char** base,int row){ //heapteki alanı temizliyorum.
    int i;
    for (i=0; i<row; i++) {
        free(*(base+i));
    }
    free(base);
    
}


char* database_search(char **elements, char* database){ 
    //Databasenin bulunduğu dosya açılıp içerisindeki veriler reserve edilmiş arraye atılır.
    FILE* database_file;
    database_file = fopen(database, "r");
    char** database_names =reserved(get_row_count(database_file), NAME_LENGTH);
    char** database_numbers =reserved(get_row_count(database_file), NUMBER_LENGTH);
    int even_check=0;
    int row=0;
    char readed[NAME_LENGTH]={"\0"};
    while (fscanf(database_file, "%s",readed)!=EOF) {
        if (!even_check) {
            strcpy(*(database_names+row),readed);
            even_check=1;
        }else{
            strcpy(*(database_numbers+row),readed);
            row++;
            even_check=0;
        }
        
    }
    //Dosya pointerı başa alınrak stır sayısı adedince belirlenmiş uzunluğ göre yer ayırtılır.
    rewind(database_file);
    char *returned_text = calloc(get_row_count(database_file)*(NAME_LENGTH+NUMBER_LENGTH+2), 1);
    fclose(database_file);
    int i,j=row-1;
    //parse edilmiş sorgu stringine göre verileri bulunduğu arraylerden sorgulama yapılır ve sonuç arreyine eklenir.
    if (!strcmp(*(elements+4), "where")) {
        if (!strcmp(*(elements+5), "name")) {
            if (!strcmp(*(elements+1), "*")) {
                for (i=0; i<=j; i++) {
                    if (!strcmp(*(database_names+i), *(elements+6))) {
                        strcat(returned_text, *(database_names+i));
                        strcat(returned_text, " ");
                        strcat(returned_text, *(database_numbers+i));
                        strcat(returned_text, "\n");
                        
                        
                    }
                }
            }else if (!strcmp(elements[1], "name") ){
                for (i=0; i<=j; i++) {
                    if (!strcmp(*(database_names+i), elements[6])) {
                        strcat(returned_text, *(database_names+i));
                        strcat(returned_text, "\n");
                    }
                }
            }else if (!strcmp(elements[1], "number")) {
                for (i=0; i<=j; i++) {
                    if (!strcmp(*(database_names+i), elements[6])) {
                        strcat(returned_text, *(database_numbers+i));
                        strcat(returned_text, "\n");
                    }
                }
            }
        }else if (!strcmp(elements[5], "number")) {
            if (!strcmp(elements[1], "*")) {
                for (i=0; i<=j; i++) {
                    if (!strcmp(*(database_numbers+i), elements[6])) {
                        strcat(returned_text, *(database_names+i));
                        strcat(returned_text, " ");
                        strcat(returned_text, *(database_numbers+i));
                        strcat(returned_text, "\n");
                    }
                }
            }else if (!strcmp(elements[1], "name") ){
                for (i=0; i<=j; i++) {
                    if (!strcmp(*(database_numbers+i), elements[6])) {
                        strcat(returned_text, *(database_names+i));
                        strcat(returned_text, "\n");
                    }
                }
            }else if (!strcmp(elements[1], "number")) {
                for (i=0; i<=j; i++) {
                    if (strcmp(*(database_numbers+i), elements[6])) {
                        strcat(returned_text, *(database_numbers+i));
                        strcat(returned_text, "\n");
                    }
                }
            }
        }
    }else{
        if (!strcmp(elements[1], "*")) {
            for (i=0; i<=j; i++) {
                strcat(returned_text, *(database_names+i));
                strcat(returned_text, " ");
                strcat(returned_text, *(database_numbers+i));
                strcat(returned_text, "\n");
            }
        }else if (!strcmp(elements[1], "name") ){
            for (i=0; i<=j; i++) {
                strcat(returned_text, *(database_names+i));
                strcat(returned_text, "\n");
            }
        }else if (!strcmp(elements[1], "number")) {
            for (i=0; i<=j; i++) {
                strcat(returned_text, *(database_numbers+i));
                strcat(returned_text, "\n");
            }
        }
        
    }
    //heapten ayrılan yerler serbest bırakılır ve program.c dosyasına aktarılacak array dondurulur.
    heap_free(database_names, j);
    heap_free(database_numbers, j);
    return returned_text;
}


void clearquery(char* query,int length,int col){ //uzunlukları verilen arrayin tüm elemanlarını null yapar.
    int i=0,j=0;
    for (i=0; i<length; i++) {
        *(query+i*col)='\0';
        for (j=1; j<col ; j++) {
            *(query+i*col+j)='\0';
        }
    }
}

void print_heap(char **printed,int row){ //test için yazılmış olup heapteki veriler kontrol edilmiştir.
    int i;
    for (i=0; i<row; i++) {
        printf("%s",*(printed+i));
    }
    
}

int main()
{
    
    int fd;
    char *myfifo = "/tmp/myfifo";
    mkfifo(myfifo, 0666);
    
    char query[100]={"\0"};
    while (1)
    {
        fd = open(myfifo, O_RDONLY);
        read(fd, query, 100);
        close(fd);
        if (!strcmp(query,"cikis\n")) {
            fd = open(myfifo, O_WRONLY);
            write(fd, "cik", strlen("cik"));
            close(fd);
            break;
        }
        char ** elements = get_query_element(query,MAX_QUERY_SIZE);
        char* returned = database_search(elements, *(elements+3));
        heap_free(elements, MAX_QUERY_SIZE);
        
        fd = open(myfifo, O_WRONLY);
        write(fd, returned, strlen(returned));
        free(returned);
        close(fd);
        clearquery(query,100,0);
    }
    
    return 0;
}

