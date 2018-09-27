#include <boot.h>

boot_params * get_boot_params(boot_params * param) {
	if(param == NULL) {
		return NULL;
	}
	
	// Get the parameters from the boot loader
	boot_params * _params = (boot_params *) ADDRESS;
	
	// If the signature is valid, then return the parameters
    if (_params->sig == SIGNATURE) {
		param->sig = _params->sig;
		param->cursor_x = _params->cursor_x;
		param->cursor_y = _params->cursor_y;
		param->memory_lower = _params->memory_lower;
		param->memory_upper = _params->memory_upper;
		param->memory_map = _params->memory_map;
		param->memory_map_length = _params->memory_map_length;
		param->kernel_size = _params->kernel_size;
        return param;
    }
	
	// If invalid, then zero all values and return NULL
	param->sig = 0;
	param->cursor_x = 0;
	param->cursor_y = 0;
	param->memory_lower = 0;
	param->memory_upper = 0;
	param->memory_map = NULL;
	param->memory_map_length = 0;
	param->kernel_size = 0;
	return NULL;
}
