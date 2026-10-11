#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "of_cart.h"
#include "of.h"

typedef struct snapshot78 {
    int rc;
    uint32_t life;
    uint32_t b1;
    uint32_t diag;
} snapshot78_t;

static snapshot78_t boot78, settle78, slot78, late78;
static long slot_size78;

#ifndef OF_PC
static void take78(snapshot78_t *s) {
    of_cart_caps_t caps;
    s->rc = of_cart_get_caps(&caps);
    if (s->rc == OF_OK) {
        s->life = caps.lifecycle_status;
        s->b1 = caps.host_b1_raw;
        s->diag = caps.host_diag;
    }
}
#endif

static void title78(unsigned page) {
    printf("\033[2J\033[H GBA B1/RESET DIAG smoke.78 %u/5\n", page);
    printf("READ ONLY; NO FLASH COMMANDS\n");
}

static void show78(const char *name, const snapshot78_t *s) {
    unsigned b1_count = (unsigned)((s->diag >> 8) & 0xffu);
    unsigned enter_count = (unsigned)((s->diag >> 16) & 0xffu);
    unsigned exit_count = (unsigned)((s->diag >> 24) & 0xffu);
    unsigned valid = (unsigned)(s->diag & 1u);
    unsigned b1_toggle = (unsigned)((s->diag >> 1) & 1u);
    unsigned enter_toggle = (unsigned)((s->diag >> 2) & 1u);
    unsigned exit_toggle = (unsigned)((s->diag >> 3) & 1u);
    unsigned reset_n = (unsigned)((s->diag >> 4) & 1u);

    printf("%s CAPS RC %d\n", name, s->rc);
    printf("LIFE %08lX B1 %08lX\n",
           (unsigned long)s->life, (unsigned long)s->b1);
    printf("HDIAG %08lX\n", (unsigned long)s->diag);
    printf("COUNT B1/ENTER/EXIT %u/%u/%u\n",
           b1_count, enter_count, exit_count);
    printf("VALID %u TOGGLE B/E/X %u/%u/%u\n",
           valid, b1_toggle, enter_toggle, exit_toggle);
    printf("RESET_N %u\n", reset_n);
}

static void page78(unsigned page) {
    title78(page);
    if (page == 1) {
        printf("APP ENTRY SNAPSHOT\n");
        show78("BOOT", &boot78);
    } else if (page == 2) {
        printf("AFTER 250 MS, BEFORE SLOT QUERY\n");
        show78("SETTLE", &settle78);
    } else if (page == 3) {
        printf("AFTER FILE_SIZE(slot:4)\n");
        printf("SLOT SIZE %ld\n", slot_size78);
        show78("POST SLOT", &slot78);
    } else if (page == 4) {
        printf("1.5 S AFTER SLOT QUERY\n");
        show78("LATE", &late78);
    } else {
        unsigned count = (unsigned)((late78.diag >> 8) & 0xffu);
        unsigned valid = (unsigned)(late78.diag & 1u);
        printf("DECISION\n");
        if (late78.rc != OF_OK) {
            printf("CAPS ACCESS FAILED RC %d\n", late78.rc);
        } else if (count == 0u && late78.b1 == 0u) {
            printf("NO B1 OBSERVED BY CORE\n");
        } else if (!valid && late78.b1 != 0u) {
            printf("B1 SEEN THEN INVALIDATED\n");
            printf("RESET ORDERING IS SUSPECT\n");
        } else if (valid && late78.life == 0u) {
            printf("B1 VALID; LIFECYCLE NOT CAPTURED\n");
        } else if (late78.life != 0u) {
            printf("LIFECYCLE BECAME NONZERO\n");
        } else {
            printf("B1 COUNT %u VALID %u\n", count, valid);
            printf("USE ALL FOUR SNAPSHOTS\n");
        }
        printf("SEND PHOTOS OF ALL FIVE PAGES\n");
    }
    printf("LEFT/RIGHT: PAGE\n");
    fflush(stdout);
}

#ifdef OF_PC
static void fixture78(void) {
    boot78.rc = settle78.rc = slot78.rc = late78.rc = OF_OK;
    boot78.diag = 0x01010100u;
    settle78.diag = 0x01010100u;
    slot78.diag = 0x02020100u;
    late78.diag = 0x02020100u;
    boot78.b1 = settle78.b1 = slot78.b1 = late78.b1 = 0x01010000u;
    slot_size78 = 8705280;
}
#else
static void run78(void) {
    take78(&boot78);
    usleep(250000);
    take78(&settle78);
    slot_size78 = OF_SVC->file_size("slot:4");
    usleep(250000);
    take78(&slot78);
    usleep(1500000);
    take78(&late78);
}
#endif

int main(void) {
#ifdef OF_PC
    fixture78();
    for (unsigned page = 1; page <= 5; ++page)
        page78(page);
    return 0;
#else
    title78(1);
    printf("CAPTURING B1/RESET TIMELINE\n");
    printf("DO NOT REMOVE CARTRIDGE\n");
    fflush(stdout);
    run78();

    unsigned page = 1;
    unsigned previous = ~0u;
    page78(page);
    while (1) {
        of_input_poll_p0();
        unsigned held = _of_input_p0.buttons;
        unsigned edge = held & ~previous;
        previous = held;
        if (edge & OF_BTN_RIGHT) {
            page = page == 5 ? 1 : page + 1;
            page78(page);
        } else if (edge & OF_BTN_LEFT) {
            page = page == 1 ? 5 : page - 1;
            page78(page);
        }
        usleep(16000);
    }
#endif
}
