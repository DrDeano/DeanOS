#include <boot.h>

boot_params * get_boot_params(void * addr) {
	// Get the parameters from the bootloader
    boot_params * params = (boot_params *) addr;
	
	// If the signature is valid, then return the parameters
    if (params->sig == SIGNATURE) {
        return params;
    }
	
	// Else return NULL as not a valid signature and the data may be wrong
	return NULL;
}
