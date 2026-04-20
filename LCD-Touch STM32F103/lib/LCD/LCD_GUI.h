#ifndef _LCD_GUI_H_
#define _LCD_GUI_H_

#include <Arduino.h>	

#define LEFT 0
#define RIGHT 9999
#define CENTER 9998
/*							      r    g    b */
#define BLACK        0x0000  /*   0,   0,   0 */
#define BLUE         0x001F  /*   0,   0, 255 */
#define RED          0xF800  /* 255,   0,   0 */
#define GREEN        0x07E0  /*   0, 255,   0 */
#define CYAN         0x07FF  /*   0, 255, 255 */
#define MAGENTA      0xF81F  /* 255,   0, 255 */
#define YELLOW       0xFFE0  /* 255, 255,   0 */
#define WHITE        0xFFFF  /* 255, 255, 255 */
#define NAVY         0x000F  /*   0,   0, 128 */
#define DARKGREEN    0x03E0  /*   0, 128,   0 */
#define DARKCYAN     0x03EF  /*   0, 128, 128 */
#define MAROON       0x7800  /* 128,   0,   0 */
#define PURPLE       0x780F  /* 128,   0, 128 */
#define OLIVE        0x7BE0  /* 128, 128,   0 */
#define LIGHTGREY    0xC618  /* 192, 192, 192 */
#define DARKGREY     0x7BEF  /* 128, 128, 128 */
#define ORANGE       0xFD20  /* 255, 165,   0 */
#define GREENYELLOW  0xAFE5  /* 173, 255,  47 */
#define PINK         0xF81F  /* 255,   0, 255 */

#define swap(a, b) { int16_t t = a; a = b; b = t; }

struct RGB {
    uint16_t val;
    RGB(uint16_t c) : val(c) {}
    RGB(uint8_t r, uint8_t g, uint8_t b) : val(((uint16_t)(r&0xF8)<<8) | ((uint16_t)(g&0xFC)<<3) | (b>>3)) {}
};

class LCD_GUI
{
	public:
	template <typename T> //display string, number and float:[d: string, number or float number] [csize: text size] [fc: fore color for RGB; RGB(x,x,x)] [bc: back color default 0 for RGB; RGB(x,x,x)] [mode: default 0, mode=0 no background mode=1 background] [system; default 10, DEC=10 BIN=2 HEX=16 OCT=8 vs...] [dec: decimal point default 2]
	void Print(T d, int16_t x, int16_t y, uint8_t csize, const RGB& fc, const RGB& bc=0, bool mode=0, int16_t system = 10, uint8_t dec = 2) {
        text_mode = mode;
		text_size = csize;
		text_color = fc.val;
		text_bgcolor = bc.val;  
		if constexpr (std::is_same_v<T, uint8_t*> || std::is_same_v<T, const uint8_t*> || std::is_same_v<T, char*> 
			|| std::is_same_v<T, const char*>) { Print_Str((uint8_t*)d, x, y);} 
		else if constexpr (std::is_same_v<T, String>) {	Print_String(d, x, y); } 
		else if constexpr (std::is_floating_point_v<T>) { Print_Number_Float((double)d, dec, x, y, '.', 0, ' '); } 
		else if constexpr (std::is_integral_v<T>) {	Print_Number_Int((long int)d, x, y, 0, ' ', system); }
	}
    //These are defined by the subclass:
	virtual void Draw_Pixe(int16_t x, int16_t y, uint16_t color)=0;
	virtual void Fill_Rect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t c)=0;
	virtual void Draw_Bit_Map(int16_t x, int16_t y, int16_t sx, int16_t sy, const uint16_t *data, int16_t scale)=0;
	virtual void Push_Any_Color(uint16_t * block, int16_t n, bool first, uint8_t flags)=0;
	virtual int16_t Read_GRAM(int16_t x, int16_t y, uint16_t *block, int16_t w, int16_t h)=0;
    virtual void Fill_Scree( uint16_t color)=0;
	virtual void Print_Str(uint8_t *st, int16_t x, int16_t y)=0;
	//These exist only with LCD_GUI(no subclass overrides)
    //Constructor to set text color
    LCD_GUI(void) {
        text_bgcolor = 0xF800; //default red
        text_color = 0x07E0;   //default green
        draw_color = 0xF800;   //default red
        text_size = 1;
        text_mode = 0;
    }

    //set text coordinate
    void Set_Text_Cursor(int16_t x, int16_t y) { text_x = x; text_y = y; }
    //get text x coordinate
    int16_t Get_Text_X_Cursor(void) const { return text_x; }
    //get text y coordinate
    int16_t Get_Text_Y_Cursor(void) const { return text_y; }
    //set text color with 16bit or RGB color
    void Set_Text_Color(const RGB& color) { text_color = color.val; }
    //get text color
    uint16_t Get_Text_Color(void) const {	return text_color; }
    //set text background color with 16bits or RGB color
    void Set_Text_Back_Color(const RGB& color) { text_bgcolor = color.val; }
    //get text background color
    uint16_t Get_Text_Back_Color(void) const { return text_bgcolor; }
    //set text size
    void Set_Text_Size(uint8_t s) { text_size = s; }
    //get text size
    uint8_t Get_Text_Size(void) const { return text_size; }
    //set text mode
    void Set_Text_Mode(bool mode) { text_mode = mode; }
    //get text mode
    bool Get_Text_Mode(void) const { return text_mode; }
    //set 16bits or RGB draw color
    void Set_Draw_Color(const RGB& color) { draw_color = color.val; }
    //get draw color
    uint16_t Get_Draw_color(void) const { return draw_color; }
    //Fill the full screen with color
    void Fill_Screen(const RGB& color) { Fill_Scree(color.val); }
    //draw a pixel point
    void Pixel(int16_t x, int16_t y, const RGB& color) {
        draw_color = color.val;
        Draw_Pixe(x, y, draw_color);
    }
    //read color data for point(x,y)
    uint16_t Read_Pixel(int16_t x, int16_t y) {
        uint16_t color;
        Read_GRAM(x, y, &color, 1, 1);
        return color;
    }
    //draw a vertical line
    void Fast_VLine(int16_t x, int16_t y, int16_t h, const RGB& color) {
        draw_color = color.val;
        Fill_Rect(x, y, 1, h, draw_color);
    }
    //draw a horizontal line
    void Fast_HLine(int16_t x, int16_t y, int16_t w,const RGB& color) {
        draw_color = color.val;
        Fill_Rect(x, y, w, 1, draw_color);
    }
    //draw an arbitrary line from (x1,y1) to (x2,y2)
    void Line(int16_t x1, int16_t y1, int16_t x2, int16_t y2, const RGB& color) {
        draw_color = color.val;
        int16_t steep = abs(y2 - y1) > abs(x2 - x1);
        if (steep) { swap(x1, y1); swap(x2, y2); }
        if (x1 > x2) { swap(x1, x2); swap(y1, y2); }
        int16_t dx, dy;	 dx = x2 - x1;  dy = abs(y2 - y1);  int16_t err = dx / 2;	int16_t ystep;
        if (y1 < y2) { ystep = 1; }	else { ystep = -1; }
        for (; x1<=x2; x1++) {
            if (steep) { Draw_Pixe(y1, x1,draw_color); } else { Draw_Pixe(x1, y1,draw_color); }
            err -= dy;
            if (err < 0) {	y1 += ystep;	err += dx; 	}
        }
    }
    //draw a rectangle
    void Rectangle(int16_t x1, int16_t y1, int16_t x2, int16_t y2,const RGB& color) {
        draw_color = color.val;
        int16_t w = x2 - x1 + 1, h = y2 - y1 + 1;
        if (w < 0) { x1 = x2; w = -w; }
        if (h < 0) { y1 = y2; h = -h; }
        Fill_Rect(x1, y1, w, 1, draw_color);
        Fill_Rect(x1, y2, w, 1, draw_color);
        Fill_Rect(x1, y1, 1, h, draw_color);
        Fill_Rect(x2, y1, 1, h, draw_color);
    }
    //fill a rectangle
    void Fill_Rectangle(int16_t x1, int16_t y1, int16_t x2, int16_t y2, const RGB& color) {
        draw_color = color.val;
        int w = x2 - x1 + 1, h = y2 - y1 + 1;
        if (w < 0) { x1 = x2; w = -w; }
        if (h < 0) { y1 = y2; h = -h; }
        Fill_Rect(x1, y1, w, h, draw_color);
    }
    //draw a round rectangle
    void Round_Rectangle(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t radius,const RGB& color) {
        draw_color = color.val;
        int w = x2 - x1 + 1, h = y2 - y1 + 1;
        if (w < 0) { x1 = x2; w = -w; }
        if (h < 0) { y1 = y2; h = -h; }
        Fill_Rect(x1+radius, y1, w-2*radius, 1, draw_color);
        Fill_Rect(x1+radius, y1+h-1, w-2*radius, 1, draw_color);
        Fill_Rect(x1, y1+radius, 1, h-2*radius, draw_color);
        Fill_Rect(x1+w-1, y1+radius, 1, h-2*radius, draw_color);
        Circle_Helper(x1+radius, y1+radius, radius, 1);
        Circle_Helper(x1+w-radius-1, y1+radius, radius, 2);
        Circle_Helper(x1+w-radius-1, y1+h-radius-1, radius, 4);
        Circle_Helper(x1+radius, y1+h-radius-1, radius, 8);
    }
    //fill a round rectangle
    void Fill_Round_Rectangle(int16_t x1, int16_t y1, int16_t x2,int16_t y2, int16_t radius,const RGB& color) {
        draw_color = color.val;
        int w = x2 - x1 + 1, h = y2 - y1 + 1;
        if (w < 0) { x1 = x2; w = -w; }
        if (h < 0) { y1 = y2; h = -h; }
        Fill_Rect(x1+radius, y1, w-2*radius, h, draw_color);
        Fill_Circle_Helper(x1+w-radius-1, y1+radius, radius, 1, h-2*radius-1);
        Fill_Circle_Helper(x1+radius, y1+radius, radius, 2, h-2*radius-1);
    }
    //draw a circle
    void Circle(int16_t x, int16_t y, int16_t radius,const RGB& color) {
        draw_color = color.val;
        int16_t f = 1 - radius, ddF_x = 1, ddF_y = -2 * radius, x1= 0, y1= radius;
        Draw_Pixe(x, y+radius,draw_color);
        Draw_Pixe(x, y-radius,draw_color);
        Draw_Pixe(x+radius, y,draw_color);
        Draw_Pixe(x-radius, y,draw_color);
        while (x1<y1) {
            if (f >= 0)	{ y1--; ddF_y += 2; f += ddF_y; }
            x1++; ddF_x += 2; f += ddF_x;
            Draw_Pixe(x + x1, y + y1,draw_color);
            Draw_Pixe(x - x1, y + y1,draw_color);
            Draw_Pixe(x + x1, y - y1,draw_color);
            Draw_Pixe(x - x1, y - y1,draw_color);
            Draw_Pixe(x + y1, y + x1,draw_color);
            Draw_Pixe(x - y1, y + x1,draw_color);
            Draw_Pixe(x + y1, y - x1,draw_color);
            Draw_Pixe(x - y1, y - x1,draw_color);
        }
    }
    //draw a circular bead
    void Circle_Helper(int16_t x0, int16_t y0, int16_t radius, uint8_t cornername) {
        int16_t f = 1 - radius, ddF_x = 1, ddF_y = -2 * radius, x = 0, y = radius;
        while (x<y)	{
            if (f >= 0)	{ y--; ddF_y += 2; f += ddF_y; }
            x++; ddF_x += 2; f += ddF_x;
            if (cornername & 0x4) {
                Draw_Pixe(x0 + x, y0 + y,draw_color);
                Draw_Pixe(x0 + y, y0 + x,draw_color);
            }
            if (cornername & 0x2) {
                Draw_Pixe(x0 + x, y0 - y,draw_color);
                Draw_Pixe(x0 + y, y0 - x,draw_color);
            }
            if (cornername & 0x8) {
                Draw_Pixe(x0 - y, y0 + x,draw_color);
                Draw_Pixe(x0 - x, y0 + y,draw_color);
            }
            if (cornername & 0x1) {
                Draw_Pixe(x0 - y, y0 - x,draw_color);
                Draw_Pixe(x0 - x, y0 - y,draw_color);
            }
        }
    }
    //fill a circle
    void Fill_Circle(int16_t x, int16_t y, int16_t radius,const RGB& color) {
        draw_color = color.val;
        Fill_Rect(x, y-radius, 1, 2*radius+1, draw_color);
        Fill_Circle_Helper(x, y, radius, 3, 0);
    }
    //fill a semi-circle
    void Fill_Circle_Helper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername,int16_t delta) {
        int16_t f = 1 - r, ddF_x = 1, ddF_y = -2 * r, x = 0, y = r;
        while (x<y)	{
            if (f >= 0)	{ y--; ddF_y += 2; f += ddF_y; }
            x++; ddF_x += 2; f += ddF_x;
            if (cornername & 0x1) {
                Fill_Rect(x0+x, y0-y, 1, 2*y+1+delta, draw_color);
                Fill_Rect(x0+y, y0-x, 1, 2*x+1+delta, draw_color);
            }
            if (cornername & 0x2) {
                Fill_Rect(x0-x, y0-y, 1, 2*y+1+delta, draw_color);
                Fill_Rect(x0-y, y0-x, 1, 2*x+1+delta, draw_color);
            }
        }
    }
    //draw a triangle
    void Triangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1,int16_t x2, int16_t y2,const RGB& color) {
        draw_color = color.val;
        Line(x0, y0, x1, y1,draw_color);
        Line(x1, y1, x2, y2,draw_color);
        Line(x2, y2, x0, y0,draw_color);
    }
    //fill a triangle
    void Fill_Triangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1,int16_t x2, int16_t y2, const RGB& color) {
        draw_color = color.val;
        int16_t a, b, y, last;
        if (y0 > y1) { swap(y0, y1); swap(x0, x1); }
        if (y1 > y2) { swap(y2, y1); swap(x2, x1); }
        if (y0 > y1) { swap(y0, y1); swap(x0, x1); }
        if(y0 == y2) {
            a = b = x0;
            if(x1 < a)	{ a = x1; }	else if(x1 > b)	{ b = x1; }
            if(x2 < a)	{ a = x2; }	else if(x2 > b)	{ b = x2; }
            Fill_Rect(a, y0, b-a+1, 1, draw_color);
            return;
        }
        int16_t dx01 = x1 - x0, dy01 = y1 - y0, dx02 = x2 - x0, dy02 = y2 - y0, dx12 = x2 - x1, dy12 = y2 - y1;
        int32_t sa = 0, sb = 0;
        if(y1 == y2) { last = y1; } else { last = y1-1; }
        for(y=y0; y<=last; y++)	{
            a = x0 + sa / dy01;	b = x0 + sb / dy02;
            sa += dx01;	sb += dx02;
            if(a > b) { swap(a,b); }
            Fill_Rect(a, y, b-a+1, 1, draw_color);
        }
        sa = dx12 * (y - y1);
        sb = dx02 * (y - y0);
        for(; y<=y2; y++) {
            a = x1 + sa / dy12; b = x0 + sb / dy02;
            sa += dx12; sb += dx02;
            if(a > b) { swap(a,b); }
            Fill_Rect(a, y, b-a+1, 1, draw_color);
        }
    }

    //print string
    void Print_String(String st, int16_t x, int16_t y) { Print_Str((uint8_t *)(st.c_str()), x, y); }
    //print int number
    void Print_Number_Int(long num, int16_t x, int16_t y, int16_t length, uint8_t filler, int16_t system) {
        uint8_t st[27] = {0};
        uint8_t *p = st+26;
        bool flag = false;
        int16_t len = 0,nlen = 0,left_len = 0,i = 0;
        *p = '\0';
        if(0 == num) { *(--p) = '0'; len = 1; }
        else if(num < 0) { num = -num;	flag = true; }
        while((num > 0) && (len < 10)) {
            if(num%system > 9) { *(--p) = 'A' + (num%system-10); } else { *(--p) = '0' + num%system; }
            num = num/system;
            len++;
        }
        if(flag) { *(--p) = '-'; }
        if(length > (len + flag + 1)) {
            if(length > (int16_t)sizeof(st)) { nlen = sizeof(st) - len - flag - 1; }
            else { nlen = length - len - flag - 1; }
            for(i = 0;i< nlen;i++) { *(--p) = filler; } 
            left_len = sizeof(st) - nlen - len - flag - 1;
        } else { left_len = sizeof(st) - len - flag - 1; }
        for(i = 0; i < (int16_t)(sizeof(st) - left_len);i++) { st[i] = st[left_len + i]; }
        st[i] = '\0';
        Print_Str(st, x, y);
    }
    //print float number
    void Print_Number_Float(double num, uint8_t dec, int16_t x, int16_t y, uint8_t divider, int16_t length, uint8_t filler) {
        uint8_t st[27] = {0};
        uint8_t * p = st;
        bool flag = false;
        int16_t i = 0;
        if(dec<1) { dec=1; } else if(dec>5) { dec=5; }
        if(num<0) { flag = true; }
        dtostrf(num, length, dec, (char *)st);
        if(divider != '.') {
            while(i < (int16_t)sizeof(st)) {
                if('.' == *(p+i)) { *(p+i) = divider; }	i++;
            }
        }
        if(filler != ' ') {
            if(flag) {
                *p = '-'; i = 1;
                while(i < (int16_t)sizeof(st)) {
                    if((*(p+i) == ' ') || (*(p+i) == '-')) { *(p+i) = filler; } i++;
                }
            }
            else {
                i = 0;
                while(i < (int16_t)sizeof(st)) {
                    if(' ' == *(p+i)) { *(p+i) = filler; } i++;
                }
            }
        }
        Print_Str(st, x, y);
    }

	protected:
	int16_t text_x, text_y;
	uint16_t text_color, text_bgcolor, draw_color;
	uint8_t text_size;
	bool text_mode; //if set,text_bgcolor
};

#endif
