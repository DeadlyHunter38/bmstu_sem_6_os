#include "./inc/print.h"

int main(int argc, char *argv[])
{   
    int pid = get_pid(argc, argv);

    if (pid != -1)
    {
        /*print_cmdline(pid);
        print_cwd(pid);
        print_environ(pid);
        print_exe(pid);
        print_fd(pid);
        print_root(pid);
        print_stat(pid);
        print_maps(pid);
        print_io(pid);
        print_comm(pid);
        print_task(pid);*/
        //print_pagemap(pid);
        get_pagemap_info(pid);
    }
    return 0;
}