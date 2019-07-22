#include<iostream>
#include<stdlib.h>
#include<stdio.h>
#include<cstring>
#include <sys/types.h>
#include <sys/stat.h>
#include<fstream>
//#include <html.h>
#define HASH_SIZE 100

#define Url_Length 1000

//#define SEED_URL "http://www.chitkara.edu.in"

#define MAX_URL_PER_PAGE 100

#define INTERVAL_TIME 10

using namespace std;
char base_url[]="www.chitkara.edu.in";
char *str1,*str2,*str3;
int depth=2;
int updatedepth=1;
int file_no=1;
char *links[100];
int current_depth=1;
class list
{
public:
	int dept,visit,key;
	list *next,*prev;
	char url[500];
	list()
	{
		url[0]='\0';
		next=prev=NULL;
		visit=0;
	}
};


class hash1
{
public:
	list *node,*last_node;
	int total_nodes;
	hash1()
	{
		node=NULL;
		last_node=NULL;
		total_nodes=0;
	}
};

hash1 hash_map[100];

list *list_head=new list();
list *list_prev;
list *current_node;

void NormalizeWord(char* word);
int NormalizeURL(char* URL) ;
void removeWhiteSpace(char* html);
void validateUrl(char *);
void workOnNextUrl();


int GetNextURL(char* html, char* urlofthispage, char* result, int pos)
{
  char c;
  int len, i, j;
  char* p1;  //!< pointer pointed to the start of a new-founded URL.
  char* p2;  //!< pointer pointed to the end of a new-founded URL.

  // NEW
  // Clean up \n chars
  	if(pos == 0) {
    	removeWhiteSpace(html);
  	}
  // /NEW

  // Find the <a> <A> HTML tag.
  	while (0 != (c = html[pos]))
  	{
    	if ((c=='<') &&
        ((html[pos+1] == 'a') || (html[pos+1] == 'A'))) {
      	break;
    	}
    	pos++;
  	}
  //! Find the URL it the HTML tag. They usually look like <a href="www.abc.com">
  //! We try to find the quote mark in order to find the URL inside the quote mark.
  if (c)
  {
    // check for equals first... some HTML tags don't have quotes...or use single quotes instead
    p1 = strchr(&(html[pos+1]), '=');

    if ((!p1) || (*(p1-1) == 'e') || ((p1 - html - pos) > 10))
    {
      // keep going...
      return GetNextURL(html,urlofthispage,result,pos+1);
    }
    if (*(p1+1) == '\"' || *(p1+1) == '\'')
      p1++;

    p1++;

    p2 = strpbrk(p1, "\'\">");
    if (!p2)
    {
      // keep going...
      return GetNextURL(html,urlofthispage,result,pos+1);
    }
    if (*p1 == '#')
    { // Why bother returning anything here....recursively keep going...

      return GetNextURL(html,urlofthispage,result,pos+1);
    }
    if (!strncmp(p1, "mailto:",7))
      return GetNextURL(html, urlofthispage, result, pos+1);
    if (!strncmp(p1, "http", 4) || !strncmp(p1, "HTTP", 4))
    {
      //! Nice! The URL we found is in absolute path.
      strncpy(result, p1, (p2-p1));
      return  (int)(p2 - html + 1);
    } else {
      //! We find a URL. HTML is a terrible standard. So there are many ways to present a URL.
      if (p1[0] == '.') {
        //! Some URLs are like <a href="../../../a.txt"> I cannot handle this.
	// again...probably good to recursively keep going..
	// NEW

        return GetNextURL(html,urlofthispage,result,pos+1);
	// /NEW
      }
      if (p1[0] == '/') {
        //! this means the URL is the absolute path
        for (i = 7; i < strlen(urlofthispage); i++)
          if (urlofthispage[i] == '/')
            break;
        strcpy(result, urlofthispage);
        result[i] = 0;
        strncat(result, p1, (p2 - p1));
        return (int)(p2 - html + 1);
      } else {
        //! the URL is a absolute path.
        len = strlen(urlofthispage);
        for (i = (len - 1); i >= 0; i--)
          if (urlofthispage[i] == '/')
            break;
        for (j = (len - 1); j >= 0; j--)
          if (urlofthispage[j] == '.')
              break;
        if (i == (len -1)) {
          //! urlofthis page is like http://www.abc.com/
            strcpy(result, urlofthispage);
            result[i + 1] = 0;
            strncat(result, p1, p2 - p1);
            return (int)(p2 - html + 1);
        }
        if ((i <= 6)||(i > j)) {
          //! urlofthis page is like http://www.abc.com/~xyz
          //! or http://www.abc.com
          strcpy(result, urlofthispage);
          result[len] = '/';
          strncat(result, p1, p2 - p1);
          return (int)(p2 - html + 1);
        }
        strcpy(result, urlofthispage);
        result[i + 1] = 0;
        strncat(result, p1, p2 - p1);
        return (int)(p2 - html + 1);
      }
    }
  }
  return -1;
}


void NormalizeWord(char* word) {
  int i = 0;
  while (word[i]) {
      // NEW
    if (word[i] < 91 && word[i] > 64) // Bounded below so this funct. can run on all urls
      // /NEW
      word[i] += 32;
    i++;
  }
}


int NormalizeURL(char* URL)
{
  int len = strlen(URL);
  if (len <= 1 )
    return 0;
  //! Normalize all URLs.
  if (URL[len - 1] == '/')
  {
    URL[len - 1] = 0;
    len--;
  }
  int i, j;
  len = strlen(URL);
  //! Safe check.
  if (len < 2)
    return 0;
  //! Locate the URL's suffix.
  for (i = len - 1; i >= 0; i--)
    if (URL[i] =='.')
      break;
  for (j = len - 1; j >= 0; j--)
    if (URL[j] =='/')
      break;
  //! We ignore other file types.
  //! So if a URL link is to a file that are not in the file type of the following
  //! one of four, then we will discard this URL, and it will not be in the URL list.
  if ((j >= 7) && (i > j) && ((i + 2) < len))
  {
    if ((!strncmp((URL + i), ".htm", 4))
        ||(!strncmp((URL + i), ".HTM", 4))
        ||(!strncmp((URL + i), ".php", 4))
        ||(!strncmp((URL + i), ".jsp", 4))
        )
    {
      len = len; // do nothing.
    }
    else
    {
      return 0; // bad type
    }
  }
  return 1;
}


void make_backup()
{
	ofstream fout;
	fout.open("/home/khyati/Desktop/crawler_backup/backup.txt",ofstream::trunc);
	list *p=list_head->next;
	while(p!=NULL)
	{
		fout<<p->url<<" ";
		fout<<p->key<<" ";
		fout<<p->dept<<" ";
		fout<<p->visit<<" "<<endl;
		p=p->next;
	}
	fout.close();
}

void read_from_backup()
{
	ifstream fin;
	fin.open("/home/khyati/Desktop/crawler_backup/backup.txt");
	while(!fin.eof())
	{
		char s[500],k[5],val[5],d[5];
		list *n;
		n=new list();
		fin>>s;
		strcpy(n->url,s);
		fin>>k;
		n->key=atoi(k);
		fin>>d;
		n->dept=atoi(d);
		fin>>val;
		n->visit=atoi(val);
		//n->dept=1;
		n->prev=list_prev;
		list_prev->next=n;
		list_prev=n;
		if(hash_map[n->key].node==NULL)
		{
			hash_map[n->key].node=n;
			hash_map[n->key].last_node=n;
			hash_map[n->key].total_nodes++;

		}
		else
		{
			hash_map[n->key].last_node=n;
			hash_map[n->key].total_nodes++;
		}
		//cout<<s<<endl;
	}
	list *x=list_head;
	while(x!=NULL)
	{
		cout<<x->url<<" "<<x->dept<<" "<<x->visit<<" "<<x->key<<endl;
		x=x->next;
	}
	fin.close();
	//current_node=list_head;
	//workOnNextUrl();

}

void removeWhiteSpace(char* html)
{
  int i;
  char *buffer =(char*) malloc(strlen(html)+1), *p=(char*)malloc (sizeof(char)+1);
  memset(buffer,0,strlen(html)+1);
  for (i=0;html[i];i++)
  {
    if(html[i]>32)
    {
      sprintf(p,"%c",html[i]);
      strcat(buffer,p);
    }
  }
  strcpy(html,buffer);
  free(buffer); free(p);
}


int validateDepth(list *ptr)
{
	if(ptr->dept<depth)
	return 1;
	return 0;
}


void delay()
{
	int i,j;
	for(i=0;i<1000;i++)
	{
		for(j=0;j<2000;j++)
		{

		}
	}
}

void workOnNextUrl()
{
		list *p=new list();
		while(current_depth<=depth)
		{
			p=list_head->next;
			while(p!=NULL)
			{
				if(current_depth==p->dept&&p->visit==0)
				{
					current_node=p;
					current_node->visit=1;
					validateUrl(current_node->url);
				}
				p=p->next;
				delay();
			}
			current_depth++;
		}
		p=list_head;
		while(p!=NULL)
		{
			list *x;
			x=p;
			p=p->next;
			free(x);
		}
}


int getIndex(char *str)
{
	int i,s=0;
	for(i=0;str[i]!='\0';i++)
	{
		s=s+str[i];
	}
	s=s%100;
	return s;
}




void makeLinkList(int index)
{
	updatedepth=current_node->dept+1;
	int value,i=0,j,flag=0,k;
	list *ptr;
	for(i=0;i<index;i++)
	{
		flag=0;
		value=getIndex(links[i]);
		list *x=hash_map[value].node;
		while(x!=NULL)
		{
			if(strcmp(x->url,links[i])==0)
			{
				flag=1;
				break;
			}
			x=x->next;
		}
		if(flag==0)
		{
			if(hash_map[value].node==NULL)
			{
				list *n=new list();
				strcpy(n->url,links[i]);
				list_prev->next=n;
				n->prev=list_prev;
				list_prev=n;
				n->dept=updatedepth;
				n->key=value;
				hash_map[value].node=n;
				hash_map[value].last_node=n;
				hash_map[value].total_nodes++;
			}
			else
			{
				list *n=new list();
				strcpy(n->url,links[i]);
				list *temp;
				temp=hash_map[value].last_node;
				n->next=temp->next;
				n->prev=temp;
				n->dept=updatedepth;
				n->key=value;
				temp->next=n;
				hash_map[value].last_node=n;
				hash_map[value].total_nodes++;

			}
		}
	}
	ptr=list_head;
	while(ptr!=NULL)
	{
		cout<<ptr->url<<"   "<<ptr->visit<<"    "<<ptr->dept<<" @  ";
		cout<<getIndex(ptr->url)<<endl;
		ptr=ptr->next;
	}
	for(i=0;i<index;i++)
	free(links[i]);
}


void makeList(char *html_buffer , int fsize)
{
	int index=0,i,pos=0,j=0,flag=0;

	//cin>>result;
//	cout<<html_buffer;
	for(i=0;i<100&&pos!=-1;i++)
	{
		char *result;
		result=(char*)malloc(500);
		pos=GetNextURL(html_buffer,base_url,result,pos);

		//cout<<html_buffer<<endl<<"7";
	//	cout<<result<<"               "<<strlen(result)<<endl;
		if(NormalizeURL(result)&&strlen(result)<80)
		{
			for(j=0;j<index;j++)
			{
				if(strcmp(links[j],result)==0)
				{
				//	cout  << links[j] <<"7"<<endl <<result<<"8" << endl;
					flag=1;
					break;
				}
			}
			if(flag==0)
		{
			links[index]=new char[500];
			strcpy(links[index],result);
			//cout<<links[index]<<endl;
			index++;
		}
		}
		flag=0;
		free(result);
	}
	makeLinkList(index);
	//updatedepth++;
	//if(updatedepth<=depth)
	//{
		//cout<<"yesss";
	//workOnNextUrl();
	//}
}


void getFileName(char *str)
{
	str[0]='\0';
	char temp[100]="/home/khyati/Desktop/sefiles/";
	char fno[100];
	sprintf(fno,"%d",file_no);
	strcat(temp,fno);
	strcat(temp,".txt");
	file_no++;
	//cout<<endl<<endl<<endl<<temp<<endl<<endl<<endl;
	strcat(str,temp);

	//return temp;
}


int findFileSize()
{
	struct stat st; //variable which will count length of file.
	stat("/home/khyati/Desktop/se/temp.txt",&st); // temp.txt is the file where wget fetch the html
	int file_size=st.st_size;

	return file_size;
}


void copyFile()
{
		char *str;
		str=new char[500];
		getFileName(str);
	//cout<<endl<<endl<<endl<<"copyfile"<<endl<<endl<<endl;
	//	cout<<endl<<endl<<endl<<str<<endl<<endl<<endl;
		char x[1000];
	  ifstream fin;
	  ofstream fout;
	  fin.open("/home/khyati/Desktop/se/temp.txt");
	  fout.open(str);
		fout<<current_node->url<<" "<<depth<<endl;
		int fsize=findFileSize();
		char *html_buffer;
		html_buffer=(char *)malloc(fsize+1);
		fin>>html_buffer;
		while(!fin.eof())
		{
		fin>>x;
		strcat(html_buffer,x);
		}
		fout<<html_buffer;
	  fout.close();
	  fin.close();
		html_buffer[fsize]='\0';
		fsize++;
		free(str);
		makeList(html_buffer,fsize);
		free(html_buffer);

}


void getPage(char *url)
{
	char urlbuffer[Url_Length+300]={0};
	strcat(urlbuffer, "wget -O ");
	strcat(urlbuffer,"/home/khyati/Desktop/se/temp.txt ");
	strcat(urlbuffer, url);
	//strcat(urlbuffer, " --proxy-user=user1 --proxy-password=user1");
	system(urlbuffer);
	//cout<<endl<<endl<<endl<<"getPage"<<endl<<endl<<endl;
	copyFile();
}


void testDir(char *dir)
{
  struct stat statbuf;
  if ( stat(dir, &statbuf) == -1 )
  {
    fprintf(stderr, "-----------------\n");
    fprintf(stderr, "Invalid directory\n");
    fprintf(stderr, "-----------------\n");
    exit(1);
  }

  //Both check if there's a directory and if it's writable
  if ( !S_ISDIR(statbuf.st_mode) )
  {
    fprintf(stderr, "-----------------------------------------------------\n");
    fprintf(stderr, "Invalid directory entry. Your input isn't a directory\n");
    fprintf(stderr, "-----------------------------------------------------\n");
    exit(1);
  }

  if ( (statbuf.st_mode & S_IWUSR) != S_IWUSR )
  {
    fprintf(stderr, "------------------------------------------\n");
    fprintf(stderr, "Invalid directory entry. It isn't writable\n");
    fprintf(stderr, "------------------------------------------\n");
    exit(1);
  }
}


void checkDepth()
{
  if(str2[0]>='1'&&str2[0]<='5'&&str2[1]=='\0')
  {
    depth=str2[0]-48;
  }
  else
  {
    printf("Invalid depth");
  }
}


void validateUrl(char *strn)
{
	char str[strlen(strn)+20]="wget --spider ";
	strcat(str,strn);
    if(!system(str))
    {
    printf("Valid URL");
    getPage(strn);
		//if(current_node->dept<=depth)
		workOnNextUrl();
    }
  else
	{

    printf("Invalid URL");
		workOnNextUrl();
	}
}


void checkUrl()
{
	int i,flag=0;
	for(i=0;base_url[i]!='\0';i++)
	{
	  if(base_url[i]!=str1[i])
	  {
	    flag=1;
	      printf("Invalid url");
	    break;
	  }
	}
	if(flag==0)
	{
	  validateUrl(str1);
	}
}


void checkCreteria(int n)
{
	  if(n==4)
	  {
	checkDepth();
	checkUrl();
	testDir(str3);
	  }
	  else
	  {
	    printf("Invalid input");
	  }
}

int main(int argc,char *argv[])
{
  str1=argv[1];
  str2=argv[2];
  str3=argv[3];
	strcpy(list_head->url,str1);
	list_prev=list_head;
	list_head->dept=updatedepth;
	list_head->visit=1;
	current_node=list_head;
	int x=getIndex(list_head->url);
	checkCreteria(argc);
return 0;
}
