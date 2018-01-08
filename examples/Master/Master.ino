/*
Mo file src/MatrixKeyToLEDs.h, comment dong 13 -> //"#define USE_RFID"
*/

#include <Wire.h>
#include <MatrixKeyToLEDs.h>


void setup()
{
	Serial.begin(115200);

	MatrixKeyToLEDs.init();
	//Note: tham so co dau (*) la tham so mac dinh

	//Do sang LED [0 -> 15]
	MatrixKeyToLEDs.SetLEDsIntensity(15);
	//Kieu coi buzzer [SUPPLY_VOLTAGE(*) | SUPPLY_FREQ]
	MatrixKeyToLEDs.SetBuzzerType(SUPPLY_VOLTAGE);
	//Muc logic kich buzzer [HIGH(*) | LOW]
	MatrixKeyToLEDs.SetBuzzerActiveLogicLevel(HIGH);
	//Dat ngo ra dieu khien (Chan, phim nhan tuong ung)
	MatrixKeyToLEDs.AddOutput(10, 1);
	MatrixKeyToLEDs.AddOutput(9, 2);
	//Dat phim nhan khoa (Chan, Muc logic [HIGH/LOW], thoi gian tu khoa (giay) [0 = DISABLE])
	MatrixKeyToLEDs.SetLockButton(12, LOW, 10);
	//Dat cong tac khoa (chan, muc logic [HIGH/LOW])
	MatrixKeyToLEDs.SetLockSwitch(11, LOW);
	//Den bao trang thai khoa
	MatrixKeyToLEDs.SetShortLockIndicatorPin(13);



	//Dat kieu nhan cho tat ca phim [CLICK_TOGGLE(*) | PRESS_AND_HOLD]
	MatrixKeyToLEDs.SetKeysToggleStyle(CLICK_TOGGLE);
	//Dat thoi diem phan hoi [ON_CLICK(*) | ON_RELEASE]
	MatrixKeyToLEDs.SetReactMoment(ON_CLICK);
	//Them nhom phim (mang chua cac phim, kich thuoc mang)
	uint8_t sampleGroup[4] = { 0,1,2,3 };
	MatrixKeyToLEDs.AddKeyGroup(sampleGroup, sizeof(sampleGroup));
	//Dat kieu nhan cho 1 phim (phim, kieu nhan [CLICK_TOGGLE | PRESS_AND_HOLD]
	MatrixKeyToLEDs.SetKeyToggleStyle(5, CLICK_TOGGLE);
	//Dat kieu nhan cho nhieu phim (mang,, kich thuoc mang, kieu nhan [CLICK_TOGGLE | PRESS_AND_HOLD]
	uint8_t sampleArray[5] = { 5,6,7,8,9 };
	MatrixKeyToLEDs.SetKeysToggleStyle(sampleArray, sizeof(sampleArray), CLICK_TOGGLE);
	//Dat kieu nhan cho tat ca phim (kieu nhan [CLICK_TOGGLE | PRESS_AND_HOLD]
	MatrixKeyToLEDs.SetKeysToggleStyle(CLICK_TOGGLE);

	//Dat trang thai cho 1 LED (thu tu LED, trang thai [0 | 1])
	MatrixKeyToLEDs.SetLed(15, 1);

	//Lay trang thai phim nhan (phim)
	uint8_t button = MatrixKeyToLEDs.GetButtonState(8);
	//Lay trang thai LED (LED)
	uint8_t led = MatrixKeyToLEDs.GetLEDState(11);
}

void loop()
{
	MatrixKeyToLEDs.Execute();
}