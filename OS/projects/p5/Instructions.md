# How to Run

1. Open two terminals , one for project P5 , and one to perform other commands
2. Setup the disk by performing below commands for disk

CPU 
    - stress --cpu 4
    - stress --vm-bytes 10G --vm-keep -m 1

Network
    - Internet speed test

Disk
    - sudo losetup -a 
    - sudo dd if=/dev/zero of=myloopfile bs=1M count=100 && sudo chmod 0777 myloopfile
    - sudo losetup /dev/loop20 myloopfile
    - sudo mkfs.ext4 /dev/loop20
    - sudo mkdir /mnt/loop20
    - sudo mount /dev/loop20 /mnt/loop20
 ---------------------------------------------------------------------------------
    - sudo cp -r /etc/* /mnt/loop20
    - sudo mkdir /home/aswin/Desktop/somedir
    - sudo cp -r /mnt/loop20/* /home/aswin/Desktop/somedir
    - sudo umount /mnt/loop20
    - sudo losetup -d /dev/loop20

