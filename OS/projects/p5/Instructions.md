# How to Run

CPU 
    - stress --cpu 4
    - stress --vm-bytes 5G --vm-keep -m 1

Network
    - Internet speed test

Disk
    - dd if=/dev/zero of=myloopfile bs=1M count=100
    - sudo losetup /dev/loop21 myloopfile
    - sudo mkfs.ext4 /dev/loop21
    - sudo mkdir /mnt/loop21
    - sudo mount /dev/loop21 /mnt/loop21
    - sudo cp -r /etc/* /mnt/loop21
    - sudo unmount /mnt/loop21
    - sudo losetup -d /dev/loop21