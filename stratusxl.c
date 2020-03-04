

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

#include "stratusxl.h"

/*****************
 * sxl_update 
 * 
 * Update SXL_STATE structure by parsing report buffer
 */
int sxl_update(SXL_STATE* sxl, u_int8_t* buffer, int evfd)
{
    int report_number;
    struct input_event ev;
    
    // Take a copy of the state
    SXL_STATE newsxl;
    memcpy(&newsxl, sxl, sizeof(SXL_STATE));
    
    report_number = buffer[0];
    switch(report_number) {
        case 0x01: // Gamepad report.
            // Parse report, updating newsxl state
            parse_r1(&newsxl, buffer);
            break;
        case 0x02:
            parse_r2(&newsxl, buffer);
            break;
        case 0x03: // Mouse report (for mouse mode)
            // We don't need to deal with this, as we are leaving
            // the kernel event nodes intact.
            break;
        case 0x04:
        case 0x05: // a 48-byte packet of vendor-specific I-Do-Not-Know
            break;
        case 0x06: // LEDs
            // I don't do LEDs
            break;
    }

    // hamfisted implementation of trigger deadzones.
    newsxl.axis_rt = newsxl.axis_rt > TRIGGER_THRESH ? newsxl.axis_rt : 0;
    newsxl.axis_lt = newsxl.axis_lt > TRIGGER_THRESH ? newsxl.axis_lt : 0;
    
    // Check for changed values in the sxl struct, and
    // send an event for each change.
    int syn=0;
    if(newsxl.south != sxl->south) {
        syn += send_event(evfd, EV_KEY, BTN_SOUTH, newsxl.south);
    }
    if(newsxl.east != sxl->east) {
        syn += send_event(evfd, EV_KEY, BTN_EAST, newsxl.east);
    }
    if(newsxl.west != sxl->west) {
        syn += send_event(evfd, EV_KEY, BTN_WEST, newsxl.west);
    }
    if(newsxl.north != sxl->north) {
        syn += send_event(evfd, EV_KEY, BTN_NORTH, newsxl.north);
    }
    if(newsxl.lb != sxl->lb) {
        syn += send_event(evfd, EV_KEY, BTN_TL, newsxl.lb);
    }
    if(newsxl.rb != sxl->rb) {
        syn += send_event(evfd, EV_KEY, BTN_TR, newsxl.rb);
    }
    if(newsxl.start != sxl->start) {
        syn += send_event(evfd, EV_KEY, BTN_START, newsxl.start);
    }
    if(newsxl.thumbr != sxl->thumbr) {
        syn += send_event(evfd, EV_KEY, BTN_THUMBR, newsxl.thumbr);
    }
    if(newsxl.thumbl != sxl->thumbl) {
        syn += send_event(evfd, EV_KEY, BTN_THUMBL, newsxl.thumbl);
    }
    if(newsxl.axis_x != sxl->axis_x) {
        syn += send_event(evfd, EV_ABS, ABS_X, newsxl.axis_x);
    }
    if(newsxl.axis_y != sxl->axis_y) {
        syn += send_event(evfd, EV_ABS, ABS_Y, newsxl.axis_y);
    }
    if(newsxl.axis_x2 != sxl->axis_x2) {
        syn += send_event(evfd, EV_ABS, ABS_RX, newsxl.axis_x2);
    }
    if(newsxl.axis_y2 != sxl->axis_y2) {
        syn += send_event(evfd, EV_ABS, ABS_RY, newsxl.axis_y2);
    }
    if(newsxl.axis_rt != sxl->axis_rt) {
        syn += send_event(evfd, EV_ABS, ABS_RZ, newsxl.axis_rt);
    }
    if(newsxl.axis_lt != sxl->axis_lt) {
        syn += send_event(evfd, EV_ABS, ABS_Z, newsxl.axis_lt);
    }
    if(newsxl.dpad_x != sxl->dpad_x) {
        syn += send_event(evfd, EV_ABS, ABS_HAT0X, newsxl.dpad_x);
    }                
    if(newsxl.dpad_y != sxl->dpad_y) {
        syn += send_event(evfd, EV_ABS, ABS_HAT0Y, newsxl.dpad_y);
    }                
    if(newsxl.back != sxl->back) {
        syn += send_event(evfd, EV_KEY, BTN_SELECT, newsxl.back);
    }
    if(newsxl.home != sxl->home) {
        syn += send_event(evfd, EV_KEY, BTN_MODE, newsxl.home);
    }
    
    // Send sync event if any values were changed.
    if(syn>0) {
        send_event(evfd, EV_SYN, SYN_REPORT, 0);
    }
    
    // Save the new state.
    memcpy(sxl, &newsxl, sizeof(SXL_STATE));
    
    return 0;
}

/*************************
 * parse_r1
 * 
 * Parse a Report #1 from the Stratus gamepad.
 */
int parse_r1(SXL_STATE* sxl, u_int8_t* buffer) {
    sxl->north = (buffer[SXLR1_BTNBLOCK1_BYTE] & SXL_NORTH_MASK) > 0;
    sxl->south = (buffer[SXLR1_BTNBLOCK1_BYTE] & SXL_SOUTH_MASK) > 0;
    sxl->east = (buffer[SXLR1_BTNBLOCK1_BYTE] & SXL_EAST_MASK) > 0;
    sxl->west = (buffer[SXLR1_BTNBLOCK1_BYTE] & SXL_WEST_MASK) > 0;

    sxl->lb = (buffer[SXLR1_BTNBLOCK1_BYTE] & SXL_LB_MASK) > 0;
    sxl->rb = (buffer[SXLR1_BTNBLOCK1_BYTE] & SXL_RB_MASK) > 0;
    sxl->thumbl = (buffer[SXLR1_BTNBLOCK2_BYTE] & SXL_THUMBL_MASK) > 0;
    sxl->thumbr = (buffer[SXLR1_BTNBLOCK2_BYTE] & SXL_THUMBR_MASK) > 0;
    sxl->start = (buffer[SXLR1_BTNBLOCK2_BYTE] & SXL_START_MASK) > 0;
    
    sxl->axis_x = (buffer[SXLR1_AXISX_HI_BYTE] << 8) | buffer[SXLR1_AXISX_LO_BYTE];
    sxl->axis_y = (buffer[SXLR1_AXISY_HI_BYTE] << 8) | buffer[SXLR1_AXISY_LO_BYTE];
    sxl->axis_x2 = (buffer[SXLR1_AXISX2_HI_BYTE] << 8) | buffer[SXLR1_AXISX2_LO_BYTE];
    sxl->axis_y2 = (buffer[SXLR1_AXISY2_HI_BYTE] << 8) | buffer[SXLR1_AXISY2_LO_BYTE];
    sxl->axis_lt = (buffer[SXLR1_AXISTL_HI_BYTE] << 8) | buffer[SXLR1_AXISTL_LO_BYTE];
    sxl->axis_rt = (buffer[SXLR1_AXISTR_HI_BYTE] << 8) | buffer[SXLR1_AXISTR_LO_BYTE];
    
    int d = buffer[SXLR1_HAT_BYTE];
    switch(d){
        case 0x00:
            sxl->dpad_y = -1;
            sxl->dpad_x = 0;
            break;
        case 0x01:
            sxl->dpad_y = -1;
            sxl->dpad_x = 1;
            break;
        case 0x02:
            sxl->dpad_y = 0;
            sxl->dpad_x = 1;
            break;
        case 0x03:
            sxl->dpad_y = 1;
            sxl->dpad_x = 1;
            break;
        case 0x04:
            sxl->dpad_y = 1;
            sxl->dpad_x = 0;
            break;
        case 0x05:
            sxl->dpad_y = 1;
            sxl->dpad_x = -1;
            break;
        case 0x06:
            sxl->dpad_y = 0;
            sxl->dpad_x = -1;
            break;
        case 0x07:
            sxl->dpad_y = -1;
            sxl->dpad_x = -1;
            break;
        case 0x0f:
        default:
            sxl->dpad_x = 0;
            sxl->dpad_y = 0;
            break;
    }
    return 0;
}

int parse_r2(SXL_STATE* sxl, u_int8_t* buffer) {
    sxl->home = (buffer[SXLR2_BUTTONS_BYTE] & SXL_HOME_MASK) > 0;
    sxl->back = (buffer[SXLR2_BUTTONS_BYTE] & SXL_BACK_MASK) > 0;
    
    return 0;
}

