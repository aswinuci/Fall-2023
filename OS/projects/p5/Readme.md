CPU Usage , memory usage , Disk Io , Kernel caching , Number of device drivers , Interrupts occured 

So many categories
We pick 2 

Memory and CPU

One category is am=lmost given
we add one more
for extra credit one more category


Where do we get status of system , especially runtime in real time ?
    File -> Abstraction to do many things
            Open/Read/ Write / Close
            Regular
            Documents
    File can be controlling block device
    /proc -> Virtual 
    sysconf to get page size  
    ioctl to get some info
    
top command
if something runs , it gets stored into proc folder

eg : run a infinite loop program 

#include<iostream>
using namespace std;

main(){
    while(1){
        // Do nothing
    }

    return 0;
}

Run it and use top command to find its process id
Once found , go to cd /proc/<process-id> and youll get the contents of it

Category 2 : Memory
            Free vs utilized 
            System vs Kernel

Category 3: IO to disk
            Network
                Broowsing web , transfer files , packets transfer and so on
                Easy to check and persistent rather than process
                