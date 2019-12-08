#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"

void halt() {
  shutdown_power_off();
}
void exit (int status) {
	thread_current()->exit = status;
	printf("%s: exit(%d)\n", thread_name(), thread_current()->exit);
	thread_exit();
}

int write (int fd, const void *buffer, unsigned size) {
if (fd == 1) {
	putbuf(buffer, size);
	return size;
}
return -1; 
}

int read (int fd, void* buffer, unsigned size) {
	return -1;
}

int exec (const char *file) {
	return process_execute(file);
}

int wait (tid_t pid) {
	return process_wait(pid);
}

int fibonacci(int n) {
	int i, f[n+1];
	f[0] = 0;
	f[1] = 1;
	for(i=2; i<=n; i++)
		f[i] = f[i-1]+f[i-2];
	return f[n];
}

int sum_of_four_int(int a, int b, int c, int d) {
	return a+b+c+d;
}

static void syscall_handler (struct intr_frame *);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
//  printf ("system call!\n");
  if( *(uint32_t*)(f->esp) == SYS_HALT) halt();
  else if( *(uint32_t*)(f->esp) == SYS_EXIT)
	if(is_user_vaddr(f->esp + 4) == 0) exit(-1);
	else exit(*(uint32_t *)(f->esp + 4));
  else if(*(uint32_t*)(f->esp) == SYS_WRITE)
	if(is_user_vaddr(f->esp + 4) == 0) exit(-1);
	else if(is_user_vaddr(f->esp + 8) == 0) exit(-1);
	else if(is_user_vaddr(f->esp + 12) == 0) exit(-1);
	else f->eax = write((int)*(uint32_t *)(f->esp+4), (void*)*(uint32_t*)(f->esp + 8), (unsigned*)*(uint32_t*)(f->esp + 12));
  else if(*(uint32_t*)(f->esp) == SYS_READ)
	if(is_user_vaddr(f->esp + 4) == 0) exit(-1);
	else if(is_user_vaddr(f->esp + 8) == 0) exit(-1);
	else if(is_user_vaddr(f->esp + 12) == 0) exit(-1);
	else f->eax = read((int)*(uint32_t *)(f->esp+4), (void*)*(uint32_t*)(f->esp + 8), (unsigned*)*(uint32_t*)(f->esp + 12));
  else if( *(uint32_t*)(f->esp) == SYS_EXEC)
	if(is_user_vaddr(f->esp + 4) == 0) exit(-1);
	else f->eax = exec((char*)*(uint32_t *)(f->esp + 4));
  else if( *(uint32_t*)(f->esp) == SYS_WAIT)
	if(is_user_vaddr(f->esp + 4) == 0) exit(-1);
	else f->eax = wait((tid_t)*(uint32_t *)(f->esp + 4));
  else if( *(uint32_t*)(f->esp) == SYS_FIBONACCI)
	if(is_user_vaddr(f->esp + 4) == 0) exit(-1);
	else f->eax = fibonacci((int)*(uint32_t *)(f->esp + 4));
  else if( *(uint32_t*)(f->esp) == SYS_SUM)
	if(is_user_vaddr(f->esp + 4) == 0) exit(-1);
	else f->eax = sum_of_four_int((int)*(uint32_t *)(f->esp + 4), (int)*(uint32_t *)(f->esp + 8), (int)*(uint32_t *)(f->esp + 12), (int)*(uint32_t *)(f->esp + 16));

 // thread_exit ();
}

