//////////////////////////////////////////////////////
//
//
//
//   Customized Virtual File System Application
//
//
//
/////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////
//
// Predefined Header files
//
//////////////////////////////////////////////////////////////////
#include<stdio.h>
#include<stdlib.h>
#include<string.h> 
#include<io.h> 


//////////////////////////////////////////////////////////////////
//
// User defined Macros
//
//////////////////////////////////////////////////////////////////

#define MAXINODE 50 // Maximum Files To Be Created 50

#define READ 1
#define WRITE 2 //  permission as 3 For Both Read & Write

#define MAXFILESIZE 1024 // Maximum Size Of A File (1024 = 1kb)

#define REGULAR 1 // ie Regular File
#define SPECIAL 2 // for future  ie .c, .py file

#define START 0  // File Offset(lseek)
#define CURRENT 1
#define END 2


//////////////////////////////////////////////////////
//
//
//    SuperBlock Structure
//
//
//
/////////////////////////////////////////////////////

typedef struct superblock
{
	int TotalInodes;  // Initially Size Is 50 For Both
	int FreeInode;
}SUPERBLOCK,*PSUPERBLOCK;

//////////////////////////////////////////////////////
//
//
//
//     Inode Structure
//
//
/////////////////////////////////////////////////////

typedef struct inode  //94 bytes gets allocated for this block
{
	char FileName[50];   // file name stored
	int InodeNumber;
	int FileSize;        // 1024 byte
	int FileActualSize;  // This gets allocated when we write into the file
	int FileType;
	char *Buffer;       
	int LinkCount;
	int ReferenceCount;
	int permission;
	struct inode *next;  //  self referential structure
}INODE,*PINODE,**PPINODE;

//////////////////////////////////////////////////////
//
//
//
//    FileTable Structure
//
//
/////////////////////////////////////////////////////

typedef struct filetable
{
	int readoffset;
	int writeoffset;
	int count;
	int mode;
	PINODE ptrinode;
}FILETABLE,*PFILETABLE;

//////////////////////////////////////////////////////
//
//
//    UFDT Structure
//
//
//
/////////////////////////////////////////////////////

typedef struct ufdt
{
	PFILETABLE ptrfiletable;   // Pointer Which Points To File Table
}UFDT;


UFDT UFDTArr[50];          // Created Array of structure
SUPERBLOCK SUPERBLOCKobj;  // global variable
PINODE head=NULL;          // global pointer 

//////////////////////////////////////////////////////
//
//  Function Name   :  man
//  Input           :  char *    
//  Output          :  Display info
//  Description     :  This function display description of all commands in VFS.
//  Author          :  Bhavana Rachelli 
//  Date            :  19 july 2021
//
/////////////////////////////////////////////////////

void man(char *name)
{
	if(name==NULL)
	{
		return;
	}

	if(_stricmp(name,"create")==0)
	{
		printf("Description : Used to create new regular file\n");
		printf("Usage : create File_name Permission\n");
	}
	else if(_stricmp(name,"read")==0)
	{
		printf("Description : Used to read data from regular file\n"); 
		printf("Usage : read File_name No_Of_Bytes_To_Read\n"); 
	}
	else if(_stricmp(name,"write")==0)
	{
		printf("Description : Used to write into regular file\n");
		printf("Usage : write File_name\n After this enter the data that we want to write\n");
	}
	else if(_stricmp(name,"ls")==0)
	{
		  printf("Description : Used to list all information of files\n"); 
        printf("Usage : ls\n"); 
	}
	else if(_stricmp(name,"stat") == 0) 
    { 
       printf("Description : Used to display information of file\n"); 
       printf("Usage : stat File_name\n"); 
    } 
    else if(_stricmp(name,"fstat") == 0) 
    { 
       printf("Description : Used to display information of file\n"); 
       printf("Usage : stat File_Descriptor\n"); 
    } 
    else if(_stricmp(name,"truncate") == 0) 
    { 
       printf("Description : Used to remove data from file\n"); 
       printf("Usage : truncate File_name\n"); 
    } 
    else if(_stricmp(name,"open") == 0) 
    { 
       printf("Description : Used to open existing file\n"); 
       printf("Usage : open File_name mode\n"); 
    } 
    else if(_stricmp(name,"close") == 0) 
    { 
       printf("Description : Used to close opened file\n"); 
       printf("Usage : close File_name\n");
    } 
    else if(_stricmp(name,"closeall") == 0) 
    { 
       printf("Description : Used to close all opened file\n"); 
       printf("Usage : closeall\n"); 
    } 
    else if(_stricmp(name,"lseek") == 0) 
    { 
       printf("Description : Used to change file offset\n"); 
       printf("Usage : lseek File_Name ChangeInOffset StartPoint\n"); 
    } 
    else if(_stricmp(name,"rm") == 0) 
    { 
       printf("Description : Used to delete the file\n"); 
       printf("Usage : rm File_Name\n"); 
    } 
    else if(_stricmp(name,"backup") == 0) 
    { 
       printf("Description : To take backup in the file\n"); 
       printf("Usage : backup\n"); 
    } 
    else 
    { 
       printf("ERROR : No manual entry available.\n"); 
    }
}

//////////////////////////////////////////////////////
//
//  Function Name   :  DisplayHelp
//  Input           :  -----    
//  Output          :  Display info
//  Description     :  It Display all the commands used in this VFS.
//  Author          :  Bhavana Rachelli 
//  Date            :  19 july 2021
//
/////////////////////////////////////////////////////


void DisplayHelp()
{
	 printf("ls : To List out all files\n"); 
    printf("clear : To clear console\n"); 
    printf("open : To open the file\n"); 
    printf("close : To close the file\n"); 
    printf("closeall : To close all opened files\n"); 
    printf("read : To Read the contents from file\n"); 
    printf("write :To Write contents into file\n"); 
    printf("exit : To Terminate file system\n"); 
    printf("stat : To Display information of file using name\n"); 
    printf("fstat :To Display information of file using file descriptor\n"); 
    printf("truncate : To Remove all data from file\n"); 
    printf("rm : To Delet the file\n");
    printf("backup : To Backup all the file in hardisk");
}

//////////////////////////////////////////////////////
//
//  Function Name   :  GetFDFromName
//  Input           :  char *    
//  Output          :  Integer
//  Description     :  To get file descriptor
//  Author          :  Bhavana Rachelli 
//  Date            :  19 july 2021
//
/////////////////////////////////////////////////////


int GetFDFromName(char *name)
{
	int i=0;

	while(i<50)
	{
		if(UFDTArr[i].ptrfiletable!=NULL)
		{
			if(_stricmp((UFDTArr[i].ptrfiletable->ptrinode->FileName),name)==0)
			{
				break;
			}
		}
		i++;
	}

	if(i==50)
	{
		return -1;
	}
	else
	{
		return i;
	}
} 


//////////////////////////////////////////////////////
//
//  Function Name   :  Get_Inode
//  Input           :  char *    
//  Output          :  PINODE
//  Description     :  Return Inode's address
//  Author          :  Bhavana Rachelli 
//  Date            :  19 july 2021
//
/////////////////////////////////////////////////////


PINODE Get_Inode(char *name)
{
	PINODE temp=head;
	int i=0;

	if(name==NULL)
	{
		return NULL;
	}

	while(temp!=NULL)
	{
		if(strcmp(name,temp->FileName)==0)
		{
			break;
		}
		temp=temp->next;
	}

	return temp;
}


//////////////////////////////////////////////////////
//
//  Function Name   :  CreateDILB
//  Input           :  -----    
//  Output          :  -----
//  Description     :  It Creates The DILB When Program Starts
//  Author          :  Bhavana Rachelli 
//  Date            :  19 july 2021
//
/////////////////////////////////////////////////////

void CreateDILB()
{
	int i=1;
	PINODE newn=NULL;
	PINODE temp=NULL;

	while(i<=MAXINODE)
	{
		newn=(PINODE)malloc(sizeof(INODE));

		newn->LinkCount=newn->ReferenceCount=0;
		newn->FileType=newn->FileSize=0;
		newn->Buffer=NULL;
		newn->InodeNumber=i;

		if(temp==NULL)
		{
			head=newn;
			temp=head;
		}
		else
		{
			temp->next=newn;
			temp=temp->next;
		}
		i++;
	}
	printf("DILB created successfully\n"); 
}


//////////////////////////////////////////////////////
//
//  Function Name   :  InitialiseSuperBlock
//  Input           :  -----    
//  Output          :  -----
//  Description     :  It Initialize super block values
//  Author          :  Bhavana Rachelli 
//  Date            :  19 july 2021
//
/////////////////////////////////////////////////////

void InitialiseSuperBlock() 
{
	int i=0;

	while(i<50)
	{
		UFDTArr[i].ptrfiletable=NULL;
		i++;
	}

	SUPERBLOCKobj.TotalInodes = MAXINODE; 
    SUPERBLOCKobj.FreeInode = MAXINODE; 
}


//////////////////////////////////////////////////////
//
//  Function Name   :  CreateFile
//  Input           :  char *, Integer
//  Output          :  -----
//  Description     :  Create New Files
//  Author          :  Bhavana Rachelli 
//  Date            :  19 july 2021
//
/////////////////////////////////////////////////////

int CreateFile(char *name,int permission)
{
	int i=0;
	PINODE temp=head;

	if((name==NULL)||(permission==0)||(permission>3))
	{
		return -1;
	}

	if(SUPERBLOCKobj.FreeInode==0)
	{
		return -2;
	}

	(SUPERBLOCKobj.FreeInode)--;

	if(Get_Inode(name)!=NULL)
	{
		return -3;
	}

	while(temp!=NULL)
	{
		if(temp->FileType==0)
		{
			break;
		}
		temp=temp->next;
	}

	while(i<50)
	{
		if(UFDTArr[i].ptrfiletable==NULL)
		{
			break;
		}
		i++;
	}

	UFDTArr[i].ptrfiletable=(PFILETABLE)malloc(sizeof(FILETABLE));

	if(UFDTArr[i].ptrfiletable==NULL)
	{
		return -4;
	}

	// For FileTable
	UFDTArr[i].ptrfiletable->count=1;
	UFDTArr[i].ptrfiletable->mode=permission;
	UFDTArr[i].ptrfiletable->readoffset=0;
	UFDTArr[i].ptrfiletable->writeoffset=0;

	// For Inode
	UFDTArr[i].ptrfiletable->ptrinode=temp;
	strcpy(UFDTArr[i].ptrfiletable->ptrinode->FileName,name);
	UFDTArr[i].ptrfiletable->ptrinode->FileType=REGULAR;
	UFDTArr[i].ptrfiletable->ptrinode->ReferenceCount=1;
	UFDTArr[i].ptrfiletable->ptrinode->LinkCount=1;
	UFDTArr[i].ptrfiletable->ptrinode->FileSize=MAXFILESIZE;
	UFDTArr[i].ptrfiletable->ptrinode->FileActualSize=0;
	UFDTArr[i].ptrfiletable->ptrinode->permission=permission;
	UFDTArr[i].ptrfiletable->ptrinode->Buffer=(char *)malloc(MAXFILESIZE);
	memset(UFDTArr[i].ptrfiletable->ptrinode->Buffer,0,1024);

	return i;
} 


//////////////////////////////////////////////////////
//
//  Function Name   :  rm_File
//  Input           :  char *
//  Output          :  -----
//  Description     :  Delete Created Files
//  Author          :  Bhavana Rachelli 
//  Date            :  19 july 2021
//
/////////////////////////////////////////////////////

int rm_File(char * name) 
{
	int fd=0;

	fd=GetFDFromName(name);

	if(fd==-1)
	{
		return -1;
	}

	(UFDTArr[fd].ptrfiletable->ptrinode->LinkCount)--;

	if(UFDTArr[fd].ptrfiletable->ptrinode->LinkCount==0)
	{
		UFDTArr[fd].ptrfiletable->ptrinode->FileType=0;
		free(UFDTArr[fd].ptrfiletable);
	}

	UFDTArr[fd].ptrfiletable=NULL;
	(SUPERBLOCKobj.FreeInode)++;

	printf("File Succesfully Deleted\n");
}


//////////////////////////////////////////////////////
//
//  Function Name   :  ReadFile
//  Input           :  Integer, char *, Integer
//  Output          :  Integer
//  Description     :  Read Data From File
//  Author          :  Bhavana Rachelli 
//  Date            :  19 july 2021
//
/////////////////////////////////////////////////////

int ReadFile(int fd, char *arr, int isize) 
{
	int read_size=0;

	if(UFDTArr[fd].ptrfiletable==NULL)
	{
		return -1;
	}

	if(UFDTArr[fd].ptrfiletable->mode!=READ && UFDTArr[fd].ptrfiletable->mode!=READ+WRITE)
	{
		return -2;
	}

	if(UFDTArr[fd].ptrfiletable->ptrinode->permission!=READ && UFDTArr[fd].ptrfiletable->ptrinode->permission!=READ+WRITE)
	{
		return -2;
	}

	if(UFDTArr[fd].ptrfiletable->readoffset==UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)
	{
		return -3;
	}

	if(UFDTArr[fd].ptrfiletable->ptrinode->FileType!=REGULAR)
	{
		return -4;
	}

	if(UFDTArr[fd].ptrfiletable->ptrinode->ReferenceCount==0)
	{
		return -5;
	}

	read_size=(UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)-(UFDTArr[fd].ptrfiletable->readoffset);

	if(read_size < isize)
	{
		strncpy(arr,(UFDTArr[fd].ptrfiletable->ptrinode->Buffer)+(UFDTArr[fd].ptrfiletable->readoffset),read_size);

		UFDTArr[fd].ptrfiletable->readoffset=UFDTArr[fd].ptrfiletable->readoffset+read_size;
	}
	else
	{
		strncpy(arr,(UFDTArr[fd].ptrfiletable->ptrinode->Buffer)+(UFDTArr[fd].ptrfiletable->readoffset),isize);
       
        (UFDTArr[fd].ptrfiletable->readoffset) = (UFDTArr[fd].ptrfiletable->readoffset) + isize;
	}

	return isize;
}


//////////////////////////////////////////////////////
//
//  Function Name   :  WriteFile
//  Input           :  Integer, char *, Integer
//  Output          :  Integer
//  Description     :  Write Data Into The File
//  Author          :  Bhavana Rachelli 
//  Date            :  19 july 2021
//
/////////////////////////////////////////////////////

int WriteFile(int fd, char *arr, int isize)
{
	if(((UFDTArr[fd].ptrfiletable->mode)!=WRITE)&&((UFDTArr[fd].ptrfiletable->mode)!=READ+WRITE))
	{
		return -1;
	}

	if(((UFDTArr[fd].ptrfiletable->ptrinode->permission) !=WRITE) && ((UFDTArr[fd].ptrfiletable->ptrinode->permission) != READ+WRITE)) 
	{
		return -1;
	}

	if((UFDTArr[fd].ptrfiletable->writeoffset) == MAXFILESIZE) 
    { 
        return -2; 
    }

    if((UFDTArr[fd].ptrfiletable->ptrinode->FileType) != REGULAR) 
    { 
        return -3; 
    }

    
    strncpy((UFDTArr[fd].ptrfiletable->ptrinode->Buffer)+(UFDTArr[fd].ptrfiletable->writeoffset),arr,isize);

    (UFDTArr[fd].ptrfiletable->writeoffset) = (UFDTArr[fd].ptrfiletable->writeoffset)+ isize; 

    (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)=(UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)+isize;
    
    
    return isize;

}


//////////////////////////////////////////////////////
//
//  Function Name   :  OpenFile
//  Input           :  char *, Integer
//  Output          :  Integer
//  Description     :  Open An Existing File
//  Author          :  Bhavana Rachelli 
//  Date            :  19 july 2021
//
/////////////////////////////////////////////////////

int OpenFile(char *name, int mode)
{
	int i=0;
	PINODE temp=NULL;


	if(name==NULL||mode<=0)
	{
		return -1;
	}

	temp=Get_Inode(name);

	if(temp==NULL)
	{
		return -2;
	}

	if(temp->permission<mode)
	{
		return -3;
	}

	if(temp->LinkCount==0)
	{
		return -2;
	}

	while(i<50)
	{
		if(UFDTArr[i].ptrfiletable==NULL)
		{
			break;
		}
		i++;
	}

	UFDTArr[i].ptrfiletable = (PFILETABLE)malloc(sizeof(FILETABLE));

	if(UFDTArr[i].ptrfiletable==NULL)
	{
		return -1;
	}
   
    UFDTArr[i].ptrfiletable->count = 1; 
    UFDTArr[i].ptrfiletable->mode = mode;

    if(mode==READ+WRITE)
    {
    	UFDTArr[i].ptrfiletable->readoffset=0;
    	UFDTArr[i].ptrfiletable->writeoffset=0;
    }
    else if(mode==READ)
    {
    	UFDTArr[i].ptrfiletable->readoffset=0;
    }
    else if(mode==READ)
    {
    	UFDTArr[i].ptrfiletable->writeoffset=0;
    }
     UFDTArr[i].ptrfiletable->ptrinode = temp;

     (UFDTArr[i].ptrfiletable->ptrinode->ReferenceCount)++; 
     
     return i;
}




//////////////////////////////////////////////////////
//
//  Function Name   :  CloseFileByName
//  Input           :  char *
//  Output          :  Integer
//  Description     :  Close Existing File By Its Name
//  Author          :  Bhavana Rachelli 
//  Date            :  19 july 2021
//
/////////////////////////////////////////////////////

int CloseFileByName(char *name) 
{ 
 int i = 0; 
 i = GetFDFromName(name);

   if(i == -1) 
   { 
      return -1; 
   } 

   if((UFDTArr[i].ptrfiletable->ptrinode->ReferenceCount) == 0)
   {
   	return -2;
   }
 
    UFDTArr[i].ptrfiletable->readoffset = 0; 
    UFDTArr[i].ptrfiletable->writeoffset = 0; 
    (UFDTArr[i].ptrfiletable->ptrinode->ReferenceCount)--; 
    printf("File Closed Succesfully\n");

  return 0; 
}


//////////////////////////////////////////////////////
//
//  Function Name   :  CloseAllFile
//  Input           :  ------
//  Output          :  ------
//  Description     :  Close All Existing Files
//  Author          :  Bhavana Rachelli 
//  Date            :  19 july 2021
//
/////////////////////////////////////////////////////

void CloseAllFile() 
{ 
    int i = 0; 
    
    while(i<50) 
    { 
        if(UFDTArr[i].ptrfiletable != NULL) 
        { 
           UFDTArr[i].ptrfiletable->readoffset = 0; 
           UFDTArr[i].ptrfiletable->writeoffset = 0; 
           (UFDTArr[i].ptrfiletable->ptrinode->ReferenceCount) = 0; 
           
       } 
       i++; 
    } 
}


//////////////////////////////////////////////////////
//
//  Function Name   :  LseekFile
//  Input           :  Integer, Integer, Integer
//  Output          :  Integer
//  Description     :  Modify the or resets the write and readoffsets to particular point is start current or end
//  Author          :  Bhavana Rachelli 
//  Date            :  19 july 2021
//
/////////////////////////////////////////////////////

int LseekFile(int fd, int size, int from) 
{
	if((fd<0)||(from>2))
	{
		return -1;
	}

	if(UFDTArr[fd].ptrfiletable->ptrinode->ReferenceCount == 0)
	{
		return -2;
	}

	if(UFDTArr[fd].ptrfiletable==NULL)
	{
		return -1;
	}

	if((UFDTArr[fd].ptrfiletable->mode == READ) || (UFDTArr[fd].ptrfiletable->mode == READ+WRITE)) 
	{
		if(from==CURRENT)
		{
			if(((UFDTArr[fd].ptrfiletable->readoffset)+size)>UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)
			{
				return -1;
			}
			if(((UFDTArr[fd].ptrfiletable->readoffset) + size) < 0) 
            {
            	return -1;
            }

            (UFDTArr[fd].ptrfiletable->readoffset)=(UFDTArr[fd].ptrfiletable->readoffset)+size;
		}
		else if(from==START)
		{
			if(size > (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)) 
			{
				return -1;
			}
			if(size<0)
			{
				return -1;
			}

			(UFDTArr[fd].ptrfiletable->readoffset) = size;
		}
		else if(from==END)
		{
			if((UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) + size > MAXFILESIZE)
			{
				return -1;
			}
			if(((UFDTArr[fd].ptrfiletable->readoffset) + size) < 0)
			{
				return -1;
			}

			(UFDTArr[fd].ptrfiletable->readoffset)=(UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)+size;
		}
	}
	else if(UFDTArr[fd].ptrfiletable->mode == WRITE)
	{
		if(from==CURRENT)
		{
			if(((UFDTArr[fd].ptrfiletable->writeoffset)+size)>MAXFILESIZE)
			{
				return -1;
			}
			if(((UFDTArr[fd].ptrfiletable->writeoffset)+size)<0)
			{
				return -1;
			}
			if(((UFDTArr[fd].ptrfiletable->writeoffset)+size)>(UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize))
			{
				(UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) = (UFDTArr[fd].ptrfiletable->writeoffset) + size; 
				(UFDTArr[fd].ptrfiletable->writeoffset)=(UFDTArr[fd].ptrfiletable->writeoffset)+size;
			}
		}
		else if(from==START)
		{
			if(size>MAXFILESIZE)
			{
				return -1;
			}
			if(size<0)
			{
				return -1;
			}
			if(size > (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)) 
			{
				(UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)=size;
				(UFDTArr[fd].ptrfiletable->writeoffset)=size;
			}

		}
		else if(from==END)
		{
			if((UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)+size>MAXFILESIZE)
			{
				return -1;
			} 
			if(((UFDTArr[fd].ptrfiletable->writeoffset)+size)<0)
			{
				return -1;
			}

			(UFDTArr[fd].ptrfiletable->writeoffset)=(UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)+size;
		}
	}
   printf("Successfully Changed\n");
}


//////////////////////////////////////////////////////
//
//  Function Name   :  ls_file
//  Input           :  ------
//  Output          :  ------
//  Description     :  List Out All Existing Files Name
//  Author          :  Bhavana Rachelli
//  Date            :  19 july 2021
//
/////////////////////////////////////////////////////


void ls_file() 
{
	int i=0;
	PINODE temp=head;

	if(SUPERBLOCKobj.FreeInode==MAXINODE)
	{
		printf("ERROR : There are no files\n");
		return;
	}

	printf("\nFile Name\tInode number\tFile size\tLink count\n"); 
   printf("---------------------------------------------------------------\n"); 
 
   while(temp != NULL) 
   { 
      if(temp->FileType != 0) 
      { 
          printf("%s\t\t%d\t\t%d\t\t%d\n",temp->FileName,temp->InodeNumber,temp->FileActualSize,temp->LinkCount); 
      } 
      temp = temp->next; 
   } 
   printf("---------------------------------------------------------------\n"); 

}


//////////////////////////////////////////////////////
//
//  Function Name   :  fstat_file
//  Input           :  Integer
//  Output          :  Integer
//  Description     :  Display Statistical Information Of The File By Using File Descriptor
//  Author          :  Bhavana Rachelli
//  Date            :  19 july 2021
//
/////////////////////////////////////////////////////


int fstat_file(int fd) 
{ 
   PINODE temp = head; 
   int i = 0; 

   if(fd < 0) 
   { 
       return -1; 
   } 
 
   if(UFDTArr[fd].ptrfiletable == NULL) 
   { 
       return -2; 
   } 

   temp = UFDTArr[fd].ptrfiletable->ptrinode; 

   printf("\n---------Statistical Information about file----------\n"); 
   printf("File name : %s\n",temp->FileName); 
   printf("Inode Number %d\n",temp->InodeNumber); 
   printf("File size : %d\n",temp->FileSize); 
   printf("Actual File size : %d\n",temp->FileActualSize); 
   printf("Link count : %d\n",temp->LinkCount); 
   printf("Reference count : %d\n",temp->ReferenceCount); 
 
 if(temp->permission == 1) 
 {
 	printf("File Permission : Read only\n");
 }
  
 else if(temp->permission == 2)
 {
   printf("File Permission : Write\n"); 
 }
 
 else if(temp->permission == 3) 
 {
   printf("File Permission : Read & Write\n");
 }
  
 printf("------------------------------------------------------\n\n"); 

return 0; 
}


//////////////////////////////////////////////////////
//
//  Function Name   :  stat_file
//  Input           :  char *
//  Output          :  Integer
//  Description     :  Display Statistical Information Of The File By Using File Name.
//  Author          :  Bhavana Rachelli
//  Date            :  19 july 2021
//
/////////////////////////////////////////////////////


int stat_file(char *name) 
{ 
     PINODE temp = head; 
     int i = 0; 

    if(name == NULL) 
    { 
       return -1; 
    } 
 
    while(temp!= NULL) 
    { 
       if(strcmp(name,temp->FileName) == 0) 
       {
       	 break;
       }
      temp = temp->next; 
    } 

    if(temp == NULL) 
    {
	    return -2;
    } 

    printf("\n---------Statistical Information about file----------\n"); 
    printf("File name : %s\n",temp->FileName); 
    printf("Inode Number %d\n",temp->InodeNumber); 
    printf("File size : %d\n",temp->FileSize); 
    printf("Actual File size : %d\n",temp->FileActualSize); 
    printf("Link count : %d\n",temp->LinkCount); 
    printf("Reference count : %d\n",temp->ReferenceCount); 

    if(temp->permission == 1) 
    {
       printf("File Permission : Read only\n"); 
    }
    else if(temp->permission == 2)
    { 
       printf("File Permission : Write\n");
    } 
    else if(temp->permission == 3)
    { 
       printf("File Permission : Read & Write\n"); 
    }
    printf("------------------------------------------------------\n\n"); 

 return 0; 
} 


//////////////////////////////////////////////////////
//
//  Function Name   :  truncate_File
//  Input           :  char *
//  Output          :  Integer
//  Description     :  Delete All Data From The File
//  Author          :  Bhavana Rachelli
//  Date            :  19 july 2021
//
/////////////////////////////////////////////////////


int truncate_File(char *name) 
{ 
    int fd = GetFDFromName(name); 

    if(fd == -1) 
    { 
       return -1; 
    } 
 memset(UFDTArr[fd].ptrfiletable->ptrinode->Buffer,0,1024);

 UFDTArr[fd].ptrfiletable->readoffset = 0; 
 UFDTArr[fd].ptrfiletable->writeoffset = 0; 
 UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize = 0; 
 printf("Data Succesfully Removed\n");
}


//////////////////////////////////////////////////////
//
//  Function Name   :  backup
//  Input           :  ------
//  Output          :  ------
//  Description     :  It backup all the file created file
//  Author          :  Bhavana Rachrlli
//  Date            :  19 july 2021
//
/////////////////////////////////////////////////////


void backup()
{
	PINODE temp = head;
	int fd=0;

	while(temp!=NULL)
	{
		if(temp->FileType!=0)
		{
			fd=creat(temp->FileName,0777);

			write(fd,temp->Buffer,temp->FileActualSize);
		}
		temp=temp->next;
	}
}	


//////////////////////////////////////////////////////////
//
//  Function Name		: 	main
//  Description 		: 	Entry Point Function
//  Author              :  Bhavana Rachelli
//  Date                :  19 july 2021
//
/////////////////////////////////////////////////////


int main()
{
	char *ptr=NULL;
	int   ret=0,fd=0,count=0;
	char command[4][80],str[80],arr[1024];
   
   InitialiseSuperBlock(); 
   CreateDILB();

   while(1)
   {
   	fflush(stdin);  // clean the input buffer
   	strcpy(str,""); // clean the string array
   	printf("\nCUSTOMIZED VFS : >> ");
   	fgets(str,80,stdin);  // or scanf("%[^'\n']s",str);

   	count=sscanf(str,"%s %s %s %s",command[0],command[1],command[2],command[3]);

   	if(count==1)
   	{
   		if(_stricmp(command[0],"ls")==0)
   		{
   			ls_file();
   		}
   		else if(_stricmp(command[0],"closeall")==0)
   		{
   			CloseAllFile();
   			printf("All files closed successfully\n");
   			continue;
   		}
   		else if(_stricmp(command[0],"clear")==0)
   		{
   			system("cls");
   			continue;
   		}
   		else if(_stricmp(command[0],"help")==0)
   		{
   			DisplayHelp();
   			continue;
   		}
         else if(_stricmp(command[0],"backup")==0)
   		{
   			backup();
   			continue;
   		}
   		else if(_stricmp(command[0],"exit")==0)
   		{
   			printf("Terminating the Marvellous Virtual File System\n"); 
   			break;    			
   		}
   		else
   		{
   			printf("\nERROR : Command not found !!!\n");
   			
   		}
   		continue;
   	}
   	else if(count==2)
   	{
   		if(_stricmp(command[0],"stat") == 0)
   		{
   			ret = stat_file(command[1]); 

   			if(ret==-1)
   			{
   				printf("ERROR : Incorrect parameters\n"); 
   			}

   			if(ret==-2)
   			{
   				printf("ERROR : There is no such file\n"); 
   			}
   			continue;
   		}
   		else if(_stricmp(command[0],"fstat") == 0)
   			{
   				
   			ret = fstat_file(atoi(command[1])); 

   			if(ret==-1)
   			{
   				printf("ERROR : Incorrect parameters\n"); 
   			}

   			if(ret==-2)
   			{
   				printf("ERROR : There is no such file\n"); 
   			}
   			continue;
   		}
   		else if(_stricmp(command[0],"close") == 0)
   		{
   			ret=CloseFileByName(command[1]);

   			if(ret==-1)
   			{
   				printf("ERROR : There is no such file\n"); 
   				
   			}

   			if(ret==-2)
   			{
   				printf("The File is already closed\n");
   			}
   			continue;
   		}
   		else if(_stricmp(command[0],"rm") == 0)
   		{
   			ret=rm_File(command[1]);

   			if(ret==-1)
   			{
   				printf("ERROR : There is no such file\n");	
   			}
   			continue;
   		}
   		else if(_stricmp(command[0],"man") == 0)
   		{
   			man(command[1]);
   		}
   		else if(_stricmp(command[0],"write") == 0)
   		{
   			fd=GetFDFromName(command[1]);

   			if(fd==-1)
   			{
   				printf("Error : Incorrect parameter\n"); 
   				continue; 
   			}

   			if(UFDTArr[fd].ptrfiletable->ptrinode->ReferenceCount == 0)
   			{
   				printf("ERROR : File is Not opened\n");
   			}
   			else
   			{
   				printf("Enter the data : \n");
   			   scanf("%[^\n]",arr);

   			   ret=strlen(arr);

   			   if(ret==0)
   			   {
   				     printf("Error : Incorrect parameter\n"); 
   				     continue;
   			   }

   			      ret=WriteFile(fd,arr,ret);

   			   if(ret==-1)
   			   {
   				    printf("ERROR : Permission denied\n");
   			   }

   			   if(ret==-2)
   			   {
   				   printf("ERROR : There is no sufficient memory to write\n");
   			   }

   			   if(ret==-3)
   			   {
   				   printf("ERROR : It is not regular file\n");
   			   }

   			   if(ret > 0)
				   {
					   printf("Sucessfully : %d bytes written\n", ret);
				   }

   			}  			
   		}
   		else if(_stricmp(command[0],"truncate") == 0)
   		{
   			ret = truncate_File(command[1]); 

   			if(ret==-1)
   			{
   				printf("Error : Incorrect parameter\n");
   			}
   		}
   		else
   		{
   			printf("\nERROR : Command not found !!!\n");  
   		}
   		continue;
   	}
   	else if(count==3)
   	{
   		if(_stricmp(command[0],"create") == 0) 
   		{
   			ret = CreateFile(command[1],atoi(command[2]));  //atoi=Asci to integer

   			if(ret >= 0) 
   			{
   				printf("File is successfully created with file descriptor : %d\n",ret); 
   			}

   			if(ret==-1)
   			{
   				printf("ERROR : Incorrect parameters\n"); 
   			}

   			if(ret==-2)
   			{
   				printf("ERROR : There is no inodes\n"); 
   			}

   			if(ret==-3)
   			{
   				printf("ERROR : File already exists\n"); 
   			}

   			if(ret==-4)
   			{
   				printf("ERROR : Memory allocation failure\n"); 
   			}
   			continue;
   		}
   		else if(_stricmp(command[0],"open") == 0) 
   		{
   			ret = OpenFile(command[1],atoi(command[2]));

   			if(ret>=0)
   			{
   				printf("File is successfully opened with file descriptor : %d\n",ret);
   			}

   			if(ret==-1)
   			{
   				printf("ERROR : Incorrect parameters\n"); 
   			}

   			if(ret==-2)
   			{
   				printf("ERROR : File not present\n"); 
   			}

   			if(ret==-3)
   			{
   				printf("ERROR : Permission denied\n");
   			}

   			continue;
   		}
   		else if(_stricmp(command[0],"read") == 0) 
   		{
   			fd = GetFDFromName(command[1]);

   			if(fd==-1)
   			{
   				printf("Error : Incorrect parameter\n"); 
   				continue;
   			}

   			ptr = (char *)malloc(sizeof(atoi(command[2]))+1); 

   			if(ptr==NULL)
   			{
   				printf("Error : Memory allocation failure\n"); 
   				continue;
   			}

   			ret = ReadFile(fd,ptr,atoi(command[2])); 

   			if(ret==-1)
   			{
   				printf("ERROR : File not existing\n");
   			}

   			if(ret==-2)
   			{
   				printf("ERROR : Permission denied\n"); 
   			}

   			if(ret==-3)
   			{
   				printf("ERROR : Reached at end of file\n"); 
   			}

   			if(ret==-4)
   			{
   				printf("ERROR : It is not regular file\n"); 
   			}

   			if(ret==-5)
   			{
   				printf("ERROR : File is not opened\n");
   			}

   			if(ret==0)
   			{
   				printf("ERROR : File empty\n");  
   			}

   			if(ret>0)
   			{
   				_write(2,ptr,ret);  
   			}
   			continue;
   		}
   		else
   		{
   			printf("\nERROR : Command no t found !!!\n"); 
   			continue; 
   		}
   	}
   	else if(count==4)
   	{
   		if(_stricmp(command[0],"lseek") == 0) 
   		{
   			fd = GetFDFromName(command[1]);

   			if(fd==-1)
   			{
   				printf("Error : Incorrect parameter\n"); 
   				continue; 
   			}

   			ret = LseekFile(fd,atoi(command[2]),atoi(command[3])); 

   			if(ret==-1)
   			{
   				printf("ERROR : Unable to perform lseek\n"); 
   			}

   			if(ret == -2)
   			{
   				printf("ERROR : File is Not opened\n");
   			}
   		}
   		else
   		{
   			printf("\nERROR : Command not found !!!\n"); 
   			continue; 
   		}
   	}
   	else
   	{
   		printf("\nERROR : Command not found !!!\n"); 
   		continue;
   	}
   } 
   return 0;

}