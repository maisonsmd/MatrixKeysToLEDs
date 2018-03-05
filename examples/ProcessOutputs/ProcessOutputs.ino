#include <Wire.h>
#include "MatrixKeyToLEDs.h"

//| vi tri nut | out A | out B | out C | out D |
OutputsProcessor out1(1, 10, 11, 12, 13);
OutputsProcessor out2(2, 15, 16, 17, 18);

void setup()
{
	//Thoi gian nhan giu de kich hoat outputs
	out1.holdDuration = 3000;
	out2.holdDuration = 5000;
}

void loop()
{
	MatrixKeyToLEDs.Execute();
	out1.Execute();
	out2.Execute();
}