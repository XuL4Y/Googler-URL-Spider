/*
Googler v 0.03
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
#include <time.h>

//set DEBUG ON
#define BUGVIEW 1
#define DEBUG(x, s...) do { \
	if (!BUGVIEW) { break; } \
		time_t t = time(NULL); \
		char *d = ctime(&t); \
		fprintf(stderr, "\n--- DEBUG-START ---\n\n %.*s %s[%d] %s(): \n", \
		(int)strnlen(d,4) - 1, d, __FILE__, \
		__LINE__, __FUNCTION__); \
		fprintf(stderr, x, ## s); \
		fprintf(stderr,"\n\n--- DEBUG-END ---\n"); \
	} while (0);

void *xcalloc (size_t mem, size_t size);
void *xrealloc (void *ptr, size_t size);
void *xmalloc (unsigned int len);
char *StrRep (char *st, char *orig, char *repl,int mim);
char *regexp (char *string, char *patrn);
static size_t WriteMemoryCallback(void *ptr, size_t size, size_t nmemb, void *data);
void chomp (char * str);
int WriteFile (char *file,char *str);

void 
help()
{
 puts(
  "Simple Googler v0.03\n"
  "./code 'dork' number_of_searchs log.txt\n"
  " ------------------------------------\n"
  " by Cooler_ , BugSec Team , contact: c00f3r[at]gmail[dot]com\n"
 );
}

struct MemoryStruct {
 char *memory;
 size_t size;
};

int 
main(int argc, char ** argv)
{
 struct MemoryStruct chunk;

// curl vars
 CURL *curl_handle;  
 curl_global_init(CURL_GLOBAL_ALL); 

// argv file
 char *arquivo=NULL;
// URL googler make
 char *GOOGLE1="http://www.google.com/search?&q=";
 char *GOOGLE2="&start=";
 char *NUM=(char *)xmalloc(10*sizeof(char));
 char *GoogleURL=(char *)xmalloc(1*sizeof(char));
 char *BUSCA=(char *)xmalloc(50*sizeof(char));
 int  times=0;

//REGEX vars
 char *match=NULL;
 char *replace="";
 char *regex="(https?|ftp)+://[^ \t\r\n\v\f]*";
 int count=0,sum_mem=0;

	if ( argc<3 ) 
	{
		help();    
		exit(0);    
	}    

	if ( strnlen(argv[1],50)<=49 )
		BUSCA=argv[1];
	else
	{
		DEBUG("argv from 'dork' very large... error");
		exit(0);
	}

	if ( strnlen(argv[2],8)<=4 )
		times=atoi(argv[2]);
	else 
	{
		DEBUG("argv from 'times' very large... error");
		exit(0);
	}

	if ( strnlen(argv[3],16)<=15 )
		arquivo=argv[3]; 
	else 
	{
		DEBUG("argv from 'log name' very large... error");
		exit(0);
	}

	fprintf(stdout,"\nSearch %s in  %d pages, log at %s \n",BUSCA,times,arquivo);

	while ( times )
	{
		sprintf(NUM,"%d",times);
		sum_mem=strlen(GOOGLE1)+strlen(GOOGLE2)+strlen(BUSCA)+strlen(NUM)+4;
		GoogleURL=(char *)xrealloc(GoogleURL,sum_mem);
 		sprintf(GoogleURL,"%s%s%s%d0",GOOGLE1,BUSCA,GOOGLE2,times);

		puts(GoogleURL);
		puts("------------------\n");
		chunk.memory=NULL; 
		chunk.size=0;  
  
		curl_handle=curl_easy_init();
		curl_easy_setopt(curl_handle, CURLOPT_URL,GoogleURL);
		curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION,WriteMemoryCallback);
		curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA,(void *)&chunk);
		curl_easy_setopt(curl_handle, CURLOPT_USERAGENT,"Mozilla/4.0 (Linux Slackware pl; rv:1.9.0.17) Gecko/2009122116 Firefox/4.1.37 ");
		curl_easy_setopt(curl_handle, CURLOPT_ENCODING,"gzip,deflate");
		curl_easy_setopt(curl_handle,CURLOPT_COOKIEJAR,"cookie_jars.txt");
 		curl_easy_setopt(curl_handle,CURLOPT_VERBOSE,0); // set one to use DEBUG 
		curl_easy_perform(curl_handle);
		curl_easy_cleanup(curl_handle);

		if ( chunk.memory )
		{
			while( strstr(chunk.memory,"http") )
			{
				match=regexp(chunk.memory,regex);
				if ( match )
					if ( !( strstr(match,"google") || strstr(match,"youtube") || strstr(match,"orkut") || strstr(match,"gstatic")) )
					{
// remove token
						chomp(match);
						puts(match);
						WriteFile(arquivo,match);
						count++;
					}
// erase URL, run next URL
				chunk.memory=StrRep(chunk.memory,match,replace,chunk.size);
				if ( match )
				{	
					free(match);
					match=NULL;
    				}

   			} 
  		}

  		if (chunk.size < 10)
  		{ 
   			free(chunk.memory);
   			chunk.memory=NULL;	
  		}	

  		curl_global_cleanup();

  		times--;
 	}

	if (NUM)
	{  
		free(NUM);
		NUM=NULL;
	}
	if (GoogleURL)
	{
		free(GoogleURL);
		GoogleURL=NULL;
	}

 chunk.memory=calloc(1,1);
 memset(chunk.memory,0,1);

	if ( chunk.memory)
	{
		free(chunk.memory);
		chunk.memory=NULL;
	}

 exit(0);
}


void *
xcalloc (size_t mem, size_t size) 
{
	void *ptr = calloc (mem, size);

	if (ptr == NULL) 
		DEBUG("Error in calloc() ");

	return ptr;
}

void *
xrealloc (void *ptr, size_t size) 
{
 void *p = realloc (ptr, size);

	if ( p==NULL) 
		DEBUG("Error in realloc()");

	return p;
}


void *
xmalloc (unsigned int len)
{
 void *ptr;

 ptr=malloc(len);

	if ( ptr==NULL ) 
 	{
		DEBUG("fail malloc with size %d",len);
	} else {
		ptr=xrealloc(ptr,len);
	}

 return ptr;
}

static size_t 
WriteMemoryCallback(void *ptr,size_t size,size_t nmemb,void *data) 
{
 size_t realsize=size*nmemb;
 struct MemoryStruct *mem=(struct MemoryStruct *)data;
 mem->memory=xrealloc(mem->memory,mem->size+realsize+1);

	if ( mem->memory ) 
	{
		memcpy(&(mem->memory[mem->size]),ptr,realsize);
		mem->size+=realsize;
		mem->memory[mem->size]=0;
	}

 return realsize;
}

//string replace
char *
StrRep (char *st,char *orig,char *repl,int mim) 
{
 char bufer[mim];
 char *ch;

	if( !orig )
		return "\0";

	if( !(ch=strstr(st, orig)) )
		return st;

 strncpy(bufer, st, ch-st);  
 bufer[ch-st] = 0;
 sprintf(bufer+(ch-st),"%s%s",repl,ch+strlen(orig));

 char *out=bufer;

 return out;
}

char *regexp (char *string, char *patrn) 
{     
 int i=0,w=0,len=0,begin=0,end=0;
 char *word=NULL;

 regex_t rgT;
 regmatch_t match;

 regcomp(&rgT,patrn,REG_EXTENDED);
 
 word=xcalloc(1,1);

	if( !(regexec(&rgT,string,1,&match,0)) )
	{
		begin=(int)match.rm_so;
		end=(int)match.rm_eo;
		len=end-begin;
		word=(char *)xrealloc(word,len+1);
		i=begin;
		while(i<end)
			word[w]=string[i],i++,w++;
		word[w]=0;
  	}
 regfree(&rgT);

 return word;
}

void 
chomp(char *str)
{
	while ( *str ) 
	{
		if( *str=='"' ) 
		{
			*str=0;
			return;
		}
		str++;
	}
}

int WriteFile(char *file,char *str)
{
 FILE *arq;
 
 arq=fopen(file,"a"); 

	if ( !arq ) 
	{
		DEBUG("error in WriteFile() %s",file); 
		return 0;
	}

 fprintf(arq,"%s\n",str); 
 fclose(arq); 

 return 1;
}
