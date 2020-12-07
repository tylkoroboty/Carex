/*
 * ILI9341.hpp
 *
 *  Created on: Oct 26, 2020
 *      Author: mikol
 */

#ifndef INC_ILI9341_HPP_
#define INC_ILI9341_HPP_

//Commands
#define REG_READ_ID 0x04
#define REG_SOFT_RESET 0x01
#define REG_SLEEP_OUT 0x11
#define REG_MEMORY_WRITE 0x2C
#define REG_MEMORY_WRITE_CONTINUE 0x3C
#define REG_DISPLAY_ON 0x29
#define REG_DISPLAY_OFF 0x28



#endif /* INC_ILI9341_HPP_ */

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
	void SendData(uint8_t R, uint8_t G, uint8_t B, uint16_t n);

	void Shift_Frame(uint16_t y);
	void Write(uint8_t command,uint8_t n,uint8_t * data);
	void Reset();
	void PowerON();

	void X_Set(uint16_t SP, uint16_t EP);
	void Y_Set(uint16_t SP, uint16_t EP);

	void SetBackgroundColor(Color);
	void SetBackgroundColor(uint8_t red, uint8_t green, uint8_t blue);
	Color GetBackgroundColor();

	private:
	Color itsBackgroundColor;

//	void Draw(Rectangle Rectangle);
//
//	void Erase(Pixel &Pixel, Color Kolor);
//	void Erase(Line &Line, Color Kolor);
};

class Shape{
	public:
	virtual void Draw(ILI9341);
	virtual void Erase(ILI9341);
	Color Kolor;

	protected:
};

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
