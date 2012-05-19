/*
Googler v 0.001
autor:   Cooler_
contato: c00f3r[at]gmail[dot]com

Just Another Googler Spider
to help you in google hacking                   

-------------------------------------------- 
THANKS sigsegv and di0_ to help me in debug ;-)
---
HOW to USE ?
apt-get install libcurl-dev; gcc -o Googler Googler.c -lcurl; ./Googler

*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <regex.h>
#include <curl/curl.h>
#include <curl/easy.h>

void *xmalloc(unsigned int len);
char *StrRep(char *st, char *orig, char *repl,int mim);
char *regexp (char *string, char *patrn);
static size_t WriteMemoryCallback(void *ptr, size_t size, size_t nmemb, void *data);
void chomp (char * str);
int WriteFile(char *file,char *str);

void help()
{
 puts(
  "Simple Googler v0.002\n"
  "./code dork number_of_searchs log.txt\n"
  " ------------------------------------\n"
  " by Cooler_ , BugSec Team , contact: c00f3r[at]gmail[dot]com\n"
 );
}

struct MemoryStruct {
 char *memory;
 size_t size;
};

int main(int argc, char ** argv)
{
 struct MemoryStruct chunk;

// curl vars
 CURL *curl_handle;  
 curl_global_init(CURL_GLOBAL_ALL); 

// file
 char *arquivo=(char *)xmalloc(sizeof(char)*12);

// Google URL vars
 char *GOOGLE1="http://www.google.com/search?&q=";
 char *GOOGLE2="&start=";
 char *NUM=(char *)xmalloc(256*sizeof(char));
 char *GoogleURL=(char *)xmalloc(768*sizeof(char));
 char *BUSCA=(char *)xmalloc(128*sizeof(char));
 int  times=0;

//REGEX vars
 char *match;
 char **stack=(char **)xmalloc(2600*sizeof(char *));
 char *replace="";
 char *regex="(https?|ftp)+://[^ \t\r\n\v\f]*";
 int count=0;

 if(argc<3) 
 {
  help();    
  return 0;    
 }    

 if(strlen(argv[1])<=127)
  BUSCA=argv[1];
 else
 {
  puts("argv from 'dork' very large... error");
  return 0;
 }

 if(strlen(argv[2])<=4)
  times=atoi(argv[2]);
 else 
 {
  puts("argv from 'times' very large... error");
  return 0;
 }

 if(strlen(argv[3])<=10)
  arquivo=argv[3]; 
 else 
 {
  puts("argv from 'arquivo' very large... error");
  return 0;
 }

 fprintf(stdout,"\nProcurando por %s em  %d paginas\n",BUSCA,times);

 while(times)
 {
  strncpy(GoogleURL,GOOGLE1,sizeof(GOOGLE1)*strlen(GOOGLE1));
  strncat(GoogleURL,BUSCA,sizeof(BUSCA)*strlen(BUSCA));
  strncat(GoogleURL,GOOGLE2,sizeof(GOOGLE2)*strlen(GOOGLE2));
  sprintf(NUM,"%255d",times);
  strncat(GoogleURL,NUM,sizeof(NUM)); 
  strncat(GoogleURL,"0",sizeof(char));

  puts(GoogleURL);
  puts("------------------\n");
  chunk.memory=NULL; 
  chunk.size=0;  
  
  curl_handle=curl_easy_init();
  curl_easy_setopt(curl_handle, CURLOPT_URL,GoogleURL);
  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION,WriteMemoryCallback);
  curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA,(void *)&chunk);
  curl_easy_setopt(curl_handle, CURLOPT_USERAGENT,"Mozilla/5.0 (Windows; U; Windows NT 5.1; pl; rv:1.9.0.17) Gecko/2009122116 Firefox/3.0.17 ");
  curl_easy_perform(curl_handle);
  curl_easy_cleanup(curl_handle);

  if(chunk.memory)
  {
   while(strstr(chunk.memory,"http"))
   {
    match=regexp(chunk.memory,regex);
    if(match)
     if( !( strstr(match,"google") || strstr(match,"youtube") || strstr(match,"orkut") || strstr(match,"gstatic")) )
     {
// tirando " das urls
      chomp(match);
      puts(match);
      WriteFile(arquivo,match);
      *(stack+count)=match;
      count++;
     }
// apagamos a URL achada para pegar a próxima
    chunk.memory=StrRep(chunk.memory,match,replace,chunk.size);

    free(match);
   } 
  }

  if(chunk.size < 10) 
   free(chunk.memory);
  curl_global_cleanup();

  times--;
 }
 
 free(arquivo);
 free(NUM);
 free(GoogleURL);
 free(stack);

 return 0;
}

// alinhamento da memória HEAP :-X
void *xmalloc(unsigned int len)
{
 void *ptr;

 ptr=malloc(len);
 if(ptr==NULL) 
  fprintf(stderr,"fail malloc with size %d",len);

 return ptr;
}

static size_t WriteMemoryCallback(void *ptr,size_t size,size_t nmemb,void *data) 
{
 size_t realsize=size*nmemb;
 struct MemoryStruct *mem=(struct MemoryStruct *)data;
 mem->memory=realloc(mem->memory,mem->size+realsize+1);
  if(mem->memory) 
  {
   memcpy(&(mem->memory[mem->size]),ptr,realsize);
   mem->size+=realsize;
   mem->memory[mem->size]=0;
  }
 return realsize;
}

//string replace
char *StrRep(char *st,char *orig,char *repl,int mim) 
{
 char bufer[mim];
 char *ch;

 if(!orig)
  return "\0";

 if(!(ch=strstr(st, orig)) )
  return st;

 strncpy(bufer, st, ch-st);  
 bufer[ch-st] = 0;
 sprintf(bufer+(ch-st),"%s%s",repl,ch+strlen(orig));

 char *out=bufer;

 return out;
}

char *regexp(char *string, char *patrn) 
{     
 int i=0,w=0,len=0,begin=0,end=0;                  
 char *word=NULL;

 regex_t rgT;
 regmatch_t match;

 regcomp(&rgT,patrn,REG_EXTENDED);

  if(!(regexec(&rgT,string,1,&match,0))) 
  {
   begin=(int)match.rm_so;
   end=(int)match.rm_eo;
   len=end-begin;
   word=(char *)malloc(len+1);
   i=begin; 
   while(i<end) 
    word[w]=string[i],i++,w++; 
   word[w]=0;
  }
 regfree(&rgT); 

 return word;
}

// remove select char
void chomp(char *str)
{
  while(*str) 
  {
   if(*str=='"') 
   {
    *str = 0;
    return;
   }
   str++;
  }
}

int WriteFile(char *file,char *str)
{
 FILE *arq;
 
 arq=fopen(file,"a"); 
  if(!arq) 
  {
   fprintf(stdout,"error in WriteFile() %s",file); 
   return 0;
  }
 fprintf(arq,"%s\n",str); 
 fclose(arq); 
 return 1;
}