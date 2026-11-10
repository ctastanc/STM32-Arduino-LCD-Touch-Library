#pragma once

#include <Arduino.h>
#include <Font_Plain.h>	
#include <Font9.h>	
#include <Font19.h>
#include <Font30.h>
#include <Font43.h>
#include <Font59.h>

#define LEFT 0
#define RIGHT 9999
#define CENTER 9998

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

#define SWAP(a, b) { int16_t t = a; a = b; b = t; }

struct RGB {
    uint32_t val;
    static inline uint16_t last_fc = 0xFFFF, last_bc = 0x0000; 
    static constexpr uint32_t AUTO_FC = 0xFFFFFFFE, AUTO_BC = 0xFFFFFFFF; 
    constexpr RGB() : val(AUTO_FC) {}
    constexpr RGB(uint16_t c) : val(c) {}
    constexpr RGB(uint8_t r, uint8_t g, uint8_t b) : val(((uint16_t)(r & 0xF8) << 8) | ((uint16_t)(g & 0xFC) << 3) | (b >> 3)) {}
    static constexpr RGB AutoBC() { RGB c; c.val = AUTO_BC; return c; }
    uint16_t getFC() const { if (val != AUTO_FC && val != AUTO_BC) last_fc = (uint16_t)val; return last_fc; }
    uint16_t getBC() const { if (val != AUTO_FC && val != AUTO_BC) last_bc = (uint16_t)val; return last_bc; }
    //uint8_t hi() const { return (uint8_t)(getFC() >> 8); }
    //uint8_t lo() const { return (uint8_t)(getFC() & 0xFF); }
    operator uint16_t() const { return getFC(); }
};

template <typename Derived>
class LCD_GUI 
{
    Derived& impl() { return static_cast<Derived&>(*this); }
    const Derived& impl() const { return static_cast<const Derived&>(*this); }
	public:
    uint8_t num_buf[27];
	int16_t text_x, text_y;
	uint16_t text_fc, text_bc, draw_color, text_w; 
	uint8_t text_size;
    size_t text_len;
    const uint8_t *text;
    uint32_t text_fh, text_fl, text_bh, text_bl;
	bool text_mode; 
    const tFont *Font;

    LCD_GUI(void) {
        text_bc = 0xF800;
        text_fc = 0x07E0;
        draw_color = RGB(0xF800);
        text_size = 1;
        text_mode = 0;
    }

    uint StringWidth() {
        int str_width = 0, spc = Font->CharSpace; 
        uint x1=(text_x==CENTER||text_x==RIGHT) ? 0:text_x;
        for (int i = 0; i < text_len; i++) { 
            int w = Font->Offset[text[i]-32][0] + spc; 
            if(x1+w+str_width > Get_Width()) { text_len = i; break; }
            str_width += w;
        }
        return (str_width-1);
    }

    void SetAlignment() {if (text_x == CENTER) text_x = (Get_Width() - text_w)/2; else if (text_x == RIGHT) text_x = Get_Width() - text_w-1;}

    /*!
    @brief    Display char, string, number and float
    @param    pFont  Font
    @param    val    Char, string, number or float number
    @param    x      x coordinate
    @param    y      y coordinate
    @param    size   Text size
    @param    fc     Foreground color for 16 bit or RGB(r,g,b) color     
    @param    bc     Background color for 16 bit or RGB(r,g,b) color (default:0)
    @param    mode   0:Foreground only 1:Foreground and Background  (default:0)
    @param    system Number system (default 10)
    @param    dec    Decimal point (default 2)
    */
    template <typename T> 	
    void PrintFont(const tFont &pFont, T val, int16_t x, int16_t y, uint8_t size, const RGB fc = RGB(), const RGB bc=RGB::AutoBC(), bool mode=0, int16_t system=10, uint8_t dec=2) {
        Font = &pFont; text_x = x; text_y = y; text_size = size;  text_fc = fc.getFC(); text_bc = bc.getBC(); text_mode = mode;
        // All Texts, Pointers, and Arrays (char*, const char[], uint8_t*, const uint8_t*)
        if constexpr (std::is_pointer_v<std::decay_t<T>> || std::is_array_v<std::remove_reference_t<T>>) {
            text = (const uint8_t*)(val); text_len = strlen((const char*)(val)); } 
        else if constexpr (std::is_same_v<std::decay_t<T>, String>) {	
            text = (const uint8_t*)(val.c_str()); text_len = val.length(); } 
        else if constexpr (std::is_floating_point_v<T>) { Print_Number_Float((double)val, dec, '.', 0, ' '); } 
        else if constexpr (std::is_integral_v<T>) {	Print_Number_Int((long long)val, 0, ' ', system); }
        text_w = StringWidth();
        SetAlignment();
        Set_Color_Hi_Lo();
        Print_String_Font(); 
    }

    /*!
    @brief    Display char, string, number and float
    @param    val    Char, string, number or float number
    @param    x      x coordinate
    @param    y      y coordinate
    @param    size   Not use can be 0 (It was left out for compatibility with the "Print" function.)
    @param    fc     Foreground color for 16 bit or RGB(r,g,b) color     
    @param    bc     Background color for 16 bit or RGB(r,g,b) color (default:0)
    @param    mode   0:Foreground only 1:Foreground and Background  (default:0)
    @param    system Number system (default 10)
    @param    dec    Decimal point (default 2)
    */
    template <typename T> 	
    void Print(T val, int16_t x, int16_t y, uint8_t size, const RGB fc = RGB(), const RGB bc=RGB::AutoBC(), bool mode=0, int16_t system=10, uint8_t dec=2) {
        text_x = x; text_y = y; text_size = size;  text_fc = fc.getFC(); text_bc = bc.getBC(); text_mode = mode;
        // All Texts, Pointers, and Arrays (char*, const char[], uint8_t*, const uint8_t*)
        if constexpr (std::is_pointer_v<std::decay_t<T>> || std::is_array_v<std::remove_reference_t<T>>) {
            text = (const uint8_t*)(val); text_len = strlen((const char*)(val)); } 
        else if constexpr (std::is_same_v<std::decay_t<T>, String>) {	
            text = (const uint8_t*)(val.c_str()); text_len = val.length(); } 
        else if constexpr (std::is_floating_point_v<T>) { Print_Number_Float((double)val, dec, '.', 0, ' '); } 
        else if constexpr (std::is_integral_v<T>) {	Print_Number_Int((long long)val, 0, ' ', system); }
        int x1 = (text_x == CENTER || text_x == RIGHT) ? 0 : text_x;
        int char_w = 5 * text_size + 1;
        if (text_len > (Get_Width() - x1) / char_w) text_len = (Get_Width() - x1) / char_w;  
        text_w = text_len * char_w;
        SetAlignment();
        Set_Color_Hi_Lo();
        Print_String(); 
    }

    // CRTP forwarding methods (non-virtual, 100% inlineable, zero-cost)
    void Pixe(int16_t x, int16_t y, uint16_t c) { impl().Pixe(x, y, c); }
    void Fill_Rect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t c) { impl().Fill_Rect(x, y, w, h, c); }
    void Fill_Scree(uint16_t c) { impl().Fill_Scree(c); }
    virtual void Print_String()=0;// { impl().Print_Str(); }
    virtual void Print_String_Font()=0;// { impl().Print_Str(); }
    int16_t Read_GRAM(int16_t x, int16_t y, uint16_t *block, int16_t w, int16_t h) { return impl().Read_GRAM(x, y, block, w, h); }
    virtual int16_t Get_Height(void) const=0;
  	virtual int16_t Get_Width(void) const=0;
    virtual void Set_Color_Hi_Lo()=0;

    void Set_Text_Cursor(int16_t x, int16_t y) { text_x = x; text_y = y; }

    int16_t Get_Text_X_Cursor(void) const { return text_x; }
 
    int16_t Get_Text_Y_Cursor(void) const { return text_y; }

    void Set_Text_Color(const RGB color = RGB()) { text_fc = color; }
  
    uint16_t Get_Text_Color(void) const {	return text_fc; }

    void Set_Text_Back_Color(const RGB color = RGB()) { text_bc = color; }

    uint16_t Get_Text_Back_Color(void) const { return text_bc; }

    void Set_Text_Size(uint8_t s) { text_size = s; }

    uint8_t Get_Text_Size(void) const { return text_size; }

    void Set_Text_Mode(bool mode) { text_mode = mode; }

    bool Get_Text_Mode(void) const { return text_mode; }
    
    void Set_Draw_Color(const RGB color = RGB()) { draw_color = color; } //set 16bits or RGB draw color

    uint16_t Get_Draw_Color(void) const { return draw_color; }

    void Cls(const RGB color = RGB()) { Fill_Scree(color); }

    void Fill_Screen(const RGB color = RGB()) { Fill_Scree(color); }

    /*!
    @brief    Write a pixel.
    @param   x   x coordinate
    @param   y   y coordinate
    @param   color 16-bit or RGB(r,g,b) Color to fill with
    */
    void Pixel(int16_t x, int16_t y, const RGB color = RGB()) { Pixe(x, y, color); }

    /*!
    @brief    Read a pixel. return 16-bit Color
    @param   x   x coordinate
    @param   y   y coordinate
    */
    uint16_t Read_Pixel(int16_t x, int16_t y) {
        uint16_t color;
        Read_GRAM(x, y, &color, 1, 1);
        return color;
    }

    /*!
    @brief    Draw a perfectly vertical line
    @param    x   Top-most x coordinate
    @param    y   Top-most y coordinate
    @param    h   Height in pixels
    @param    color 16-bit or RGB(r,g,b) Color to fill with
    */
    void Fast_VLine(int16_t x, int16_t y, int16_t h, const RGB color = RGB()) {
        Fill_Rect(x, y, 1, h, color);
    }

    /*!
    @brief    Draw a perfectly horizontal line
    @param    x   Left-most x coordinate
    @param    y   Left-most y coordinate
    @param    w   Width in pixels
    @param    color 16-bit or RGB(r,g,b) Color to fill with
    */
    void Fast_HLine(int16_t x, int16_t y, int16_t w,const RGB color = RGB()) {
        Fill_Rect(x, y, w, 1, color);
    }
    
    /*!
    @brief    Draw a line using high-speed Run-Length Slice algorithm
    @param    x1  Start point x coordinate
    @param    y1  Start point y coordinate
    @param    x2  End point x coordinate
    @param    y2  End point y coordinate
    @param    color 16-bit or RGB(r,g,b) Color to draw with
    */
    void Line(int16_t x1, int16_t y1, int16_t x2, int16_t y2, const RGB color = RGB()) {
        const uint16_t c = color.getFC();
        if (x1 == x2) {
            if (y1 > y2) SWAP(y1, y2); Fill_Rect(x1, y1, 1, y2 - y1 + 1, c);
        } else if (y1 == y2) {
            if (x1 > x2) SWAP(x1, x2); Fill_Rect(x1, y1, x2 - x1 + 1, 1, c);
        } else {
            int16_t dx = abs(x2 - x1), dy = abs(y2 - y1);
            if (dx >= dy) {
                if (x1 > x2) { SWAP(x1, x2); SWAP(y1, y2); }
                int16_t ystep = (y1 < y2) ? 1 : -1, err = dx >> 1, run_start = x1;
                for (int16_t x = x1; x < x2; x++) {
                    err -= dy;
                    if (err < 0) {
                        int16_t len = x - run_start + 1;
                        if (len == 1) Pixe(run_start, y1, c); else Fill_Rect(run_start, y1, len, 1, c);
                        run_start = x + 1; y1 += ystep; err += dx;
                    }
                }
                int16_t len = x2 - run_start + 1;
                if (len == 1) Pixe(run_start, y1, c); else Fill_Rect(run_start, y1, len, 1, c);
            } else {
                if (y1 > y2) { SWAP(x1, x2); SWAP(y1, y2); }
                int16_t xstep = (x1 < x2) ? 1 : -1, err = dy >> 1, run_start = y1;
                for (int16_t y = y1; y < y2; y++) {
                    err -= dx;
                    if (err < 0) {
                        int16_t len = y - run_start + 1;
                        if (len == 1) Pixe(x1, run_start, c); else Fill_Rect(x1, run_start, 1, len, c);
                        run_start = y + 1; x1 += xstep; err += dy;
                    }
                }
                int16_t len = y2 - run_start + 1;
                if (len == 1) Pixe(x1, run_start, c); else Fill_Rect(x1, run_start, 1, len, c);
            }
        }
    }

    /*!
    @brief   Draw a rectangle with W, h no fill color
    @param    x     Top left corner x coordinate
    @param    y     Top left corner y coordinate
    @param    w     Width in pixels
    @param    h     Height in pixels
    @param    color 16-bit or RGB(r,g,b) Color to draw with
    */
    void Rectangle(int16_t x, int16_t y, int16_t w, int16_t h, const RGB color = RGB()) {
        const uint16_t c = color.getFC();
        Fill_Rect(x, y, w, 1, c); Fill_Rect(x, y+h-1, w, 1, c);
        Fill_Rect(x, y, 1, h, c); Fill_Rect(x+w-1, y, 1, h, c);
    }

    /*!
    @brief    Fill a rectangle completely with one color. 
    @param    x     Top left corner x coordinate
    @param    y     Top left corner y coordinate
    @param    w     Width in pixels
    @param    h     Height in pixels
    @param    color 16-bit or RGB(r,g,b) Color to fill with
    */
    void Fill_Rectangle(int16_t x, int16_t y, int16_t w, int16_t h, const RGB color = RGB()) {
        const uint16_t c = color.getFC();
        Fill_Rect(x, y, w, h, color);
    }

    /*!
    @brief   Draw a rectangle with W, h no fill color
    @param    x     Top left corner x coordinate
    @param    y     Top left corner y coordinate
    @param    w     Width in pixels
    @param    h     Height in pixels
    @param    t     Edge thickness
    @param    color 16-bit or RGB(r,g,b) Color to draw with
    */
    void Rectangle_Thickness(int16_t x, int16_t y, int16_t w, int16_t h, int16_t t, const RGB color = RGB()) {
        const uint16_t c = color.getFC();
        for(int i = 0; i < t; i++) {
            Fill_Rect(x+i, y+i, w-(2*i), 1, c); Fill_Rect(x+i, y+h-1-(i), w-(2*i), 1, c);
            Fill_Rect(x+i, y+i, 1, h-(2*i), c); Fill_Rect(x+w-1-i, y+i, 1, h-(2*i), c);
        }
    }

    /*!
    @brief   Draw a rounded rectangle with w, h no fill color
    @param    x     Top left corner x coordinate
    @param    y     Top left corner y coordinate
    @param    w     Width in pixels
    @param    h     Height in pixels
    @param    r     Radius of corner rounding
    @param    color 16-bit or RGB(r,g,b) Color to draw with
    */
    void Round_Rectangle(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t r,const RGB color = RGB()) {
        const uint16_t c = color.getFC();
        Fill_Rect(x+r, y, w-2*r, 1, c); Fill_Rect(x+r, y+h-1, w-2*r, 1, c);
        Fill_Rect(x, y+r, 1, h-2*r, c); Fill_Rect(x+w-1, y+r, 1, h-2*r, c);
        Circle_Helper(x+r, y+r, r, 1, c); Circle_Helper(x+w-r-1, y+r, r, 2, c);
        Circle_Helper(x+w-r-1, y+h-r-1, r, 4, c); Circle_Helper(x+r, y+h-r-1, r, 8, c);
    }

    /*!
    @brief   Draw a rounded rectangle with w, h no fill color
    @param    x     Top left corner x coordinate
    @param    y     Top left corner y coordinate
    @param    w     Width in pixels
    @param    h     Height in pixels
    @param    r     Radius of corner rounding
    @param    t     Thickness
    @param    color 16-bit or RGB(r,g,b) Color to draw with
    */
    void Round_Rectangle_Thickness(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t r, int16_t t, const RGB color = RGB()) {
        const uint16_t c = color.getFC();
        for(int i = 0; i < t; i++) {
            Round_Rectangle(x+i, y+i, w-(2*i), h-(2*i), r-i, c);
        }
    }
    /*!
    @brief   Draw a rounded rectangle with fill color
    @param    x     Top left corner x coordinate
    @param    y     Top left corner y coordinate
    @param    w     Width in pixels
    @param    h     Height in pixels
    @param    r     Radius of corner rounding
    @param    color 16-bit or RGB(r,g,b) Color to draw/fill with
    */
    void Fill_Round_Rectangle(int16_t x, int16_t y, int16_t w,int16_t h, int16_t r, const RGB color = RGB()) {
        const uint16_t c = color.getFC();
        Fill_Rect(x+r, y, w-2*r, h, c);
        Fill_Circle_Helper(x+w-r-1, y+r, r, 1, h-2*r-1, c);
        Fill_Circle_Helper(x+r, y+r, r, 2, h-2*r-1, c);
    }

    /*!
    @brief     Rotate a point in standard position
    @param    x0  x coordinate of point to rotate. This is passed by reference
                    and updated upon return
    @param    y0  y coordinate of point to rotate. This is passed by reference
                    and updated upon return
    @param    angleDeg  angle to rotate the point by (degrees)
    */
    void Rotate_Point(int16_t &x0, int16_t &y0, int16_t angleDeg) {
        double angleRad = radians(angleDeg);
        double s = sin(angleRad), c = cos(angleRad), x = x0, y = y0;
        x0 = (int16_t)((x * c) - (y * s)); y0 = (int16_t)((x * s) + (y * c));
    }
    
    /*!
    @brief     Draw a rotated rectangle
    @param    cenX  x coordinate of center of rectangle.
                    For even width rectangles, this point
                    represents the pixel to the left of
                    true center.
    @param    cenY  y coordinate of center of rectangle.
                    For even height rectangles, this point
                    represents the pixel above
                    true center.
    @param    w     width of rectangle
    @param    h     height of rectangle
    @param    angleDeg  angle of rotation of rectangle
    @param    color 16-bit 5-6-5 Color to fill/draw with
    */
    void Rotated_Rectangle(int16_t cenX, int16_t cenY, int16_t w, int16_t h, int16_t angleDeg, const RGB color = RGB()) {
        if (w < 1 || h < 1) return; // We don't draw zero dimensioned objects
        const uint16_t c = color.getFC();
        int16_t W = w - 1, H = h - 1;
        int16_t halfW = (W >> 1); int16_t halfH = (H >> 1); // Midpoint should always be integer
        int16_t x0 = W - halfW; int16_t y0 = H - halfH; // bottom-right
        int16_t x1 = -halfW; int16_t y1 = H - halfH; // bottom-left
        int16_t x2 = -halfW; int16_t y2 = -halfH;    // top-left
        int16_t x3 = W - halfW; int16_t y3 = -halfH;    // top-right
        Rotate_Point(x0, y0, angleDeg); Rotate_Point(x1, y1, angleDeg);
        Rotate_Point(x2, y2, angleDeg); Rotate_Point(x3, y3, angleDeg);
        x0 += cenX; x1 += cenX; x2 += cenX; x3 += cenX; y0 += cenY; y1 += cenY; y2 += cenY; y3 += cenY;
        Line(x0, y0, x1, y1, c); // bottom right to bottom left
        Line(x1, y1, x2, y2, c); // bottom left to top left
        Line(x2, y2, x3, y3, c); // top left to top right
        Line(x3, y3, x0, y0, c); // top right to bottom right
    }

    /*!
    @brief     Draw a filled rotated rectangle
    @param    cenX  x coordinate of center of rectangle.
                    For even width rectangles, this point
                    represents the pixel to the left of
                    true center.
    @param    cenY  y coordinate of center of rectangle.
                    For even height rectangles, this point
                    represents the pixel above
                    true center.
    @param    w     width of rectangle
    @param    h     height of rectangle
    @param    angleDeg  angle of rotation of rectangle
    @param    color 16-bit 5-6-5 Color to fill/draw with
    */
    void Fill_Rotated_Rectangle(int16_t cenX, int16_t cenY, int16_t w, int16_t h, int16_t angleDeg, const RGB color = RGB()) {
        if (w < 1 || h < 1) return; // We don't draw zero dimensioned objects
        const uint16_t c = color.getFC();
        int16_t W = w - 1, H = h - 1;
        int16_t halfW = (W >> 1); int16_t halfH = (H >> 1); // Midpoint should always be integer
        int16_t x0 = W - halfW; int16_t y0 = H - halfH; // bottom-right
        int16_t x1 = -halfW; int16_t y1 = H - halfH; // bottom-left
        int16_t x2 = -halfW; int16_t y2 = -halfH;    // top-left
        int16_t x3 = W - halfW; int16_t y3 = -halfH;    // top-right
        Rotate_Point(x0, y0, angleDeg); Rotate_Point(x1, y1, angleDeg);
        Rotate_Point(x2, y2, angleDeg); Rotate_Point(x3, y3, angleDeg);
        x0 += cenX; x1 += cenX; x2 += cenX; x3 += cenX; y0 += cenY; y1 += cenY; y2 += cenY; y3 += cenY;
        Fill_Triangle(x0, y0, x1, y1, x2, y2, c);
        Fill_Triangle(x2, y2, x3, y3, x0, y0, c);
    }

    /*!
    @brief    Draw a circle outline
    @param    x   Center-point x coordinate
    @param    y   Center-point y coordinate
    @param    r   Radius of circle
    @param    color 16-bit or RGB(r,g,b) Color to draw with
    */
    void Circle(int16_t x, int16_t y, int16_t r, const RGB color = RGB()) {
        const uint16_t c = color.getFC();
        int16_t f = 1 - r, ddF_x = 1, ddF_y = -2 * r, x1= 0, y1= r;
        Pixe(x, y+r, c); Pixe(x, y-r, c); Pixe(x+r, y, c); Pixe(x-r, y, c);
        while (x1<y1) {
            if (f >= 0)	{ y1--; ddF_y += 2; f += ddF_y; }
            x1++; ddF_x += 2; f += ddF_x;
            Pixe(x + x1, y + y1, c); Pixe(x - x1, y + y1, c); Pixe(x + x1, y - y1, c); Pixe(x - x1, y - y1, c);
            Pixe(x + y1, y + x1, c); Pixe(x - y1, y + x1, c); Pixe(x + y1, y - x1, c); Pixe(x - y1, y - x1, c);
        }
    }

    /*!
    @brief    Draw a circle outline
    @param    x   Center-point x coordinate
    @param    y   Center-point y coordinate
    @param    r   Radius of circle
    @param    t   Thickness
    @param    color 16-bit or RGB(r,g,b) Color to draw with
    */
    void Circle_Thickness(int16_t x, int16_t y, int16_t r, int16_t t, const RGB color = RGB()) {
        const uint16_t c = color.getFC();
        for(int i = 0; i < t; i++) {
            Circle(x, y, r-i, c); Circle(x+i,y,r,c); Circle(x-i,y,r,c);
        }
    }
    /*!
    @brief    Quarter-circle drawer, used to do circles and roundrects
    @param    x0   Center-point x coordinate
    @param    y0   Center-point y coordinate
    @param    r    Radius of circle
    @param    corners  Mask bit #1, #2, #4, and #8 to indicate which quarters
                        of the circle we're doing
    @param    c     16-bit or RGB(r,g,b) Color to draw with
    */
    void Circle_Helper(int16_t x0, int16_t y0, int16_t r, uint8_t corners, uint16_t c) {
        int16_t f = 1 - r, ddF_x = 1, ddF_y = -2 * r, x = 0, y = r;
        while (x<y)	{
            if (f >= 0)	{ y--; ddF_y += 2; f += ddF_y; }
            x++; ddF_x += 2; f += ddF_x;
            if (corners & 0x4) { Pixe(x0 + x, y0 + y, c); Pixe(x0 + y, y0 + x, c); }
            if (corners & 0x2) { Pixe(x0 + x, y0 - y, c); Pixe(x0 + y, y0 - x, c); }
            if (corners & 0x8) { Pixe(x0 - y, y0 + x, c); Pixe(x0 - x, y0 + y, c); }
            if (corners & 0x1) { Pixe(x0 - y, y0 - x, c); Pixe(x0 - x, y0 - y, c); }
        }
    }

    /*!
    @brief    Draw a circle with filled color
    @param    x   Center-point x coordinate
    @param    y   Center-point y coordinate
    @param    r   Radius of circle
    @param    color 16-bit or RGB(r,g,b) Color to fill with
    */
    void Fill_Circle(int16_t x, int16_t y, int16_t r, const RGB color = RGB()) {
        const uint16_t c = color.getFC();
        Fill_Rect(x, y-r, 1, 2*r+1, c); Fill_Circle_Helper(x, y, r, 3, 0, c);
    }

    /*!
    @brief  Half-circle drawer with fill, used for circles and roundrects
    @param  x0       Center-point x coordinate
    @param  y0       Center-point y coordinate
    @param  r        Radius of circle
    @param  corners  Mask bits indicating which sides of the circle we are
                        doing, left (1) and/or right (2)
    @param  delta    Offset from center-point, used for round-rects
    @param  c        16-bit or RGB(r,g,b) Color to fill with
    */
    void Fill_Circle_Helper(int16_t x0, int16_t y0, int16_t r, uint8_t corners, int16_t delta, uint16_t c) {
        int16_t f = 1 - r, ddF_x = 1, ddF_y = -2 * r, x = 0, y = r, last_y = r;
        while (x < y) {
            x++;
            if (f >= 0) { y--; ddF_y += 2; f += ddF_y; }
            ddF_x += 2; f += ddF_x;
            if (corners & 0x1) {
                Fill_Rect(x0 + x, y0 - y, 1, 2 * y + 1 + delta, c);
                if (y != last_y) Fill_Rect(x0 + last_y, y0 - x + 1, 1, 2 * x - 1 + delta, c);
            }
            if (corners & 0x2) {
                Fill_Rect(x0 - x, y0 - y, 1, 2 * y + 1 + delta, c);
                if (y != last_y) Fill_Rect(x0 - last_y, y0 - x + 1, 1, 2 * x - 1 + delta, c);
            }
            if (y != last_y) last_y = y;
        }
    }

     /*!
    @brief   Draw a triangle with no fill color
    @param    x0  Vertex #0 x coordinate
    @param    y0  Vertex #0 y coordinate
    @param    x1  Vertex #1 x coordinate
    @param    y1  Vertex #1 y coordinate
    @param    x2  Vertex #2 x coordinate
    @param    y2  Vertex #2 y coordinate
    @param    color 16-bit or RGB(r,g,b) Color to draw with
    */
    void Triangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1,int16_t x2, int16_t y2,const RGB color = RGB()) {
        const uint16_t c = color.getFC();
        Line(x0, y0, x1, y1, c); Line(x1, y1, x2, y2, c); Line(x2, y2, x0, y0, c);
    }

    /*!
    @brief     Draw a triangle with color-fill
    @param    x0  Vertex #0 x coordinate
    @param    y0  Vertex #0 y coordinate
    @param    x1  Vertex #1 x coordinate
    @param    y1  Vertex #1 y coordinate
    @param    x2  Vertex #2 x coordinate
    @param    y2  Vertex #2 y coordinate
    @param    color 16-bit or RGB(r,g,b) Color to fill/draw with
    */
    void Fill_Triangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, const RGB color = RGB()) {
        const uint16_t c = color.getFC();
        if (y0 > y1) { SWAP(y0, y1); SWAP(x0, x1); }
        if (y1 > y2) { SWAP(y2, y1); SWAP(x2, x1); }
        if (y0 > y1) { SWAP(y0, y1); SWAP(x0, x1); }
        if (y0 == y2) return; 
        int32_t dy01 = y1 - y0, dy02 = y2 - y0, dy12 = y2 - y1;
        int32_t step01 = (dy01 > 0) ? ((int32_t)(x1 - x0) << 16) / dy01 : 0;
        int32_t step02 = (dy02 > 0) ? ((int32_t)(x2 - x0) << 16) / dy02 : 0;
        int32_t step12 = (dy12 > 0) ? ((int32_t)(x2 - x1) << 16) / dy12 : 0;
        int32_t sa = (int32_t)x0 << 16, sb = (int32_t)x0 << 16;
        for (int16_t y = y0; y < y1; y++) {
            int16_t a = sa >> 16, b = sb >> 16;
            sa += step01; sb += step02;
            if (a > b) SWAP(a, b);
            Fill_Rect(a, y, b - a + 1, 1, c);
        }
        sa = (int32_t)x1 << 16; 
        for (int16_t y = y1; y <= y2; y++) {
            int16_t a = sa >> 16, b = sb >> 16;
            sa += step12; sb += step02;
            if (a > b) SWAP(a, b);
            Fill_Rect(a, y, b - a + 1, 1, c);
        }
    }

    /*!
    @brief    Draw an ellipse outline
    @param    x0   Center-point x coordinate
    @param    y0   Center-point y coordinate
    @param    rw   Horizontal radius of ellipse
    @param    rh   Vertical radius of ellipse
    @param    color 16-bit 5-6-5 Color to draw with
    */
    void Ellipse(int16_t x0, int16_t y0, int16_t rw, int16_t rh, const RGB color = RGB()) {
        // Bresenham's ellipse algorithm
        const uint16_t c = color.getFC();
        int16_t x = 0, y = rh;
        int32_t rw2 = rw * rw, rh2 = rh * rh;
        int32_t twoRw2 = 2 * rw2, twoRh2 = 2 * rh2;
        int32_t decision = rh2 - (rw2 * rh) + (rw2 >> 2);
        while ((twoRh2 * x) < (twoRw2 * y)) {
            Pixe(x0 + x, y0 + y, c); Pixe(x0 - x, y0 + y, c); Pixe(x0 + x, y0 - y, c); Pixe(x0 - x, y0 - y, c);
            x++;
            if (decision < 0) decision += rh2 + (twoRh2 * x); 
                else { decision += rh2 + (twoRh2 * x) - (twoRw2 * y); y--; }
        }
        decision = ((rh2 * (2 * x + 1) * (2 * x + 1)) >> 2) + (rw2 * (y - 1) * (y - 1)) - (rw2 * rh2);
        while (y >= 0) {
            Pixe(x0 + x, y0 + y, c); Pixe(x0 - x, y0 + y, c); Pixe(x0 + x, y0 - y, c); Pixe(x0 - x, y0 - y, c);
            y--;
            if (decision > 0) decision += rw2 - (twoRw2 * y); 
                else { decision += rw2 + (twoRh2 * x) - (twoRw2 * y); x++; }
        }
    }

    /*!
    @brief    Draw an ellipse with filled colour
    @param    x0   Center-point x coordinate
    @param    y0   Center-point y coordinate
    @param    rw   Horizontal radius of ellipse
    @param    rh   Vertical radius of ellipse
    @param    color 16-bit 5-6-5 Color to draw with
    */
    void Fill_Ellipse(int16_t x0, int16_t y0, int16_t rw, int16_t rh, const RGB color = RGB()) {
        // Bresenham's ellipse algorithm
        const uint16_t c = color.getFC();
        int16_t x = 0, y = rh;
        int32_t rw2 = rw * rw, rh2 = rh * rh;
        int32_t twoRw2 = 2 * rw2, twoRh2 = 2 * rh2;
        int32_t decision = rh2 - (rw2 * rh) + (rw2 >> 2);
        while ((twoRh2 * x) < (twoRw2 * y)) {
            x++;
            if (decision < 0) decision += rh2 + (twoRh2 * x); 
            else {
                decision += rh2 + (twoRh2 * x) - (twoRw2 * y);
                Fill_Rect(x0 - (x - 1), y0 + y, 2 * (x - 1) + 1, 1, c);
                Fill_Rect(x0 - (x - 1), y0 - y, 2 * (x - 1) + 1, 1, c);
                y--;
            }
        }
        decision = ((rh2 * (2 * x + 1) * (2 * x + 1)) >> 2) + (rw2 * (y - 1) * (y - 1)) - (rw2 * rh2);
        while (y >= 0) {
            Fill_Rect(x0 - x, y0 + y, 2 * x + 1, 1, c); Fill_Rect(x0 - x, y0 - y, 2 * x + 1, 1, c);
            y--;
            if (decision > 0) decision += rw2 - (twoRw2 * y);
                else { decision += rw2 + (twoRh2 * x) - (twoRw2 * y); x++; }
        }
    }

    /*!
    @brief    Draw icon bytes
    @param    icon Icon bytes
    @param    x    x coordinate
    @param    y    y coordinate
    @param    color 16-bit 5-6-5 Color to draw with
    */
    void DrawIcon(const uint8_t icon[], int16_t x, int16_t y, const RGB color) {
        int16_t h = icon[0], w = icon[1], bpr = (w + 7) >> 3;
        for(int he=0; he<h; he++) {
            int row = 0;
            int bits = 256;
            int _bytePos = bpr * he + 2;
            int _byte = icon[_bytePos];
            for(int i=0; i<w; i++){
                if ( bits == 1 ) {
                    bits = 256;
                    _byte = icon[++_bytePos];
                }
                if ((_byte & (bits >>=1)) != 0 ) { Pixe(x+(row++), y+he, color); } else row++;
            }
        }      
    }

    /*!
    @brief    Print number int
    @param    num  Number
    @param    lenght  Number lenght (default 0)
    @param    filler  Filler (default ' ')
    @param    system  Number system (default 10)
    */
    void Print_Number_Int(long long num, int16_t length, uint8_t filler, int16_t system) {
        uint8_t *p = num_buf+26;
        bool flag = false;
        int16_t len = 0,nlen = 0,left_len = 0,i = 0, h=0;
        *p = '\0';
        if(0 == num) {
            *(--p) = '0';
            len = 1;
        }
        else if(num < 0) {
             num = -num;
             flag = true;
        }
        for(len,num;(num > 0) && (len < 10);len++) {
            if(num%system > 9) { *(--p) = 'A' + (num%system-10); }
            else *(--p) = '0' + num%system;
            num = num/system;
        }
        if(flag) { *(--p) = '-'; }
        if(system==16) { *(--p) ='x'; *(--p) ='0'; h = 2; }
        if(system==8) { *(--p) ='o'; *(--p) ='0'; h = 2; }

        if(length > (len + flag + 1 + h)) {
            if(length > sizeof(num_buf)) { nlen = sizeof(num_buf) - len - flag - 1 - h; }
            else nlen = length - len - flag - 1 - h;
            for(i = 0;i< nlen;i++) *(--p) = filler;
            left_len = sizeof(num_buf) - nlen - len - flag - 1 - h;
        } 
        else left_len = sizeof(num_buf) - len - flag - 1 - h;
        
        for(i = 0; i < (sizeof(num_buf)-left_len);i++) num_buf[i] = num_buf[left_len + i];
        num_buf[i] = '\0';
        text_len = (i-1);
        text = num_buf;
    }

    /*!
    @brief    Print number int
    @param    num  Float Number
    @param    dec  Decimal lenght (default 2)
    @param    divider Float divider (default '.')
    @param    lenght  Number lenght (default 0)
    @param    filler  Filler (default ' ')
    */
    void Print_Number_Float(double num, uint8_t dec, uint8_t divider, int16_t length, uint8_t filler) {
        uint8_t * p = num_buf;
        bool flag = false;
        int16_t i = 0;
        if(dec<1) dec=1; else if(dec>5) dec=5;
        if(num<0) flag = true;
        dtostrf(num, length, dec, (char *)num_buf);
        if(divider != '.') for(i;i < (int16_t)sizeof(num_buf);i++) if('.' == *(p+i)) *(p+i) = divider; 
        if(filler != ' ') {
            if(flag) {
                *p = '-'; i = 1;
                for(i;i < (int16_t)sizeof(num_buf);i++) if((*(p+i) == ' ') || (*(p+i) == '-')) *(p+i) = filler;  
            }
            else {
                i = 0;
                for(i;i < (int16_t)sizeof(num_buf);i++) if(' ' == *(p+i)) *(p+i) = filler; 
            }
        }
        text_len = strlen((const char*)num_buf);
        text = num_buf;
    }
};

