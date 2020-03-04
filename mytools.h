
#ifndef MYTOOLS_H
#define MYTOOLS_H


// Parse uevent string
int parse_uevent_info(const char *uevent, int *bus_type,
	unsigned short *vendor_id, unsigned short *product_id,
	char **serial_number_utf8, char **product_name_utf8);

int init_uevdev();

int send_event(int fd, int type, int code, int value);
#endif

