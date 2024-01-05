#include <stdio.h> // for printf, scanf
#include <pthread.h> // for pthread functions
#include <time.h> // for clock function
#include <unistd.h> // for fork function
#include <sys/types.h> // for pid_t type
#include <sys/wait.h> // for wait function
#include <string.h> // for strcmp function
#include <sys/times.h>  // for tms struct




// Structure to store data for each thread or process
struct arg {
    int id; // ID of the thread or process
    int numberOfSlaves; // Total number of threads or processes
    unsigned long MaxPrime; // Maximum number to check for primality
};


int Number; // Number of threads or processes to use
unsigned long MaxThr; // Maximum number to check for primality
char choice[10]; // Choice of using threads or processes
unsigned long primes = 0; // count of prime numbers found

// Function to calculate the number of prime numbers within the specified range
void *calculate_primes(void *arg) {
    // Cast the argument to a pointer to a arg structure
    struct arg *data = (struct arg *)arg;

    // Extract the data from the structure
    int slave_id = data->id;
    int num_slaves = data->numberOfSlaves;
    unsigned long MaxPrime = data->MaxPrime;

    // Initialize variables
    unsigned long num;
    int i, flag;

    // Set the starting number based on the thread or process ID
    num = 3 + 2 * slave_id;

    // Initialize the count of prime numbers to 0
    unsigned long count = 0;

    // Iterate through the range of numbers to check for primality
    while (num <= MaxPrime) {
        flag = 0;
        // Check for divisibility by all integers from 2 to the square root of the number
        for (i = 2; i <= num / 2; i++) {
            if (num % i == 0) {
                // If the number is divisible, set the flag and break out of the loop
                flag = 1;
                break;
            }
        }
        // If the number is not divisible and is greater than 1, it is a prime number
        if (flag == 0 && (num > 1)) {
            ++count;
        }
        // Increment the number by twice the number of threads or processes, to skip over numbers that have already been checked
        num += 2 * num_slaves;
    }

    // Return the number of prime numbers found as a void pointer
    return (void *)count;
}




// main function creating threads/processes
int main() {

    //initializing clock variables for elapsed time and each slave's time
    clock_t start, end;
    double total;
    static double starter;
    static double ender;
    static struct tms timTwo;
    static struct tms timOne;

    //starting the clock count
    start = clock();



    //input the number of slaves to be created, store the input into global variable Number
    printf("Enter the number of slaves: ");
    scanf("%d", &Number);

    //input the maximum prime to be checked, store into global variable MaxThr
    printf("Enter the maximum prime to find: ");
    scanf("%lu", &MaxThr);

    //input choice between threads and processes, store into global variable choice
    printf("Enter 'thread' or 'process': ");
    scanf("%s", &choice);

    //compare input choice to string "process", if 0 proceed with process ,
    if ((strcmp(choice, "process")) == 0) {

        //creating a variable ids to store child process id's
        pid_t ids[Number];

        //initializing a variable count to store number of primes
        unsigned long count;

        //create input number of children processes
        for (int i = 0; i < Number; i++) {

            //starting the slave's clock
            starter = times(&timOne);

            //store child id into ids
            ids[i] = fork();

            //check if child's id is equal to 0, proceed
            if (ids[i] == 0) {

                //create an instance of arg to use as arguments, input data into struct
                struct arg data;
                data.id = i;
                data.numberOfSlaves = Number;
                data.MaxPrime = MaxThr;

                //call the function calculate_primes and store its result as an unsigned long into variable count
                count = (unsigned long)calculate_primes(&data);


                //stop the slave's clock, calculate total
                ender = times(&timTwo);
                total = ((double)(ender - starter) / CLOCKS_PER_SEC)*1000;

                //print out the results
                printf("Process %d found %lu prime numbers in %f milliseconds.\n", i, count, total);
                return 0;

            //if the process id is less than 0 indicate an error in the fork() function
            } else if (ids[i] < 0) {
                printf("Fork was unsuccessful");
            }
        }

        //call wait() function for wach process to ensure success
        for (int i = 0; i < Number; i++) {
            wait(NULL);
        }
}
//compare input choice to "thread", if 0 proceed with threads
else if ((strcmp(choice, "thread")) == 0) {


        //initialize threads array to store thread identifiers
        pthread_t threads[Number];

        //create an instance of arg to use as arguments
        struct arg myData[Number];

        //input data into struct, use it as arguments in pthread_create
        for (int i = 0; i < Number; i++) {
            myData[i].id = i;
            myData[i].numberOfSlaves = Number;
            myData[i].MaxPrime = MaxThr;

            //start the slave's clock
            starter = times(&timOne);

            //create threads, call calculate_primes using a pointer to data struct
            pthread_create(&threads[i], NULL, (void *)calculate_primes, &myData[i]);
        }

        //use pthread_join to join the output of function calculate_prime with void pointer
        for (int i = 0; i < Number; i++) {
            void *result;
            pthread_join(threads[i], &result);
            //store result as unsigned long
            unsigned long count = (unsigned long)result;
            //stop the slave's clock, calculate total time 
             ender = times(&timTwo);
             total = ((double)(ender - starter) / CLOCKS_PER_SEC)*1000;
            //print results
            printf("thread %d found %lu prime numbers in %f milliseconds.\n", i, count, total);

        }
 
    }
    //if choice input is neither "thread" or " process" indicate an error
    else {
        printf("Error: not found\n");
    }


    //stop the clock, calculate time, print out total
    end = clock();
    total = ((double) (end - start) / CLOCKS_PER_SEC) * 1000;
    printf("\nTotal elapsed time was %f miliseconds\n",total  );

    return 0;
}