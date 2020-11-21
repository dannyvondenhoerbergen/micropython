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
#include "py/mpprint.h"

void i2c_init0() {}

STATIC int i2c_find(mp_obj_t id)
{
    if (MP_OBJ_IS_STR(id))
    {
        // given a string id
        const char *port = mp_obj_str_get_str(id);
        if (0)
        {
#ifdef MICROPY_HW_I2C0_NAME
        }
        else if (strcmp(port, MICROPY_HW_I2C0_NAME) == 0)
        {
            return I2C_0;
#endif
#ifdef MICROPY_HW_I2C1_NAME
        }
        else if (strcmp(port, MICROPY_HW_I2C1_NAME) == 0)
        {
            return I2C_1;
#endif
#ifdef MICROPY_HW_I2C2_NAME
        }
        else if (strcmp(port, MICROPY_HW_I2C2_NAME) == 0)
        {
            return I2C_2;
#endif
#ifdef MICROPY_HW_I2C3_NAME
        }
        else if (strcmp(port, MICROPY_HW_I2C3_NAME) == 0)
        {
            return I2C_3;
#endif
        }
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                                                "I2C(%s) doesn't exist", port));
    }
    else
    {
        // given an integer id
        int i2c_id = mp_obj_get_int(id);
        if (i2c_id >= 0 && i2c_id <= MP_ARRAY_SIZE(MP_STATE_PORT(machine_spi_obj_all)))
        {
            return i2c_id;
        }
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                                                "I2C(%d) doesn't exist", i2c_id));
    }
}

// INITIALIZATION
//initialize I2C module 0
//Slightly modified version of TI's example code

void InitI2C0(machine_hard_i2c_obj_t *self)
{
    const pin_obj_t *pins[2] = {NULL, NULL};

    if (0)
    {
#if defined(MICROPY_HW_I2C0_SCL)
    }
    else if (self->i2c_id == I2C_0)
    {
        self->i2c_base = I2C0_BASE;
        self->periph = SYSCTL_PERIPH_I2C0;
        self->master_regs = (periph_i2c_master_t *)I2C0_BASE;
        self->slave_regs = (periph_i2c_slave_t *)(I2C0_BASE + 0x800);
        self->status_control = (periph_i2c_stctl_t *)(I2C0_BASE + 0xFC0);
        self->irqn = INT_I2C0;
        pins[0] = MICROPY_HW_I2C0_SCL;
#if defined(MICROPY_HW_I2C0_SDA)
        pins[1] = MICROPY_HW_I2C0_SDA;
#endif
#endif
#if defined(MICROPY_HW_I2C1_SCL)
    }
    else if (self->i2c_id == I2C_1)
    {
        self->i2c_base = I2C1_BASE;
        self->periph = SYSCTL_PERIPH_I2C1;
        self->master_regs = (periph_i2c_master_t *)I2C1_BASE;
        self->slave_regs = (periph_i2c_slave_t *)(I2C1_BASE + 0x800);
        self->status_control = (periph_i2c_stctl_t *)(I2C1_BASE + 0xFC0);
        self->irqn = INT_I2C1;
        pins[0] = MICROPY_HW_I2C1_SCL;
#if defined(MICROPY_HW_I2C1_SDA)
        pins[1] = MICROPY_HW_I2C1_SDA;
#endif
#endif
#if defined(MICROPY_HW_I2C2_SCL)
    }
    else if (self->i2c_id == I2C_2)
    {
        self->i2c_base = I2C2_BASE;
        self->periph = SYSCTL_PERIPH_I2C2;
        self->master_regs = (periph_i2c_master_t *)I2C2_BASE;
        self->slave_regs = (periph_i2c_slave_t *)(I2C2_BASE + 0x800);
        self->status_control = (periph_i2c_stctl_t *)(I2C2_BASE + 0xFC0);
        self->irqn = INT_I2C2;
        pins[0] = MICROPY_HW_I2C2_SCL;
#if defined(MICROPY_HW_I2C2_SDA)
        pins[1] = MICROPY_HW_I2C2_SDA;
#endif
#endif
#if defined(MICROPY_HW_I2C3_SCL)
    }
    else if (self->i2c_id == I2C_3)
    {
        self->i2c_base = I2C3_BASE;
        self->periph = SYSCTL_PERIPH_I2C3;
        self->master_regs = (periph_i2c_master_t *)I2C3_BASE;
        self->slave_regs = (periph_i2c_slave_t *)(I2C3_BASE + 0x800);
        self->status_control = (periph_i2c_stctl_t *)(I2C3_BASE + 0xFC0);
        self->irqn = INT_I2C3;
        pins[0] = MICROPY_HW_I2C3_SCL;
#if defined(MICROPY_HW_I2C3_SDA)
        pins[1] = MICROPY_HW_I2C3_SDA;
#endif
#endif
    }
    else
    {
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
    if (self->mode == I2C_MODE_MASTER)
    {
        // Init Master Module for given I2C Port with normal Clock Frequency (100kHz)
        I2CMasterInitExpClk(self->i2c_base, SysCtlClockGet(), false); // 0x40020000 = I2C0_BASE
    }
    else if (self->mode == I2C_MODE_SLAVE)
    {
        // Init Slave Module for given I2C Port and Slave Address
        I2CSlaveInit(self->i2c_base, (uint8_t)self->i2c_id);
    }

    //clear I2C FIFOs
    HWREG(self->i2c_base + I2C_O_FIFOCTL) = 80008000;
}

//sends data and, if requested, a memory location (register) via I2C
void i2c_master_tx(mp_obj_t *self_in, uint8_t device_address, uint8_t mem_loc, bool mem_loc_flag, uint8_t *device_data, size_t size)
{
    machine_hard_i2c_obj_t *self = (machine_hard_i2c_obj_t *)self_in;
    uint32_t err_code;

    mp_printf(MP_PYTHON_PRINTER, "reached c function\nsize of data buffer: %d\n", size);

    // specify that we want to communicate to device address with an intended write to bus
    // set bitrate to 100 kHz
    I2CMasterSlaveAddrSet(self->i2c_base, device_address, false);

    // make sure no other Master is currently using the BUS
    // while(I2CMasterBusBusy(self->i2c_base));

    err_code = I2CMasterErr(self->i2c_base);
    mp_printf(MP_PYTHON_PRINTER, "error code %d\n", err_code);

    // if BUS is free and no errors occured, proceed
    if (err_code == I2C_MASTER_ERR_NONE)
    {
        // single send routine
        if (size == 1)
        {
            mp_printf(MP_PYTHON_PRINTER, "single send\nsize of data buffer: %d\n", size);
            // memory location to be written to
            I2CMasterDataPut(self->i2c_base, *(device_data));

            // send start condition, address byte and bit for write direction to slave device
            I2CMasterControl(self->i2c_base, I2C_MASTER_CMD_SINGLE_SEND);

            // wait for Master Control Unit to finish transaction
            while(I2CMasterBusy(self->i2c_base))
            {
                // mp_printf(MP_PYTHON_PRINTER, "Master Busy: %d\n", I2CMasterBusy(self->i2c_base));
            }

            // check for errors
            if (I2CMasterErr(self->i2c_base))
            {
                // return error code
                err_code = I2CMasterErr(self->i2c_base);
                mp_printf(MP_PYTHON_PRINTER, "ERROR CODE: %d\n", err_code);
                // nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "data transfer failed\n"));
            }
            // end transfer
            return;
        }
        else
        {
            if (mem_loc_flag)
            {
                mp_printf(MP_PYTHON_PRINTER, "register address send reached\nsize of data buffer: %d\n", size);
                // memory location to be written to
                I2CMasterDataPut(self->i2c_base, mem_loc);

                // send start condition, address byte and bit for write direction to slave device
                I2CMasterControl(self->i2c_base, I2C_MASTER_CMD_BURST_SEND_START);

                // wait for Master Control Unit to finish transaction
                while (I2CMasterBusy(self->i2c_base))
                {}

                // check for errors
                if (I2CMasterErr(self->i2c_base))
                {
                    // if error occured, cancel write routine
                    I2CMasterControl(self->i2c_base, I2C_MASTER_CMD_BURST_SEND_ERROR_STOP);

                    // return error code
                    nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "data transfer failed - memory location unkown\n"));
                }
            }
            else
            {

                mp_printf(MP_PYTHON_PRINTER, "write function data buffer reached\nsize of data buffer: %d\n", size);
                // write data to FIFO
                I2CMasterDataPut(self->i2c_base, *(device_data++));

                // send data to the slave
                I2CMasterControl(self->i2c_base, I2C_MASTER_CMD_BURST_SEND_START);

                // wait for Master Control Unit to finish transaction
                while (I2CMasterBusy(self->i2c_base))
                {}

                // check for errors
                if (I2CMasterErr(self->i2c_base))
                {
                    // if error occured, cancel write routine
                    I2CMasterControl(self->i2c_base, I2C_MASTER_CMD_BURST_SEND_ERROR_STOP);

                    // return error code
                    nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "data transfer failed\n"));
                }
            }

            // send payload
            for (size_t i = 1; i < size; i++)
            {
                mp_printf(MP_PYTHON_PRINTER, "rest of buffer write reached\nsize of data buffer: %d\n", size);
                // last byte?
                if (i == (size - 1))
                {
                    // data to be send
                    I2CMasterDataPut(self->i2c_base, *device_data);

                    // send desired memory location to the slave (can be anything)
                    I2CMasterControl(self->i2c_base, I2C_MASTER_CMD_BURST_SEND_FINISH);

                    // wait for Master Control Unit to finish transaction
                    while (I2CMasterBusy(self->i2c_base))
                    {}

                    // check for errors
                    if (I2CMasterErr(self->i2c_base))
                    {
                        // if error occured, cancel write routine
                        I2CMasterControl(self->i2c_base, I2C_MASTER_CMD_BURST_SEND_ERROR_STOP);

                        // return error code
                        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "data transfer failed\n"));
                    }

                    // transfer finished
                    return;
                }
                // write data to FIFO
                I2CMasterDataPut(self->i2c_base, *(device_data++));

                // send data to the slave
                I2CMasterControl(self->i2c_base, I2C_MASTER_CMD_BURST_SEND_CONT);

                // wait for Master Control Unit to finish transaction
                while (I2CMasterBusy(self->i2c_base))
                {}

                // check for errors
                if (I2CMasterErr(self->i2c_base))
                {
                    // if error occured, cancel write routine
                    I2CMasterControl(self->i2c_base, I2C_MASTER_CMD_BURST_SEND_ERROR_STOP);

                    // return error code
                    nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "data transfer failed\n"));
                }
            }
        }
    }
}

//reads data from device via I2C
void i2c_master_rx(mp_obj_t *self_in, uint8_t device_address, uint8_t mem_loc, bool mem_loc_flag, uint8_t *device_data, size_t size)
{
    //     machine_hard_i2c_obj_t *self = (machine_hard_i2c_obj_t *)self_in;

    // // specify that we want to communicate to device address with an intended write to bus
    // // set bitrate to 100 kHz
    // I2CMasterSlaveAddrSet(self->i2c_base, device_address, false);

    // // make sure, no other Master is currently using the BUS
    // while (I2CMasterBusy(self->i2c_base));

    // // mp_printf(MP_PYTHON_PRINTER, "we made it to c write-function\n");
    // // if BUS is free and no errors occured, proceed
    // if (I2CMasterErr(self->i2c_base) == I2C_MASTER_ERR_NONE)
    // {
    //     if (size == 1)
    //     {
    //         // hier kommt die single send routine rein
    //         mp_printf(MP_PYTHON_PRINTER, "size: %d\n", size);
    //     }
    //     else
    //     {
    //         if (mem_loc_flag)
    //         {
    //             // memory location to be written to
    //             I2CMasterDataPut(self->i2c_base, mem_loc);

    //             // send start condition, address byte and bit for write direction to slave device
    //             I2CMasterControl(self->i2c_base, I2C_MASTER_CMD_BURST_SEND_START);

    //             // wait for Master Control Unit to finish transaction
    //             while (I2CMasterBusy(self->i2c_base))
    //                 ;

    //             // check for errors
    //             if (I2CMasterErr(self->i2c_base))
    //             {
    //                 // if error occured, cancel write routine
    //                 I2CMasterControl(self->i2c_base, I2C_MASTER_CMD_BURST_SEND_ERROR_STOP);

    //                 // return error code
    //                 nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "no device with specified address on i2c bus\n"));
    //             }
    //         }

    //         // wait for Master Control Unit to finish transaction
    //         while (I2CMasterBusy(self->i2c_base));

    //         // check for errors
    //         if (I2CMasterErr(self->i2c_base))
    //         {
    //             // if error occured, cancel write routine
    //             I2CMasterControl(self->i2c_base, I2C_MASTER_CMD_BURST_SEND_ERROR_STOP);

    //             // return error code
    //             nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "specified memory location code is unidentified\n"));
    //         }

    //         // send payload
    //         for (size_t i = 0; i < size; i++)
    //         {

    //             // last byte?
    //             if (i == (size - 1))
    //             {
    //                 // data to be send
    //                 I2CMasterDataPut(self->i2c_base, *device_data);

    //                 // send desired memory location to the slave (can be anything)
    //                 I2CMasterControl(self->i2c_base, I2C_MASTER_CMD_BURST_SEND_FINISH);

    //                 // wait for Master Control Unit to finish transaction
    //                 while (I2CMasterBusy(self->i2c_base));

    //                 // check for errors
    //                 if (I2CMasterErr(self->i2c_base))
    //                 {
    //                     // if error occured, cancel write routine
    //                     I2CMasterControl(self->i2c_base, I2C_MASTER_CMD_BURST_SEND_ERROR_STOP);

    //                     // return error code
    //                     nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "data transfer failed\n"));
    //                 }

    //                 // transfer finished
    //                 return;
    //             }
    //             // write data to FIFO
    //             I2CMasterDataPut(self->i2c_base, *(device_data++));

    //             // send data to the slave
    //             I2CMasterControl(self->i2c_base, I2C_MASTER_CMD_BURST_SEND_CONT);

    //             // wait for Master Control Unit to finish transaction
    //             while (I2CMasterBusy(self->i2c_base));

    //             // check for errors
    //             if (I2CMasterErr(self->i2c_base))
    //             {
    //                 // if error occured, cancel write routine
    //                 I2CMasterControl(self->i2c_base, I2C_MASTER_CMD_BURST_SEND_ERROR_STOP);

    //                 // return error code
    //                 nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "data transfer failed\n"));
    //             }

    //             //specify data to be written to the above mentioned device_register
    //             I2CMasterDataPut(self->i2c_base, *device_data);

    //             //wait while checking for MCU to complete the transaction
    //             I2CMasterControl(self->i2c_base, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);

    //             //wait for MCU & device to complete transaction
    //             while (I2CMasterBusy(self->i2c_base))
    //             {
    //             }
    //         }
    //     }
    // }
}

void deinitI2C0(const mp_obj_t *self_in)
{
    machine_hard_i2c_obj_t *self = (machine_hard_i2c_obj_t *)self_in;
    //I2CDisable(self->i2c_base);
    SysCtlPeripheralDisable(self->periph);
}

/* --------- Binding for Micropython ------------ */
/* ---------------------------------------------- */

STATIC void machine_hard_i2c_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    machine_hard_i2c_obj_t *i2c = (machine_hard_i2c_obj_t *)self_in;
    ;

    mp_printf(print, "I2C(%u", i2c->i2c_id);

    if (i2c->mode == I2C_MODE_MASTER)
    {
        mp_printf(print, ", I2C.MASTER)");
    }
    else if (i2c->mode == I2C_MODE_SLAVE)
    {
        mp_printf(print, ", I2C.SLAVE)");
    }
}

/*
	Wrapping Function
*/
STATIC mp_obj_t machine_hard_i2c_init_helper(mp_obj_t *self_in, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    enum
    {
        ARG_id,
        ARG_mode
    };
    static const mp_arg_t allowed_args[] = {
        {MP_QSTR_id, MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = I2C_0}},            // default: I2C0
        {MP_QSTR_mode, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = I2C_MODE_MASTER}}, // default: Master
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    machine_hard_i2c_obj_t *self = (machine_hard_i2c_obj_t *)self_in;

    // set the I2C id value
    if (args[ARG_id].u_int > 3)
    {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "Available I2C-Ports: 0 - 3"));
    }
    self->i2c_id = args[ARG_id].u_int;

    // set the I2C mode
    if (args[ARG_mode].u_int > I2C_MODE_SLAVE)
    {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "Mode accepts only MASTER or SLAVE"));
    }
    self->mode = (uint16_t)(args[ARG_mode].u_int & 0xFFFF);

    // calling I2C-Init function
    InitI2C0(self);

    return mp_const_none;
}

STATIC void machine_hard_i2c_write(mp_obj_base_t *self_in, uint16_t dev_addr, uint16_t reg_addr, bool mem_loc_flag, uint8_t *i2c_data, size_t size)
{
    i2c_master_tx((mp_obj_t *)self_in, dev_addr, reg_addr, mem_loc_flag, i2c_data, size);
}

/* Write Function */

STATIC mp_obj_t mp_machine_hard_i2c_write_into(size_t n_args, const mp_obj_t *args)
{

    if (n_args == 4)
    {
        // casting Micropython data types to c data types
        uint16_t int_dev_address = mp_obj_get_int(args[1]);
        mp_printf(MP_PYTHON_PRINTER, "device address: %d\n", int_dev_address);
        uint16_t int_reg_address = mp_obj_get_int(args[2]);
        mp_printf(MP_PYTHON_PRINTER, "register address: %d\n", int_reg_address);
        mp_buffer_info_t i2c_data;
        mp_get_buffer_raise(args[3], &i2c_data, MP_BUFFER_READ);
        mp_printf(MP_PYTHON_PRINTER, "1. byte of buffer: %d\n", *(uint8_t *)i2c_data.buf);

        // calling I2C-Send Function
        machine_hard_i2c_write(args[0], int_dev_address, int_reg_address, true, (uint8_t *)i2c_data.buf, i2c_data.len);
    }
    else
    {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "Wrong number of arguments\n"));
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_machine_hard_i2c_write_into_obj, 4, 4, mp_machine_hard_i2c_write_into);

STATIC mp_obj_t mp_machine_hard_i2c_write(size_t n_args, const mp_obj_t *args)
{
    if (n_args == 3)
    {
        // casting Micropython data types to c data types
        uint8_t int_dev_address = mp_obj_get_int(args[1]);
        mp_printf(MP_PYTHON_PRINTER, "device address: %d\n", int_dev_address);
        mp_buffer_info_t i2c_data;
        mp_get_buffer_raise(args[2], &i2c_data, MP_BUFFER_READ);
        mp_printf(MP_PYTHON_PRINTER, "1. byte of buffer: %d\n", *(uint8_t *)i2c_data.buf);

        // calling I2C-Send Function
        machine_hard_i2c_write(args[0], int_dev_address, 0, false, (uint8_t *)i2c_data.buf, i2c_data.len);
    }
    else
    {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "Wrong number of arguments\n"));
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_machine_hard_i2c_write_obj, 3, 3, mp_machine_hard_i2c_write);

/* DeInit Function */

STATIC mp_obj_t i2c_deinit(mp_obj_t self_in)
{
    deinitI2C0(&self_in);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(i2c_deinit_obj, i2c_deinit);

STATIC mp_obj_t machine_hard_i2c_init(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    return machine_hard_i2c_init_helper(args[0], n_args - 1, args + 1, kw_args);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(machine_i2c_init_obj, 1, machine_hard_i2c_init);

/*
	Define uPy-Fuctions
*/

/*
	Table with all globals
*/
STATIC const mp_rom_map_elem_t machine_hard_i2c_locals_dict_table[] = {
    // methods
    {MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_i2c)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_init), MP_ROM_PTR(&machine_i2c_init_obj)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_write), MP_ROM_PTR(&mp_machine_hard_i2c_write_obj)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_write_into), MP_ROM_PTR(&mp_machine_hard_i2c_write_into_obj)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&i2c_deinit_obj)},

    // constants
    {MP_ROM_QSTR(MP_QSTR_MASTER), MP_ROM_INT(I2C_MODE_MASTER)},
    {MP_ROM_QSTR(MP_QSTR_SLAVE), MP_ROM_INT(I2C_MODE_SLAVE)},
};

/*
	Define all globals as uPy-globals
*/
STATIC MP_DEFINE_CONST_DICT(machine_hard_i2c_locals_dict, machine_hard_i2c_locals_dict_table);

mp_obj_t machine_hard_i2c_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args)
{

    // check arguments
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    // find I2C port
    i2c_id_t i2c_id = i2c_find(all_args[0]);

    // create dynamic peripheral object
    machine_hard_i2c_obj_t *self;

    // get SPI object
    if (MP_STATE_PORT(machine_i2c_obj_all)[i2c_id - 1] == NULL)
    {
        // create new I2C object
        self = m_new0(machine_hard_i2c_obj_t, 1);
        self->base.type = &machine_hard_i2c_type;
        self->i2c_id = i2c_id;
        MP_STATE_PORT(machine_i2c_obj_all)
        [i2c_id - 1] = self;
    }
    else
    {
        // reference existing I2C object
        self = MP_STATE_PORT(machine_i2c_obj_all)[i2c_id - 1];
    }

    if (n_args > 1 || n_kw > 0)
    {
        // start the peripheral
        mp_map_t kw_args;
        mp_map_init_fixed_table(&kw_args, n_kw, all_args + n_args);
        mp_obj_t *self_out = (mp_obj_t *)self;
        machine_hard_i2c_init_helper(self_out, n_args - 1, all_args + 1, &kw_args);
    }

    return MP_OBJ_FROM_PTR(self);
}

const mp_obj_type_t machine_hard_i2c_type = {
    {&mp_type_type},
    .name = MP_QSTR_I2C,
    .print = machine_hard_i2c_print,
    .make_new = machine_hard_i2c_make_new,
    .locals_dict = (mp_obj_t)&machine_hard_i2c_locals_dict,
};
