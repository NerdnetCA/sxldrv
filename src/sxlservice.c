/***************************************
 *
 * 
 * 
 *
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/poll.h>
#include <dirent.h>
#include <signal.h>
#include <fcntl.h>

#include <string.h>

#include <libudev.h>

#define BUFLEN	256

int main(int ac, char **av) {
  struct udev *udev;             // handle to udev system
  struct udev_monitor *m_monitor;// handle to udev monitor
  struct pollfd pfd[1];          // for fd polling
  int fd_monitor;                // - the monitor has a fd we can wait on
  char buf[BUFLEN];
  struct sigaction sa, osa;

  memset(&sa, 0, sizeof(sa));
  memset(&osa, 0, sizeof(osa));
  udev = udev_new();  // open udev system
  if(!udev) { exit(1); }

  // Ignore signals from child processes - this is so we can ignore return values of spawned tasks without creating zombies.
  sa.sa_handler = SIG_IGN;
  sa.sa_flags = SA_NOCLDWAIT;
  sigaction(SIGCHLD,&sa,&osa);
 
  m_monitor = udev_monitor_new_from_netlink(udev, "udev");
  if(!m_monitor) { exit(1); }
  udev_monitor_filter_add_match_subsystem_devtype(m_monitor, "hidraw", NULL);
  udev_monitor_filter_add_match_subsystem_devtype(m_monitor, "input", NULL);
  udev_monitor_enable_receiving(m_monitor);
  fd_monitor = udev_monitor_get_fd(m_monitor);

  pfd[0].fd = fd_monitor;
  pfd[0].events = POLLIN;

  for(;;) {
    poll(pfd,1,1000);
    if(pfd[0].revents & POLLIN) {
      struct udev_device *dev;
      const char * action;
      const char * sysName;
      const char * sysPath;
      const char * devPath;

      dev = udev_monitor_receive_device(m_monitor);
      if(dev != NULL) {
        action = udev_device_get_action(dev);
        sysName = udev_device_get_sysname(dev);
        sysPath = udev_device_get_syspath(dev);
        devPath = udev_device_get_devpath(dev);

        if(!strcmp(action,"add")) {
          if(!strncmp(sysName,"hidraw",6)) {
            char *s;
            s = strstr(devPath,"0005:0111:1419");
            if(s) {
              pid_t pid;
              printf("Stratus XL detected - Spawning sxldrv\n\n");
              pid=fork();
              if(pid==0) {
                sleep(2);
                execlp("./sxldrv",NULL);
                exit(0);
              }
            }
          } else if(!strncmp(sysName,"js",2)) {
            int s;
            s = strncmp(devPath,"/devices/virtual",strlen("/devices/virtual"));
            if(s) {
              pid_t pid;
              DIR *dir;
              struct dirent *de;
              char nambuf[256];
              char fdev[256];

              printf("Found nonvirtual joystick node - spawning evtest --grab\n");
              fdev[0] = '\0';

              sleep(1);

              strcpy(nambuf,"/sys");
              strcat(nambuf,devPath);
              *strrchr(nambuf,'/') = '\0';
              dir = opendir(nambuf);
              if(dir) {
                while((de=readdir(dir)) != NULL) {
                  if(!strncmp(de->d_name, "event", 5)) {
                    int nullfd;
                    strcpy(fdev,"/dev/input/");
                    strcat(fdev,de->d_name);
                  }
                }
              }

              if(strlen(fdev) > 0) {
                int nullfd;
                
                pid = fork();
                if(pid==0) {
                  nullfd = open("/dev/null",O_WRONLY);
                  dup2(nullfd,1);
                  execlp("evtest","evtest","--grab",fdev);
                }
              } else {
                printf("Unable to find event node for joystick\n");
              }
            }
          }
        }
        udev_device_unref(dev);
      } else {
        printf("FAILED RECEIVING DEVICE FROM MONITOR\n");
      }
    }
    
  }
  


}
