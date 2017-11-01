/*
CSCI 340 - Operating Systems
Fall 2017

Homework assignment #4

NOTE: you will get a warning about unused variable, sthreads, but
this will go away, as you complete the code.
*/

#include <stdio.h>
#include <stdlib.h>  // for exit(), rand(), strtol()
#include <pthread.h>
#include <time.h>    // for nanosleep()
#include <errno.h>   // for EINTR error check in millisleep()
#include <signal.h>  // for pthread_kill()

#include "binary_semaphore.h"

// you can adjust next two values to speedup/slowdown the simulation
#define MIN_SLEEP      20   // minimum sleep time in milliseconds
#define MAX_SLEEP     100   // maximum sleep time in milliseconds

#define START_SEED     11   // arbitrary value to seed random number generator

// guard_state with a value of k:
//          k < 0 : means guard is waiting in the room
//          k = 0 : means guard is in the hall of department
//          k > 0 : means guard is IN the room
int guard_state;         // waiting, in the hall, or in the room
int num_students;        // number of students in the room


// will malloc space for seeds[] in the main
unsigned int *seeds;     // rand seeds for guard and students generating delays

// NOTE:  globals below are initialized by command line args and never changed !
int capacity;       // maximum number of students in a room
int num_checks;     // number of checks the guard makes

// *******************************************************************************
// Code to be completed by you. See TODO comments.
// *******************************************************************************

// TODO:  list here the "handful" of semaphores you will need to synchronize
//        I've listed one you will need for sure, to "get you going"
binary_semaphore mutex;  // to protect shared variables (including semaphores)

// this function contains the main synchronization logic for the guard
inline void guard_check_room()
{
  // TODO: complete this routine to execute the behavior of the guard,
  // with proper synchronization.  You will need to determine how many
  // students are in the room to perform the appropriate action.
  // Depending on the number of students, you will either:
  //    * properly wait to enter, if some students are there
  //    * clear out the room, if capacity (or more) number of students
  //    * assess the security of the room, if no students are there
  //       (see function assess_security() above)

  // You will also need to properly maintain the global variable,
  // guard_state.  Once you have performed the appropriate action,
  // with proper synchronization, the guard will leave the room.

  // Remember, that whenever you access or change a global variable
  // (eg. num_students), you need to insure you are doing so in a
  // mutually exclusive fashion, for example, by calling
  // semWait(&mutex).
}

// this function contains the main synchronization logic for a student
inline void student_study_in_room(long id)
{
  // TODO: complete this routine to execute the behavior of a student,
  // with proper synchronization.  You will need to determine if the
  // guard is in the room.  You will also need to synchronize with the
  // guard, to clear out the room, and, to allow a possible waiting
  // guard to enter.  At the proper place, you will call the function
  // study(), above.  You will also need to properly maintain the
  // global variable, num_students.  When done, students leave the
  // room.
}

// *******************************************************************************
// Code that DOES NOT need to be modified
// *******************************************************************************

inline void millisleep(long millisecs)   // delay for "millisecs" milliseconds
{ // details of this function are unimportant for the assignment
  struct timespec req;
  req.tv_sec  = millisecs / 1000;
  millisecs -= req.tv_sec * 1000;
  req.tv_nsec = millisecs * 1000000;
  while(nanosleep(&req, &req) == -1 && errno == EINTR)
    ;
}

// generate random int in range [min, max]
inline int rand_range(unsigned int *seedptr, long min, long max)
{ // details of this function are unimportant for the assignment
  // using reentrante version of rand() function (because multithreaded)
  // NOTE: however, overall behavior of code will still be non-deterministic
  return min + rand_r(seedptr) % (max - min + 1);
}

inline void study(long id)  // student studies for some random time
{ // details of this function are unimportant for the assignment
  int ms = rand_range(&seeds[id], MIN_SLEEP, MAX_SLEEP);
  printf("student %2ld studying in room with %2d students for %3d millisecs\n",
	 id, num_students, ms);
  millisleep(ms);
}

inline void do_something_else(long id)    // student does something else
{ // details of this function are unimportant for the assignment
  int ms = rand_range(&seeds[id], MIN_SLEEP, MAX_SLEEP);
  millisleep(ms);
}

inline void assess_security()  // guard assess room security
{ // details of this function are unimportant for the assignment
  // NOTE:  we have (own) the mutex when we first enter this routine
  guard_state = 1;     // positive means in the room
  int ms = rand_range(&seeds[0], MIN_SLEEP, MAX_SLEEP/2);
  printf("\tguard assessing room security for %3d millisecs...\n", ms);
  millisleep(ms);
  printf("\tguard done assessing room security\n");
}

inline void guard_walk_hallway()  // guard walks the hallway
{ // details of this function are unimportant for the assignment
  int ms = rand_range(&seeds[0], MIN_SLEEP, MAX_SLEEP/2);
  printf("\tguard walking the hallway for %3d millisecs...\n", ms);
  millisleep(ms);
}

// guard thread function  --- NO need to change this function !
void* guard(void* arg)
{
  int i;            // loop control variable
  srand(seeds[0]);  // seed the guard thread random number generator

  // the guard repeatedly checks the room (limited to num_checks) and
  // walks the hallway
  for (i = 0; i < num_checks; i++) {
    guard_check_room();
    guard_walk_hallway();
  }

  pthread_exit((void*)0);   // thread needs to return a void*
}

// student thread function --- NO need to change this function !
void* student(void* arg)
{
  long id = (long) arg;  // determine thread id from arg
  srand(seeds[id]);      // seed this threads random number generator

  // repeatedly study and do something else
  while (1) {
    student_study_in_room(id);
    do_something_else(id);
  }

  pthread_exit((void*)0);   // thread needs to return a void*
}


// *******************************************************************************
// Code to be completed by you. See TODO comments.
// *******************************************************************************

int main(int argc, char** argv)  // the main function
{
  int n = 0;               // number of student threads
  pthread_t  cthread;      // guard thread
  pthread_t* sthreads;     // student threads
  long i;                  // loop control variable

  if (argc < 4) {
    fprintf(stderr, "USAGE: %s num_threads capacity num_checks\n", argv[0]);
    return 0;
  }

  // TODO: get three input parameters, convert, and properly store
  // HINT: use atoi() function (see man page for more details).





  // allocate space for the seeds[] array
  // NOTE: seeds[0] is guard seed, seeds[k] is the seed for student k
  // allocate space for the student threads array, sthreads

  seeds = (unsigned int*) malloc((n + 1)*sizeof(unsigned int));
  sthreads = (pthread_t*) malloc(n*sizeof(pthread_t));

  // Initialize global variables and semaphores
  guard_state = 0;   // not in room (walking the hall)
  num_students = 0;  // number of students in the room

  semInitB(&mutex, 1);  // initialize mutex
  // TODO: for all your binary semaphores, complete the semaphore initializations

  // initialize guard seed and create the guard thread
  seeds[0] = START_SEED;
  pthread_create(&cthread, NULL, guard, (void*) NULL);
  
  for (i = 1; i <= n; i++) {
    seeds[i] = START_SEED + i;
    pthread_create(&sthreads[i-1], NULL, student, (void*) i);
  }

  pthread_join(cthread, NULL);   // wait for guard thread to complete

  for (i = 0; i < n; i++) {
    pthread_kill(sthreads[i],0); // stop all threads (guard and students)
  }

  free(seeds);
  free(sthreads);
  
  return 0;
}
