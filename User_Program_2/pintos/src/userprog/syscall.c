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

int read (int fd, const void* buffer, unsigned size) {
	int i;
	if(is_user_vaddr(buffer) == 0) exit(-1);
	char tmp;
	if (fd == 0) {
//		for(i=0; i<size; i++)
//			((char*)buffer)[i] = input_getc();
		return size;
	}
	else if(fd >= 3) {
		file_deny_write(thread_current()->files[fd]);
		i = file_read(thread_current()->files[fd], buffer, size);
		return i;
	}
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

int open(const char* file) {
	int i = -1;
	struct file* new = filesys_open(file);
	if(new != NULL) {
		for(i=3; i<131; i++) {
			if(thread_current()->files[i] == NULL) {
				thread_current()->files[i] = new;
				break;
			}
		}
	}
	return i;
}

void close(int fd) {
//	printf("%d th files closing\n", fd);
	file_close(thread_current()->files[fd]);
	thread_current()->files[fd] = NULL;
}
int write (int fd, const void *buffer, unsigned size) {
	if (fd == 1) {
		putbuf(buffer, size);
		return size;
	}
	else if(fd >= 3)
		return file_write(thread_current()->files[fd], buffer, size);
	return -1; 
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
	if(is_user_vaddr(f->esp + 4) == 0 || is_user_vaddr(f->esp + 8) == 0 || is_user_vaddr(f->esp + 12) == 0) exit(-1);
	else f->eax = write((int)*(uint32_t *)(f->esp+4), (void*)*(uint32_t*)(f->esp + 8), (unsigned)*(uint32_t*)(f->esp + 12));
  else if(*(uint32_t*)(f->esp) == SYS_READ)
	if(is_user_vaddr(f->esp + 4) == 0 || is_user_vaddr(f->esp + 8) == 0 || is_user_vaddr(f->esp + 12) == 0) exit(-1);
	else f->eax = read((int)*(uint32_t *)(f->esp+4), (void*)*(uint32_t*)(f->esp + 8), (unsigned)*(uint32_t*)(f->esp + 12));
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
	if(is_user_vaddr(f->esp + 4) == 0 || is_user_vaddr(f->esp + 8) == 0 || is_user_vaddr(f->esp + 12) == 0 || is_user_vaddr(f->esp + 16) == 0) exit(-1);
	else f->eax = sum_of_four_int((int)*(uint32_t *)(f->esp + 4), (int)*(uint32_t *)(f->esp + 8), (int)*(uint32_t *)(f->esp + 12), (int)*(uint32_t *)(f->esp + 16));
  else if( *(uint32_t*)(f->esp) == SYS_CREATE)
	if(is_user_vaddr(f->esp + 4) == 0 || is_user_vaddr(f->esp + 8) == 0 || (char*)*(uint32_t *)(f->esp+4) == NULL) exit(-1);
	else f->eax = filesys_create((char*)*(uint32_t*)(f->esp+4), (unsigned)*(uint32_t*)(f->esp + 8));
  else if( *(uint32_t*)(f->esp) == SYS_REMOVE)
	if(is_user_vaddr(f->esp + 4) == 0) exit(-1);
	else f->eax = filesys_remove((char*)*(uint32_t*)(f->esp+4));
  else if( *(uint32_t*)(f->esp) == SYS_OPEN)
	if(is_user_vaddr(f->esp + 4) == 0 || (char*)*(uint32_t*)(f->esp+4) == NULL) exit(-1);
	else f->eax = open((char*)*(uint32_t*)(f->esp+4));
  else if( *(uint32_t*)(f->esp) == SYS_SEEK)
	if(is_user_vaddr(f->esp + 4) == 0 || is_user_vaddr(f->esp + 8) == 0) exit(-1);
	else file_seek(thread_current()->files[(int)*(uint32_t*)(f->esp+4)], (unsigned)*(uint32_t*)(f->esp + 8));
  else if( *(uint32_t*)(f->esp) == SYS_TELL)
	if(is_user_vaddr(f->esp + 4) == 0) exit(-1);
	else f->eax = file_tell(thread_current()->files[(int)*(uint32_t*)(f->esp+4)]);
  else if( *(uint32_t*)(f->esp) == SYS_CLOSE)
	if(is_user_vaddr(f->esp + 4) == 0) exit(-1);
	else close((int)*(uint32_t*)(f->esp+4));
  else if( *(uint32_t*)(f->esp) == SYS_FILESIZE)
	if(is_user_vaddr(f->esp + 4) == 0) exit(-1);
	else f->eax = file_length(thread_current()->files[(int)*(uint32_t*)(f->esp+4)]);

 // thread_exit ();
}

