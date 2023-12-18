#include "main.h"

#define number_of_74hc595s 6
#define numOfRegisterPins number_of_74hc595s * 8
#define numRGBLeds 7


uint8_t registers[45];
LedColour_t LedColour;
Slot_Para_t SlotParameter;
extern uint8_t SlotParam[5];

uint8_t redPin[]   =  {0, 3, 6, 9, 12, 15, 18, 21, 24, 27, 30, 33, 36, 39, 42}; //Red LED connected Pins
//uint8_t greenPin[]  = {1, 4, 7, 10,13, 16, 19, 22, 25, 28, 31, 34, 37, 40, 43};  //Green LED connected Pins
//uint8_t bluePin[] =   {2, 5, 8, 11,14, 17, 20, 23, 26, 29, 32, 35, 38, 41, 44};//Blue LED connected Pins
uint8_t bluePin[]  = {1, 4, 7, 10,13, 16, 19, 22, 25, 28, 31, 34, 37, 40, 43};  //Green LED connected Pins New Board
uint8_t greenPin[] =   {2, 5, 8, 11,14, 17, 20, 23, 26, 29, 32, 35, 38, 41, 44};//Blue LED connected Pins New Board
void RGBInit(void)
{

  for (int i = 45 - 1; i >=  0; i--) {
    registers[i] = 0xFF;
  }

}

void registersWrite(int index, int value) {
  HAL_GPIO_WritePin(RGB_CS_GPIO_Port, RGB_CS_Pin, GPIO_PIN_RESET);
  for (int i = 45 - 1; i >=  0; i--) {
    HAL_GPIO_WritePin(BB_CLK_GPIO_Port, BB_CLK_Pin, GPIO_PIN_RESET);
    int val = registers[i];
    HAL_GPIO_WritePin(BB_MOSI_GPIO_Port, BB_MOSI_Pin, val);
    HAL_GPIO_WritePin(BB_CLK_GPIO_Port, BB_CLK_Pin, GPIO_PIN_SET);
  }
  HAL_GPIO_WritePin(RGB_CS_GPIO_Port, RGB_CS_Pin, GPIO_PIN_SET);
  registers[index] = value;
}

void Red(int startPin, int endPin, int ledBT) {
  for (uint8_t count = startPin; count <= endPin; count++) {
    registersWrite(bluePin[count], 0x01);     //Green
    registersWrite(greenPin[count], 0x01);    //Blue
    registersWrite(redPin[count], 0x00);
  }
}
void Green(int startPin, int endPin, int ledBT) {
  for (uint8_t count = startPin; count <= endPin; count++) {
   registersWrite(redPin[count], 0x01);
	registersWrite(bluePin[count], 0x01);     //Green
	registersWrite(greenPin[count], 0x00);    //Blue
//    delay(ledBT);
  }
}

void Blue(int startPin, int endPin, int ledBT) {
  for (uint8_t count = startPin; count <= endPin; count++) {
	registersWrite(bluePin[count], 0x00);     //Green
	registersWrite(redPin[count], 0x01);
	registersWrite(greenPin[count], 0x01);    //Blue
//    delay(ledBT);
  }
}

void Yellow(int startPin, int endPin, int ledBT) {
  for (uint8_t count = startPin; count <= endPin; count++) {
	registersWrite(bluePin[count], 0x01);     //Green
	registersWrite(redPin[count], 0x00);
	registersWrite(greenPin[count], 0x00);    //Blue
//    delay(ledBT);
  }
}

void White(int startPin, int endPin, int ledBT) {
  for (uint8_t count = startPin; count <= endPin; count++) {
	registersWrite(bluePin[count], 0x00);     //Green
	registersWrite(redPin[count], 0x00);
	registersWrite(greenPin[count], 0x00);    //Blue
//    delay(ledBT);
  }
}

void Cyan(int startPin, int endPin, int ledBT) {
  for (uint8_t count = startPin; count <= endPin; count++) {
	registersWrite(redPin[count], 0x01);
	registersWrite(greenPin[count], 0x00);    //Blue
	registersWrite(bluePin[count], 0x00);     //Green
//    delay(ledBT);
  }
}

void Pink(int startPin, int endPin, int ledBT) {
  for (uint8_t count = startPin; count <= endPin; count++) {

	registersWrite(redPin[count], 0x00);
	registersWrite(bluePin[count], 0x00);     //Green
	registersWrite(greenPin[count], 0x01);    //Blue
//    delay(ledBT);
  }
}
void LedOff(int startPin, int endPin, int ledBT) {
  for (uint8_t count = startPin; count <= endPin; count++) {

	registersWrite(redPin[count], 0x01);
	registersWrite(bluePin[count], 0x01);     //Green
	registersWrite(greenPin[count], 0x01);    //Blue
//    delay(ledBT);
  }
}
void AllLedOff(int clearSpeed) {
  for (int i = 0; i <= 44; i++) {
    registersWrite(i, 0xFF);
    delay(clearSpeed);
  }
}

 void SetLed(uint8_t Slotnum,LedColour_t Colour)
{
	switch(Colour)
	{
	case RGBLedOff:
					LedOff(Slotnum, Slotnum,0x00);
					LedOff(Slotnum, Slotnum,0x00);
					break;
	case RGBRed:
					Red(Slotnum, Slotnum,0x00);
					Red(Slotnum, Slotnum,0x00);
					break;
	case RGBGreen:
					Green(Slotnum, Slotnum,0x00);
					Green(Slotnum, Slotnum,0x00);
					break;
	case RGBBlue:
					Blue(Slotnum, Slotnum,0x00);
					break;
	case RGBYellow:
					Yellow(Slotnum, Slotnum,0x00);
					Yellow(Slotnum, Slotnum,0x00);
					break;
	case RGBPink:
					Pink(Slotnum, Slotnum,0x00);
					Pink(Slotnum, Slotnum,0x00);
					break;

	case RGBCyan:
					Cyan(Slotnum, Slotnum,0x00);
					Cyan(Slotnum, Slotnum,0x00);
					break;
	case RGBWhite:
					White(Slotnum, Slotnum,0x00);
					White(Slotnum, Slotnum,0x00);
					break;
	default:
					break;
	}
}

void UpdateSlotLeds(uint16_t Data, Slot_Para_t status)
{
	int8_t i = 0;
	for(i = 0; i< 15; i++)
	{
		if(Data & (0x4000 >> i))
			SetLed(i,SlotParam[status]);
	}
}


