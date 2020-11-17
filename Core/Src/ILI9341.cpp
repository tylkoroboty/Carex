/*
 * ILI9341.cpp
 *
 *  Created on: Oct 26, 2020
 *      Author: mikol
 */

#include "stm32f4xx_hal.h"
#include <math.h>
/* Obsługa wyświetlacza high level */

/* --------------------------COLOR---------------------*/
class Color{
	public:
		void SetColor (uint8_t Red, uint8_t Green, uint8_t Blue);
		uint8_t GetRed();
		uint8_t GetGreen();
		uint8_t GetBlue();
	private:
		uint8_t itsRed = 0;
		uint8_t itsGreen = 0;
		uint8_t itsBlue = 0;
};

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

/* -------------------------- POINT --------------------*/
class Point {
	public:

		Point();
		void SetX(float x);
		void SetY(float y);
		void Set(float x, float y);
		float GetX();
		float GetY();
		Point operator+(Point &);
		Point operator-(Point &);
	protected:
		float itsX;
		float itsY;
};

Point::Point(){
	itsX =0;
	itsY =0;
		}


Point Point::operator+(Point & rhs){
	Point tempPoint;
	tempPoint.SetX(itsX + rhs.GetX());
	tempPoint.SetY(itsY + rhs.GetY());

	return tempPoint;
}

Point Point::operator-(Point & rhs){
	Point tempPoint;
	tempPoint.SetX(itsX - rhs.GetX());
	tempPoint.SetY(itsY - rhs.GetY());

	return tempPoint;
}

void Point::Set(float x, float y){
	itsX = x;
	itsY = y;
}

void Point::SetX(float x){
	itsX = x;
}
void Point::SetY(float y){
	itsY = y;
}

float Point::GetX(){
	return itsX;
}

float Point::GetY(){
	return itsY;
}


/* ------------------------------------------------------- PIXEL -------------------*/
class Pixel : public Point {
	public:

		Color Kolor;
		void ChangeSize(uint8_t size);
		uint8_t GetSize();
	private:
		uint8_t itsSize=1;
};

void Pixel::ChangeSize(uint8_t size){
	itsSize = size;
}

uint8_t Pixel::GetSize(){
	return itsSize;
}

/* ------------------------------------------ VECTOR ------------------------------------*/
class Vector{
	public:
		void SetA(Point A);
		void SetA(float Ax, float Ay);
		void SetB(Point B);
		void SetB(float Bx, float By);

		Point GetA();
		Point GetB();
		float GetL();
		float GetAlpha();
		void Rotate(double angle, Point axis);

		Vector operator+ (Vector &);
		Vector operator- (Vector &);
	protected:
		Point itsA;
		Point itsB;
};

Vector Vector::operator+(Vector & rhs){
	Vector tempVector;
	tempVector.SetA(itsA + rhs.itsA);
	tempVector.SetB(itsB + rhs.itsB);

	return tempVector;
}

Vector Vector::operator-(Vector & rhs){
	Vector tempVector;
	tempVector.SetA(itsA - rhs.itsA);
	tempVector.SetB(itsB - rhs.itsB);

	return tempVector;
}

Point Vector::GetA(){
	return itsA;
}

Point Vector::GetB(){
	return itsB;
}

void Vector::SetA(Point PointA){
	itsA = PointA;
}

void Vector::SetA(float Ax, float Ay){
	itsA.SetX(Ax);
	itsA.SetY(Ay);
}

void Vector::SetB(float Bx, float By){
	itsB.SetX(Bx);
	itsB.SetY(By);
}

void Vector::SetB(Point PointB){
	itsB= PointB;
}

float Vector::GetL(){
	float lenght;
	lenght = sqrt(pow(abs(itsA.GetX()-itsB.GetX()),2) + pow(abs(itsA.GetY() - itsB.GetY()),2));
	return lenght;
}

float Vector::GetAlpha(){
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

void Vector::Rotate(double angle, Point axis){

			angle = -angle*3.1415926545/180;
			/* Wektory do obliczeń */
			Point Vect_OA;
			Point Vect_OB = axis;
			Point Vect_AB;

			/* Wektor wyjściowy */
			Point Vect_OAp;

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

/*------------------------------------------------ LINE ------------------------------------------------------*/
class Line : public Vector{
	public:
		Color Kolor;
		void ChangeDir();
		void SetSize(uint8_t size);
		uint8_t GetSize();
	private:
		uint8_t itsSize = 1;
};

void Line::SetSize(uint8_t size){
	itsSize = size;
	}

uint8_t Line::GetSize(){
	return itsSize;
}

void Line::ChangeDir(){
		Point tempPoint;

		tempPoint = GetA();
		SetA(GetB());
		SetB(tempPoint);

	}

/* ----------------------------BACKGROUND------------------------------------------*/
class Rectangle{
	public:
		Color Kolor;
		Color Gradient;
		Point LeftUp;
		Point RightDown;
		void SetFill(bool isFilled);
	private:
		bool itsFill = 1;
};

void Rectangle::SetFill(bool IsFilled){
	itsFill = IsFilled;
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

	void X_Set(uint16_t SP, uint16_t EP);
	void Y_Set(uint16_t SP, uint16_t EP);

	void Draw(Pixel Pixel);
	void Draw(Line Line);
	void Draw(Rectangle Rectangle);

	void Erase(Pixel &Pixel, Color Kolor);
	void Erase(Line &Line, Color Kolor);

	private:

		void Enable();
		void Disable();
		void Synchro();

		void CSX_ON();
		void CSX_OFF();
		void RDX(uint8_t RW);
		void DCX(uint8_t CD);
};

void ILI9341::Draw(Pixel Pix){

	if(Pix.GetX()<320 && Pix.GetY()<240){
		Line tempLine;
		tempLine.SetSize(1);
		tempLine.Kolor = Pix.Kolor;

		/* Rysujemy okrąg o środku Pixel i promieniu size */
		int xs,xe,ys,ye,temp;
		xs = Pix.GetX()-(Pix.GetSize()-1);
		xe = Pix.GetX()+(Pix.GetSize()-1);

		for(int x=xs; x<xe; ++x){
			temp = sqrt((Pix.GetSize()*Pix.GetSize())-(x-Pix.GetX())*(x-Pix.GetX()));
			ys=Pix.GetY()+temp;
			ye=Pix.GetY()-temp;
			tempLine.SetA(x, ys);
			tempLine.SetB(x, ye);
			Draw(tempLine);
			}

	X_Set(Pix.GetX(), Pix.GetX()+1);
	Y_Set(Pix.GetY(), Pix.GetY()+1);

	Write(REG_MEMORY_WRITE, 0, 0);

	Write(REG_MEMORY_WRITE_CONTINUE, 0, 0);
	SendData(Pix.Kolor, 1);
	}
}


void ILI9341::Draw(Line L)
{
	Point Dif;
	Point A,B;

	A = L.GetA();
	B = L.GetB();

	Dif = B-A;

	int y = 0;
	int x = 0
			;
	Pixel Pixel;
	Pixel.Kolor = L.Kolor;

	float t;

	if(abs(Dif.GetX())>abs(Dif.GetY())){
		if(Dif.GetX() < 0) L.ChangeDir();
			/* Kąt nachylenia jest mniejszy lub równy 45 deg */
			/*Iterujemy po x */

			/* z równania parametrycznego prostej*/

			for(x=L.GetA().GetX(); x<L.GetB().GetX(); ++x){
				t = (float)(x - L.GetA().GetX())/(float)Dif.GetX();
				y = L.GetA().GetY() + t*Dif.GetY();

				Pixel.ChangeSize(L.GetSize());
				Pixel.SetX(x);
				Pixel.SetY(y);
				Draw(Pixel);

			}
		}
		else{
			if(Dif.GetY() < 0 ) L.ChangeDir();

			for(y=L.GetA().GetY(); y<L.GetB().GetY(); ++y)
			{
				t = float(y - L.GetA().GetY())/(float)Dif.GetY();
				x = L.GetA().GetX() + t*Dif.GetX();

				Pixel.ChangeSize(L.GetSize());
				Pixel.SetX(x);
				Pixel.SetY(y);
				Draw(Pixel);
			}


		}
	}

void ILI9341::Erase(Pixel &Pixel, Color Kolor){
	//Chujowe rozwiązanie
	Color tempColor;
	tempColor = Pixel.Kolor;
	Pixel.Kolor = Kolor;
	Draw(Pixel);
	Pixel.Kolor = tempColor;
}

void ILI9341::Erase(Line &Line, Color Kolor){
	//Chujowe rozwiązanie
	Color tempColor;
	tempColor = Line.Kolor;
	Line.Kolor = Kolor;
	Draw(Line);
	Line.Kolor = tempColor;
}

void ILI9341::Draw(Rectangle BG){

	Line tempLine;
	tempLine.Kolor=BG.Kolor;

	for(int x = BG.LeftUp.GetX(); x<BG.RightDown.GetX();++x)
	{
		tempLine.SetA(x,BG.LeftUp.GetY());
		tempLine.SetB(x,BG.RightDown.GetY());
		Draw(tempLine);
	}
}

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

void ILI9341::SendData(Color Kolor, uint16_t n){

	uint8_t data[3];

	data[2]= (Kolor.GetRed()>>1)|0x80;
	data[1]= (Kolor.GetGreen()>>1)|0x80;
	data[0]= (Kolor.GetBlue()>>1)|0x80;

	DCX(DATA);
	CSX_ON();

	for(uint16_t i=0; i<n; i++)
	{
		HAL_SPI_Transmit(spi, data, 3, 100);
	}
	CSX_OFF();

}

void ILI9341::X_Set(uint16_t SP, uint16_t EP){

	uint8_t data[4];
	data[0] = (SP >> 8) & 0x00ff;
	data[1] = SP & 0x00ff;
	data[2] = (EP >> 8) & 0x00ff;
	data[3] = (EP & 0x00ff);

	Write(REG_PAGE_ADDRESS_SET, 4, data);

}

void ILI9341::Y_Set(uint16_t SP, uint16_t EP){

	uint8_t data[4];
	data[0] = (SP >> 8) & 0x00ff;
	data[1] = SP & 0x00ff;
	data[2] = (EP >> 8) & 0x00ff;
	data[3] = (EP & 0x00ff);

	Write(REG_COLUMN_ADDRESS_SET, 4, data);

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
