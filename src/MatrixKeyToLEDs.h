// MatrixKeyToLEDs.h

#ifndef _MATRIXKEYTOLEDS_h
#define _MATRIXKEYTOLEDS_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

//Uncomment this line to use external RFID module
#define USE_RFID

#include <SPI.h>
#include <DirectIO.h>
#ifdef USE_RFID
#include "MS_Communication.h"
#endif // USE_RFID


#define LATCH_PIN	53
#define LOAD_PIN	49
#define BUZZER_PIN	48

#define MAX_GROUPS	10
#define MAX_GR_MEMBERS	10

#define LATCH_PORT	PORTB
#define LOAD_PORT	PORTL
#define LATCH_BIT	0
#define LOAD_BIT	0

#define SCAN_INTERVAL_US	4000
#define SCAN_INTERVAL_MS	4

#define ROWS		8		//Max value. If use more, change the algorithm back to 2D array (changed!!)
#define COLUMNS		8

#define ENABLE_LOG

#ifdef ENABLE_LOG
#define BEGIN(x)	Serial.begin(x)
#define LOG(x)		Serial.print(x)
#define LOGF(x)		Serial.print(F(x))
#define LOGLN(x)	Serial.println(x)
#define LOGLNF(x)	Serial.println(F(x))
#else
#define BEGIN(x)
#define LOG(x)
#define LOGF(x)
#define LOGLN(x)
#define LOGLNF(x)
#endif // ENABLE_LOG
#define obj		MatrixKeyToLEDs

//Bit manipulation
#define READ_BIT(BYTE,BIT)		((BYTE >> BIT) & 0x01)
#define SET_BIT(BYTE,BIT)		BYTE |= 0x01 << BIT
#define CLEAR_BIT(BYTE,BIT)		BYTE &= ~(0x01 << BIT)
#define WRITE_BIT(BYTE,BIT,STATE)	BYTE ^= (-STATE ^ BYTE) & (0x01 << BIT)
#define TOGGLE_BIT(BYTE,BIT)		BYTE ^= 0x01 << BIT

#define NO_PIN		255
#define LOCK_INDICATOR_BLINK_TIME	10000

//ButtonLogic
#define	INACTIVE	0x00	//onRelease
#define	ACTIVE		0x01	//onClick

//ToggleStyle
#define CLICK_TOGGLE	0x00
#define PRESS_AND_HOLD	0x01

//ButtonState
#define	RELEASED	INACTIVE
#define HOLDING		ACTIVE

//ReactMoment
#define	ON_CLICK	0x01
#define	ON_RELEASE	0x00

//BuzzerType
#define	SUPPLY_FREQ	0x00
#define	SUPPLY_VOLTAGE	0x01

//Matrix7219Code
#define	OP_NOOP		0
#define	OP_DIGIT0	1
#define	OP_DIGIT1	2
#define	OP_DIGIT2	3
#define	OP_DIGIT3	4
#define	OP_DIGIT4	5
#define	OP_DIGIT5	6
#define	OP_DIGIT6	7
#define	OP_DIGIT7	8
#define	OP_DECODEMODE	9
#define	OP_INTENSITY	10
#define	OP_SCANLIMIT	11
#define	OP_SHUTDOWN	12
#define	OP_DISPLAYTEST	15

enum IndicatorState
{
	OFF,
	ON,
	BLINK
};

enum LockState
{
	SHORT_LOCKED,
	LOCKED,
	UNLOCKED,
};

enum LoginState
{
	LOGGED_IN,
	LOGGED_OUT
};

struct OutputsStruct
{
	uint8_t keyIndex;
	uint8_t outputPin;
};


class MatrixKeyToLEDsClass
{
protected:
	Output<LATCH_PIN> LATCH;
	Output<LOAD_PIN> LOAD;

	uint8_t pShiftInData;
	uint8_t pShiftOutData595[1];
	uint8_t pShiftOutData7219[2];
	uint8_t pStatusByte[ROWS];

	uint8_t pButtonStateArray[ROWS][COLUMNS];
	uint32_t pButtonTimerArray[ROWS][COLUMNS];
	uint8_t pLEDStateArray[ROWS][COLUMNS];

	uint8_t pCurrentButtonColumnIndex = 0;
	uint8_t pCurrentButtonRowIndex = 0;
	uint8_t pDebounceInterval = 75;		//Equal to computer keyboard
	uint16_t pToneFrequency = 1244;
	uint32_t pToneDuration = 30;
	uint8_t pBuzzerActiveLogicLevel = HIGH;

	uint8_t pBuzzerType = SUPPLY_VOLTAGE;
	boolean pIsBuzzerOn = false;
	uint32_t pBuzzerTimer = 0;

	uint8_t pToggleStyle[ROWS][COLUMNS];
	uint8_t pReactMoment = ON_CLICK;

	uint32_t pShortLockAutoEnableCounter = 0;
	IndicatorState pShortLockIndicatorState = OFF;


	IndicatorState pLockIndicatorState = OFF;
	uint8_t pLockButtonActiveLogicLevel = LOW;
	uint32_t pLockDelayTime;
	uint32_t pLockButtonHoldTime = 1000;
	uint8_t pLockIndicatorPin = NO_PIN;

	uint32_t pLockButtonPressTime = 0;
	uint32_t pLockAutoEnableCounter = 0;
#ifdef USE_RFID
	//hold the lock state
	LockState pLockState = LOCKED;
#else
	//hold the lock state
	LockState pLockState = SHORT_LOCKED;
	//Use Lock Switch to lock keypad
	uint8_t pLockSwitchPin = NO_PIN;
	uint8_t pLockSwitchActiveLogicLevel = LOW;

#endif // !USE_RFID

	//Hold the login state, seperate with the lock state above
public:	//not good!
	LoginState pLoginState = LOGGED_OUT;
private:

	uint8_t pShortLockButtonPin = NO_PIN;
	uint8_t pShortLockButtonPinActiveLogicLevel = LOW;
	uint8_t pShortLockIndicatorPin = NO_PIN;
	uint32_t pShortLockDelayTime;



	//uint8_t pTempOutputPin = NO_PIN;
	//uint8_t pTempOutputKeyIndex;
	//uint32_t pTempOutputInitTmr = 0;
	//uint32_t pTempOutputIntervalTmr = 0;
	//uint32_t pTempOutputInitTime = 0;	//wait to ON
	//uint32_t pTempOutputInterval = 100;	//wait to OFF
	//boolean isWaitingToTurnTempOutputOn = false;

	uint8_t pNumberOfOutputs = 0;
	OutputsStruct * pOutputArray = nullptr;

	struct KeyGroup {
		volatile uint8_t size = 0;
		volatile uint8_t member[MAX_GR_MEMBERS];
	} pKeyGroup[MAX_GROUPS];
	uint8_t pKeyGroupsCount = 0;				//Number of groups
	/*uint8_t pKeyGroupMembersCount[MAX_GROUPS];		//Size of each group
	uint8_t pKeyGroup[MAX_GROUPS][MAX_GR_MEMBERS];		//Members*/

	boolean pScanGroup(uint8_t row, uint8_t column, uint8_t state, uint8_t toggleStyle);
	void pReact(uint8_t row, uint8_t column, uint8_t state);
	void pShiftOut(void);
	void pShiftIn(void);
	void pTransfer7219(uint8_t opcode, uint8_t data);

	uint8_t toIndex(uint8_t row, uint8_t col);
	void toRowCol(uint8_t index, uint8_t * row, uint8_t * col);
	//void pHandleTempOutput(void);
	//void pFlashTempOutput(uint8_t keyIndex);
	void pHandleOutputs(void);

	void pResetAutoLockTimers(void);
	void pHandleShortLockButton(void);
	void pHandleLockStateIndicator(void);
#ifdef USE_RFID
	void pHandleLockButton(void);
#else
	void pHandleLockSwitch(void);
#endif // USE_RFID
	boolean pIsLockSwitchActivated(void);

public:
	static void Execute(void);
	void init(void);
	void Beep(void);
	void SetLEDsIntensity(uint8_t intensity);
	void ClearAll(void);
	void SetAll(void);
	void SetLed(uint8_t row, uint8_t column, uint8_t state);
	void SetLed(uint8_t index, uint8_t state);

	void SetToneStyle(uint16_t freq, uint32_t duration);

	void SetKeyToggleStyle(uint8_t row, uint8_t column, uint8_t toggleStyle);
	void SetKeyToggleStyle(uint8_t index, uint8_t toggleStyle);
	void SetRowToggleStyle(uint8_t row, uint8_t toggleStyle);
	void SetColumnToggleStyle(uint8_t column, uint8_t toggleStyle);
	void SetKeysToggleStyle(uint8_t toggleStyle);
	void SetKeysToggleStyle(uint8_t * key, uint8_t size, uint8_t toggleStyle);

	void SetReactMoment(uint8_t reactMoment);
	void SetDebounceInterval(uint32_t ms);
	void SetBuzzerType(uint8_t buzzerType);
	void SetBuzzerActiveLogicLevel(uint8_t level);

	void AddKeyGroup(uint8_t * btnArray, uint8_t size);

	void ShortLock(void);
	void Unlock(void);
	void Lock(void);

	boolean IsLocked(void);
	LoginState GetLoginState(void);
	LockState GetLockState(void);

	void SetLockButton(uint8_t pin, uint8_t level, uint32_t shortLockDelayTime, uint32_t lockDelayTime = 0);

	void SetShortLockIndicatorPin(uint8_t pin);
	//void SetTempOutput(uint8_t pin, uint8_t keyIndex, uint32_t initTime, uint32_t interval = 100);
	//void SetTempOutput(uint8_t pin, uint8_t keyIndex);
	void AddOutput(uint8_t pin, uint8_t keyIndex);

#ifdef USE_RFID
	//Set the period to completely lock the key pad after the short lock event
	void SetLockIndicatorPin(uint8_t pin);
#else
	void SetLockSwitch(uint8_t pin, uint8_t level);
#endif // USE_RFID


	void GetButtonsState(uint8_t(*buttonArray)[ROWS][COLUMNS]);
	void GetButtonsState(uint8_t(*buttonArray)[ROWS*COLUMNS]);
	uint8_t GetButtonState(uint8_t buttonNumber);
	uint8_t GetButtonState(uint8_t row, uint8_t column);

	void GetLEDsState(uint8_t(*LEDArray)[ROWS][COLUMNS]);
	void GetLEDsState(uint8_t(*LEDArray)[ROWS*COLUMNS]);
	uint8_t GetLEDState(uint8_t LEDNumber);
	uint8_t GetLEDState(uint8_t row, uint8_t column);

#pragma region ADDED_FEATURES

#pragma endregion

};

extern MatrixKeyToLEDsClass MatrixKeyToLEDs;

class OutputsProcessor
{
private:
	uint32_t clickTick = 0;		//click moment
	uint32_t startTick = 0;		//click accept moment
	uint32_t startBTick = 0;		//Turning B on moment
	boolean started = false;		//time counting started?
	boolean isOutputBStarted = false;//time B counting started?
	uint8_t previousButtonState = 0;
	boolean isHolding = false;	//is button being held down?
	uint8_t currentButtonState = 0;
public:
	uint32_t t1 = 1000;	//ms, time to trigger output B
	uint32_t t2 = 2000;	//ms, time to trigger output C
	uint32_t t3 = 3000;	//ms, time to trigger output D
	uint32_t t_on_B = 500;	//0.5s, time to hold output B state
	uint8_t buttonIndex, outputApin, outputBpin, outputCpin, outputDpin;

	uint32_t holdDuration = 3000;
	uint8_t ACTIVE_LOGIC_LEVEL = HIGH;
	OutputsProcessor(const uint8_t _buttonIndex, const uint8_t _outputApin, const uint8_t _outputBpin
		= NO_PIN, const uint8_t _outputCpin = NO_PIN, const uint8_t _outputDpin = NO_PIN)
		:buttonIndex(_buttonIndex),
		outputApin(_outputApin),
		outputBpin(_outputBpin),
		outputCpin(_outputCpin),
		outputDpin(_outputDpin)
	{
		pinMode(outputApin, OUTPUT);
		digitalWrite(outputApin, !ACTIVE_LOGIC_LEVEL);
		if (outputBpin != NO_PIN)
		{
			pinMode(outputBpin, OUTPUT);
			digitalWrite(outputBpin, !ACTIVE_LOGIC_LEVEL);
		}
		if (outputCpin != NO_PIN)
		{
			pinMode(outputCpin, OUTPUT);
			digitalWrite(outputCpin, !ACTIVE_LOGIC_LEVEL);
		}
		if (outputDpin != NO_PIN)
		{
			pinMode(outputDpin, OUTPUT);
			digitalWrite(outputDpin, !ACTIVE_LOGIC_LEVEL);
		}
	}

	void Execute()
	{
		currentButtonState = MatrixKeyToLEDs.GetButtonState(buttonIndex);
		currentLEDState = MatrixKeyToLEDs.GetLEDState(buttonIndex);
		
		uint32_t currentTick = millis();
		//check if variables overflowed

		if (currentTick < clickTick)
			clickTick = currentTick;

		if (currentTick < startBTick)
			startBTick = currentTick;


		//Exit if locked
		if (MatrixKeyToLEDs.GetLockState() != LockState::UNLOCKED)
			return;

		if (currentButtonState == 1 && previousButtonState == 0)	//just clicked
		{
			isHolding = true;
			clickTick = currentTick;
		}
		if (currentButtonState == 0 && previousButtonState == 1)	//just released
		{
			isHolding = false;
		}
		previousButtonState = currentButtonState;	//update previous button state

		if (isHolding)
		{
			if (currentTick - clickTick > holdDuration)	// click acepted
			{
				isHolding = false;		//to avoid entering this section in the next function call
				if (started)
				{
					started = false;
					isOutputBStarted = false;
					digitalWrite(outputApin, !ACTIVE_LOGIC_LEVEL);
					digitalWrite(outputBpin, !ACTIVE_LOGIC_LEVEL);
					if (outputCpin != 255)
						digitalWrite(outputCpin, !ACTIVE_LOGIC_LEVEL);
					if (outputDpin != 255)
						digitalWrite(outputDpin, !ACTIVE_LOGIC_LEVEL);
				}
				else
				{
					//ON COMMAND
					startTick = currentTick;
					started = true;
					digitalWrite(outputApin, ACTIVE_LOGIC_LEVEL);
				}
			}
		}
		
		if (started)
		{
			if (currentTick - startTick > t1 && isOutputBStarted == false)
			{
				isOutputBStarted = true;
				startBTick = currentTick;
				digitalWrite(outputBpin, ACTIVE_LOGIC_LEVEL);
			}
			if (currentTick - startBTick > t_on_B && isOutputBStarted == true)
			{
				digitalWrite(outputBpin, !ACTIVE_LOGIC_LEVEL);
			}
			if (currentTick - startTick > t2)
			{
				if (outputCpin != 255)
					digitalWrite(outputCpin, ACTIVE_LOGIC_LEVEL);
			}
			if (currentTick - startTick > t3)
			{
				if (outputDpin != 255)
					digitalWrite(outputDpin, ACTIVE_LOGIC_LEVEL);
			}
		}
	}
};
#endif

