#include "py/runtime.h"
#include "py/obj.h"
#include "py/builtin.h"
#include "kosaken.c"


/*
	Wrapping Function
*/
STATIC mp_obj_t test_kosaken(mp_obj_t buf_in) {
	printit();
	return mp_const_none;
}


/*
	Define uPy-Fuction
*/
STATIC MP_DEFINE_CONST_FUN_OBJ_1(test_kosaken_obj, test_kosaken);


/*
	Table with all globals
*/
STATIC const mp_map_elem_t test_globals_table[]= {
	{ MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_test) },
	{ MP_OBJ_NEW_QSTR(MP_QSTR_kosaken), (mp_obj_t)&test_kosaken_obj },
};


/*
	Define all globals as uPy-globals
*/
STATIC MP_DEFINE_CONST_DICT(test_globals, test_globals_table);


/*
	Define addresses
*/
const mp_obj_module_t test_module = {
	.base = { &mp_type_module },
	.globals = (mp_obj_dict_t*)&test_globals,
}; 
