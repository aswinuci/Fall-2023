I need to create threads
scheduler-execute() will be called
Each thread will run 
whenever yield is called , one thread leaves and whatever thread is waiting in the queue takes over

Yield is called after every printing , so basically we would have
Thread 1
Thread 2
Thread 3
Thread 4 
Thread 5

Each thread will have its corresponding function and argument 
Whenever a thread runs , it will run its function partly and then yield , so whatever state the function of that thread has run till now , we need to save it and keep


How the proces will work
    - We ask a thread to be created passing a function and an argument to it
    - Each thread will be assigned its function , argument , memory and created
    - We will call scheduler execute
        - It will start with first thread
        - Thread runs for a while
        - prints the string and number 
        - Yields control to another thread waiting next
        - Terminate when all threads are completed

- We need to have a function stack to see if each thread's function is completed
- We need to have a thread stack to see if all threads are completed



