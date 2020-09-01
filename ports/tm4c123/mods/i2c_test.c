#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
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
// #include "<MODULE>.c"


void i2c_init0() {}


// INITIALIZATION
//initialize I2C module 0
//Slightly modified version of TI's example code
void InitI2C0(void)
{
    //enable GPIO peripheral that contains I2C 0
    //SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    //while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOB));
    
    //disable I2C module 0
    SysCtlPeripheralDisable(SYSCTL_PERIPH_I2C0);
    
    //reset module
    SysCtlPeripheralReset(SYSCTL_PERIPH_I2C0);
    
    //enable I2C module 0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);
    
    mp_hal_pin_config_alt(pin_PB2, PIN_FN_I2C, 0);
    mp_hal_pin_config_alt(pin_PB3, PIN_FN_I2C, 0);
    // Configure the pin muxing for I2C0 functions on port B2 and B3.
    //GPIOPinConfigure(GPIO_PB2_I2C0SCL);
    //GPIOPinConfigure(GPIO_PB3_I2C0SDA);
     
    //Versuch: Interrupt zu I2C0 aus machen
    //IntDisable(INT_I2C0);
    //I2CMasterDisable(I2C0_BASE);
     
    // Select the I2C function for these pins.
    //GPIOPinTypeI2CSCL(GPIO_PORTB_BASE, GPIO_PIN_2); // 0x40005000 = GPIO_PORTB_BASE
    //GPIOPinTypeI2C(0x40005000, GPIO_PIN_3);
 
    // Enable and initialize the I2C0 master module.  Use the system clock for
    // the I2C0 module.  The last parameter sets the I2C data transfer rate.
    // If false the data rate is set to 100kbps and if true the data rate will
    // be set to 400kbps.
    I2CMasterInitExpClk(0x40020000, SysCtlClockGet(), false); // 0x40020000 = I2C0_BASE
     
    //clear I2C FIFOs
    HWREG(0x40020000 + I2C_O_FIFOCTL) = 80008000;
}


//sends an I2C command to the specified slave
void I2CSend(uint8_t slave_addr, uint8_t num_of_args, ...)
{
    // Tell the master module what address it will place on the bus when
    // communicating with the slave.
    I2CMasterSlaveAddrSet(I2C0_BASE, slave_addr, false);
     
    //stores list of variable number of arguments
    va_list vargs;
     
    //specifies the va_list to "open" and the last fixed argument
    //so vargs knows where to start looking
    va_start(vargs, num_of_args);
     
    //put data to be sent into FIFO
    I2CMasterDataPut(I2C0_BASE, va_arg(vargs, uint32_t));
     
    //if there is only one argument, we only need to use the
    //single send I2C function
    if(num_of_args == 1)
    {
        //Initiate send of data from the MCU
        I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_SEND);
         
        // Wait until MCU is done transferring.
        while(I2CMasterBusy(I2C0_BASE));
         
        //"close" variable argument list
        va_end(vargs);
    }
     
    //otherwise, we start transmission of multiple bytes on the
    //I2C bus
    else
    {
        //Initiate send of data from the MCU
        I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_START);
         
        // Wait until MCU is done transferring.
        while(I2CMasterBusy(I2C0_BASE));
         
        //send num_of_args-2 pieces of data, using the
        //BURST_SEND_CONT command of the I2C module
        for(uint8_t i = 1; i < (num_of_args - 1); i++)
        {
            //put next piece of data into I2C FIFO
            I2CMasterDataPut(I2C0_BASE, va_arg(vargs, uint32_t));
            //send next data that was just placed into FIFO
            I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_CONT);
     
            // Wait until MCU is done transferring.
            while(I2CMasterBusy(I2C0_BASE));
        }
     
        //put last piece of data into I2C FIFO
        I2CMasterDataPut(I2C0_BASE, va_arg(vargs, uint32_t));
        //send next data that was just placed into FIFO
        I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);
        // Wait until MCU is done transferring.
        while(I2CMasterBusy(I2C0_BASE));
         
        //"close" variable args list
        va_end(vargs);
    }
}



/* --------- Binding for Micropython ------------ */
/* ---------------------------------------------- */




/*
	Wrapping Function
*/
STATIC mp_obj_t i2c_testfunction() {
	
    // calling I2C-Init function
    InitI2C0();

    // sending test value
    // I2CSend(5, 1, 5);



	return mp_const_none;
}


/*
	Define uPy-Fuction
*/
STATIC MP_DEFINE_CONST_FUN_OBJ_0(i2c_testfunction_obj, i2c_testfunction);


/*
	Table with all globals
*/
STATIC const mp_map_elem_t i2c_globals_table[]= {
	{ MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_i2c) },
	{ MP_OBJ_NEW_QSTR(MP_QSTR_testfunction), (mp_obj_t)&i2c_testfunction_obj },
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
