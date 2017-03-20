#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <errno.h>
#include "semaphoreHelper.h"

#define MEMORY_SIZE__ 4096

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

int main(int argc, char *argv[])
{	void *memoryShared = NULL;
	struct dataShare *data_share__;
	char tab[20];

  	int *buf;
	
	int sharedMemoryId =shmget((key_t)45821,MEMORY_SIZE__,0666|IPC_CREAT);
	isError( sharedMemoryId );
	
	printf("Segment pamieci %d\n", sharedMemoryId);
	sprintf(tab,"ipcs -m -i %d",sharedMemoryId);
	
	system(tab);
	memoryShared = shmat(sharedMemoryId,NULL,0);
  
	if(memoryShared==(void *)-1){
		fprintf(stderr,"Funkcja shmat nie powiodla sie. Kod bledu: %d.\n", errno);
		exit(1);
	}
  	
	system(tab);
	
	//-------------------------------------------------
  	data_share__=(struct dataShare *)memoryShared;
  	data_share__->record=1;
  	printf("Segment podlczono pod adresem %p\n",(void *)data_share__);
  	
	FILE *plik;
	plik = fopen("source.txt", "r");
	isNullFile( plik );

	waitForProducer( data_share__ );
	isRun( data_share__, sharedMemoryId, plik );


	if (fclose(plik) != 0)
		printf("Nie moge zamknac pliku!\n");
	//------------------------------------------------
  	disconnectMemoryShared(data_share__, &sharedMemoryId);

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

	if( semctl( idSemaphore, 0, SETVAL, 1 ) == -1 )
	{
		perror( "Blad inicjalizacji" );
	}
	if( semctl( idSemaphore, 1, SETVAL, 0 ) == -1 )
	{
		perror( "Blad inicjalizacji" );
	}
	if( semctl( idSemaphore, 2, SETVAL, 0 ) == -1 )
	{
		perror( "Blad inicjalizacji" );
	}

	int counter = 0;
	int program_dziala=1;
	char znak;
	while(program_dziala && znak != EOF){
		exitSemaphoreNrSem( idSemaphore, 0);
		while(data_share__->record==1){
			counter++;
			if(counter>=10){
				printf("Utracono polacznie z konsumentem(Timeout >10sec)!\n");
				disconnectMemoryShared(data_share__, &sharedMemoryId);
				deleteMemoryShared(data_share__, &sharedMemoryId);
				exit(-2);
			}
			sleep(1);
		}
		counter = 0;
		znak = getc(plik);
		if (znak != '\n' && znak != EOF){
			data_share__->sign = znak;
			data_share__->record=1;
			printf("Przekazano znak: %c\n", znak);
		}
		else if(znak == '\n'){
			data_share__->sign = znak;
			data_share__->record=1;
			printf("Znak konca linii zostal przekazany.\n");
		}
		else if(znak == EOF){
			data_share__->sign = znak;
			data_share__->record=1;
			printf("Znak konca pliku zostal przekazany.\n");
			visitSemaphoreNrSem(idSemaphore, 1);
			break;
		}
		sleep(1);
		visitSemaphoreNrSem(idSemaphore, 1);
	}

	exitSemaphoreNrSem( idSemaphore, 2 );
	if( semctl( idSemaphore, 3, IPC_RMID ) == -1 )
	{	perror( "Nie mozna usunac zbioru semaforow\n" );
	}
}

static void
waitForProducer( struct dataShare *data_share__ )
{	while(data_share__->record==0)
	{	printf("Czekam na podlacznie producenta...\n");
		sleep(1);
	}
	return;
}
