/*
 * ILI9341.cpp
 *
 *  Created on: Oct 26, 2020
 *      Author: mikol
 */

#include "stm32f4xx_hal.h"
#include <math.h>

/* --------------------------COLOR---------------------*/
class Color{
	public:
		Color();
		void SetColor (uint8_t Red, uint8_t Green, uint8_t Blue);
		uint8_t GetRed();
		uint8_t GetGreen();
		uint8_t GetBlue();
	private:
		uint8_t itsRed = 0;
		uint8_t itsGreen = 0;
		uint8_t itsBlue = 0;
};

Color::Color(){
}

void Color::SetColor(uint8_t R, uint8_t G, uint8_t B){
	itsRed = R;
	itsGreen = G;
	itsBlue = B;
}

uint8_t Color::GetRed(){
	return itsRed;
}

uint8_t Color::GetGreen(){
	return itsGreen;
}

uint8_t Color::GetBlue(){
	return itsBlue;
}

/* ----------------------------Obsługa wyświetlacza low level --------------------------------*/

#define READ 0x01
#define WRITE 0x00
#define COMMAND 0x00
#define DATA 0x01



//Commands
#define REG_READ_ID 0x04
#define REG_SOFT_RESET 0x01
#define REG_SLEEP_OUT 0x11
#define REG_MEMORY_WRITE 0x2C
#define REG_MEMORY_WRITE_CONTINUE 0x3C
#define REG_DISPLAY_ON 0x29
#define REG_COLUMN_ADDRESS_SET 0x2A
#define REG_PAGE_ADDRESS_SET 0x2B
#define REG_VERTICAL_SCROLLING_START_ADDRESS 0x37
#define REG_VERTICAL_SCROLLING_DEFINITION 0x33

class ILI9341 {
	public:
	/* CSX pin */
	uint16_t csx_pin;
	GPIO_TypeDef * csx_port;

	/* RDX pin */
	uint16_t rdx_pin;
	GPIO_TypeDef * rdx_port;

	/* DCX pin */
	uint16_t dcx_pin;
	GPIO_TypeDef * dcx_port;

	/* SPI */
	SPI_HandleTypeDef * spi;

	void Read(uint8_t command, uint8_t n, uint8_t * data);
	void SendData(Color Kolor, uint16_t n);

	void Shift_Frame(uint16_t y);
	void Write(uint8_t command,uint8_t n,uint8_t * data);
	void Reset();
	void PowerON();

	void SetDrawArea(uint16_t xs, uint16_t ys, uint16_t xe, uint16_t ye);
	void DrawPixel(Color Kolor);

	void SetBackgroundColor(Color);
	void SetBackgroundColor(uint8_t red, uint8_t green, uint8_t blue);
	Color GetBackgroundColor();

//	void Draw(Rectangle Rectangle);
//
//	void Erase(Pixel &Pixel, Color Kolor);
//	void Erase(Line &Line, Color Kolor);

	private:

		void Enable();
		void Disable();
		void Synchro();

		void CSX_ON();
		void CSX_OFF();
		void RDX(uint8_t RW);
		void DCX(uint8_t CD);

		void SetX(uint16_t SP, uint16_t EP);
		void SetY(uint16_t SP, uint16_t EP);

		Color itsBackgroundColor;
};

//void ILI9341::Draw(Rectangle BG){
//
//	Line tempLine;
//	tempLine.Kolor=BG.Kolor;
//
//	for(int x = BG.LeftUp.GetX(); x<BG.RightDown.GetX();++x)
//	{
//		tempLine.SetA(x,BG.LeftUp.GetY());
//		tempLine.SetB(x,BG.RightDown.GetY());
//		Draw(tempLine);
//	}
//}

void ILI9341::CSX_ON(){
	HAL_GPIO_WritePin(csx_port, csx_pin, GPIO_PIN_RESET);
}

void ILI9341::CSX_OFF(){
	HAL_GPIO_WritePin(csx_port, csx_pin, GPIO_PIN_SET);
}

void ILI9341::RDX(uint8_t RW){
	if(RW==READ) HAL_GPIO_WritePin(rdx_port, rdx_pin, GPIO_PIN_SET);
	else HAL_GPIO_WritePin(rdx_port, rdx_pin, GPIO_PIN_RESET);
}

void ILI9341::DCX(uint8_t CD){
	if(CD==DATA) HAL_GPIO_WritePin(dcx_port, dcx_pin, GPIO_PIN_SET);
	else HAL_GPIO_WritePin(dcx_port, dcx_pin, GPIO_PIN_RESET);
}

void ILI9341::PowerON(){
	Write(REG_DISPLAY_ON, 0, 0);
}

void ILI9341::Reset(){
	uint8_t reg_com = REG_SOFT_RESET;
	DCX(COMMAND);
	CSX_ON();

	HAL_SPI_Transmit(spi, &reg_com, 1, 100);
	CSX_OFF();
	HAL_Delay(200);
	CSX_ON();
	reg_com = REG_SLEEP_OUT;
	HAL_SPI_Transmit(spi, &reg_com , 1, 100);
	CSX_OFF();
	HAL_Delay(200);
}

void ILI9341::Read(uint8_t command,uint8_t n,uint8_t * data){

	DCX(COMMAND);
	CSX_ON();

	//Send Command
	HAL_SPI_Transmit(spi, &command, 1, 100);

	//Receive n bytes
	DCX(DATA);
	if(n!=0) HAL_SPI_Receive(spi, data, n, 100);

	CSX_OFF();

}

void ILI9341::Write(uint8_t command,uint8_t n,uint8_t * data){

	DCX(COMMAND);
	CSX_ON();

	//Send Command
	HAL_SPI_Transmit(spi, &command, 1, 100);

	DCX(DATA);
	//Receive n bytes
	if(n!=0) 	HAL_SPI_Transmit(spi, data, n, 100);

	CSX_OFF();

}

void ILI9341::SetX(uint16_t sp, uint16_t ep){

	uint8_t data[4];
	data[0] = (sp >> 8) & 0x00ff;
	data[1] = sp & 0x00ff;
	data[2] = (ep >> 8) & 0x00ff;
	data[3] = (ep & 0x00ff);

	Write(REG_PAGE_ADDRESS_SET, 4, data);

}

void ILI9341::SetY(uint16_t sp, uint16_t ep){

	uint8_t data[4];
	data[0] = (sp >> 8) & 0x00ff;
	data[1] = sp & 0x00ff;
	data[2] = (ep >> 8) & 0x00ff;
	data[3] = (ep & 0x00ff);

	Write(REG_COLUMN_ADDRESS_SET, 4, data);
}

void ILI9341::SetDrawArea(uint16_t xs, uint16_t ys, uint16_t xe, uint16_t ye){
	SetX(xs, xe);
	SetY(ys, ye);
	Write(REG_MEMORY_WRITE, 0, 0);
}

void ILI9341::Shift_Frame(uint16_t y){

	uint8_t data[6];

	data[0] = 0;
	data[1] = 0;
	data[2] = 255;
	data[3] = 255;
	data[4] = 255;
	data[5] = 255;

	Write(REG_VERTICAL_SCROLLING_DEFINITION, 6, data);
	Write(REG_MEMORY_WRITE,0,0);

	data[0] = (y >> 8) & 0x00ff;
	data[1] = y & 0x00ff;

	Write(REG_VERTICAL_SCROLLING_START_ADDRESS, 2, data);
	Write(REG_MEMORY_WRITE,0,0);

}

void ILI9341::DrawPixel(Color Kolor){

	/* For corrent Pixel Draw first set draw area using SetDrawArea class */

	Write(REG_MEMORY_WRITE_CONTINUE, 0, 0);

	uint8_t data[3];

	data[2]= (Kolor.GetRed()>>1)|0x80;
	data[1]= (Kolor.GetGreen()>>1)|0x80;
	data[0]= (Kolor.GetBlue()>>1)|0x80;

	DCX(DATA);
	CSX_ON();

	HAL_SPI_Transmit(spi, data, 3, 100);

	CSX_OFF();
}

void ILI9341::SetBackgroundColor(Color Kolor){

	itsBackgroundColor = Kolor;

	SetDrawArea(0, 0, 320, 240);
	for(int i=0; i<320*240;i++)
	{
		DrawPixel(itsBackgroundColor);
	}
}

void ILI9341::SetBackgroundColor(uint8_t red, uint8_t green, uint8_t blue)
{
	itsBackgroundColor.SetColor(red, green, blue);
	SetBackgroundColor(itsBackgroundColor);
}

Color ILI9341::GetBackgroundColor(){
	return itsBackgroundColor;
}

/* Obsługa wyświetlacza high level */


/*-----------------------------------------------------------------------------------------------------------------*/
/* ---------------------------------------------------- PIXEL -----------------------------------------------------*/
/* ----------------------------------------------------------------------------------------------------------------*/
class Shape{
	public:
	Color Kolor;
	virtual void Draw(ILI9341);
	virtual void Erase(ILI9341);
//		virtual void Erase();

};

void Shape::Draw(ILI9341){

}

void Shape::Erase(ILI9341){

}

class Pixel : public Shape {
	public:
		Pixel();
		void SetX(float x);
		void SetY(float y);
		void Set(float x, float y);
		float GetX();
		float GetY();

		void Draw(ILI9341);
		void Erase(ILI9341);

		Pixel operator+ (Pixel &);
		Pixel operator- (Pixel &);

	protected:

		float itsX;
		float itsY;
};

Pixel::Pixel(){
	itsX =0;
	itsY =0;
		}


Pixel Pixel::operator+ (Pixel & rhs){
	Pixel tempPoint;
	tempPoint.SetX(itsX + rhs.GetX());
	tempPoint.SetY(itsY + rhs.GetY());

	return tempPoint;
}

Pixel Pixel::operator- (Pixel & rhs){
	Pixel tempPoint;
	tempPoint.SetX(itsX - rhs.GetX());
	tempPoint.SetY(itsY - rhs.GetY());

	return tempPoint;
}

void Pixel::Set(float x, float y){
	itsX = x;
	itsY = y;
}

void Pixel::SetX(float x){
	itsX = x;
}
void Pixel::SetY(float y){
	itsY = y;
}

float Pixel::GetX(){
	return itsX;
}

float Pixel::GetY(){
	return itsY;
}

void Pixel::Draw(ILI9341 LCD){

	if(GetX()<320 && GetY()<240){
			LCD.SetDrawArea(GetX(), GetY(), GetX()+1, GetY()+1);
			LCD.DrawPixel(Kolor);
		}
}

void Pixel::Erase(ILI9341 LCD){
	if(GetX()<320 && GetY()<240){
			LCD.SetDrawArea(GetX(), GetY(), GetX()+1, GetY()+1);
			LCD.DrawPixel(LCD.GetBackgroundColor());
		}
}


/*-----------------------------------------------------------------------------------------------------------------*/
/* ---------------------------------------------------- LINE ------------------------------------------------------*/
/* ----------------------------------------------------------------------------------------------------------------*/

class Line : public Shape{
	public:
		void ChangeDir();
		void SetSize(uint8_t size);
		uint8_t GetSize();

		void SetA(Pixel A);
		void SetA(float Ax, float Ay);
		void SetB(Pixel B);
		void SetB(float Bx, float By);

		Pixel GetA();
		Pixel GetB();
		float GetL();
		float GetAlpha();
		void Rotate(double angle, Pixel axis);

		void Draw(ILI9341);
		void Erase(ILI9341);

		Line operator+ (Line &);
		Line operator- (Line &);
	protected:

		uint8_t itsSize = 1;
		Pixel itsA;
		Pixel itsB;
};

Line Line::operator+(Line & rhs){
	Line tempVector;
	tempVector.SetA(itsA + rhs.itsA);
	tempVector.SetB(itsB + rhs.itsB);

	return tempVector;
}

Line Line::operator-(Line & rhs){
	Line tempVector;
	tempVector.SetA(itsA - rhs.itsA);
	tempVector.SetB(itsB - rhs.itsB);

	return tempVector;
}

Pixel Line::GetA(){
	return itsA;
}

Pixel Line::GetB(){
	return itsB;
}

void Line::SetA(Pixel PointA){
	itsA = PointA;
}

void Line::SetA(float Ax, float Ay){
	itsA.SetX(Ax);
	itsA.SetY(Ay);
}

void Line::SetB(float Bx, float By){
	itsB.SetX(Bx);
	itsB.SetY(By);
}

void Line::SetB(Pixel PointB){
	itsB= PointB;
}

float Line::GetL(){
	float lenght;
	lenght = sqrt(pow(abs(itsA.GetX()-itsB.GetX()),2) + pow(abs(itsA.GetY() - itsB.GetY()),2));
	return lenght;
}

float Line::GetAlpha(){
	float alpha;
	float tgalpha;
	float DiffX = itsB.GetX()-itsA.GetX();

	if(DiffX != 0)
	{
		tgalpha = (itsB.GetY()-itsA.GetY())/(DiffX);
		alpha = atan(tgalpha);
	}
	else if (itsB.GetY()-itsA.GetY() > 0) {
		alpha = 90;
	}
	else alpha = 270;

	return alpha;

}

void Line::Rotate(double angle, Pixel axis){

			angle = -angle*3.1415926545/180;
			/* Wektory do obliczeń */
			Pixel Vect_OA;
			Pixel Vect_OB = axis;
			Pixel Vect_AB;

			/* Wektor wyjściowy */
			Pixel Vect_OAp;

			/* Obrót punktu Startowego */
			Vect_OA = GetA();

			Vect_AB = Vect_OA - Vect_OB;

			/* Rotacja OA-OB */
			Vect_OAp.SetX(Vect_AB.GetX()*cos(angle) - Vect_AB.GetY()*sin(angle));
			Vect_OAp.SetY(Vect_AB.GetX()*sin(angle) + Vect_AB.GetY()*cos(angle));

			/* Przesunięcie do punktu B */
			Vect_OAp = Vect_OAp + Vect_OB;

			SetA(Vect_OAp);

			/* Obrót punktu Końcowego */
			Vect_OA = GetB();

			Vect_AB = Vect_OA - Vect_OB;

			/* Rotacja OA-OB */
			Vect_OAp.SetX(Vect_AB.GetX()*cos(angle) - Vect_AB.GetY()*sin(angle));
			Vect_OAp.SetY(Vect_AB.GetX()*sin(angle) + Vect_AB.GetY()*cos(angle));

			/* Przesunięcie do punktu B */
			Vect_OAp = Vect_OAp + Vect_OB;

			SetB(Vect_OAp);

}

void Line::SetSize(uint8_t size){
	itsSize = size;
	}

uint8_t Line::GetSize(){
	return itsSize;
}

void Line::ChangeDir(){
		Pixel tempPoint;

		tempPoint = GetA();
		SetA(GetB());
		SetB(tempPoint);

	}

void Line::Draw(ILI9341 LCD){
		Pixel Dif;
		Pixel A,B;

		A = GetA();
		B = GetB();

		Dif = B-A;

		int y = 0;
		int x = 0
				;
		Pixel Pix;
		Pix.Kolor = Kolor;

		float t;

		if(abs(Dif.GetX())>=abs(Dif.GetY())){
			if(Dif.GetX() < 0) ChangeDir();
				/* Kąt nachylenia jest mniejszy lub równy 45 deg */
				/*Iterujemy po x */

				/* z równania parametrycznego prostej*/

				for(x=GetA().GetX(); x<GetB().GetX(); ++x){
					t = (float)(x - GetA().GetX())/(float)Dif.GetX();
					y = GetA().GetY() + t*Dif.GetY();

					Pix.SetX(x);
					Pix.SetY(y);

					Pix.Draw(LCD);

				}
			}
			else{
				if(Dif.GetY() < 0 ) ChangeDir();

				for(y=GetA().GetY(); y<GetB().GetY(); ++y)
				{
					t = float(y - GetA().GetY())/(float)Dif.GetY();
					x = GetA().GetX() + t*Dif.GetX();

					Pix.SetX(x);
					Pix.SetY(y);
					Pix.Draw(LCD);
				}


			}
}

void Line::Erase(ILI9341 LCD){
	Color tempColor;
	tempColor = Kolor;
	Kolor = LCD.GetBackgroundColor();
	Draw(LCD);
	Kolor = tempColor;

}


/* ----------------------------BACKGROUND------------------------------------------*/
class Rectangle : public Shape{
	public:

		void Set(Pixel LeftUp, Pixel RightDown);
		void Set(uint16_t LeftUpX, uint16_t LeftUpY, uint16_t RightDownX, uint16_t RightDownY);
		void Set(Pixel LeftUp, Pixel RightUp, Pixel RightDown, Pixel LeftDown);
		void SetSize(uint8_t size);
		uint8_t GetSize();

		void Rotate(double angle, Pixel axis);

		void SetFill(bool isFilled);
		bool IsFilled();

		void Draw(ILI9341);
		void Erase(ILI9341);

	private:
		Pixel itsLeftUp;
		Pixel itsRightUp;
		Pixel itsRightDown;
		Pixel itsLeftDown;

		bool itsFill = 0;
		uint8_t itsSize = 1;
};

void Rectangle::Set(uint16_t LeftUpX, uint16_t LeftUpY, uint16_t RightDownX, uint16_t RightDownY){
	Pixel tempPixLU;
	Pixel tempPixRD;

	tempPixLU.Set(LeftUpX, LeftUpY);
	tempPixRD.Set(RightDownX, RightDownY);
	Set(tempPixLU, tempPixRD);

}

void Rectangle::Set(Pixel LeftUp, Pixel RightDown){
	itsLeftUp = LeftUp;
	itsRightUp.Set(RightDown.GetX(), LeftUp.GetY());
	itsRightDown = RightDown;
	itsLeftDown.Set(LeftUp.GetX(), RightDown.GetY());
}

void Rectangle::Set(Pixel LU, Pixel RU, Pixel RD, Pixel LD){
	itsLeftUp = LU;
	itsRightUp = RU;
	itsRightDown = RD;
	itsLeftDown = LD;
}

void Rectangle::SetFill(bool IsFilled){
	itsFill = IsFilled;
}

bool Rectangle::IsFilled(){
	return itsFill;
}

void Rectangle::SetSize(uint8_t size){
	itsSize = size;
}

uint8_t Rectangle::GetSize(){
	return itsSize;
}

void Rectangle::Draw(ILI9341 LCD){

	Pixel A = itsLeftUp;
	Pixel B = itsRightUp;
	Pixel C = itsRightDown;
	Pixel D = itsLeftDown;

	Pixel X;
	Pixel Y;

	Line tempLine;
	tempLine.Kolor = Kolor;

	if(IsFilled()){

	}
	else{

		for(uint8_t i=0; i<itsSize; ++i){

			X.Set(A.GetX()-i, A.GetY()-i);
			Y.Set(B.GetX()-i, B.GetY()-i);
			tempLine.SetA(X);
			tempLine.SetB(Y);
			tempLine.Draw(LCD);

			X.Set(B.GetX()-i, B.GetY()-i);
			Y.Set(C.GetX()-i, C.GetY()-i);
			tempLine.SetA(X);
			tempLine.SetB(Y);
			tempLine.Draw(LCD);

			X.Set(C.GetX()-i, C.GetY()-i);
			Y.Set(D.GetX()-i, D.GetY()-i);
			tempLine.SetA(X);
			tempLine.SetB(Y);
			tempLine.Draw(LCD);

			X.Set(D.GetX()-i, D.GetY()-i);
			Y.Set(A.GetX()-i, A.GetY()-i);
			tempLine.SetA(X);
			tempLine.SetB(Y);
			tempLine.Draw(LCD);

			}
		}


}

void Rectangle::Erase(ILI9341 LCD){
	Color tempColor;

	tempColor = Kolor;
	Kolor = LCD.GetBackgroundColor();
	Draw(LCD);
	Kolor = tempColor;
}

void Rectangle::Rotate(double angle, Pixel axis){
	Line tempLine;

	tempLine.SetA(itsLeftUp);
	tempLine.SetB(itsLeftDown);
	tempLine.Rotate(angle, axis);

	itsLeftUp = tempLine.GetA();
	itsLeftDown = tempLine.GetB();

	tempLine.SetA(itsRightUp);
	tempLine.SetB(itsRightDown);
	tempLine.Rotate(angle, axis);

	itsRightUp = tempLine.GetA();
	itsRightDown = tempLine.GetB();
}
