#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include "vga_flappy.h"

#define MAP_SIZE 4096UL
#define HW_REGS_BASE 0xFF200000  // LW bridge base address

int main() {
    int fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd < 0) {
        perror("open");
        return -1;
    }

    void *h2p_lw_virtual_base = mmap(NULL, MAP_SIZE, PROT_READ | PROT_WRITE,
                                     MAP_SHARED, fd, HW_REGS_BASE);
    if (h2p_lw_virtual_base == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return -1;
    }

    volatile int *bird_y    = (int *)(h2p_lw_virtual_base + BIRD_Y_OFFSET);
    volatile int *pillar_x  = (int *)(h2p_lw_virtual_base + PILLAR_X_OFFSET);

    int y = 100, velocity = 0, x = 640;

    while (1) {
        velocity += GRAVITY;
        if (getchar() == ' ') velocity = -JUMP_STRENGTH;

        y += velocity;
        if (y < 0) y = 0;
        if (y > SCREEN_HEIGHT - 20) y = SCREEN_HEIGHT - 20;

        x -= PILLAR_SPEED;
        if (x < -50) x = SCREEN_WIDTH;

        *bird_y = y;
        *pillar_x = x;

        usleep(FRAME_DELAY);
    }

    munmap(h2p_lw_virtual_base, MAP_SIZE);
    close(fd);
    return 0;
}
