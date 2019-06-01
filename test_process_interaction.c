#include <fcntl.h>
#include <sys/prctl.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdio.h>
#include <errno.h>

static void subprocess_run(void){
   printf("current pid=%lu\n", getpid());
   prctl(PR_SET_PDEATHSIG, SIGKILL);
   setpgid(0, 0);
    while(1){
        printf("sub process run!! wait 1 second\n");
        sleep(1);
    }
}

#define D(...) do{printf("@[%s:%d]",__FUNCTION__,__LINE__);printf(__VA_ARGS__);printf("\n");}while(0)
pid_t pid_;
static void sig_handler(int sign/**/)
{
    int exit_code = 1;

    D("waiting for pid %d", pid_);
        int status;
        if (pid_ == waitpid(pid_, &status, 0)) {
            D("post waitpid (pid=%d) status=%04x", pid_, status);
            if (WIFSIGNALED(status)) {
                exit_code = 0x80 | WTERMSIG(status);
                D("subprocess killed by signal %d", WTERMSIG(status));
            } else if (!WIFEXITED(status)) {
                D("subprocess didn't exit");
            } else if (WEXITSTATUS(status) >= 0) {
                exit_code = WEXITSTATUS(status);
                D("subprocess exit code = %d", WEXITSTATUS(status));
            }
            D("fork the subporcee~ \n");
            pid_ = fork();
            if(pid_ == 0){
                subprocess_run();
            }else{
                D("fork the process pid=%ld", pid_);
            }
            return;
        }
}

int main(){
   
   signal(SIGCHLD, sig_handler);

    if((pid_=fork()) < 0){
        perror("fork failure!");
        abort();
    }

    if(pid_ == 0){
        subprocess_run();
    }else {
         while(1){
            printf("parent running wait for kill me!\n");
            sleep(1);
         }
    }

    return 0;
}




