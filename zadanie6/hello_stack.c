#include <minix/drivers.h>
#include <minix/chardriver.h>
#include <stdio.h>
#include <stdlib.h>
#include <minix/ds.h>
#include "hello_stack.h"

/*
 * Function prototypes for the hello driver.
 */
static int hello_open(devminor_t minor, int access, endpoint_t user_endpt);
static int hello_close(devminor_t minor);
static ssize_t hello_read(devminor_t minor, u64_t position, endpoint_t endpt,
    cp_grant_id_t grant, size_t size, int flags, cdev_id_t id);
static ssize_t hello_write(devminor_t minor, u64_t position, endpoint_t endpt,
    cp_grant_id_t grant, size_t size, int flags, cdev_id_t id);

/* SEF functions and variables. */
static void sef_local_startup(void);
static int sef_cb_init(int type, sef_init_info_t *info);
static int sef_cb_lu_state_save(int);
static int lu_state_restore(void);

/* Entry points to the hello driver. */
static struct chardriver hello_tab = {
    .cdr_write  = hello_write,
    .cdr_open	= hello_open,
    .cdr_close	= hello_close,
    .cdr_read	= hello_read,
};

static struct __attribute__ ((packed)) buffer {
    size_t buf_size;
    size_t cur_in_buffer;
    char *buf;
} buffer;

static int hello_open(devminor_t UNUSED(minor), int UNUSED(access),
    endpoint_t UNUSED(user_endpt)) {
    return OK;
}

static int hello_close(devminor_t UNUSED(minor)) {
    return OK;
}

static void hello_signal_handler(int signumber) {
    if (signumber == SIGTERM) {
        free(buffer.buf);
        exit(0);
    }
}

static ssize_t hello_read(devminor_t UNUSED(minor), u64_t position,
    endpoint_t endpt, cp_grant_id_t grant, size_t size, int UNUSED(flags),
    cdev_id_t UNUSED(id)) {
    int ret;
    char *ptr;
    void *tmp;
    if (buffer.cur_in_buffer < size) 
        size = buffer.cur_in_buffer;

    if (size == 0)
        return 0;
    /* Copying user ptr */
    ptr = buffer.buf + buffer.cur_in_buffer - size;
    if ((ret = sys_safecopyto(endpt, grant, 0, (vir_bytes) ptr, size)) != OK) {
        return ret;
    }

    /* Decreasing current number of chars in buffer */
    buffer.cur_in_buffer -= size;
    /* reallocating buffer.buf if it's needed */
    if (buffer.buf_size > 1) {
        if (buffer.cur_in_buffer * 4 <= buffer.buf_size) {
            if ((buffer.buf = (char*)realloc(buffer.buf, buffer.buf_size / 2 * sizeof(char))) == NULL)
                return -1;
            buffer.buf_size /= 2;
        }
    }

    /* Return the number of bytes read. */
    return size;
}

static ssize_t hello_write(devminor_t UNUSED(minor), u64_t position,
    endpoint_t endpt, cp_grant_id_t grant, size_t size, int UNUSED(flags),
    cdev_id_t UNUSED(id)) {
    char *ptr;
    int ret;
    void *tmp;
    /* reallocating buffer.buf as long as it's not big enough */
    while (buffer.cur_in_buffer + size > buffer.buf_size) {
        if ((buffer.buf = (char*)realloc(buffer.buf, 2 * buffer.buf_size * sizeof(char))) == NULL) {
            printf("error1\n");
            return -1;
        }
        buffer.buf_size *= 2;
    }
    /* copying user ptr */
    ptr = buffer.buf + buffer.cur_in_buffer;
    
    if ((ret = sys_safecopyfrom(endpt, grant, 0, (vir_bytes) ptr, size)) != OK) {
        printf("error2\n");
        return ret;
    }
    buffer.cur_in_buffer += size;
    /* Return the number of bytes read. */
    return size;
}

static int sef_cb_lu_state_save(int UNUSED(state)) {
    /* Save the state. */
    ds_publish_mem("buffer", (char*) &buffer, sizeof(struct buffer), DSF_OVERWRITE);
    ds_publish_mem("buf", buffer.buf, buffer.buf_size, DSF_OVERWRITE);

    return OK;
}

static int lu_state_restore() {
    /* Restore the state. */
    size_t aux = sizeof(struct buffer);
    ds_retrieve_mem("buffer", (char *) &buffer, &aux);
    buffer.buf = malloc(buffer.buf_size * sizeof(char));
    aux = buffer.buf_size;
    ds_retrieve_mem("buf", buffer.buf, &aux);

    ds_delete_mem("buf");
    ds_delete_mem("buffer");

    return OK;
}

static void sef_local_startup() {
    /*
     * Register init callbacks. Use the same function for all event types
     */
    sef_setcb_init_fresh(sef_cb_init);
    sef_setcb_init_lu(sef_cb_init);
    sef_setcb_init_restart(sef_cb_init);

    /*
     * Register live update callbacks.
     */
    /* - Agree to update immediately when LU is requested in a valid state. */
    sef_setcb_lu_prepare(sef_cb_lu_prepare_always_ready);
    /* - Support live update starting from any standard state. */
    sef_setcb_lu_state_isvalid(sef_cb_lu_state_isvalid_standard);
    /* - Register a custom routine to save the state. */
    sef_setcb_lu_state_save(sef_cb_lu_state_save);

    // TODO - czy to jest konieczne
    sef_setcb_signal_handler(hello_signal_handler);

    /* Let SEF perform startup. */
    sef_startup();
}

static int sef_cb_init(int type, sef_init_info_t *UNUSED(info)) {
/* Initialize the hello driver. */
    int do_announce_driver = TRUE;

    switch(type) {
        case SEF_INIT_FRESH:
            buffer.buf_size = DEVICE_SIZE;
            buffer.cur_in_buffer = DEVICE_SIZE;
	        buffer.buf = malloc(sizeof(char) * buffer.buf_size);
            char aux[3] = {'a', 'b', 'c'};
            int aux_counter = 0;
            for (int i = 0; i < DEVICE_SIZE; i++) {
                buffer.buf[i] = aux[aux_counter];
                aux_counter = (aux_counter + 1) % 3;
            }
        break;

        case SEF_INIT_LU:
            /* Restore the state. */
            lu_state_restore();
            do_announce_driver = FALSE;
        break;

        case SEF_INIT_RESTART:
        break;  
    }

    /* Announce we are up when necessary. */
    if (do_announce_driver) {
        chardriver_announce();
    }

    /* Initialization completed successfully. */
    return OK;
}

int main(void) {
    /*
     * Perform initialization.
     */
    sef_local_startup();

    /*
     * Run the main loop.
     */
    chardriver_task(&hello_tab);
    return OK;
}
