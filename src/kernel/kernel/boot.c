#include <boot.h>

boot_params * get_boot_params(boot_params * param) {
	if(param == NULL) {
		return NULL;
	}
	
	// Get the parameters from the bootloader
	boot_params * _params = (boot_params *) ADDRESS;
	
	// If the signature is valid, then return the parameters
    if (param->sig == SIGNATURE) {
		param->sig = _params->sig;
		param->cursor_x = _params->cursor_x;
		param->cursor_y = _params->cursor_y;
        return param;
    }
	
	param->sig = 0;
	param->cursor_x = 0;
	param->cursor_y = 0;
	
	// Else return NULL as not a valid signature and the data may be wrong
	return NULL;
}
