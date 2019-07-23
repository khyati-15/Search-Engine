#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<string.h>
struct fileno
{
char	url[100];
int	countf;
struct	fileno	*up;
struct	fileno	*down;
};

struct	Node
{
char	word[100];
struct fileno	*f_ptr;
struct	Node	*prev;
struct	Node *next;
};

struct	hashtable
{
struct	Node	*ptr;
int	count;
};


char* getfiledata(int	i);
int getfilesize(int	i);
char*	getfilepath(int	i);
int	getnextindex(int	start,char	*tag,int	i,char	*filedata);
void	findwords(char	*str,char	**words,int	*s);
char*	getword(char	*str,int	a,int	b);
int	word_count(char	*word,int	index,char	**words,int	size);
int	generate_key(char	*word);
int	check_hash(int	key,struct	hashtable	*hash);
void	movetofile(struct	hashtable	*hash);

void	movetofile(struct	hashtable	*hash)
{
FILE	*file;
file=fopen("/home/khyati/Desktop/backup.txt","wb+");
struct	Node	*node=(struct	Node*)malloc(sizeof(struct	Node));
for(int	i=65;i<122;i++)
{
int	count=hash[i].count;
if(hash[i].count>0)
{
//printf("%d,%d,%s\n",i,count,hash[i].ptr->word);
fprintf(file,"\n%d\n",i);
node=hash[i].ptr;
while(node!=NULL)
{
printf("%s,",node->word);
fprintf(file,"%s\n",node->word);
struct	fileno	*f=(struct	fileno*)malloc(sizeof(struct	fileno));
f=node->f_ptr;
while(f!=NULL)
{
printf("\n%s,%d",f->url,f->countf);
fprintf(file,"%s,",f->url);
fprintf(file,"%d\n",f->countf);
f=f->down;
}
node=node->next;
}
}
}
}

char*	getword(char	*str,int	a,int	b)
{
char	*word=(char*)malloc(100);
int	s=0;
for(int	i=a;i<b;i++)
if(str[i]!=' ')
{
word[s]=str[i];
s++;
}
return	word;
}

int	word_count(char	*word,int	index,char	**words,int	size)
{
int	count=1;

for(int	i=index+1;i<size;i++)
{
if(word=="" ||  words[i]=="")
continue;
else if(strcmp(word,words[i])==0)
{
count++;
words[i]="";
}
}
return	count;
}

int	generate_key(char	*word)
{
int	a=word[0];
if(word=="")
return	-1;
return	a;
}

int	check_hash(int	key,struct	hashtable	*hash)
{
if(hash[key].ptr==NULL)
return	1;
else
return	0;
}

void insert_in_hash(char **words,int words_index,struct Node **head,struct hashtable *hash, int page,char *url1)
{
  //printf("%s\n",words[0]);
  int count=0,key=0;
  for(int i=0;i<words_index;i++)
  {

    count=word_count(words[i],i,words,words_index);
  // printf("%d\n",count);
    key=generate_key(words[i]);
    if(key<0)
    continue;
   // printf("%s ",words[i]);
    int toappend=0;
    int toinsert=0;
    int todo=check_hash(key,hash);
    if(todo==1)
     toappend=1;
    else
     toinsert=1;
     if(toappend==1&&words[i]!="")
     {
       struct Node *node;
       node=(struct Node *)malloc(sizeof(struct Node));
       struct Node *last=*head;
       struct fileno *file;
       file=(struct fileno *)malloc(sizeof(struct fileno));
       file->countf=count;
       file->down=NULL;
       file->up=NULL;
       strcpy(file->url,url1);

       strcpy(node->word,words[i]);
       node->next=NULL;
       while(last->next!=NULL)
       last=last->next;
       last->next=node;
       node->prev=last;
       node->f_ptr=file;
       hash[key].ptr=node;
       hash[key].count=1;
	//free(node);
	///free(file);
     }
     else if(toinsert &&words[i]!="")
     {
       struct Node *last=*head;
       struct fileno *file;
       file=(struct fileno *)malloc(sizeof(struct fileno));
       struct Node *addr=(struct Node*)malloc(sizeof(struct Node));
       addr=hash[key].ptr;
       int totrace=hash[key].count;
       while(totrace!=1)
       {
         //sprintf("%s %s\n",addr->word,words[i]);
        	 if(strcmp(addr->word,words[i])==0)
         	{
          // printf("*********************************************\n");

          struct fileno *f;
          f=addr->f_ptr;
          while(f->down!=NULL)
          f=f->down;
	
          file->countf=count;
          strcpy(file->url,url1);
          file->down=f->down;
          file->up=f;
          f->down=file;
          toinsert=0;
          break;
        	}
         addr=addr->next;
         totrace--;
      }

       if(toinsert)
       {
         struct Node *node;
         node=(struct Node *)malloc(sizeof(struct Node));
         if(addr->next!=NULL)
         {
           node->next=addr->next;
           node->prev=addr;
           addr->next=node;
           strcpy(node->word,words[i]);
           (hash[key].count)++;
         }
         else
         {
           strcpy(node->word,words[i]);
           node->next=NULL;
            addr->next=node;
            node->prev=addr;
          }
        strcpy(file->url,url1);
        file->countf=count;
        file->down=NULL;
        file->up=NULL;
        node->f_ptr=file;
	//free(node);
      }
	
	//free(file);
    }
  }
}

void	findwords(char	*str,char	**words,int	*s)
{
int	flag=0,a=-1,b=-1,size=0,i;
char	**temp;
temp=(char**)malloc(1000);
for(i=0;i<strlen(str);i++)
{
if(str[i]==' ')
{
if(flag==1)
{
b=i;
}
flag=0;
}
else
{
if(flag==0)
{
a=i;
}
flag=1;
}
if(a!=-1	&&	b!=-1)
{
words[*s+size]=getword(str,a,b);
size++;
a=-1;
b=-1;
}
}
if(a!=-1)
{
words[*s+size]=getword(str,a,i);
size++;
}
*s=*s+size;
}


char*	getfilepath(int	i)
{
char	ptr[1000]={0};
char	num[20];
strcat(ptr,"/home/khyati/Desktop/sefiles/");
sprintf(num,"%d",i);
strcat(ptr,num);
strcat(ptr,".txt");
char	*ptr1=ptr;
return	ptr1;
}

int	getnextindex(int	start,char	*tag,int	i,char	*data)
{
char	*file=getfilepath(i);
//printf("%s\n",file);
int	file_size=getfilesize(i);
int	taglength=strlen(tag);
for(int	i=start;i<file_size;i++)
{
int	j=0;
if(tag[j]=='>')
{
while(data[i]!=tag[j]	&&	i<file_size)
	i++;
return	i;
}
else	if(data[i]=='<')
{	
int	count=0;
	while(data[i]==' ')
		count++;
	int	flag=0;
	i+=count;
	while(j<taglength)
	{	
		i++;
		if(data[i]!=tag[j])
			flag=1;
		j++;
	}
	i-=count;
	if(flag==0)
	return	i-j;
}
}
return	-1;
}

int	getfilesize(int	i)
{
int	file_size;
struct	stat	st;
char	*ptr=getfilepath(i);
stat(ptr,&st);
file_size=st.st_size;
return	file_size;
}

char* getfiledata(int	i)
{
int	file_size=getfilesize(i);
char	*data=(char *)malloc((file_size+1)*sizeof(char));
char	ch;
FILE	*temp;
char	*ptr=getfilepath(i);
temp=fopen(ptr,"r");
while(!feof(temp))
{
ch=fgetc(temp);
data[i]=ch;
i++;
}
data[i]='\0';
return	data;
}	

char*	gettitle(int	i)
{
char	*filedata=getfiledata(i);
int	m=getnextindex(0,"title",i,filedata);
m+=6;
int	n=getnextindex(m,"/title",i,filedata);
char	title[1000];
int	size=0;
for(int	j=m+1;j<n;j++,size++)
title[size]=filedata[j];
title[size]='\0';
char	*result=title;
return	result;
}

char*	geturl(int	i)
{
char	*filedata=getfiledata(i);
int	m=getnextindex(0,"l",i,filedata);
m+=2;
int	n=getnextindex(n,"/l",i,filedata);
char	url[1000];
int	size=0;
for(int	j=m+1;j<n;j++,size++)
url[size]=filedata[j];
url[size]='\0';
char	*result=url;
return	result;
}

char*	getmeta(int	i,int	*start)
{
char	*filedata=getfiledata(i);
int	begin=0,end=0;
begin=getnextindex(*start,"meta",i,filedata);
if(begin!=-1)
end=getnextindex(begin+1,">",i,filedata);
else	
return	NULL;
*start=end;
char	meta[1000];
int	size=0;
char	content[]="content";
for(int	j=begin+5;j<end;j++)
{
if(filedata[j]=='c'	&&	filedata[j+1]=='o'	&&	filedata[j+2]=='n'	&&	filedata[j+3]=='t'	&&	filedata[j+4]=='e'	&&	filedata[j+5]=='n'	&&	filedata[j+6]=='t')
{
j+=9;
while(filedata[j]!='\"')
{
meta[size]=filedata[j];
j++;
size++;
}
}
}
meta[size]='\0';
char	*result=meta;
return	result;
}

char*	getheader(int	i,int	*start,int	num)
{
char	ch[3]={0};
sprintf(ch,"%d",num);
char	tag[20]={0},tagend[20]={0};
strcat(tagend,"/");
strcat(tag,"h");
strcat(tag,ch);
strcat(tagend,tag);

char	*filedata=getfiledata(i);
int	begin=0,end=0;
begin=getnextindex(*start,tag,i,filedata);
if(begin!=-1)
end=getnextindex(begin+1,tagend,i,filedata);
else	
return	NULL;

*start=end;
char	meta[1000];
int	size=0;
int	j=begin;
for(j;j<end;j++)
{
if(filedata[j]=='<')
{
while(filedata[j]!='>')
j++;
}
else
{
meta[size]=filedata[j];
size++;
}
}
meta[size]='\0';

char	*result=meta;
return	result;
}

int main(int argc,char	*argv[])
{
if(argc!=2)
printf("Insufficient arguments");

else
{
 struct hashtable *hash=(struct hashtable *)malloc(256*sizeof(struct hashtable));
  struct Node *head=(struct Node*)malloc(sizeof(struct Node));

//=(struct	Node*)malloc(sizeof(struct	Node));

char **words=(char **)malloc(5000*sizeof(char **));
  for(int i=0;i<5000;i++)
  {
  words[i]=(char *)malloc(50*sizeof(char));
  }
int	i=1;
while(i<=10)
{
//title
char	*title=gettitle(i);
 for(int j=0;j<5000;j++)
  {
  words[j]=(char *)malloc(50*sizeof(char));
  }
int	size=0;
findwords(title,words,&size);


//meta
int	res=0;
while(1)
{
char	*meta=getmeta(i,&res);
if(meta)
{
findwords(meta,words,&size);
}
else
break;
}

//heading
for(int	j=1;j<=6;j++)
{
res=0;
while(1)
{
char	*result=getheader(i,&res,j);
if(result){
findwords(result,words,&size);
}
else
break;
}
}

char	*url=geturl(i);
printf("%d\n",size);
    insert_in_hash(words,size,&head,hash,i,url);
printf("*\n");
i++;
}

movetofile(hash);
char	*search=argv[1];
struct	Node	*s=(struct	Node*)malloc(sizeof(struct	Node));
int	key=generate_key(search);
s=hash[key].ptr;
while(s->next!=NULL)
{
if(strcmp(search,s->word)==0)
{
struct	fileno	*f=s->f_ptr;
while(f!=NULL){
printf("%s %d\n",f->url,f->countf);
f=f->down;}
}
s=s->next;
}
free(words);
}
return	0;
}
