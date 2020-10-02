#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>
#include "inc/hw_i2c.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "driverlib/i2c.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "i2c.h"

#include "py/runtime.h"
#include "py/obj.h"
#include "py/builtin.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"


void i2c_init0() {}



// INITIALIZATION
//initialize I2C module 0
//Slightly modified version of TI's example code

void InitI2C0(machine_hard_i2c_obj_t *self)
{
    const pin_obj_t *pins[2] = { NULL, NULL};

    if (0) {
    // #if defined(MICROPY_HW_I2C0_SCL)
    } else if (self->i2c_id == I2C_0) {
        self->i2c_base = I2C0_BASE;
        self->periph = SYSCTL_PERIPH_I2C0;
        self->master_regs = (periph_i2c_master_t*)I2C0_BASE;
        self->slave_regs = (periph_i2c_slave_t*)(I2C0_BASE + 0x800);
        self->status_control = (periph_i2c_stctl_t*)(I2C0_BASE + 0xFC0);
        self->irqn = INT_I2C0;
        pins[0] = MICROPY_HW_I2C0_SCL;
        // #if defined(MICROPY_HW_I2C0_SDA)
        pins[1] = MICROPY_HW_I2C0_SDA;
        // #endif
    // #endif
    // #if defined(MICROPY_HW_I2C1_SCL)
    } else if (self->i2c_id == I2C_1) {
        self->i2c_base = I2C1_BASE;
        self->periph = SYSCTL_PERIPH_I2C1;
        self->master_regs = (periph_i2c_master_t*)I2C1_BASE;
        self->slave_regs = (periph_i2c_slave_t*)(I2C1_BASE + 0x800);
        self->status_control = (periph_i2c_stctl_t*)(I2C1_BASE + 0xFC0);
        self->irqn = INT_I2C1;
        pins[0] = MICROPY_HW_I2C1_SCL;
        //#if defined(MICROPY_HW_I2C1_SDA)
        pins[1] = MICROPY_HW_I2C1_SDA;
    //     #endif
    // // #endif
    // // #if defined(MICROPY_HW_I2C2_SCL)
    } else if (self->i2c_id == I2C_2) {
        self->i2c_base = I2C2_BASE;
        self->periph = SYSCTL_PERIPH_I2C2;
        self->master_regs = (periph_i2c_master_t*)I2C2_BASE;
        self->slave_regs = (periph_i2c_slave_t*)(I2C2_BASE + 0x800);
        self->status_control = (periph_i2c_stctl_t*)(I2C2_BASE + 0xFC0);
        self->irqn = INT_I2C2;
        pins[0] = MICROPY_HW_I2C2_SCL;
        // #if defined(MICROPY_HW_I2C2_SDA)
        pins[1] = MICROPY_HW_I2C2_SDA;
    //     #endif
    // // #endif
    // // #if defined(MICROPY_HW_I2C3_SCL)
    } else if (self->i2c_id == I2C_3) {
        self->i2c_base = I2C3_BASE;
        self->periph = SYSCTL_PERIPH_I2C3;
        self->master_regs = (periph_i2c_master_t*)I2C3_BASE;
        self->slave_regs = (periph_i2c_slave_t*)(I2C3_BASE + 0x800);
        self->status_control = (periph_i2c_stctl_t*)(I2C3_BASE + 0xFC0);
        self->irqn = INT_I2C3;
        pins[0] = MICROPY_HW_I2C3_SCL;
        //#if defined(MICROPY_HW_I2C3_SDA)
        pins[1] = MICROPY_HW_I2C3_SDA;
    //     #endif
    // #endif
    } else {
        // I2C does not exist for this board (shouldn't get here, should be checked by caller)
        return;
    }
    
    //disable I2C module 0
    SysCtlPeripheralDisable(self->periph);
    
    //reset module
    SysCtlPeripheralReset(self->periph);
    
    //enable I2C module 0
    SysCtlPeripheralEnable(self->periph);

    // config of Alternative Function for Pins
    mp_hal_pin_config_alt(pins[0], PIN_FN_I2C, self->i2c_id);
    mp_hal_pin_config_alt(pins[1], PIN_FN_I2C, self->i2c_id);
 
    // Enable and initialize the I2C0 master module.  Use the system clock for
    // the I2C0 module.  The last parameter sets the I2C data transfer rate.
    // If false the data rate is set to 100kbps and if true the data rate will
    // be set to 400kbps.
    I2CMasterInitExpClk(self->i2c_base, SysCtlClockGet(), false); // 0x40020000 = I2C0_BASE
     
    //clear I2C FIFOs
    HWREG(self->i2c_base + I2C_O_FIFOCTL) = 80008000;
}


//sends an I2C command to the specified slave
void writeI2C0(uint16_t device_address, uint16_t device_register, uint8_t device_data)
{
   //specify that we want to communicate to device address with an intended write to bus
   I2CMasterSlaveAddrSet(I2C1_BASE, device_address, false);

   //register to be read
   I2CMasterDataPut(I2C1_BASE, device_register);

   //send control byte and register address byte to slave device
   I2CMasterControl(I2C1_BASE, I2C_MASTER_CMD_BURST_SEND_START);

   //wait for MCU to finish transaction
   while(I2CMasterBusy(I2C1_BASE));

   I2CMasterSlaveAddrSet(I2C1_BASE, device_address, false);

   //specify data to be written to the above mentioned device_register
   I2CMasterDataPut(I2C1_BASE, device_data);

   //wait while checking for MCU to complete the transaction
   I2CMasterControl(I2C1_BASE, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);

   //wait for MCU & device to complete transaction
   while(I2CMasterBusy(I2C1_BASE));
}


void deinitI2C0(const mp_obj_t *self_in) {
    machine_hard_i2c_obj_t* self = (machine_hard_i2c_obj_t*) self_in;
    //I2CDisable(self->i2c_base);
    SysCtlPeripheralDisable(self->periph);
}



/* --------- Binding for Micropython ------------ */
/* ---------------------------------------------- */




/*
	Wrapping Function
*/
STATIC mp_obj_t i2c_init_helper() {
	// enum{ARG_mode, ARG_id};
    // static const mp_arg_t allowed_args[] = {
    //     {MP_QSTR_mode, MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },    // default: Master
    //     {MP_QSTR_id, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = I2C_0} },     // default: I2C0
    // };

    // mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    // mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // uint8_t i2c_mode = mp_obj_get_int(mode);
    // uint8_t i2c_port = mp_obj_get_int(port);

    machine_hard_i2c_obj_t self = {0};


    self.i2c_id = I2C_1;
    self.mode = 0;
     

    // if(args[ARG_mode].u_int > 2)
    // {
    //     nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "Mode accepts only MASTER or SLAVE"));
    // }
    // self->mode = args[0].u_int;
    

    // if(args[ARG_id].u_int > 3)
    // {
    //     nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "Available I2C-Ports: 0 - 3"));
    // }
    // self->i2c_id = args[1].u_int;
    

    // calling I2C-Init function
    InitI2C0(&self);

    // sending test value
    // I2CSend(5, 1, 5);



	return mp_const_none;
}




/* Write Function */

STATIC mp_obj_t i2c_write(mp_obj_t dev_address, mp_obj_t reg_address, mp_obj_t i2c_data) {

    // casting Micropython data types to c data types
    uint16_t int_dev_address = mp_obj_get_int(dev_address);
    uint16_t int_reg_address = mp_obj_get_int(reg_address);
    uint8_t int_i2c_data = mp_obj_get_int(i2c_data);

    // calling I2C-Send Function
    writeI2C0(int_dev_address, int_reg_address, int_i2c_data);

    return mp_const_none;
}

/* DeInit Function */

STATIC mp_obj_t i2c_deinit(mp_obj_t self_in) {
    deinitI2C0(&self_in);

    return mp_const_none;
}

STATIC mp_obj_t i2c_init() {  
    return i2c_init_helper();
}

/*
	Define uPy-Fuctions
*/
STATIC MP_DEFINE_CONST_FUN_OBJ_0(i2c_init_obj, i2c_init);
STATIC MP_DEFINE_CONST_FUN_OBJ_3(i2c_write_obj, i2c_write);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(i2c_deinit_obj, i2c_deinit);


/*
	Table with all globals
*/
STATIC const mp_map_elem_t i2c_globals_table[]= {
	{ MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_i2c) },
	{ MP_OBJ_NEW_QSTR(MP_QSTR_init), (mp_obj_t)&i2c_init_obj },
	{ MP_OBJ_NEW_QSTR(MP_QSTR_write), (mp_obj_t)&i2c_write_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_deinit), (mp_obj_t)&i2c_deinit_obj },
};


/*
	Define all globals as uPy-globals
*/
STATIC MP_DEFINE_CONST_DICT(i2c_globals, i2c_globals_table);


/*
	Define addresses
*/
const mp_obj_module_t i2c_module = {
	.base = { &mp_type_module },
	.globals = (mp_obj_dict_t*)&i2c_globals,
}; 
