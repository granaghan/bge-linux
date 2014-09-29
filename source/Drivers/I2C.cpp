#include "Drivers/I2C.h"

uint8_t* const I2C::I2C0BaseAddress = (uint8_t* const)  0x20205000;
uint8_t* const I2C::I2C1BaseAddress = (uint8_t* const)  0x20804000;
uint8_t* const I2C::I2C2BaseAddress = (uint8_t* const)  0x20805000;

I2C::I2C(uint8_t* baseAddress):
   baseAddress(baseAddress)
{}

I2C::~I2C(){}

void I2C::enable()
{
   writeI2CCTL_I2CEN(baseAddress, 1);
}

void I2C::disable()
{
   writeI2CCTL_I2CEN(baseAddress, 0);
}

void I2C::startTransfer()
{
	writeI2CCTL_ST(baseAddress, 1);
}

void I2C::clearFIFO()
{
	writeI2CCTL_CLEAR(baseAddress, 1);
}

void I2C::setTransferMode(TransferMode transferMode)
{
	if(transferMode == transferModeRead)
	{
		writeI2CCTL_READ(baseAddress, 1);
	}
	else if(transferMode == transferModeWrite)
	{
		writeI2CCTL_READ(baseAddress, 0);
	}
}

bool I2C::checkForError()
{
	return readI2CSTATUS_CLKT(baseAddress) || readI2CSTATUS_ERR(baseAddress);
}

bool I2C::ackErrorDetected()
{
	bool detected = readI2CSTATUS_ERR(baseAddress);
	writeI2CSTATUS_ERR(baseAddress, 1);
	return detected;

}

bool I2C::clockStretchTimeoutDetected()
{
	bool detected = readI2CSTATUS_CLKT(baseAddress);
	writeI2CSTATUS_CLKT(baseAddress, 1);
	return detected;
}

bool I2C::txFIFOEmpty()
{
	return readI2CSTATUS_TXE(baseAddress);
}

bool I2C::dataAvailable()
{
	return readI2CSTATUS_RXD(baseAddress);
}

bool I2C::transferDone()
{
	return readI2CSTATUS_DONE(baseAddress);
}

bool I2C::transferActive()
{
	return readI2CSTATUS_TA(baseAddress);
}

void I2C::setDataLength(uint16_t dataLength)
{
	writeI2CDLEN_DLEN(baseAddress, dataLength);
}

void I2C::setAddress(uint8_t address)
{
	writeI2CADDR_ADDR(baseAddress, address);
}

void I2C::setClockDivider(uint16_t clockDivider)
{
	writeI2CDIV_CDIV(baseAddress, clockDivider);
}

void I2C::setClockStretchTimeout(uint16_t timeoutLengthInClockCycles)
{
	writeI2CCLKT_TOUT(baseAddress, timeoutLengthInClockCycles);
}

void I2C::write(uint8_t data)
{
	writeI2CFIFO_DATA(baseAddress, data);
}

uint8_t I2C::read()
{
	return readI2CFIFO_DATA(baseAddress);
}
