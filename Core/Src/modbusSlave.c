/*
 * modbusSlave.c
 *
 *  Created on: Oct 27, 2022
 *
 */
#include "main.h"
#include "modbusSlave.h"
#include "string.h"

extern uint8_t RxData[50];
extern uint8_t TxData[50];
extern UART_HandleTypeDef huart1;
extern Slot_Para_t SlotParameter;
extern Slot15_t Slot;
extern uint8_t SlaveID;
uint8_t errorDataPosition[16],LoadRequestFlag = 0,PickupFlag = 0;
static uint16_t AccupiedData,WrongPickData,WrongPlaceData,PickupSlots;

void sendData (uint8_t *data, int size)
{
	// we will calculate the CRC in this function itself
	uint8_t crc = crc16(data, size);
	data[size] = crc;   // CRC LOW
	data[size+1] = (crc>>8)&0xFF;  // CRC HIGH
	HAL_GPIO_WritePin(RS485_CNTL_GPIO_Port, RS485_CNTL_Pin, GPIO_PIN_SET);
	HAL_UART_Transmit(&huart1, data, size+2, 1000);
	HAL_GPIO_WritePin(RS485_CNTL_GPIO_Port, RS485_CNTL_Pin, GPIO_PIN_RESET);
}

void modbusException (uint8_t exceptioncode)
{
	//| SLAVE_ID | FUNCTION_CODE | Exception code | CRC     |
	//| 1 BYTE   |  1 BYTE       |    1 BYTE      | 2 BYTES |

	TxData[0] = RxData[0];       // slave ID
	TxData[1] = RxData[1]|0x80;  // adding 1 to the MSB of the function code
	TxData[2] = exceptioncode;   // Load the Exception code
	sendData(TxData, 3);         // send Data... CRC will be calculated in the function
}

void updateSlotData(void)
{
	uint16_t StorageSlots = ((RxData[2]<<8)|RxData[3]);
	uint16_t WrongPickup = ((RxData[4]<<8)|RxData[5]);
	uint16_t PresentSlotData = *(uint16_t *)&Slot;
	uint16_t WrongEmptySlot = 0,WrongReelPlaced = 0,WrongPick = 0,EmptySlot = 0;
	int8_t i = 0, errcnt = 0;

	if(LoadRequestFlag == 1){
		PresentSlotData |= WrongPickup;
		PresentSlotData &= ~(WrongPlaceData);
	}

	if(PickupFlag == 1){
		StorageSlots &= ~(WrongPickData);
//		PresentSlotData |= WrongPickup;
//		PresentSlotData &= ~(WrongPlaceData);
	}
	if((RxData[1] == 0x01) && (LoadRequestFlag == 0) && (PickupFlag == 0)){
		WrongPickData = 0x00;
		WrongPlaceData = 0x00;
	}

	uint16_t DataCompare = StorageSlots ^ PresentSlotData;
	UpdateSlotLeds(WrongPickup,PickupError);
	if(DataCompare)
	{
		for(i = 0; i < 16; i++)
		{
			if(DataCompare & (0x01 << i))
				errorDataPosition[errcnt++] = i;
		}
		i = 0;

		while(errcnt > 0)
		{
			if(StorageSlots & (0x01 << errorDataPosition[i]))
				WrongEmptySlot |= (0x01 << errorDataPosition[i]); //Wrong Empty Slot
			else
				WrongReelPlaced |= (0x01 << errorDataPosition[i]); //Wrong material placed
			i++;
			errcnt--;
		}

		if(LoadRequestFlag == 1){
//			WrongReelPlaced  &=~(WrongPlaceData);
			StorageSlots |= WrongReelPlaced;
//			UpdateSlotLeds(WrongEmptySlot,PickupError);
//			WrongPickData |= WrongEmptySlot;
		}
		else
		{
			WrongPlaceData = WrongReelPlaced;
			WrongPlaceData &= ~(WrongPickData);
			UpdateSlotLeds(WrongPlaceData,PlaceError);

		}

		if(PickupFlag == 1){
//			PickupFlag = 0;
			WrongPick = WrongEmptySlot & (StorageSlots ^ PickupSlots);
			WrongPickData |= WrongPick;
			WrongPickData &= ~(WrongPlaceData);
			if(WrongPick == 0)
				UpdateSlotLeds(WrongEmptySlot,EmptyLocaction);
			UpdateSlotLeds((WrongEmptySlot & (StorageSlots ^ PickupSlots)),PickupError);

//			StorageSlots &= ~(WrongEmptySlot & PickupSlots);
//			WrongEmptySlot &= ~(StorageSlots ^ PickupSlots);

		}
		else
		{
//			WrongPlaceData = WrongReelPlaced;
			WrongEmptySlot &= ~(PickupSlots);
			UpdateSlotLeds(WrongEmptySlot,PickupError);
			WrongPickData |= WrongEmptySlot;
			StorageSlots &=~(WrongPickup);
			UpdateSlotLeds(StorageSlots,OcupiedLocation);
		}
//		StorageSlots &= ~(WrongEmptySlot & PickupSlots);
//		StorageSlots &=~(WrongPickup | PickupSlots);
//		UpdateSlotLeds(StorageSlots,OcupiedLocation);
//		StorageSlots |= WrongPlaceData;
		EmptySlot = (EMPTY_SLOT ^ (StorageSlots | DataCompare));
		if(LoadRequestFlag == 1)
			EmptySlot &=~(WrongPickup | WrongPlaceData | WrongPickData);
		else
			EmptySlot &=~(WrongPickup);
		UpdateSlotLeds(EmptySlot,EmptyLocaction);
		TxData[0] = SlaveID;            // slave ID
		if((LoadRequestFlag == 1) && (WrongEmptySlot == 0)){
			LoadRequestFlag = 0;
			TxData[1] = 0x03;				// function code
			TxData[4] = 0x00;            // Error Status
			PresentSlotData |= WrongPickData;
		}
		else if((PickupFlag == 1) &&(WrongPick == 0)){
			PickupFlag = 0;
			TxData[1] = 0x04;				// function code
			TxData[4] = 0x00;            // Error Status
			PresentSlotData |= WrongPickData;

		}
		else{
			TxData[1] = RxData[1];           // function code
			TxData[4] = 0x01;            // Error Status
		}
		TxData[2] = ((PresentSlotData >>8) & 0xFF);                //Higher Slot Data byte
		TxData[3] = PresentSlotData & 0xFF;     					//  Lower Slot Data byte;     // SlotData count
		sendData(TxData, 5);
//		WrongPlaceData = WrongReelPlaced;
//		UpdateSlotLeds(WrongEmptySlot,PickupError);
//		UpdateSlotLeds(WrongReelPlaced,PlaceError);
//		AccupiedData = StorageSlots;
//		AccupiedData &=~(WrongPickup);
//		UpdateSlotLeds(AccupiedData,OcupiedLocation);
//		EmptySlot = AccupiedData ^ (EMPTY_SLOT ^ DataCompare);
//		EmptySlot &=~(WrongPickup);
//		UpdateSlotLeds(EmptySlot,EmptyLocaction);

//		TxData[0] = SlaveID;             // slave ID
//		TxData[1] = RxData[1];           // function code
//		TxData[2] = ((PresentSlotData >>8) & 0xFF);                //Higher Slot Data byte
//		TxData[3] = PresentSlotData & 0xFF;     					//  Lower Slot Data byte
//		if(DataCompare)
//			TxData[4] = 0x01;            // Error Status
//		else
//			TxData[4] = 0x00;            // Error Status
//
//		sendData(TxData, 5);  			 // send data... CRC will be calculated in the function itself
    }
	else{
		AccupiedData = (EMPTY_SLOT ^ PresentSlotData);
		if(LoadRequestFlag == 1)
			AccupiedData &=~(WrongPickup | WrongPlaceData);
		else
			AccupiedData &=~(WrongPickup);
		UpdateSlotLeds(AccupiedData,EmptyLocaction);
		if(PickupFlag == 0){
			StorageSlots &=~(WrongPickup);
			UpdateSlotLeds(StorageSlots,OcupiedLocation);
		}


	}
	LoadRequestFlag = 0;
	PickupFlag = 0;
//	UpdateSlotLeds((EMPTY_SLOT ^ PresentSlotData),EmptyLocaction);

}
/*
 * |       |        	 |			  |			   |    |
 * |SlaveID|Function Code|Storage Data|Error Status|CRC |
 * |       |  		 	 |			  |			   |    |
 */


void lrr(void)                     //Load Reel Request
{
	LoadRequestFlag = 1;
}
//	uint16_t StorageSlots = ((RxData[2]<<8)|RxData[3]);
//	uint16_t PresentSlotData = *(uint16_t *)&Slot;
//	PresentSlotData |= WrongPickData;
//	PresentSlotData	&= ~(WrongPlaceData);
//	uint16_t DataCompare = StorageData ^ PresentSlotData;
//	uint16_t WrongEmptySlot = 0,NewReelPlaced = 0;
//	int8_t i = 0, errcnt = 0;
////	UpdateSlotLeds(StorageSlots,OcupiedLocation);
//	if(DataCompare)
//	{
//		for(i = 0; i < 16; i++)
//		{
//			if(DataCompare & (0x01 << i))
//				errorDataPosition[errcnt++] = i;
//		}
//		i = 0;
//		while(errcnt > 0)
//		{
//			if(StorageData & (0x01 << errorDataPosition[i]))
//				WrongEmptySlot |= (0x01 << errorDataPosition[i]); //Wrong Empty Slot
//			else
//				NewReelPlaced |= (0x01 << errorDataPosition[i]); //New material placed
//			i++;
//			errcnt--;
//		}
//		UpdateSlotLeds(WrongEmptySlot,PickupError);
//		UpdateSlotLeds(NewReelPlaced,OcupiedLocation);
//
//		TxData[0] = SlaveID;            // slave ID
//		TxData[1] = RxData[1];           // function code
//		TxData[2] = ((PresentSlotData >>8) & 0xFF);                //Higher Slot Data byte
//		TxData[3] = PresentSlotData & 0xFF;     					//  Lower Slot Data byte
//		if(WrongEmptySlot)
//			TxData[4] = 0x01;            // Error Status
//		else
//			TxData[4] = 0x00;            // Error Status
//
//		sendData(TxData, 5);  			 // send data... CRC wil
//	}

//}

/*
 * |       |        	 |			  |			  |    |
 * |SlaveID|Function Code|Storage Data|Pickup Data|CRC |
 * |       |  		 	 |			  |			  |    |
 */


void prr(void)                     //Pick Reel Request
{
	PickupSlots  = ((RxData[4]<<8)|RxData[5]);      //Data to pickup from slot
	UpdateSlotLeds(PickupSlots,PickupLocation);
	printf("%d",UpdateSlotLeds);
	PickupFlag = 1;
}
//	uint16_t StorageSlots = ((RxData[2]<<8)|RxData[3]);     //Reel storage in the slot
//	uint16_t PickupSlots  = ((RxData[4]<<8)|RxData[5]);      //Data to pickup from slot
//	uint16_t PresentSlotData = *(uint16_t *)&Slot;
//	PresentSlotData |= WrongPickData;
//	PresentSlotData	&= ~(WrongPlaceData);
//	uint16_t DataCompare = StorageSlots ^ PresentSlotData;
//	uint16_t PickedFromSlot = 0,WrongReelPlaced = 0,WrongPick = 0;
//	int8_t i = 0, errcnt = 0;
//	uint16_t ReservedSlot = (StorageSlots ^ PickupSlots);
//	ReservedSlot &= ~(WrongPickData);
//	UpdateSlotLeds(ReservedSlot,OcupiedLocation);
//	UpdateSlotLeds(PickupSlots,PickupLocation);
//	UpdateSlotLeds(WrongPickData,PickupError);
//	if(DataCompare)
//	{
//		for(i = 0; i < 16; i++)
//		{
//			if(DataCompare & (0x01 << i))
//				errorDataPosition[errcnt++] = i;
//		}
//		i = 0;
//		while(errcnt > 0)
//		{
//			if(StorageSlots & (0x01 << errorDataPosition[i]))
//				PickedFromSlot |= (0x01 << errorDataPosition[i]); //Reel picked From Slot
//			else
//				WrongReelPlaced |= (0x01 << errorDataPosition[i]); //Wrong material placed
//			i++;
//			errcnt--;
//		}
//		WrongPickData = PickedFromSlot & (StorageSlots ^ PickupSlots);
//		UpdateSlotLeds(WrongPickData,PickupError);
////		WrongPickData |= WrongPick;
////		WrongPlaceData = WrongReelPlaced;
//		PickedFromSlot &= ~(StorageSlots ^ PickupSlots);
//		UpdateSlotLeds(PickedFromSlot,EmptyLocaction);
//		UpdateSlotLeds(WrongReelPlaced,PlaceError);
////		UpdateSlotLeds(WrongPick,PickupError);
//		PresentSlotData |= WrongPick;
//		TxData[0] = SlaveID;            // slave ID
//		TxData[1] = RxData[1];           // function code
//		TxData[2] = ((PresentSlotData >>8) & 0xFF);                //Higher Slot Data byte
//		TxData[3] = PresentSlotData & 0xFF;     					//  Lower Slot Data byte
//		if(WrongReelPlaced)
//			TxData[4] = 0x01;            // Error Status
//		else
//			TxData[4] = 0x00;            // Error Status
//		TxData[5] = ((WrongPick >>8) & 0xFF);
//		TxData[6] = WrongPick & 0xFF;
//		sendData(TxData, 7);  			 // send data... CRC wil
//	}


//}

//uint8_t readHoldingRegs (void)
//{
//	uint16_t startAddr = ((RxData[2]<<8)|RxData[3]);  // start Register Address
//
//	uint16_t numRegs = ((RxData[4]<<8)|RxData[5]);   // number to registers master has requested
//	if ((numRegs<1)||(numRegs>125))  // maximum no. of Registers as per the PDF
//	{
//		modbusException (ILLEGAL_DATA_VALUE);  // send an exception
//		return 0;
//	}
//
//	uint16_t endAddr = startAddr+numRegs-1;  // end Register
//	if (endAddr>49)  // end Register can not be more than 49 as we only have record of 50 Registers in total
//	{
//		modbusException(ILLEGAL_DATA_ADDRESS);   // send an exception
//		return 0;
//	}
//
//	// Prepare TxData buffer
//
//	//| SLAVE_ID | FUNCTION_CODE | BYTE COUNT | DATA      | CRC     |
//	//| 1 BYTE   |  1 BYTE       |  1 BYTE    | N*2 BYTES | 2 BYTES |
//
//	TxData[0] = SLAVE_ID;  // slave ID
//	TxData[1] = RxData[1];  // function code
//	TxData[2] = numRegs*2;  // Byte count
//	int indx = 3;  // we need to keep track of how many bytes has been stored in TxData Buffer
//
//	for (int i=0; i<numRegs; i++)   // Load the actual data into TxData buffer
//	{
//		TxData[indx++] = (Holding_Registers_Database[startAddr]>>8)&0xFF;  // extract the higher byte
//		TxData[indx++] = (Holding_Registers_Database[startAddr])&0xFF;   // extract the lower byte
//		startAddr++;  // increment the register address
//	}
//
//	sendData(TxData, indx);  // send data... CRC will be calculated in the function itself
//	return 1;   // success
//}
//
//uint8_t readInputRegs (void)
//{
//	uint16_t startAddr = ((RxData[2]<<8)|RxData[3]);  // start Register Address
//
//	uint16_t numRegs = ((RxData[4]<<8)|RxData[5]);   // number to registers master has requested
//	if ((numRegs<1)||(numRegs>125))  // maximum no. of Registers as per the PDF
//	{
//		modbusException (ILLEGAL_DATA_VALUE);  // send an exception
//		return 0;
//	}
//
//	uint16_t endAddr = startAddr+numRegs-1;  // end Register
//	if (endAddr>49)  // end Register can not be more than 49 as we only have record of 50 Registers in total
//	{
//		modbusException(ILLEGAL_DATA_ADDRESS);   // send an exception
//		return 0;
//	}
//
//	// Prepare TxData buffer
//
//	//| SLAVE_ID | FUNCTION_CODE | BYTE COUNT | DATA      | CRC     |
//	//| 1 BYTE   |  1 BYTE       |  1 BYTE    | N*2 BYTES | 2 BYTES |
//
//	TxData[0] = SLAVE_ID;  // slave ID
//	TxData[1] = RxData[1];  // function code
//	TxData[2] = numRegs*2;  // Byte count
//	int indx = 3;  // we need to keep track of how many bytes has been stored in TxData Buffer
//
//	for (int i=0; i<numRegs; i++)   // Load the actual data into TxData buffer
//	{
//		TxData[indx++] = (Input_Registers_Database[startAddr]>>8)&0xFF;  // extract the higher byte
//		TxData[indx++] = (Input_Registers_Database[startAddr])&0xFF;   // extract the lower byte
//		startAddr++;  // increment the register address
//	}
//
//	sendData(TxData, indx);  // send data... CRC will be calculated in the function itself
//	return 1;   // success
//}
//
//uint8_t readCoils (void)
//{
//	uint16_t startAddr = ((RxData[2]<<8)|RxData[3]);  // start Coil Address
//
//	uint16_t numCoils = ((RxData[4]<<8)|RxData[5]);   // number to coils master has requested
//	if ((numCoils<1)||(numCoils>2000))  // maximum no. of coils as per the PDF
//	{
//		modbusException (ILLEGAL_DATA_VALUE);  // send an exception
//		return 0;
//	}
//
//	uint16_t endAddr = startAddr+numCoils-1;  // Last coils address
//	if (endAddr>199)  // end coil can not be more than 199 as we only have record of 200 (0-199) coils in total
//	{
//		modbusException(ILLEGAL_DATA_ADDRESS);   // send an exception
//		return 0;
//	}
//
//
//	//reset TxData buffer
//	memset (TxData, '\0', 256);
//
//	// Prepare TxData buffer
//
//	//| SLAVE_ID | FUNCTION_CODE | BYTE COUNT | DATA      | CRC     |
//	//| 1 BYTE   |  1 BYTE       |  1 BYTE    | N*2 BYTES | 2 BYTES |
//
//	TxData[0] = SLAVE_ID;  // slave ID
//	TxData[1] = RxData[1];  // function code
//	TxData[2] = (numCoils/8) + ((numCoils%8)>0 ? 1:0);  // Byte count
//	int indx = 3;  // we need to keep track of how many bytes has been stored in TxData Buffer
//
//	/* The approach is simple. We will read 1 bit at a time and store them in the Txdata buffer.
//	 * First find the offset in the first byte we read from, for eg- if the start coil is 13,
//	 * we will read from database[1] with an offset of 5. This bit will be stored in the TxData[0] at 0th position.
//	 * Then we will keep shifting the database[1] to the right and read the bits.
//	 * Once the bitposition has crossed the value 7, we will increment the startbyte
//	 * When the indxposition exceeds 7, we increment the indx variable, so to copy into the next byte of the TxData
//	 * This keeps going until the number of coils required have been copied
//	 */
//	int startByte = startAddr/8;  // which byte we have to start extracting the data from
//	uint16_t bitPosition = startAddr%8;  // The shift position in the first byte
//	int indxPosition = 0;  // The shift position in the current indx of the TxData buffer
//
//	// Load the actual data into TxData buffer
//	for (int i=0; i<numCoils; i++)
//	{
//		TxData[indx] |= ((Coils_Database[startByte] >> bitPosition) &0x01) << indxPosition;
//		indxPosition++; bitPosition++;
//		if (indxPosition>7)  // if the indxposition exceeds 7, we have to copy the data into the next byte position
//		{
//			indxPosition = 0;
//			indx++;
//		}
//		if (bitPosition>7)  // if the bitposition exceeds 7, we have to increment the startbyte
//		{
//			bitPosition=0;
//			startByte++;
//		}
//	}
//
//	if (numCoils%8 != 0)indx++;  // increment the indx variable, only if the numcoils is not a multiple of 8
//	sendData(TxData, indx);  // send data... CRC will be calculated in the function itself
//	return 1;   // success
//}
//
//uint8_t readInputs (void)
//{
//	uint16_t startAddr = ((RxData[2]<<8)|RxData[3]);  // start Register Address
//
//	uint16_t numCoils = ((RxData[4]<<8)|RxData[5]);   // number to coils master has requested
//	if ((numCoils<1)||(numCoils>2000))  // maximum no. of coils as per the PDF
//	{
//		modbusException (ILLEGAL_DATA_VALUE);  // send an exception
//		return 0;
//	}
//
//	uint16_t endAddr = startAddr+numCoils-1;  // Last coils address
//	if (endAddr>199)  // end coil can not be more than 199 as we only have record of 200 (0-199) coils in total
//	{
//		modbusException(ILLEGAL_DATA_ADDRESS);   // send an exception
//		return 0;
//	}
//
//
//	//reset TxData buffer
//	memset (TxData, '\0', 256);
//
//	// Prepare TxData buffer
//
//	//| SLAVE_ID | FUNCTION_CODE | BYTE COUNT | DATA      | CRC     |
//	//| 1 BYTE   |  1 BYTE       |  1 BYTE    | N*2 BYTES | 2 BYTES |
//
//	TxData[0] = SLAVE_ID;  // slave ID
//	TxData[1] = RxData[1];  // function code
//	TxData[2] = (numCoils/8) + ((numCoils%8)>0 ? 1:0);  // Byte count
//	int indx = 3;  // we need to keep track of how many bytes has been stored in TxData Buffer
//
//	/* The approach is simple. We will read 1 bit at a time and store them in the Txdata buffer.
//	 * First find the offset in the first byte we read from, for eg- if the start coil is 13,
//	 * we will read from database[1] with an offset of 5. This bit will be stored in the TxData[0] at 0th position.
//	 * Then we will keep shifting the database[1] to the right and read the bits.
//	 * Once the bitposition has crossed the value 7, we will increment the startbyte
//	 * When the indxposition exceeds 7, we increment the indx variable, so to copy into the next byte of the TxData
//	 * This keeps going until the number of coils required have been copied
//	 */
//	int startByte = startAddr/8;  // which byte we have to start extracting the data from
//	uint16_t bitPosition = startAddr%8;  // The shift position in the first byte
//	int indxPosition = 0;  // The shift position in the current indx of the TxData buffer
//
//	// Load the actual data into TxData buffer
//	for (int i=0; i<numCoils; i++)
//	{
//		TxData[indx] |= ((Inputs_Database[startByte] >> bitPosition) &0x01) << indxPosition;
//		indxPosition++; bitPosition++;
//		if (indxPosition>7)  // if the indxposition exceeds 7, we have to copy the data into the next byte position
//		{
//			indxPosition = 0;
//			indx++;
//		}
//		if (bitPosition>7)  // if the bitposition exceeds 7, we have to increment the startbyte
//		{
//			bitPosition=0;
//			startByte++;
//		}
//	}
//
//	if (numCoils%8 != 0)indx++;  // increment the indx variable, only if the numcoils is not a multiple of 8
//	sendData(TxData, indx);  // send data... CRC will be calculated in the function itself
//	return 1;   // success
//}
//
//uint8_t writeHoldingRegs (void)
//{
//	uint16_t startAddr = ((RxData[2]<<8)|RxData[3]);  // start Register Address
//
//	uint16_t numRegs = ((RxData[4]<<8)|RxData[5]);   // number to registers master has requested
//	if ((numRegs<1)||(numRegs>123))  // maximum no. of Registers as per the PDF
//	{
//		modbusException (ILLEGAL_DATA_VALUE);  // send an exception
//		return 0;
//	}
//
//	uint16_t endAddr = startAddr+numRegs-1;  // end Register
//	if (endAddr>49)  // end Register can not be more than 49 as we only have record of 50 Registers in total
//	{
//		modbusException(ILLEGAL_DATA_ADDRESS);   // send an exception
//		return 0;
//	}
//
//	/* start saving 16 bit data
//	 * Data starts from RxData[7] and we need to combine 2 bytes together
//	 * 16 bit Data = firstByte<<8|secondByte
//	 */
//	int indx = 7;  // we need to keep track of index in RxData
//	for (int i=0; i<numRegs; i++)
//	{
//		Holding_Registers_Database[startAddr++] = (RxData[indx++]<<8)|RxData[indx++];
//	}
//
//	// Prepare Response
//
//	//| SLAVE_ID | FUNCTION_CODE | Start Addr | num of Regs    | CRC     |
//	//| 1 BYTE   |  1 BYTE       |  2 BYTE    | 2 BYTES      | 2 BYTES |
//
//	TxData[0] = SLAVE_ID;    // slave ID
//	TxData[1] = RxData[1];   // function code
//	TxData[2] = RxData[2];   // Start Addr HIGH Byte
//	TxData[3] = RxData[3];   // Start Addr LOW Byte
//	TxData[4] = RxData[4];   // num of Regs HIGH Byte
//	TxData[5] = RxData[5];   // num of Regs LOW Byte
//
//	sendData(TxData, 6);  // send data... CRC will be calculated in the function itself
//	return 1;   // success
//}
//
//uint8_t writeSingleReg (void)
//{
//	uint16_t startAddr = ((RxData[2]<<8)|RxData[3]);  // start Register Address
//
//	if (startAddr>49)  // The Register Address can not be more than 49 as we only have record of 50 Registers in total
//	{
//		modbusException(ILLEGAL_DATA_ADDRESS);   // send an exception
//		return 0;
//	}
//
//	/* Save the 16 bit data
//	 * Data is the combination of 2 bytes, RxData[4] and RxData[5]
//	 */
//
//	Holding_Registers_Database[startAddr] = (RxData[4]<<8)|RxData[5];
//
//	// Prepare Response
//
//	//| SLAVE_ID | FUNCTION_CODE | Start Addr | Data     | CRC     |
//	//| 1 BYTE   |  1 BYTE       |  2 BYTE    | 2 BYTES  | 2 BYTES |
//
//	TxData[0] = SLAVE_ID;    // slave ID
//	TxData[1] = RxData[1];   // function code
//	TxData[2] = RxData[2];   // Start Addr HIGH Byte
//	TxData[3] = RxData[3];   // Start Addr LOW Byte
//	TxData[4] = RxData[4];   // Reg Data HIGH Byte
//	TxData[5] = RxData[5];   // Reg Data LOW  Byte
//
//	sendData(TxData, 6);  // send data... CRC will be calculated in the function itself
//	return 1;   // success
//}
//
//uint8_t writeSingleCoil (void)
//{
//	uint16_t startAddr = ((RxData[2]<<8)|RxData[3]);  // start Coil Address
//
//	if (startAddr>199)  // The Coil Address can not be more than 199 as we only have record of 200 Coils in total
//	{
//		modbusException(ILLEGAL_DATA_ADDRESS);   // send an exception
//		return 0;
//	}
//
//	/* Calculation for the bit in the database, where the modification will be done */
//	int startByte = startAddr/8;  // which byte we have to start writing the data into
//	uint16_t bitPosition = startAddr%8;  // The shift position in the first byte
//
//
//	/* The next 2 bytes in the RxData determines the state of the coil
//	 * A value of FF 00 hex requests the coil to be ON.
//	 * A value of 00 00 requests it to be OFF.
//	 * All other values are illegal and will not affect the coil.
//	 */
//
//	if ((RxData[4] == 0xFF) && (RxData[5] == 0x00))
//	{
//		Coils_Database[startByte] |= 1<<bitPosition; // Replace that bit with 1
//	}
//
//	else if ((RxData[4] == 0x00) && (RxData[5] == 0x00))
//	{
//		Coils_Database[startByte] &= ~(1<<bitPosition); // Replace that bit with 0
//	}
//
//	// Prepare Response
//
//	//| SLAVE_ID | FUNCTION_CODE | Start Addr | Data     | CRC     |
//	//| 1 BYTE   |  1 BYTE       |  2 BYTE    | 2 BYTES  | 2 BYTES |
//
//	TxData[0] = SLAVE_ID;    // slave ID
//	TxData[1] = RxData[1];   // function code
//	TxData[2] = RxData[2];   // Start Addr HIGH Byte
//	TxData[3] = RxData[3];   // Start Addr LOW Byte
//	TxData[4] = RxData[4];   // Coil Data HIGH Byte
//	TxData[5] = RxData[5];   // Coil Data LOW  Byte
//
//	sendData(TxData, 6);  // send data... CRC will be calculated in the function itself
//	return 1;   // success
//}
//
//uint8_t writeMultiCoils (void)
//{
//	uint16_t startAddr = ((RxData[2]<<8)|RxData[3]);  // start Coil Address
//
//	uint16_t numCoils = ((RxData[4]<<8)|RxData[5]);   // number to coils master has requested
//	if ((numCoils<1)||(numCoils>1968))  // maximum no. of coils as per the PDF
//	{
//		modbusException (ILLEGAL_DATA_VALUE);  // send an exception
//		return 0;
//	}
//
//	uint16_t endAddr = startAddr+numCoils-1;  // Last coils address
//	if (endAddr>199)  // end coil can not be more than 199 as we only have record of 200 (0-199) coils in total
//	{
//		modbusException(ILLEGAL_DATA_ADDRESS);   // send an exception
//		return 0;
//	}
//
//	/* Calculation for the bit in the database, where the modification will be done */
//	int startByte = startAddr/8;  // which byte we have to start writing the data into
//	uint16_t bitPosition = startAddr%8;  // The shift position in the first byte
//	int indxPosition = 0;  // The shift position in the current indx of the RxData buffer
//
//	int indx = 7;  // we need to keep track of index in RxData
//
//	/* The approach is simple. We will read 1 bit (starting from the very first bit in the RxData Buffer)
//	 * at a time and store them in the Database.
//	 * First find the offset in the first byte we write into, for eg- if the start coil is 13,
//	 * we will Write into database[1] with an offset of 5. This bit is read from the RxData[indx] at 0th indxposition.
//	 * Then we will keep shifting the RxData[indx] to the right and read the bits.
//	 * Once the bitposition has crossed the value 7, we will increment the startbyte and start modifying the next byte in the database
//	 * When the indxposition exceeds 7, we increment the indx variable, so to copy from the next byte of the RxData
//	 * This keeps going until the number of coils required have been modified
//	 */
//
//	// Modify the bits as per the Byte received
//	for (int i=0; i<numCoils; i++)
//	{
//		if (((RxData[indx]>>indxPosition)&0x01) == 1)
//		{
//			Coils_Database[startByte] |= 1<<bitPosition;  // replace that bit with 1
//		}
//		else
//		{
//			Coils_Database[startByte] &= ~(1<<bitPosition);  // replace that bit with 0
//		}
//
//		bitPosition++; indxPosition++;
//
//		if (indxPosition>7)  // if the indxposition exceeds 7, we have to copy the data into the next byte position
//		{
//			indxPosition = 0;
//			indx++;
//		}
//		if (bitPosition>7)  // if the bitposition exceeds 7, we have to increment the startbyte
//		{
//			bitPosition=0;
//			startByte++;
//		}
//	}
//
//	// Prepare Response
//
//	//| SLAVE_ID | FUNCTION_CODE | Start Addr | Data     | CRC     |
//	//| 1 BYTE   |  1 BYTE       |  2 BYTE    | 2 BYTES  | 2 BYTES |
//
//	TxData[0] = SLAVE_ID;    // slave ID
//	TxData[1] = RxData[1];   // function code
//	TxData[2] = RxData[2];   // Start Addr HIGH Byte
//	TxData[3] = RxData[3];   // Start Addr LOW Byte
//	TxData[4] = RxData[4];   // num of coils HIGH Byte
//	TxData[5] = RxData[5];   // num of coils LOW  Byte
//
//	sendData(TxData, 6);  // send data... CRC will be calculated in the function itself
//	return 1;   // success
//}
