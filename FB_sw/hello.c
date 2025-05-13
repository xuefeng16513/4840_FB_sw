#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "usbkeyboard.h"
#include "vga_ball.h"

#define FLAP_KEY 0x2C  // USB keycode for spacebar
#define ESC_KEY  0x29  // USB keycode for ESC
#define DEVICE_FILE "/dev/vga_ball"

int main() {
    struct libusb_device_handle *keyboard;
    struct usb_keyboard_packet packet;
    uint8_t endpoint_address;
    int transferred;
    int vga_fd;

    // Open USB keyboard
    printf("Opening USB keyboard...\n");
    keyboard = openkeyboard(&endpoint_address);
    if (!keyboard) {
        fprintf(stderr, "Could not find a USB keyboard.\n");
        return 1;
    }

    // Open memory-mapped peripheral
    printf("Opening VGA Ball device...\n");
    vga_fd = open(DEVICE_FILE, O_RDWR);
    if (vga_fd < 0) {
        perror("Failed to open /dev/vga_ball");
        libusb_close(keyboard);
        return 1;
    }

    printf("Press SPACE to flap the bird. Press ESC to quit.\n");

    // Main loop
    while (1) {
        // Poll keyboard with a timeout of 10ms
        int result = libusb_interrupt_transfer(keyboard, endpoint_address,
            (unsigned char *)&packet, sizeof(packet),
            &transferred, 10);

        if (result == 0 && transferred == sizeof(packet)) {
            uint8_t code = packet.keycode[0];

            if (code == FLAP_KEY) {
                printf("SPACEBAR detected! Sending flap command...\n");
                
                // Trigger the flap command to FPGA using IOCTL
                vga_ball_arg_t vla = {0};  // Initialize all fields to 0
                vla.flap = 1;              // Set flap bit
                
                if (ioctl(vga_fd, VGA_BALL_WRITE_FLAP, &vla) == -1) {
                    perror("ioctl(VGA_BALL_WRITE_FLAP) failed");
                } else {
                    printf("Flap triggered successfully!\n");
					usleep(20000);  // 20ms should be enough for at least one frame

					// Reset flap signal
					vla.flap = 0;
					ioctl(vga_fd, VGA_BALL_WRITE_FLAP, &vla);
                }
            }

            if (code == ESC_KEY) {
                printf("Exiting...\n");
                break;
            }
        } else if (result != 0 && result != LIBUSB_ERROR_TIMEOUT) {
            // Only report non-timeout errors
            fprintf(stderr, "libusb_interrupt_transfer error: %d\n", result);
        }

        usleep(10000);  // 10ms sleep to avoid CPU hogging
    }

    close(vga_fd);
    libusb_close(keyboard);
    return 0;
}
