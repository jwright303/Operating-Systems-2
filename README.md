# Operating-Systems-2
For my Operating Systems 2 class we created a primitive operating system written in c called JOS. The operating system is meant to run on x86 architecture which is emulated using QEMU. There were four main labs for this project centered around booting the operating system, memory management, user envionrments, and preemptive multitasking. The specifics of what was implemented for each lab can be found in more detail in the corresponding section. 

# Labs
## Lab 1
This lab introduced topcis on the physical address space, BIOS, the boot loader, the Kernel, and virtual memory. For this lab I implemented a backtrace function which prints out the information from the pervious function calls in order of how recent they were called. The information that gets printed is the eip, ebp, and their arguments.
</br>
<img width="570" alt="Screen Shot 2022-11-02 at 4 00 16 PM" src="https://user-images.githubusercontent.com/41707123/199619090-8085fcda-09a1-4e15-9a6d-c37fc91a87bb.png">
<img width="481" alt="lab1" src="https://user-images.githubusercontent.com/41707123/199620437-1dc3d263-646e-4444-8395-9512d252ea19.png">


## Lab 2
This lab focused on memory management including mapping virtual memory to physical memory, and using the page data structure for allocating and freeing memory. The file `kern/pmap.c` contains the functions I implemented to enable the paging system for memory and some initial memory mapping.
</br><img width="1002" alt="Screen Shot 2022-11-02 at 4 00 49 PM" src="https://user-images.githubusercontent.com/41707123/199619151-210ec6f2-393d-4a00-b460-6e4c08fd1880.png"><img width="455" alt="lab2" src="https://user-images.githubusercontent.com/41707123/199620434-7efafe4e-6ef6-4807-a36e-5ad4df73b086.png">


## Lab 3
This lab introduced us to user envionrments, running programs in user envionrments, and system calls from the user envionrment. The functions for creating and running user envionrments can be found in `kern\env.c`. `kern\trapentry.S` and `kern\trap.c` contains the code used for handling system calls.
</br><img width="458" alt="lab3" src="https://user-images.githubusercontent.com/41707123/199620418-8c3c4076-909a-43c8-825b-38acc21a2629.png">

## Lab4
This lab focused on adding multiprocessor support to JOS, implementing round-robin scheduling, adding basic environment management system calls, adding forking, and adding inter-process communication (IPC).
</br><img width="584" alt="lab4" src="https://user-images.githubusercontent.com/41707123/199620389-391307a7-7ad9-4ae3-9783-55906c8a03fb.png">
