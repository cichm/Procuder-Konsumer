#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <errno.h>
#include "semaphoreHelper.h"

#define memorySize 4096

struct dataShare 
{	int record;
	char sign;
};

/*
*/
static void 
disconnectMemoryShared(struct dataShare *data_share__, int *sharedMemoryId);

/*
*/
static void 
deleteMemoryShared(struct dataShare *data_share__, int *sharedMemoryId);

/*
*/
static void
isRun( struct dataShare *data_share__, int sharedMemoryId, FILE* plik );

/*
*/
static void
waitForProducer( struct dataShare *data_share__ );

void 
main(){
  	void *memoryShared = NULL;
  	struct dataShare *data_share__;
  	char charArray[20];
	int sharedMemoryId 	= ( int )0;
  	sharedMemoryId 		= shmget((key_t)45821,memorySize,0666|IPC_CREAT);

  	isError( sharedMemoryId );
  	printf("\nSegment pamieci %d\n", sharedMemoryId);
  	sprintf(charArray,"ipcs -m -i %d",sharedMemoryId);
  	system(charArray);
  
  	memoryShared=shmat(sharedMemoryId, NULL, 0);
  	memorySharedError( memoryShared );
  	system(charArray);
  	//-------------------------------------------------
  	data_share__ = (struct dataShare *)memoryShared;
  
  	data_share__->record=0;
  	printf("Segment podlaczono pod adresem %p\n",(void *)data_share__);

	FILE *plik;
	plik = fopen("output.txt", "w");
	isNullFile( plik );

	waitForProducer( data_share__ );
	isRun( data_share__, sharedMemoryId, plik );
	
	
	if (fclose(plik)==EOF)
		printf("Nie moge zamknac pliku!\n");
	//------------------------------------------------
  	disconnectMemoryShared(data_share__, &sharedMemoryId);
	deleteMemoryShared(data_share__, &sharedMemoryId);

	if (fclose(plik) != 0)
		printf("Nie moge zamknac pliku!\n");  

  	exit(0);
}

static void 
disconnectMemoryShared(struct dataShare *data_share__, int *sharedMemoryId)
{	if(shmdt(data_share__)==-1)
	{	perror( "Funkcja shmdt nie powiodla sie." );
    }
   	else 
   	{	printf("Odlaczono segment pamieci %d spod adresu %p\n", *sharedMemoryId, (void *)data_share__);
   	}
}

static void 
deleteMemoryShared(struct dataShare *data_share__, int *sharedMemoryId){
	if(shmctl(*sharedMemoryId,IPC_RMID,0)==-1)
	{	perror( "Funkcja shmdctl nie powiodla sie. " );
    }
   	else
   	{	printf("Usunieto segment pamieci %d spod adresu %p\n", *sharedMemoryId, (void *)data_share__);
   	}
}

static void
isRun( struct dataShare *data_share__, int sharedMemoryId, FILE* plik )
{	key_t keyT = ( key_t )0;
	keyT = createKey();

	int idSemaphore = ( int )0;
	if( ( idSemaphore = semget( keyT, 3, 0600 | IPC_CREAT ) ) == -1 )
	{	perror( "Semget - blad" );
	}
	else {}

	int isRun = 1;
	int counter = 0;
	for( ; isRun ; )
	{	exitSemaphoreNrSem( idSemaphore, 1);
		while(data_share__->record==0){
			counter++;
			if(counter >= 10){
				printf("Utracono polacznie z producentem(Timeout >10sec)!\n");
				disconnectMemoryShared(data_share__, &sharedMemoryId);
				deleteMemoryShared(data_share__, &sharedMemoryId);
				exit(-1);
			}
			sleep(1);
		}
		counter = 0;
		sleep(1);
		if(data_share__->record==1){
			if(data_share__->sign == 0)
				data_share__->record=0;
			else if (data_share__->sign != '\n' && data_share__->sign != EOF){
				printf("Zapisuje znak %c...\n", data_share__->sign);
				fputc(data_share__->sign, plik);
				data_share__->record=0;
			}
			else if(data_share__->sign == '\n'){
				printf("Zapisywanie znaku konca linii...\n");
				fputc(data_share__->sign, plik);
				data_share__->record=0;
			}
			else if(data_share__->sign == EOF){
				data_share__->record=0;
				printf("Zakonczono przepisywanie tekstu.\n");
				isRun = 0;
				break;
			}
		}
		visitSemaphoreNrSem(idSemaphore, 0);
	}
	visitSemaphoreNrSem(idSemaphore, 2);

	return;
}

static void
waitForProducer( struct dataShare *data_share__ )
{	while(data_share__->record==0)
	{	printf("Czekam na podlacznie producenta...\n");
		sleep(1);
	}
	return;
}
