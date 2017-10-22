#include "dpsim.h"
#include <signal.h>

static const unsigned int NUM_PHILOSOPHERS = 5;
static const unsigned int NUM_CHOPSTICKS = 5;

static int chopsticks[5];
static pthread_mutex_t mutex[5];
static pthread_t philosophers[5];


void* th_main( void* th_main_args ) {

	// ---------------------------------------
	// TODO: you add your implementation here
	int i = 0;
	int phils[5];
		
	for ( i=0; i<NUM_CHOPSTICKS; i++ ) {
		printf("Chopstick %d set on the table...\n", i);
		chopsticks[i] = -1;
	}
	
	for ( i=0; i<NUM_PHILOSOPHERS; i++ ) {
		printf("Philosopher %d sits down at the table...\n", i);
		phils[i] = i;
		if ( pthread_create(&philosophers[i], NULL, th_phil, (void*) &phils[i]) ) {
			perror("Error creating philosopher thread...\n");
			exit(1);		
		}
	}
	
	int deadlock = 0;
	while ( deadlock != TRUE ) {
		int chops_cpy[5];

		memcpy(chops_cpy, chopsticks, sizeof(chopsticks));

		if ( chops_cpy[0] == 0 && chops_cpy[1] == 1 && chops_cpy[2] == 2 && chops_cpy[3] == 3 && chops_cpy[4] == 4 ) {
			printf("Deadlock condition (0,1,2,3,4) ... terminating\n");			
			deadlock = TRUE;
		} else {
			int count = 0;
			printf("Philosopher(s) ");
			if ( chops_cpy[0] == 0 && chops_cpy[1] == 0 ) {
				printf("%d ", 0);		
				count++;
			}
			if ( chops_cpy[1] == 1 && chops_cpy[2] == 1 ) {
				printf("%d ", 1);
				count++;
			}
			if ( chops_cpy[2] == 2 && chops_cpy[3] == 2 ) {
				printf("%d ", 2);
				count++;			
			}
			if ( chops_cpy[3] == 3 && chops_cpy[4] == 3 ) {
				printf("%d ", 3);
				count++;	
			}
			if ( chops_cpy[4] == 4 && chops_cpy[0] == 4 ) {
				printf("%d ", 4);
				count++;
			}
			
			if ( count == 0 ) {
				printf("are not eating...\n");
			} else if ( count == 1 ) {
				printf("is eating...\n");
			} else {
				printf("are eating...\n");
			}
		}
		
		delay(10000);
	}
	
	for ( i=0; i<NUM_PHILOSOPHERS; i++ ) {
		printf("Philosopher %d sets chopstick %d down and leaves the table...\n", i, i);
		pthread_kill(philosophers[i], 0);	
	}
	
	pthread_exit(0);
} // end th_main function


void* th_phil( void* th_phil_args ) {

	// ---------------------------------------
	// TODO: you add your implementation here
	int id = *((int*) th_phil_args);
	
	while( TRUE ) {
		delay(1000);
		eat(id);	
	}
} // end th_phil function


void delay( long nanosec ) {

	struct timespec t_spec;

	t_spec.tv_sec = 0;
	t_spec.tv_nsec = nanosec;

	nanosleep( &t_spec, NULL );

} // end think function


void eat( int phil_id ) {

	// ---------------------------------------
	// TODO: you add your implementation here
	pthread_mutex_lock(&mutex[phil_id]);
	chopsticks[phil_id] = phil_id;
	
	pthread_mutex_lock(&mutex[(phil_id + 1) % 5]);
	chopsticks[(phil_id + 1) % 5] = phil_id;

	delay(5000);
	
	pthread_mutex_unlock(&mutex[(phil_id + 1) % 5]);
	chopsticks[(phil_id + 1) % 5] = -1;

	pthread_mutex_unlock(&mutex[phil_id]);
	chopsticks[phil_id] = -1;
} // end eat function
