#include <boot.h>

struct boot_params * get_boot_params(void * addr) {
    struct boot_params * params = (struct boot_params *) addr;

    if (params->sig == SIGNATURE) {
        return params;
    } else {
        return NULL;
    }
	return NULL;
}