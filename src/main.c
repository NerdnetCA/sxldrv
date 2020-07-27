
/************************************************************8
 * 
 *  sxldrv
 * Colin R. Hahn
 * 
 * main.c
 * 
 * 
 * 
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <assert.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#include <fcntl.h>

#include <linux/input.h>
#include <linux/uinput.h>
#include <linux/hidraw.h>

#include "hidapi.h"
#include "mytools.h"
#include "stratusxl.h"

#define MAX_STR 256

int dump_desc(int fd);
int evdev_run(int fd);

int main(int argc, char *argv[])
{
    int c;
    int flag_d = 0;
    int flag_tf = 0;
    char *value_h = NULL;
    int dfd = 0;
    
    while ((c=getopt(argc,argv, "dh:t")) != -1) {
        switch(c) {
            case 't':
                flag_tf = 1;
                break;
            case 'd':
                flag_d = 1;
                break;
            case 'h':
                value_h = optarg;
                break;
            case '?':
                printf("?.\n");
                break;
        }
    }

    if(flag_tf) {
        int res;
        hid_device *handle;
        wchar_t wstr[MAX_STR];
        
        res = hid_init();
        handle = hid_open(0x0111,0x1419,NULL);
        res = hid_get_manufacturer_string(handle, wstr, MAX_STR);
        wprintf(L"Manufacturer String: %s\n", wstr);
        res = hid_get_product_string(handle, wstr, MAX_STR);
        wprintf(L"Product String: %s\n",wstr);
        
        exit(0);
    }
    
    if(!value_h) {
        int res;
        hid_device *handle;
        handle = hid_open(0x0111,0x1419,NULL);
        if(!handle) { printf("Unable to open controller\n");
        }
        dfd = hid_get_fd(handle);
    } else {
        dfd = open(value_h, O_RDWR|O_EXCL);
    }
    
    
    if(!(dfd>0)) {
        fprintf(stderr,"Failed to open device %s",value_h);
        return 1;
    }
    
    if(flag_d) {
        dump_desc(dfd);
        printf("\n\n");
    }
    
    evdev_run(dfd);
    
    if(dfd>0) close(dfd);
    
    return 0;
}

/****
 * main driver loop.
 * 
 */
int evdev_run(int hidfd) {
    // Set up the virtual evdev node
    int evfd = init_uevdev();

    u_int8_t* buffer;
    SXL_STATE* state;
    
    buffer = malloc(SXL_REPORT_BUFFER_SIZE);
    memset(buffer, 0, SXL_REPORT_BUFFER_SIZE);
    state = malloc(sizeof(SXL_STATE));
    memset(state,0,sizeof(SXL_STATE));
    
    int go = 1;
    while(go) {
        int c, pn;
        
        c = read(hidfd,buffer, 50);
        if(c > 0) {
            sxl_update(state, buffer, evfd);
        } else {
            go=0;
        }
    }
    
}


int dump_desc(int fd) {
    int res, rsize;
    struct hidraw_report_descriptor rdesc;
    
    res = ioctl(fd, HIDIOCGRDESCSIZE, &rsize);
    rdesc.size = rsize;
    res = ioctl(fd, HIDIOCGRDESC, &rdesc);
    
    for(int i=0; i<rsize; i++) {
        if(!(i%16)) printf("\n");
            
        printf("0x%.2x ",rdesc.value[i]);
    }
    return fd;
}

