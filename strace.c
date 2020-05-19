#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <unistd.h>
#include "strace.h"

//exceptional handling
void exception(char* message){
  printf("failed!error: '%s'\n", message);
  exit(-1);
}
//child,PTRACE_TRACEME requests father trace child
void tracee(char* cmd[]){
  printf("I'm the tracee with pid=%d\n", getpid());
  if(ptrace(PTRACE_TRACEME, NULL, NULL, NULL)<0){
    ("error traceing myself");
  }
  execvp(cmd[0], cmd);
}
//attach 
void tracee_with_pid(int pid){
  if(ptrace(PTRACE_ATTACH, pid, NULL, NULL)<0){
   exception("error attaching to a pid\ndoes the pid exist? are you root?");
  }
}
//show base on number
void show_syscall(unsigned int syscall_no, unsigned int syscall_ret){
  char* syscall_name = syscall_table[syscall_no];
  char* return_prefix = "";
  if(syscall_ret > 15){
    return_prefix = "0x";
  }
  printf("%-15s = %s%x\n", syscall_name, return_prefix, syscall_ret);
}
//tracer,parent
void tracer(int child_pid){
  int child_status;

  printf("I'm the tracer with pid=%d\n", getpid());

  wait(&child_status);
  printf("child_status=%d\n", child_status);

  int syscall_count = 0;
  enum syscall_state syscall_state = PRE_SYSCALL;
  struct user_regs_struct tracee_regs;
  while(1){

    if(ptrace(PTRACE_SYSCALL, child_pid, NULL, NULL)<0){
      exception("error tracing syscalls");
    }
    wait(&child_status);
    syscall_count += 1;

    if(WIFEXITED(child_status)){
      printf("exited in %d syscalls with status=%d\n", syscall_count, child_status);
      break;
    }
    //PTRACE_GETREGS to get tracee'register,orig_rax to get no,rax to get return
    if(ptrace(PTRACE_GETREGS, child_pid, NULL, &tracee_regs)<0){
      exception("error getting registers");
    }
    if(syscall_state == POST_SYSCALL){
      unsigned int current_syscall_no = tracee_regs.orig_rax;
      unsigned int current_syscall_return = tracee_regs.rax;
      show_syscall(current_syscall_no, current_syscall_return);
      syscall_state = PRE_SYSCALL;
    }else{
      syscall_state = POST_SYSCALL;
    }

  }
}

int main(int argc, char* argv[]){
  char* usage_banner = "usage: ./mystrace [<cmd>|-p <pid>]";
  //simple check
  if(argc < 2){
    exception(usage_banner);
  }

  //check is it pid? and simply string transfer
  if(strcmp(argv[1], "-p")==0){
    if(argc < 3){
      exception(usage_banner);
    }
    int pid = atoi(argv[2]);
    tracee_with_pid(pid);
    tracer(pid);
  }else{
    int pid = fork();
    switch(pid){
      case -1:
        exception("error forking");
        break;
      case 0:
        tracee(argv+1);
        break;
      default:
        tracer(pid);
        break;
    }
  }
  return 0;
}
