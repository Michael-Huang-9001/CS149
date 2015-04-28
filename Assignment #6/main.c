#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <time.h>
#include <string.h>

#define PROGRAM_DURATION 5//30
#define SLEEP_DURATION 3
#define NUM_CHILDREN 5
#define READ_END 0
#define WRITE_END 1
#define BUFFER_SIZE 80

/**
* Print a line for each event:
* elapsed time
* message from child process
* user standard input
*/
typedef struct {
    int id;
    int messageCount;
} CHILD;

//global start time variable
double startTime; // the time the forking starts
FILE *fp; // file pointer for opening file

/**
Creates a random wait time
**/
int sleepTime() {
	return rand()%SLEEP_DURATION;
}


/**
Gets the elapsed time
**/
double getElapsedTime() {
	struct timeval now;
    gettimeofday(&now, NULL);
	double currentTime = (now.tv_sec) * 1000 + (now.tv_usec) / 1000;
	return (currentTime - startTime)/1000;
}

/**
Adds the time to the childs event
**/
void addTimeToUser(char *time, char *userEvent) {
   double sec = getElapsedTime();
   double min = 0;

   while (sec >=60){
       min++;
       sec -=60;
   }

   // Elapsed time.
   sprintf(time, "%02.0f:%06.3lf | %c", min, sec);
   strcat(time, userEvent);
}

/**
Adds the time to the childs event
**/
void addTimeToEvent(char *event, CHILD child) {
   double sec = getElapsedTime();
   double min = 0;

   while (sec >=60){
       min++;
       sec -=60;
   }

   // Elapsed time.
   sprintf(event, "%02.0f:%06.3lf | Child %d message %d", min, sec, child.id, child.messageCount);
}

/**
Prints the time and the event for the parent
e.g. 00:00.000 | event
**/
void printEvent(char *event) {
    double sec = getElapsedTime();
    double min = 0;

    while (sec >=60){
        min++;
        sec -=60;
    }

    // Elapsed time.
    printf("%02.0f:%06.3lf | ", min, sec);
	char time[BUFFER_SIZE] = "";
	sprintf(time,"%02.0f:%06.3lf | ", min, sec);
	fputs(time, fp);
	fputs(event,fp);
	fputs("\n",fp);
    //What they are doing
    printf(event);
    printf("\n");
}


/****************************************************************************************************
main
*/
int main(void)
{
    char write_msg[BUFFER_SIZE] = ""; //read to pipe
    char read_msg[BUFFER_SIZE] = ""; //write to pipe
	CHILD children[NUM_CHILDREN]; // array of children
    pid_t pid;  // child process id
    int fd[NUM_CHILDREN][2];  // file descriptors for the pipe
    
	struct timeval start; // time struct to get start time in secs

	int result; // gets the result of select 0 means no file descriptors
	struct timeval timeout; // used for timeout in seconds
	fd_set inputs, inputfds;
	FD_ZERO(&inputs);    // initialize inputs to the empty set
	FD_SET(0, &inputs);  // set file descriptor 0 (stdin)
	
	// start the timer
	gettimeofday(&start, NULL);
	startTime = (start.tv_sec) * 1000 + (start.tv_usec) / 1000;
	
	int child;
	for( child = 0; child<NUM_CHILDREN; child++)
	{
			// Create the pipe.
		if (pipe(fd[child]) == -1) {
			fprintf(stderr,"pipe() failed");
			return 1;
		}
		
		// Fork a child process.
		pid = fork();
		 // error handling
		if (pid == -1)
		{
			perror("select");
			exit(1);
		}
		else if(pid == 0)
		{ 
			//create a child
			CHILD aChild;
			aChild.id = child+1;
			aChild.messageCount = 1;
			children[child] = aChild;
			break;
		}
		
	}
	
	//initialize random
	srand(time(NULL)*child);
	
	if(pid > 0)
	{
		fp = fopen("theta1.txt","w"); // opens the file to write to
	}
	
	while (PROGRAM_DURATION > getElapsedTime())
	{
		
		// CHILD PROCESS
		if (pid == 0)
		{
			//sleep for 0-2 seconds
			sleep(sleepTime());
			
			//LAST CHILD used for standard input
			if (child == 4)
			{
				// Close the unused READ end of the pipe.
				close(fd[child][READ_END]);
				
				//scans input for a user
				printf("Type a short message: ");
				char userInput[BUFFER_SIZE] = "";
				scanf("%s", &userInput);
				
				addTimeToUser(write_msg, userInput);
				
				// Write to the WRITE end of the pipe.
				write(fd[child][WRITE_END], write_msg, BUFFER_SIZE);
			}
			
			//all other children
			//else
			{
				// Close the unused READ end of the pipe.
				close(fd[child][READ_END]);

				// Adds time to the child message
				addTimeToEvent(write_msg, children[child]);

				//increment message count
				children[child].messageCount++;
				
				// Write to the WRITE end of the pipe.
				write(fd[child][WRITE_END], write_msg, BUFFER_SIZE);

			}
		}
		// PARENT PROCESS
		else if (pid > 0) {  
			inputfds = inputs; // clear input file descriptors
			
			// 2.5 second timeout
			timeout.tv_sec = 30;
			timeout.tv_usec = 000000;
			
			result = select(FD_SETSIZE, &inputfds, (fd_set *) 0, (fd_set *) 0, &timeout);
			
			if(result == -1)
			{
				perror("select");
				exit(1);
			}
			else if (result == 0) {		}
			//print out info
			else
			{
				int readChild;
				for (readChild = 0; readChild < NUM_CHILDREN; readChild++)
				{
					// Close the unused WRITE end of the pipe.
					close(fd[readChild][WRITE_END]);

					char event[BUFFER_SIZE] = "";
					sprintf(event, "Parent read: ");
					// Read from the READ end of the pipe.
					read(fd[readChild][READ_END], read_msg, BUFFER_SIZE);
					strcat(event, read_msg);
					printEvent(event);

				}
			}
		}
		else {
			fprintf(stderr, "fork() failed");
			return 1;
		}
	}//for
	
	//close pipes for write of child
	close(fd[child][WRITE_END]);
	
	if(pid > 0)
	{

		int parentPipe;
		for(parentPipe = 0; parentPipe < NUM_CHILDREN; parentPipe++)
		{
			waitpid(-1, NULL, 0); //parent waits for children to finish
			close(fd[parentPipe][READ_END]); //close pipes for read of parent
		}
		fclose(fp); //closes the file
	}

    return 0;
}