
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#include <fcntl.h>

#include <linux/input.h>
#include <linux/uinput.h>

#include "mytools.h"

//#define UINPUT_NAME "Stratus XL Colinized"
//#define MY_UINPUT_NAME "SteelSeries Stratus XL (NerdnetCA)"
//#define MY_UINPUT_VENDOR   0x0111
//#define MY_UINPUT_PRODUCT  0x1419
//#define MY_UINPUT_VERSION  0x0001
#define MY_UINPUT_NAME "Microsoft X-Box Controller"
#define MY_UINPUT_VENDOR   0x045e
#define MY_UINPUT_PRODUCT  0x0287
#define MY_UINPUT_VERSION  0x0001
//#define UINPUT_VENDOR   


/***********************
 * parse_uevent_info
 * 
 * Stolen function, can't remember from where.
 * All this does is parse the uevent info text for 
 * the vendor and product id's
 */

/*
 * The caller is responsible for free()ing the (newly-allocated) character
 * strings pointed to by serial_number_utf8 and product_name_utf8 after use.
 */
int
parse_uevent_info(const char *uevent, int *bus_type,
	unsigned short *vendor_id, unsigned short *product_id,
	char **serial_number_utf8, char **product_name_utf8)
{
	char *tmp = strdup(uevent);
	char *saveptr = NULL;
	char *line;
	char *key;
	char *value;

	int found_id = 0;
	int found_serial = 0;
	int found_name = 0;

	line = strtok_r(tmp, "\n", &saveptr);
	while (line != NULL) {
		/* line: "KEY=value" */
		key = line;
		value = strchr(line, '=');
		if (!value) {
			goto next_line;
		}
		*value = '\0';
		value++;

		if (strcmp(key, "HID_ID") == 0) {
			/**
			 *        type vendor   product
			 * HID_ID=0003:000005AC:00008242
			 **/
			int ret = sscanf(value, "%x:%hx:%hx", bus_type, vendor_id, product_id);
			if (ret == 3) {
				found_id = 1;
			}
		} else if (strcmp(key, "HID_NAME") == 0) {
			/* The caller has to free the product name */
			*product_name_utf8 = strdup(value);
			found_name = 1;
		} else if (strcmp(key, "HID_UNIQ") == 0) {
			/* The caller has to free the serial number */
			*serial_number_utf8 = strdup(value);
			found_serial = 1;
		}

next_line:
		line = strtok_r(NULL, "\n", &saveptr);
	}

	free(tmp);
	return (found_id && found_name && found_serial);
}



/**********************
 * init_uevdev()
 * 
 * Initialize uinput-based virtual evdev node
 */

int init_uevdev() {
    int evfd;
    struct uinput_user_dev uidev;
    
    evfd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if(evfd < 0) {
        evfd = open("/dev/input/uinput", O_WRONLY | O_NONBLOCK);
    }
    if (evfd < 0) {
        fprintf(stderr, "Unable to open uinput device!!\n");
    }
    
    ioctl(evfd, UI_SET_EVBIT, EV_KEY);
    
    ioctl(evfd, UI_SET_KEYBIT, BTN_SOUTH);
    ioctl(evfd, UI_SET_KEYBIT, BTN_EAST);
    ioctl(evfd, UI_SET_KEYBIT, BTN_NORTH);
    ioctl(evfd, UI_SET_KEYBIT, BTN_WEST);
    
    ioctl(evfd, UI_SET_KEYBIT, BTN_TL);
    ioctl(evfd, UI_SET_KEYBIT, BTN_TR);

    ioctl(evfd, UI_SET_KEYBIT, BTN_SELECT);
    ioctl(evfd, UI_SET_KEYBIT, BTN_START);
    ioctl(evfd, UI_SET_KEYBIT, BTN_MODE);

    ioctl(evfd, UI_SET_KEYBIT, BTN_THUMBL);
    ioctl(evfd, UI_SET_KEYBIT, BTN_THUMBR);
    
    ioctl(evfd, UI_SET_EVBIT, EV_ABS);
    ioctl(evfd, UI_SET_ABSBIT, ABS_X);
    ioctl(evfd, UI_SET_ABSBIT, ABS_Y);
    ioctl(evfd, UI_SET_ABSBIT, ABS_Z);
    ioctl(evfd, UI_SET_ABSBIT, ABS_RX);
    ioctl(evfd, UI_SET_ABSBIT, ABS_RY);
    ioctl(evfd, UI_SET_ABSBIT, ABS_RZ);

    ioctl(evfd, UI_SET_ABSBIT, ABS_HAT0X);
    ioctl(evfd, UI_SET_ABSBIT, ABS_HAT0Y);
    
    
    memset(&uidev, 0, sizeof(uidev));
    snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, MY_UINPUT_NAME);
    uidev.id.bustype = BUS_USB;
    uidev.id.vendor = MY_UINPUT_VENDOR;
    uidev.id.product = MY_UINPUT_PRODUCT;
    uidev.id.version = MY_UINPUT_VERSION;
    
    uidev.absmin[ABS_X]=-2048;
    uidev.absmax[ABS_X]=2047;
    uidev.absfuzz[ABS_X]=15;
    uidev.absflat[ABS_X]=255;
    uidev.absmin[ABS_Y]=-2048;
    uidev.absmax[ABS_Y]=2047;
    uidev.absfuzz[ABS_Y]=15;
    uidev.absflat[ABS_Y]=255;
    uidev.absmin[ABS_RX]=-2048;
    uidev.absmax[ABS_RX]=2047;
    uidev.absfuzz[ABS_RX]=15;
    uidev.absflat[ABS_RX]=255;
    uidev.absmin[ABS_RY]=-2048;
    uidev.absmax[ABS_RY]=2047;
    uidev.absfuzz[ABS_RY]=15;
    uidev.absflat[ABS_RY]=255;
    uidev.absmin[ABS_Z]=0;
    uidev.absmax[ABS_Z]=4096;
    uidev.absfuzz[ABS_Z]=15;
    uidev.absflat[ABS_Z]=255;
    uidev.absmin[ABS_RZ]=0;
    uidev.absmax[ABS_RZ]=4096;
    uidev.absfuzz[ABS_RZ]=15;
    uidev.absflat[ABS_RZ]=255;
    
    uidev.absmin[ABS_HAT0X] = -1;
    uidev.absmax[ABS_HAT0X] = 1;
    uidev.absmin[ABS_HAT0Y] = -1;
    uidev.absmax[ABS_HAT0Y] = 1;
    
    write(evfd, &uidev, sizeof(uidev));
    ioctl(evfd, UI_DEV_CREATE);
    
   return evfd; 
}

int send_event(int fd, int type, int code, int value) {
    struct input_event ev;
    
    memset(&ev, 0, sizeof(ev));
    ev.type = type;
    ev.code = code;
    ev.value = value;
    write(fd, &ev, sizeof(ev));

    return 1;
}


