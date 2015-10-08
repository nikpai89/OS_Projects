#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include<assert.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/time.h>

#define NUMRECS 24

typedef struct __rec_t {
    unsigned int key;
    unsigned int record[NUMRECS];
} rec_t;

long partition(rec_t* recArray,long startIndex,long endIndex)
{
  long i,j;
  rec_t temp;
  i = startIndex + 1;
  j = startIndex + 1;
  while (j <= endIndex)
  {
     if ((recArray+j)->key <= (recArray+startIndex)->key)
     {
       temp =  *(recArray+i);
       *(recArray+i) = *(recArray+j);
       *(recArray+j) = temp;  
       i++;
     }
     j++;
  }
  i--;
  temp =  *(recArray+i);
  *(recArray+i) = *(recArray+startIndex);
  *(recArray+startIndex) = temp;  
  return i;
}

//Quicksort implemented on struct __rec_t
void mySort(rec_t* recArray,long startIndex,long endIndex)
{
   if(startIndex < endIndex)
   {
     long midIndex = partition(recArray,startIndex,endIndex);
     mySort(recArray,startIndex,midIndex - 1);
     mySort(recArray,midIndex + 1,endIndex);
   }
} 

void usage(char *programName)
{
   fprintf(stderr,"usage: %s -i <Input File> -o <Output File> ",programName);
   exit(1);
}


int main(int argc,char** argv)
{
  int tempIndex;
  char* inFile;
  char* outFile;
  long retVal;
  rec_t* recArray = NULL;

  struct timeval startTime;
  struct timeval endTime;
  struct timeval execTime;

  while ((tempIndex = getopt(argc, argv, "i:o:")) != -1)
  { 
     switch(tempIndex)
     {
      
        case 'i' : inFile = strdup(optarg);
                   break;

        case 'o' : outFile = strdup(optarg);
                   break;

        default : usage(argv[0]); 
                  break; 
     }
  }

  gettimeofday(&startTime,NULL);
  int inputFd = open(inFile,O_RDONLY);
  if (inputFd < 0)
  {
     fprintf(stderr,"Error in opening input file");
     exit(1);
  } 
  
    
  int outputFd = open(outFile,O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);
  if (outputFd < 0)
  {
     fprintf(stderr,"Error in creating output file");
     exit(1);
  } 

  struct stat outFileStat;
  retVal = fstat(inputFd,&outFileStat);
  if (retVal != 0)
  {
     fprintf(stderr,"Error in checking file parameters");
     exit(1);
  } 

  long fileSize = outFileStat.st_size;
  long elementCount = fileSize / sizeof(rec_t); 
  recArray = malloc(fileSize);
  if (recArray == 0)
  {
     fprintf(stderr,"Error in checking file parameters");
     exit(1);
  }
  
  retVal = read(inputFd,(void *)recArray,fileSize);  
  assert(retVal != 0);
   
  mySort(recArray,0,elementCount-1);
     
  
  retVal = write(outputFd,(void *)recArray,fileSize);  
  assert(retVal != 0);
 
  retVal = close(outputFd);
  assert(retVal == 0);


  retVal = close(inputFd);
  assert(retVal == 0);
  gettimeofday(&endTime,NULL);

  timersub(&endTime,&startTime,&execTime);
  printf("Time: %ld.%06ld \n",execTime.tv_sec,execTime.tv_usec);
  
  free(recArray);
  free(inFile);
  free(outFile);

  fflush(stdout);
  fflush(stderr);

  close(STDIN_FILENO);
  close(STDOUT_FILENO);
  close(STDERR_FILENO);
  return 0;
}
