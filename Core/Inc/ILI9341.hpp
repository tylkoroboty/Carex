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
		void SetColor (uint8_t Red, uint8_t Green, uint8_t Blue);
		uint8_t GetRed();
		uint8_t GetGreen();
		uint8_t GetBlue();
	private:
		uint8_t itsRed = 0;
		uint8_t itsGreen = 0;
		uint8_t itsBlue = 0;
};

class Point {
	public:
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

class Pixel: public Point{
	public:

		Color Kolor;
		void SetSize(uint8_t size);
		uint8_t GetSize();
};

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

class Line: public Vector{
	public:
		Color Kolor;

		void ChangeDir();
		void SetSize(uint8_t size);
		uint8_t GetSize();
};

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

	void Draw(Pixel Pixel);
	void Draw(Line Line);
	void Draw(Rectangle Rectangle);

	void Erase(Pixel &Pixel, Color Kolor);
	void Erase(Line &Line, Color Kolor);
};
