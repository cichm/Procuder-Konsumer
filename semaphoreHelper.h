#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <math.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

/*
*/
static void
checkArgcCounter(int argc, int numberOfArgs);

/*
*/
static void 
checkIsOk( int firstNumber, int secondNumber );

/*
*/
static int
returnPositive(int number);

/*
*/
static key_t
createKey( void );

/*
*/
static void 
isError( int check );

/*
*/
static void
createProcess( int processNumber, char* childProgram, char* argv3 );

/*
*/
static void
processWait( int processNumber );

/*
*/
static void
removeSemaphore( int semaphoreNumber );

/*
*/
static void
visitSemaphore( int semaphoreNumber );

/*
*/
static void
exitSemaphore( int semaphoreNumber );

/*
*/
static void
checkErrno( int number );

/*
*/
static void
isNullFile( FILE* number );

/*
*/
static void
memorySharedError( void *memoryShared );

/*
 * === === === === === === === ===
 * ... ... ... ... ... ... ... ...
 * === === === === === === === ===
*/
static void checkArgcCounter(int argc, int numberOfArgs )
{	if(argc != numberOfArgs)
    {	perror( "Bledny format argumentow" );
        exit( 1 );
    }
    return;
}

static void 
checkIsOk( int firstNumber, int secondNumber )
{	if(!firstNumber && !secondNumber)
    {	perror( "Bledny format argumentow" );
        exit( 1 );
    }
    return;
}

static int
returnPositive( int number )
{	return fabs( number );
}

static key_t
createKey( void )
{	key_t key = ( key_t )0;
	if( ! (key = ftok( ".",'F' ) ) ) 
	{	perror( "IPC error: ftok" );
		exit( 1 );
	}
	return key;
}

static void 
isError( int check )
{	if ( check==-1 )
	{	perror( "Nie utworzono prawidlowo obiektu, \n\tlub operacja na nim nie jest mozliwa" );
		exit(1);
	}
	else 
	{	printf( "Utworzono prawidlowo obiekt, lub operacja na nim jest mozliwa");
	}
	return;
}

static void
createProcess( int processNumber, char* childProgram, char* argv3 )
{	int counter = 1;
	for( ; counter <= processNumber; counter++)
	{	switch(fork()) 
		{
			case -1: 
			{	perror( "Blad tworzenia procesu potomnego" );
				exit( 1 );
			}
			case 0: 
			{	if(execl(childProgram, childProgram, argv3, NULL)==-1)
				{
			 		perror( "Blad exec dla potomnego programu glownego" );
					exit( 1 );
				}
			}
			default:
			{}
		}
	}
	return;
}

static void
processWait( int processNumber )
{	int keyReturn;
	int childPid;
	int counter = 1;
	for(; counter<=processNumber; counter++)
	{
		childPid = wait(&keyReturn);
		if(childPid == -1) {
			perror( "Blad funkcji wait\n" );
			exit( 1 );
		}
		printf("Proces potomny %d zakonczyl sie z kodem powrotu %d\n",childPid, keyReturn);
	}
	return;
}

static void
removeSemaphore( int semaphoreNumber )
{	if (semctl(semaphoreNumber,0,IPC_RMID)==-1)
	{	perror("Nie mozna usunac semafora");
		exit( 1 );
	}
	else
	{	printf("Semafor zostal usuniety, id: %d\n", semaphoreNumber);
	}
	
	return;
}

static void
checkErrno( int number )
{	if( errno != number ) 
	{	perror( "Nie udalo sie wykonac operacji na semaforze." );
		exit( 1 );
	}
	return;
}

static void
isNullFile( FILE* number )
{	if( number == NULL ) 
	{	perror( "NULL: " );
		exit( 1 );
	}
	return;
}

/*
*/
static void
memorySharedError( void *memoryShared )
{  	if(memoryShared==(void *)-1)
	{    	perror("Funkcja shmat nie powiodla sie. Kod bledu");
      		exit(1);
    }
}

/*
*/
static void
visitSemaphore( int semaphoreNumber )
{	int zmien_sem;
	struct sembuf semaphoreBuffor;
	semaphoreBuffor.sem_num=0;
	semaphoreBuffor.sem_op=1;
	semaphoreBuffor.sem_flg=SEM_UNDO;

	for(;;) 
	{  	zmien_sem=semop(semaphoreNumber, &semaphoreBuffor, 1);
		if (zmien_sem==0 || errno !=4) 
		{	break;
		}
	}

	if( zmien_sem == -1 ) 
	{	checkErrno( semaphoreNumber );
	}
	else {
		printf("Semafor zostal otwarty.\n\n");
	}
}

/*
*/
static void
exitSemaphore( int semaphoreNumber )
{	int zmien_sem;
	struct sembuf semaphoreBuffor;
	semaphoreBuffor.sem_num=0;
	semaphoreBuffor.sem_op=-1;
	semaphoreBuffor.sem_flg=SEM_UNDO;

	for(;;) 
	{	zmien_sem=semop(semaphoreNumber, &semaphoreBuffor, 1);
		if (zmien_sem==0 || errno != 4){
			break;
		}
	}

	if( zmien_sem == -1 ) 
	{	checkErrno( semaphoreNumber );
	}
	else
	{	printf("\nSemafor zostal zamkniety.\n");
	}
}

/*
*/
static void
visitSemaphoreNrSem( int semaphoreNumber, short nrSem )
{	int zmien_sem;
	struct sembuf semaphoreBuffor;
	semaphoreBuffor.sem_num=nrSem;
	semaphoreBuffor.sem_op=1;
	semaphoreBuffor.sem_flg=SEM_UNDO;

	for(;;) 
	{  	zmien_sem=semop(semaphoreNumber, &semaphoreBuffor, 1);
		if (zmien_sem==0 || errno !=4) 
		{	break;
		}
	}

	if( zmien_sem == -1 ) 
	{	checkErrno( semaphoreNumber );
	}
	else {
		printf("Semafor zostal otwarty %d.\n", nrSem);
	}
}

/*
*/
static void
exitSemaphoreNrSem( int semaphoreNumber, short nrSem )
{	int zmien_sem;
	struct sembuf semaphoreBuffor;
	semaphoreBuffor.sem_num=nrSem;
	semaphoreBuffor.sem_op=-1;
	semaphoreBuffor.sem_flg=SEM_UNDO;

	for(;;) 
	{	zmien_sem=semop(semaphoreNumber, &semaphoreBuffor, 1);
		if (zmien_sem==0 || errno != 4){
			break;
		}
	}

	if( zmien_sem == -1 ) 
	{	checkErrno( semaphoreNumber );
	}
	else
	{	printf("\nSemafor zostal zamkniety %d.\n", nrSem);
	}
}

