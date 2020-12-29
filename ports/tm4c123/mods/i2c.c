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

    self->pin_SCL = pins[0];
    self->pin_SDA = pins[1];

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



STATIC void i2c_start_stop_condition(mp_obj_t *self_in, bool start_stop)
{
    machine_hard_i2c_obj_t *self = (machine_hard_i2c_obj_t *)self_in;
    
    // start == false ; stop == true
    (start_stop == false) ? I2CMasterControl(self->i2c_base, I2C_MASTER_CMD_BURST_SEND_START) : I2CMasterControl(self->i2c_base, I2C_MASTER_CMD_BURST_SEND_STOP);

    //mp_printf(MP_PYTHON_PRINTER, "start_stop_condition: %d\n", start_stop);
}

STATIC uint32_t initialize_i2c_write(mp_obj_t *self_in, uint8_t device_address)
{
    machine_hard_i2c_obj_t *self = (machine_hard_i2c_obj_t *)self_in;

    // specify that we want to communicate to device address with an intended write to bus
    // set bitrate to 100 kHz
    I2CMasterSlaveAddrSet(self->i2c_base, device_address, false);

    // make sure no other Master is currently using the BUS
    // while(I2CMasterBusBusy(self->i2c_base));
    //mp_printf(MP_PYTHON_PRINTER, "initialize base %d\n", self->i2c_base);

    return (uint32_t)I2CMasterErr(self->i2c_base);
}

STATIC uint32_t initialize_i2c_read(mp_obj_t *self_in, uint8_t device_address)
{
    machine_hard_i2c_obj_t *self = (machine_hard_i2c_obj_t *)self_in;

    // set slave address and initiate read
    I2CMasterSlaveAddrSet(self->i2c_base, device_address, true);



    return (uint32_t)I2CMasterErr(self->i2c_base);
}

STATIC uint32_t start_i2c_bus_write(mp_obj_t *self_in, uint8_t first_byte)
{
    uint32_t err_code = 0;
    machine_hard_i2c_obj_t *self = (machine_hard_i2c_obj_t *)self_in;

    // mp_printf(MP_PYTHON_PRINTER, "register address send reached\nsize of data buffer: %d\n", size);
    // memory location to be written to
    I2CMasterDataPut(self->i2c_base, first_byte);

    // send start condition, address byte and bit for write direction to slave device
    I2CMasterControl(self->i2c_base, I2C_MASTER_CMD_BURST_SEND_START);

    // wait for Master Control Unit to finish transaction
    while (I2CMasterBusy(self->i2c_base))
    {
    }

    // check for errors
    err_code = I2CMasterErr(self->i2c_base);
    if (err_code != I2C_MASTER_ERR_NONE)
    {
        // if error occured, cancel write routine
        I2CMasterControl(self->i2c_base, I2C_MASTER_CMD_BURST_SEND_ERROR_STOP);
        return err_code;
    }

    mp_printf(MP_PYTHON_PRINTER, "continuos base %d\n", self->i2c_base);
    // return error code
    return err_code;
}

STATIC uint32_t i2c_single_write(mp_obj_t *self_in, uint8_t i2c_data)
{
    uint32_t err_code = 0;
    machine_hard_i2c_obj_t *self = (machine_hard_i2c_obj_t *)self_in;

    // memory location to be written to
    I2CMasterDataPut(self->i2c_base, i2c_data);

    // send start condition, address byte and bit for write direction to slave device
    I2CMasterControl(self->i2c_base, I2C_MASTER_CMD_SINGLE_SEND);

    // wait for Master Control Unit to finish transaction
    while (I2CMasterBusy(self->i2c_base))
    {
    }

    // check for errors
    err_code = I2CMasterErr(self->i2c_base);
    if (err_code != I2C_MASTER_ERR_NONE)
    {
        // if error occured, cancel write routine
        I2CMasterControl(self->i2c_base, I2C_MASTER_CMD_BURST_SEND_ERROR_STOP);
        return err_code;
    }

    // return error code
    return err_code;
}
// changed by Danny, vorher (mp_obj_t * self_in, uint8_t *i2c_data, size_t size)
STATIC uint8_t i2c_write_bytes_to_bus(mp_obj_t *self_in, uint8_t *i2c_data, size_t size)
{
    uint32_t err_code = 0;
    uint8_t num_of_ack = 0;
    machine_hard_i2c_obj_t *self = (machine_hard_i2c_obj_t *)self_in;
    uint8_t *local_buf;
    local_buf = i2c_data;

    for (size_t i = 0; i < size; i++)
    {
        num_of_ack++;

        // write data to FIFO
        I2CMasterDataPut(self->i2c_base, *(local_buf++));

        // send data to the slave
        I2CMasterControl(self->i2c_base, I2C_MASTER_CMD_BURST_SEND_CONT);

        // wait for Master Control Unit to finish transaction
        while (I2CMasterBusy(self->i2c_base))
        {
        }

        // check for errors
        err_code = I2CMasterErr(self->i2c_base);

        if (err_code != I2C_MASTER_ERR_NONE)
        {
            // if error occured, cancel write routine
            I2CMasterControl(self->i2c_base, I2C_MASTER_CMD_BURST_SEND_ERROR_STOP);

            // return error code
            return num_of_ack;
        }
    }
    //mp_printf(MP_PYTHON_PRINTER, "bytes_to_bus base %d\n", self->i2c_base);
    return num_of_ack;
}

STATIC uint32_t terminate_i2c_bus_write(mp_obj_t *self_in, uint8_t last_byte)
{
    uint32_t err_code;
    machine_hard_i2c_obj_t *self = (machine_hard_i2c_obj_t *)self_in;

    // mp_printf(MP_PYTHON_PRINTER, "register address send reached\nsize of data buffer: %d\n", size);
    // memory location to be written to
    I2CMasterDataPut(self->i2c_base, last_byte);

    // send start condition, address byte and bit for write direction to slave device
    I2CMasterControl(self->i2c_base, I2C_MASTER_CMD_BURST_SEND_STOP);

    // wait for Master Control Unit to finish transaction
    while (I2CMasterBusy(self->i2c_base))
    {
    }

    // check for errors
    err_code = I2CMasterErr(self->i2c_base);
    if (err_code != I2C_MASTER_ERR_NONE)
    {
        // if error occured, cancel write routine
        I2CMasterControl(self->i2c_base, I2C_MASTER_CMD_BURST_SEND_ERROR_STOP);
        return err_code;
    }
    mp_printf(MP_PYTHON_PRINTER, "terminate base %d\n", self->i2c_base);
    // return error code
    return err_code;
}

//sends data and, if requested, a memory location (register) via I2C
void i2c_master_tx(mp_obj_t *self_in, uint8_t device_address, bool address_flag, uint16_t mem_loc, bool mem_loc_flag, uint8_t *device_data, size_t size)
{
    uint32_t err_code;
    uint8_t local_memloc_buf[2] = {0};
    uint8_t *local_data_buf;
    uint8_t i = 0;

    local_data_buf = device_data;

    // if BUS is free and no errors occured, proceed
    if (initialize_i2c_write(self_in, device_address) == I2C_MASTER_ERR_NONE)
    {
        // single send routine
        if (size == 1)
        {
            if (i2c_single_write(self_in, *local_data_buf) != I2C_MASTER_ERR_NONE)
            {
                // if error occured, abort
                return;
            }
        }
        else
        {
            if (mem_loc_flag)
            {
                // memory location LSByte second
                local_memloc_buf[1] = (uint8_t)(mem_loc & 0xFF);

                // memory location MSByte first
                local_memloc_buf[0] = (uint8_t)((mem_loc >> 8) & 0xFF);

                err_code = start_i2c_bus_write(self_in, local_memloc_buf[0]);
                err_code = i2c_write_bytes_to_bus(self_in, &local_memloc_buf[1], sizeof(uint8_t));

                if (err_code != I2C_MASTER_ERR_NONE)
                {
                    // if error occured, abort
                    return;
                }
            }
            else
            {

                err_code = start_i2c_bus_write(self_in, *(local_data_buf++));
                i++;

                if (err_code != I2C_MASTER_ERR_NONE)
                {
                    // if error occured, abort
                    return;
                }
            }

            // send payload
            err_code = i2c_write_bytes_to_bus(self_in, local_data_buf, (size - 1) - i);
            err_code = terminate_i2c_bus_write(self_in, *(local_data_buf + (size - 1) - i));

            if (err_code != I2C_MASTER_ERR_NONE)
            {
                // if error occured, abort
                return;
            }
        }
    }
}

// ends i2c receive burst and returns last byte or error code
// STATIC uint8_t terminate_i2c_bus_read(mp_obj_t *self_in)
// {
//     uint8_t err_code;
//     machine_hard_i2c_obj_t *self = (machine_hard_i2c_obj_t *)self_in;

//     // send desired memory location to the slave (can be anything)
//     I2CMasterControl(self->i2c_base, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);

//     // wait for Master Control Unit to finish transaction
//     while (I2CMasterBusy(self->i2c_base))
//     {
//     }

//     err_code = (uint8_t)I2CMasterErr(self->i2c_base);

//     // check for errors
//     if (err_code == I2C_MASTER_ERR_NONE)
//     {
//         // if everything is fine, return final byte
//         return (uint8_t)I2CMasterDataGet(self->i2c_base);
//     }
//     else
//     {
//         // if error occured, cancel write routine
//         I2CMasterControl(self->i2c_base, I2C_MASTER_CMD_BURST_RECEIVE_ERROR_STOP);

//         // return error code
//         nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "data read failed\n"));
//     }
// }

// STATIC uint8_t i2c_single_read(mp_obj_t *self_in, uint8_t device_address)
// {
//     uint8_t err_code;
//     machine_hard_i2c_obj_t *self = (machine_hard_i2c_obj_t *)self_in;

//     // set slave address and initiate read
//     I2CMasterSlaveAddrSet(self->i2c_base, device_address, true);

//     I2CMasterControl(self->i2c_base, I2C_MASTER_CMD_SINGLE_RECEIVE);

//     // wait for Master Control Unit to finish transaction
//     while (I2CMasterBusy(self->i2c_base))
//     {
//     }

//     // check for errors
//     err_code = (uint8_t)I2CMasterErr(self->i2c_base);

//     if (err_code == I2C_MASTER_ERR_NONE)
//     {
//         // everything okay, return received data
//         return (uint8_t)I2CMasterDataGet(self->i2c_base);
//     }
//     else
//     {
//         // something went wrong, return error code
//         // mp_printf(MP_PYTHON_PRINTER, "ERROR CODE: %d\n", err_code);
//         return 0x00;
//     }
// }

// reads number of bytes (len) from bus to address buf
STATIC void i2c_read_bytes_from_bus(mp_obj_t *self_in, uint8_t *buf, size_t len, uint8_t nack)
{
    uint8_t *buf_local;
    machine_hard_i2c_obj_t *self = (machine_hard_i2c_obj_t *)self_in;

    // initialize buffer root pointer
    buf_local = buf;

    // send payload
    for (size_t i = 0; i < len; i++)
    {
        if (i == (len - 1))
        {
            I2CMasterControl(self->i2c_base, nack ? I2C_MASTER_CMD_BURST_RECEIVE_FINISH: I2C_MASTER_CMD_BURST_RECEIVE_CONT);
            
            // wait for Master Control Unit to finish transaction
            while (I2CMasterBusy(self->i2c_base))
            {
            }

            // check for errors
            if (I2CMasterErr(self->i2c_base))
            {
                // if error occured, cancel write routine
                I2CMasterControl(self->i2c_base, I2C_MASTER_CMD_BURST_RECEIVE_ERROR_STOP);

                // return error code
                nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "data read failed\n"));
            }

            return;
        }
        // mp_printf(MP_PYTHON_PRINTER, "rest of buffer read reached\nsize of data buffer: %d\n", len);

        // send data to the slave
        I2CMasterControl(self->i2c_base, I2C_MASTER_CMD_BURST_RECEIVE_CONT);

        // wait for Master Control Unit to finish transaction
        while (I2CMasterBusy(self->i2c_base))
        {
        }

        // check for errors
        if (I2CMasterErr(self->i2c_base))
        {
            // if error occured, cancel write routine
            I2CMasterControl(self->i2c_base, I2C_MASTER_CMD_BURST_RECEIVE_ERROR_STOP);

            // return error code
            nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "data read failed\n"));
        }

        // get received byte from initialized I2C port
        *buf_local++ = (uint8_t)I2CMasterDataGet(self->i2c_base);
    }
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
    

    mp_printf(print, "I2C(%u, scl=%q, sda=%q", i2c->i2c_id, i2c->pin_SCL->name, i2c->pin_SDA->name);

    if (i2c->mode == I2C_MODE_MASTER)
    {
        mp_printf(print, ", I2C.MASTER)");
    }
    else if (i2c->mode == I2C_MODE_SLAVE)
    {
        mp_printf(print, ", I2C.SLAVE)");
    }
}

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

STATIC void machine_hard_i2c_start(mp_obj_base_t *self_in)
{
    i2c_start_stop_condition((mp_obj_t *)self_in, false);
}

STATIC void machine_hard_i2c_stop(mp_obj_base_t *self_in)
{
    i2c_start_stop_condition((mp_obj_t *)self_in, true);
}


STATIC uint8_t machine_hard_i2c_write(mp_obj_base_t *self_in, uint8_t *buf, size_t size)
{
    uint8_t num_of_ack = 0;
    uint8_t *plocal_buf = buf;

    num_of_ack = i2c_write_bytes_to_bus((mp_obj_t *)self_in, plocal_buf, size);

    return num_of_ack;
}

STATIC uint8_t machine_hard_i2c_writeto(mp_obj_base_t *self_in, uint8_t dev_addr, size_t size, uint8_t *buf, uint8_t stop)
{
    uint8_t num_of_ack = 0;
    uint8_t *plocal_buf = buf;

    initialize_i2c_write((mp_obj_t *)self_in, dev_addr);
    machine_hard_i2c_start(self_in);
    num_of_ack = i2c_write_bytes_to_bus((mp_obj_t *)self_in, plocal_buf, size);

    if(stop)
    {
        machine_hard_i2c_stop(self_in);
    }

    return num_of_ack;
}

STATIC uint8_t machine_hard_i2c_writeto_mem(mp_obj_base_t *self_in, uint8_t dev_addr, uint8_t mem_addr, size_t size, uint8_t *buf)
{
    uint8_t num_of_ack = 0;
    uint8_t *plocal_buf = buf;
    uint8_t *pmem_addr = &mem_addr;

    initialize_i2c_write((mp_obj_t *)self_in, dev_addr);
    machine_hard_i2c_start(self_in);
    num_of_ack = i2c_write_bytes_to_bus((mp_obj_t *)self_in, pmem_addr, size);
    num_of_ack += i2c_write_bytes_to_bus((mp_obj_t *)self_in, plocal_buf, size);

    if(1)
    {
        machine_hard_i2c_stop(self_in);
    }

    return num_of_ack;
}

STATIC void machine_hard_i2c_readinto(mp_obj_base_t *self_in, uint8_t *buf, uint8_t buf_size, uint8_t nack)
{
    uint8_t *plocal_buf = buf;

    // read bytes from bus into given buffer
    i2c_read_bytes_from_bus((mp_obj_t *)self_in, plocal_buf, buf_size, nack);
}

STATIC void machine_hard_i2c_readfrom_into(mp_obj_base_t *self_in, uint8_t dev_address, uint8_t *buf, uint8_t buf_size, uint8_t stop)
{
    uint8_t *local_buf = buf;

    // continue reading i2c bus and send nack at the end
    i2c_read_bytes_from_bus((mp_obj_t *)self_in, local_buf, buf_size, 1);

    if (1)
    {
        // terminate i2c read with stop
        machine_hard_i2c_stop(self_in);
    }
    
}

STATIC mp_obj_t machine_hard_i2c_readfrom_mem(mp_obj_base_t *self_in, uint8_t dev_address, uint16_t memaddr, size_t size)
{

    // OBACHT!
    // AN DER STELLE SOLL EIN REPEATED START PROZESS PASSIEREN.
    // DAZU MUSST DU DIE HELPER FUNCTIONS UM EINEN ÜBERGABE PARAMETER ERWEITERN: OP-CODE
    // HIER WIRD DAS MAKRO ÜBERGEBEN, DAS IN DAS MASTER CONTROL/STATUS REGISTER GESCHRIEBEN WIRD.
    // SO KANN MAN REPEATED START CONDITION ERZEUGEN, STOP CONDITION UNTERBINDEN, ETC.

    return mp_const_none;
}

STATIC mp_obj_t machine_hard_i2c_readfrom(mp_obj_base_t *self_in, uint8_t dev_address, uint8_t *buf, uint8_t nbytes, uint8_t stop)
{
    machine_hard_i2c_obj_t *self = (machine_hard_i2c_obj_t *)self_in;
    uint8_t *local_buffer;

    local_buffer = buf;

    initialize_i2c_read((mp_obj_t *)self_in, dev_address);
    machine_hard_i2c_start((mp_obj_base_t *)self_in);

    // read bytes from bus into buffer
    i2c_read_bytes_from_bus((mp_obj_t *)self, local_buffer, nbytes, 1);
    
    mp_printf(MP_PYTHON_PRINTER, "finished bytes_from_bus\n");
    if(1)
    {
        // terminate i2c read with stop
        machine_hard_i2c_stop(self_in);
    }
    return mp_const_none;
}

STATIC mp_obj_t machine_hard_i2c_scan(mp_obj_base_t *self_in)
{
    //pyb_i2c_obj_t *self = MP_OBJ_TO_PTR(self_in);
    

    // if (!in_master_mode(self)) {
    //     mp_raise_TypeError("I2C must be a master");
    // }
    uint8_t buf = 1;
    uint8_t *plocal_buf = &buf;

    mp_obj_t list = mp_obj_new_list(0, NULL);

    for (uint addr = 0x08; addr <= 0x77; addr++) {
        //HAL_StatusTypeDef status = HAL_I2C_IsDeviceReady(self->i2c, addr << 1, 1, 200);
        
        uint8_t status = machine_hard_i2c_writeto(self_in, addr, 1, plocal_buf, 1);
        if (status >= 1) {
            mp_obj_list_append(list, MP_OBJ_NEW_SMALL_INT(addr));
        }
    }

    return list;
}

/* MP WRAPPING FUNCTIONS*/

STATIC mp_obj_t mp_machine_i2c_stop(mp_obj_t self)
{
    machine_hard_i2c_stop((mp_obj_base_t *)self);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_machine_i2c_stop_obj, mp_machine_i2c_stop);

STATIC mp_obj_t mp_machine_i2c_start(mp_obj_t self)
{
    
    machine_hard_i2c_start((mp_obj_base_t *)self);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_machine_i2c_start_obj, mp_machine_i2c_start);

STATIC mp_obj_t mp_machine_hard_i2c_readinto(mp_obj_t self, mp_obj_t buf, mp_obj_t nack)
{
    // get read buffer pointer from mp
    mp_buffer_info_t i2c_read_buf;
    mp_get_buffer_raise(buf, &i2c_read_buf, MP_BUFFER_WRITE);

    // get NACK
    uint8_t int_nack = mp_obj_get_int(nack);

    // pass to c-interface function
    machine_hard_i2c_readinto(self, (uint8_t *)i2c_read_buf.buf, i2c_read_buf.len, int_nack);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(mp_machine_hard_i2c_readinto_obj, mp_machine_hard_i2c_readinto);

STATIC mp_obj_t mp_machine_hard_i2c_write(mp_obj_t self_in, mp_obj_t wr_buf)
{
    // get write buffer
    mp_buffer_info_t i2c_data;
    mp_get_buffer_raise(wr_buf, &i2c_data, MP_BUFFER_READ);

    // write bytes to bus and return number of acks
    return mp_obj_new_int((mp_int_t)machine_hard_i2c_write((mp_obj_base_t *)self_in, (uint8_t *)i2c_data.buf, i2c_data.len));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mp_machine_hard_i2c_write_obj, mp_machine_hard_i2c_write);

STATIC mp_obj_t mp_machine_hard_i2c_readfrom(size_t n_args, const mp_obj_t *args)
{

    // casting Micropython data types to c data types
    uint8_t int_dev_address = mp_obj_get_int(args[1]);
    uint8_t int_nbytes = mp_obj_get_int(args[2]);
    uint8_t int_stop = mp_obj_get_int(args[3]);

    vstr_t vstr;
    vstr_init_len(&vstr, int_nbytes);

    // read bytes into vstr buffer
    machine_hard_i2c_readfrom(args[0], int_dev_address, (uint8_t *)vstr.buf, vstr.len, int_stop);

    // return vstr buffer
    return mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_machine_hard_i2c_readfrom_obj, 4, 4, mp_machine_hard_i2c_readfrom);

STATIC mp_obj_t mp_machine_hard_i2c_readfrom_into(size_t n_args, const mp_obj_t *args)
{
    // casting Micropython data types to c data types
    // get device address
    uint8_t int_dev_address = mp_obj_get_int(args[1]);
    // mp_printf(MP_PYTHON_PRINTER, "device address: %d\n", int_dev_address);

    // get buffer to read to
    mp_buffer_info_t i2c_data;
    mp_get_buffer_raise(args[2], &i2c_data, MP_BUFFER_READ);
    // mp_printf(MP_PYTHON_PRINTER, "1. byte of buffer: %d\n", *(uint8_t *)i2c_data.buf);

    // get stop
    uint8_t int_stop = mp_obj_get_int(args[3]);

    // pass to c-interface function
    machine_hard_i2c_readfrom_into(args[0], int_dev_address, i2c_data.buf, i2c_data.len, int_stop);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_machine_hard_i2c_readfrom_into_obj, 4, 4, mp_machine_hard_i2c_readfrom_into);


STATIC mp_obj_t mp_machine_hard_i2c_writeto(size_t n_args, const mp_obj_t *args)
{
    if (n_args == 4)
    {
        // casting Micropython data types to c data types
        uint8_t int_dev_address = mp_obj_get_int(args[1]);
        mp_buffer_info_t i2c_data;
        mp_get_buffer_raise(args[2], &i2c_data, MP_BUFFER_READ);
        uint8_t int_stop = mp_obj_get_int(args[3]);

        // calling I2C-Send Function
        machine_hard_i2c_writeto(args[0], int_dev_address, i2c_data.len, (uint8_t *)i2c_data.buf, int_stop);
    }
    else
    {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "Wrong number of arguments\n"));
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_machine_hard_i2c_writeto_obj, 4, 4, mp_machine_hard_i2c_writeto);

STATIC mp_obj_t mp_machine_hard_i2c_readfrom_mem(size_t n_args, const mp_obj_t *args)
{

    // casting Micropython data types to c data types
    uint8_t int_dev_address = mp_obj_get_int(args[1]);
    uint16_t int_memaddr = mp_obj_get_int(args[2]);
    uint8_t int_nbytes = mp_obj_get_int(args[3]);

    return machine_hard_i2c_readfrom_mem(args[0], int_dev_address, int_memaddr, (size_t)int_nbytes);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_machine_hard_i2c_readfrom_mem_obj, 4, 4, mp_machine_hard_i2c_readfrom_mem);

/* Write Function */

STATIC mp_obj_t mp_machine_hard_i2c_writeto_mem(size_t n_args, const mp_obj_t *args)
{
    if (n_args == 4)
    {
        // casting Micropython data types to c data types
        uint8_t int_dev_address = mp_obj_get_int(args[1]);
        uint8_t int_mem_address = mp_obj_get_int(args[2]);
        mp_buffer_info_t i2c_data;
        mp_get_buffer_raise(args[3], &i2c_data, MP_BUFFER_READ);
        //uint8_t int_stop = mp_obj_get_int(args[3]);

        // calling I2C-Send Function
        machine_hard_i2c_writeto_mem(args[0], int_dev_address, int_mem_address, i2c_data.len, (uint8_t *)i2c_data.buf);
    }
    else
    {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "Wrong number of arguments\n"));
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_machine_hard_i2c_writeto_mem_obj, 4, 4, mp_machine_hard_i2c_writeto_mem);

/* Scan Function */ 

STATIC mp_obj_t mp_machine_hard_i2c_scan(mp_obj_t self)
{
    return machine_hard_i2c_scan(self);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_machine_hard_i2c_scan_obj, mp_machine_hard_i2c_scan);

/* DeInit Function */

STATIC mp_obj_t i2c_deinit(mp_obj_t self_in)
{
    deinitI2C0(&self_in);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(i2c_deinit_obj, i2c_deinit);

/* Init Helper Function */

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
    {MP_OBJ_NEW_QSTR(MP_QSTR_stop), MP_ROM_PTR(&mp_machine_i2c_stop_obj)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_start), MP_ROM_PTR(&mp_machine_i2c_start_obj)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_readinto), MP_ROM_PTR(&mp_machine_hard_i2c_readinto_obj)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_write), MP_ROM_PTR(&mp_machine_hard_i2c_write_obj)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_readfrom), MP_ROM_PTR(&mp_machine_hard_i2c_readfrom_obj)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_readfrom_into), MP_ROM_PTR(&mp_machine_hard_i2c_readfrom_into_obj)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_writeto), MP_ROM_PTR(&mp_machine_hard_i2c_writeto_obj)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_writeto_mem), MP_ROM_PTR(&mp_machine_hard_i2c_writeto_mem_obj)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_readfrom_mem), MP_ROM_PTR(&mp_machine_hard_i2c_readfrom_mem_obj)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_scan), MP_ROM_PTR(&mp_machine_hard_i2c_scan_obj)},
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
