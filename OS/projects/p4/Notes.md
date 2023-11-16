# Project 4

There is a disk -> and there is an APi at the top of the disk.
Through some kind of API / Block interface we manipulate the disk data.

We have to modify entire block . If we want to modify a particular data inside a block, we still have to read the block and modify the block and write the block, instead of modifying a specific data.

Important functions : open(), close(), read(), write()

read(block_addr, memory_buf)

However for a File allows byte level modification. 
(File & Disk are diff)

OS takes Block interface to Byte interface

We have Key value Stores instead of file , it gives user level abstraction to physical disk memory
put(), get(), del()
Put takes , key and value , but write takes addr write(addr)

make
./cs238 
usage: ./cs238 block-device

lsblk -> gives devices
Create a file and call losetup -> sets up a virtual disk in Kernel -> it will create a loob back device

Prof takes nvme0n1

sudo ./cs238 /dev/nvme0n1

Read write is a very complicated logic , Say we put(key,value1) , we allocated some value in the disk .

Now we say (key,value2) where value2 size >> value1 size. In that case , we cant accomodate value2 in the same space , so its very complictaed to how we can implement put(key,value)

cd /dev/ gives list iof all devices.
Some are block devices , some are serial devices etc 

dd if=/dev/zero of=file bs=4096 count=10000

./cs238 file

To directly manipulate a block device , we need to use some system calls -> its in device.c and device.h 

device_read(device object , buffer , offset, length)

offset -> memory address but it has to be aligned (block->aligned)

If block is 1024 aligned , anything multiplication of block addr only will work. Eg : 0,1024,2048,4096 etc
512 is not a correct length

In logfs() -> We can pass any memory, no need to send memory aligned address

logfs has append() -> it appends at the end of current memory loc and moves the mem loc by length.

## Device

    - device_read() -> The buffer, offset and length has to be aligned
    - Its all working , dont modify anything
    - ioctl 
    - device->size = u64/u32 8 u32 will just make it integer aligned
    - we do pread and pwrite system calls

## Logfs

    - logfs_read() -. offset and length can be anything , it doesnt need alignment
    - logfs_append() -> Doesnt take an offset calls always appends at the end.
    






