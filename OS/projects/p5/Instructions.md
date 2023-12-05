# How to Run

CPU 
    - stress --cpu 4
    - stress --vm-bytes 5G --vm-keep -m 1

Network
    - Internet speed test

Disk
    - dd if=/dev/zero of=myloopfile bs=1M count=100 && chmod 0777 myloopfile
    - sudo losetup /dev/loop20 myloopfile
    - sudo mkfs.ext4 /dev/loop20
    - sudo mkdir /mnt/loop20
    - sudo mount /dev/loop20 /mnt/loop20
    - sudo cp -r /etc/* /mnt/loop20
    - sudo umount /mnt/loop20
    - sudo losetup -d /dev/loop20