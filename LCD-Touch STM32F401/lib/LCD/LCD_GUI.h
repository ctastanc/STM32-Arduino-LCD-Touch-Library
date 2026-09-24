#pragma once

#include <Arduino.h>	
#include <type_traits>
#include <string_view>

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
// Color 16 bit or RGB(r,g,b)
struct RGB {
    uint16_t val;
    constexpr RGB(uint16_t c) : val(c) {}
    constexpr RGB(uint8_t r, uint8_t g, uint8_t b) : val(((uint16_t)(r&0xF8)<<8) | ((uint16_t)(g&0xFC)<<3) | (b>>3)) {} 
    constexpr operator uint16_t() const { return val; }
}__attribute__((packed));

template <typename Derived>
class LCD_GUI
{
    Derived& impl() { return static_cast<Derived&>(*this); }
    const Derived& impl() const { return static_cast<const Derived&>(*this); }
	public:

    /*!
        @brief    Display char, string, number and float
        @param    val    Char, string, number or float number
        @param    x      x coordinate
        @param    y      y coordinate
        @param    size   Text size
        @param    fc     Fore color for 16 bit or RGB(r,g,b) color     
        @param    bc     Back color for 16 bit or RGB(r,g,b) color (default:0)
        @param    mode   1:Back color, 0:No back color (default:0)
        @param    system Number system (default 10)
        @param    dec    Decimal point (default 2)
    */
    template <typename T> 	
    void Print(T val, int16_t x, int16_t y, uint8_t size, const RGB& fc, const RGB& bc=0, bool mode=0, int16_t system = 10, uint8_t dec = 2) {
        text_mode = mode;
        text_size = size;
        text_fc = fc;
        text_bc = bc;  
        text_x = x;
        text_y = y;
        // Control for static texts, arrays, and pointers
        if constexpr (std::is_convertible_v<T, std::string_view> || std::is_same_v<std::decay_t<T>, uint8_t*> || 
                    std::is_same_v<std::decay_t<T>, const uint8_t*>) { 
            std::string_view sv;
            if constexpr (std::is_pointer_v<std::decay_t<T>> && sizeof(std::remove_pointer_t<std::decay_t<T>>) == 1) {
                sv = std::string_view(reinterpret_cast<const char*>(val));
            } else { sv = std::string_view(val); }
            text_len = sv.length();
            text = (const uint8_t*)sv.data();
            Print_Str();
        } 
        // If an Arduino-style dynamic String object is received
        else if constexpr (std::is_same_v<T, String>) {	
            text_len = val.length();
            text = (const uint8_t *)(val.c_str());
            Print_Str(); 
        } 
        else if constexpr (std::is_floating_point_v<T>) { Print_Number_Float((double)val, dec, '.', 0, ' '); } 
        else if constexpr (std::is_integral_v<T>) {	Print_Number_Int((long long)val, 0, ' ', system); }
    }
    // CRTP forwarding methods (non-virtual, 100% inlineable, zero-cost)
    void Draw_Pixe(int16_t x, int16_t y, uint16_t color) { impl().Draw_Pixe(x, y, color); }
    void Fill_Rect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t c) { impl().Fill_Rect(x, y, w, h, c); }
    void Draw_Bit_Map(int16_t x, int16_t y, int16_t sx, int16_t sy, const uint16_t *data, int16_t scale) { impl().Draw_Bit_Map(x, y, sx, sy, data, scale); }
    void Push_Any_Color(uint16_t * block, int16_t n, bool first, uint8_t flags) { impl().Push_Any_Color(block, n, first, flags); }
    int16_t Read_GRAM(int16_t x, int16_t y, uint16_t *block, int16_t w, int16_t h) { return impl().Read_GRAM(x, y, block, w, h); }
    void Fill_Scree(uint16_t color) { impl().Fill_Scree(color); }
    void Print_Str() { impl().Print_Str(); }
    
    LCD_GUI(void) {
        text_bc = 0xF800;
        text_fc = 0x07E0;
        draw_color= RGB(0xF800);
        text_size = 1;
        text_mode = 0;
    }
   
    void Set_Text_Cursor(int16_t x, int16_t y) { text_x = x; text_y = y; }

    int16_t Get_Text_X_Cursor(void) const { return text_x; }
 
    int16_t Get_Text_Y_Cursor(void) const { return text_y; }

    void Set_Text_Color(const RGB& color) { text_fc = color; }
  
    uint16_t Get_Text_Color(void) const {	return text_fc; }

    void Set_Text_Back_Color(const RGB& color) { text_bc = color; }

    uint16_t Get_Text_Back_Color(void) const { return text_bc; }

    void Set_Text_Size(uint8_t s) { text_size = s; }

    uint8_t Get_Text_Size(void) const { return text_size; }

    void Set_Text_Mode(bool mode) { text_mode = mode; }

    bool Get_Text_Mode(void) const { return text_mode; }
    
    void Set_Draw_Color(const RGB& color) { draw_color = color; } //set 16bits or RGB draw color

    uint16_t Get_Draw_Color(void) const { return draw_color; }

    void Fill_Screen(const RGB& color) { Fill_Scree(color); }

    /*!
    @brief    Write a pixel.
        @param   x   x coordinate
        @param   y   y coordinate
        @param   color 16-bit or RGB(r,g,b) Color to fill with
    */
    void Pixel(int16_t x, int16_t y, const RGB& color) { Draw_Pixe(x, y, color); }

    /*!
    @brief    Read a pixel.
        @param   x   x coordinate
        @param   y   y coordinate
        @param   color return 16-bit Color
    */
    uint16_t Read_Pixel(int16_t x, int16_t y) {
        uint16_t color;
        Read_GRAM(x, y, &color, 1, 1);
        return color;
    }

    /*!
    @brief    Draw a perfectly vertical line (this is often optimized in a
    subclass!)
        @param    x   Top-most x coordinate
        @param    y   Top-most y coordinate
        @param    h   Height in pixels
        @param    color 16-bit or RGB(r,g,b) Color to fill with
    */
    void Fast_VLine(int16_t x, int16_t y, int16_t h, const RGB& color) {
         Fill_Rect(x, y, 1, h, color);
    }

    /*!
    @brief    Draw a perfectly horizontal line (this is often optimized in a
    subclass!)
        @param    x   Left-most x coordinate
        @param    y   Left-most y coordinate
        @param    w   Width in pixels
        @param    color 16-bit or RGB(r,g,b) Color to fill with
    */
    void Fast_HLine(int16_t x, int16_t y, int16_t w,const RGB& color) {
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
    void Line(int16_t x1, int16_t y1, int16_t x2, int16_t y2, const RGB& color) {
        if (x1 == x2) {
            if (y1 > y2) swap(y1, y2);
            Fast_VLine(x1, y1, y2 - y1 + 1, color);
        } else if (y1 == y2) {
            if (x1 > x2) swap(x1, x2);
            Fast_HLine(x1, y1, x2 - x1 + 1, color);
        } else {
            int16_t dx = abs(x2 - x1);
            int16_t dy = abs(y2 - y1);
            if (dx >= dy) {
                // X-major: horizontal run slices
                if (x1 > x2) { swap(x1, x2); swap(y1, y2); }
                int16_t ystep = (y1 < y2) ? 1 : -1;
                int16_t err = dx / 2, run_start = x1;
                for (int16_t x = x1; x < x2; x++) {
                    err -= dy;
                    if (err < 0) {
                        int16_t len = x - run_start + 1;
                        if (len == 1) Draw_Pixe(run_start, y1, color); else Fast_HLine(run_start, y1, len, color);
                        run_start = x + 1;
                        y1 += ystep;
                        err += dx;
                    }
                }
                int16_t len = x2 - run_start + 1;
                if (len == 1) Draw_Pixe(run_start, y1, color); else Fast_HLine(run_start, y1, len, color);
            } else {
                // Y-major: vertical run slices
                if (y1 > y2) { swap(x1, x2); swap(y1, y2); }
                int16_t xstep = (x1 < x2) ? 1 : -1;
                int16_t err = dy / 2, run_start = y1;
                for (int16_t y = y1; y < y2; y++) {
                    err -= dx;
                    if (err < 0) {
                        int16_t len = y - run_start + 1;
                        if (len == 1) Draw_Pixe(x1, run_start, color); else Fast_VLine(x1, run_start, len, color);
                        run_start = y + 1;
                        x1 += xstep;
                        err += dy;
                    }
                }
                int16_t len = y2 - run_start + 1;
                if (len == 1) Draw_Pixe(x1, run_start, color); else Fast_VLine(x1, run_start, len, color);
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
    void Rectangle(int16_t x, int16_t y, int16_t w, int16_t h, const RGB& color) {
        Fill_Rect(x, y, w, 1, color);
        Fill_Rect(x, y+h-1, w, 1, color);
        Fill_Rect(x, y, 1, h, color);
        Fill_Rect(x+w-1, y, 1, h, color);
    }

    /*!
    @brief    Fill a rectangle completely with one color. 
        @param    x     Top left corner x coordinate
        @param    y     Top left corner y coordinate
        @param    w     Width in pixels
        @param    h     Height in pixels
        @param    color 16-bit or RGB(r,g,b) Color to fill with
    */
      void Fill_Rectangle(int16_t x, int16_t y, int16_t w, int16_t h, const RGB& color) {
        Fill_Rect(x, y, w, h, color);
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
    void Round_Rectangle(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t r,const RGB& color) {
        Fill_Rect(x+r, y, w-2*r, 1, color);
        Fill_Rect(x+r, y+h-1, w-2*r, 1, color);
        Fill_Rect(x, y+r, 1, h-2*r, color);
        Fill_Rect(x+w-1, y+r, 1, h-2*r, color);
        Circle_Helper(x+r, y+r, r, 1, color);
        Circle_Helper(x+w-r-1, y+r, r, 2, color);
        Circle_Helper(x+w-r-1, y+h-r-1, r, 4, color);
        Circle_Helper(x+r, y+h-r-1, r, 8, color);
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
    void Fill_Round_Rectangle(int16_t x, int16_t y, int16_t w,int16_t h, int16_t r,const RGB& color) {
        Fill_Rect(x+r, y, w-2*r, h, color);
        Fill_Circle_Helper(x+w-r-1, y+r, r, 1, h-2*r-1, color);
        Fill_Circle_Helper(x+r, y+r, r, 2, h-2*r-1, color);
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
        double s = sin(angleRad);
        double c = cos(angleRad);
        double x = x0;
        double y = y0;
        // Rotate point
        x0 = (int16_t)((x * c) - (y * s));
        y0 = (int16_t)((x * s) + (y * c));
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
    void Rotated_Rectangle(int16_t cenX, int16_t cenY, int16_t w, int16_t h, int16_t angleDeg, uint16_t color) {
        if (w < 1 || h < 1) return; // We don't draw zero dimensioned objects
        int16_t W = w - 1;
        int16_t H = h - 1;
        int16_t halfW = (W / 2); // Midpoint should always be integer
        int16_t halfH = (H / 2); // Midpoint should always be integer
        int16_t x0 = W - halfW; // bottom-right
        int16_t y0 = H - halfH; // bottom-right
        int16_t x1 = -halfW;    // bottom-left
        int16_t y1 = H - halfH; // bottom-left
        int16_t x2 = -halfW;    // top-left
        int16_t y2 = -halfH;    // top-left
        int16_t x3 = W - halfW; // top-right
        int16_t y3 = -halfH;    // top-right
        Rotate_Point(x0, y0, angleDeg);
        Rotate_Point(x1, y1, angleDeg);
        Rotate_Point(x2, y2, angleDeg);
        Rotate_Point(x3, y3, angleDeg);
        x0 += cenX;
        x1 += cenX;
        x2 += cenX;
        x3 += cenX;
        y0 += cenY;
        y1 += cenY;
        y2 += cenY;
        y3 += cenY;
        Line(x0, y0, x1, y1, color); // bottom right to bottom left
        Line(x1, y1, x2, y2, color); // bottom left to top left
        Line(x2, y2, x3, y3, color); // top left to top right
        Line(x3, y3, x0, y0, color); // top right to bottom right
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
    void Fill_Rotated_Rectangle(int16_t cenX, int16_t cenY, int16_t w, int16_t h, int16_t angleDeg, uint16_t color) {
        if (w < 1 || h < 1) return; // We don't draw zero dimensioned objects
        int16_t W = w - 1;
        int16_t H = h - 1;
        int16_t halfW = (W / 2); // Midpoint should always be integer
        int16_t halfH = (H / 2); // Midpoint should always be integer
        int16_t x0 = W - halfW; // bottom-right
        int16_t y0 = H - halfH; // bottom-right
        int16_t x1 = -halfW;    // bottom-left
        int16_t y1 = H - halfH; // bottom-left
        int16_t x2 = -halfW;    // top-left
        int16_t y2 = -halfH;    // top-left
        int16_t x3 = W - halfW; // top-right
        int16_t y3 = -halfH;    // top-right
        Rotate_Point(x0, y0, angleDeg);
        Rotate_Point(x1, y1, angleDeg);
        Rotate_Point(x2, y2, angleDeg);
        Rotate_Point(x3, y3, angleDeg);
        x0 += cenX;
        x1 += cenX;
        x2 += cenX;
        x3 += cenX;
        y0 += cenY;
        y1 += cenY;
        y2 += cenY;
        y3 += cenY;
        Fill_Triangle(x0, y0, x1, y1, x2, y2, color);
        Fill_Triangle(x2, y2, x3, y3, x0, y0, color);
    }

    /*!
    @brief    Draw a circle outline
        @param    x   Center-point x coordinate
        @param    y   Center-point y coordinate
        @param    r   Radius of circle
        @param    color 16-bit or RGB(r,g,b) Color to draw with
    */
    void Circle(int16_t x, int16_t y, int16_t r, const RGB& color) {
        int16_t f = 1 - r, ddF_x = 1, ddF_y = -2 * r, x1= 0, y1= r;
        Draw_Pixe(x, y+r, color);
        Draw_Pixe(x, y-r, color);
        Draw_Pixe(x+r, y, color);
        Draw_Pixe(x-r, y, color);
        while (x1<y1) {
            if (f >= 0)	{ y1--; ddF_y += 2; f += ddF_y; }
            x1++; ddF_x += 2; f += ddF_x;
            Draw_Pixe(x + x1, y + y1, color);
            Draw_Pixe(x - x1, y + y1, color);
            Draw_Pixe(x + x1, y - y1, color);
            Draw_Pixe(x - x1, y - y1, color);
            Draw_Pixe(x + y1, y + x1, color);
            Draw_Pixe(x - y1, y + x1, color);
            Draw_Pixe(x + y1, y - x1, color);
            Draw_Pixe(x - y1, y - x1, color);
        }
    }

    /*!
        @brief    Quarter-circle drawer, used to do circles and roundrects
        @param    x0   Center-point x coordinate
        @param    y0   Center-point y coordinate
        @param    r    Radius of circle
        @param    corners  Mask bit #1, #2, #4, and #8 to indicate which quarters
                           of the circle we're doing
        @param    color 16-bit or RGB(r,g,b) Color to draw with
    */
    void Circle_Helper(int16_t x0, int16_t y0, int16_t r, uint8_t corners, uint16_t color) {
        int16_t f = 1 - r, ddF_x = 1, ddF_y = -2 * r, x = 0, y = r;
        while (x<y)	{
            if (f >= 0)	{ y--; ddF_y += 2; f += ddF_y; }
            x++; ddF_x += 2; f += ddF_x;
            if (corners & 0x4) {
                Draw_Pixe(x0 + x, y0 + y, color);
                Draw_Pixe(x0 + y, y0 + x, color);
            }
            if (corners & 0x2) {
                Draw_Pixe(x0 + x, y0 - y, color);
                Draw_Pixe(x0 + y, y0 - x, color);
            }
            if (corners & 0x8) {
                Draw_Pixe(x0 - y, y0 + x, color);
                Draw_Pixe(x0 - x, y0 + y, color);
            }
            if (corners & 0x1) {
                Draw_Pixe(x0 - y, y0 - x, color);
                Draw_Pixe(x0 - x, y0 - y, color);
            }
        }
    }

    /*!
    @brief    Draw a circle with filled color
        @param    x   Center-point x coordinate
        @param    y   Center-point y coordinate
        @param    r   Radius of circle
        @param    color 16-bit or RGB(r,g,b) Color to fill with
    */
    void Fill_Circle(int16_t x, int16_t y, int16_t r, const RGB& color) {
        Fill_Rect(x, y-r, 1, 2*r+1, color);
        Fill_Circle_Helper(x, y, r, 3, 0, color);
    }

    /*!
        @brief  Half-circle drawer with fill, used for circles and roundrects
        @param  x0       Center-point x coordinate
        @param  y0       Center-point y coordinate
        @param  r        Radius of circle
        @param  corners  Mask bits indicating which sides of the circle we are
                         doing, left (1) and/or right (2)
        @param  delta    Offset from center-point, used for round-rects
        @param  color    16-bit or RGB(r,g,b) Color to fill with
    */
    void Fill_Circle_Helper(int16_t x0, int16_t y0, int16_t r, uint8_t corners, int16_t delta, uint16_t color) {
        int16_t f = 1 - r, ddF_x = 1, ddF_y = -2 * r, x = 0, y = r, last_y = r;
        while (x < y) {
            x++;
            if (f >= 0) {
                y--;
                ddF_y += 2;
                f += ddF_y;
            }
            ddF_x += 2;
            f += ddF_x;
            if (corners & 0x1) {
                Fill_Rect(x0 + x, y0 - y, 1, 2 * y + 1 + delta, color);
                if (y != last_y) Fill_Rect(x0 + last_y, y0 - x + 1, 1, 2 * x - 1 + delta, color);
            }
            if (corners & 0x2) {
                Fill_Rect(x0 - x, y0 - y, 1, 2 * y + 1 + delta, color);
                if (y != last_y) Fill_Rect(x0 - last_y, y0 - x + 1, 1, 2 * x - 1 + delta, color);
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
    void Triangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1,int16_t x2, int16_t y2,const RGB& color) {
        Line(x0, y0, x1, y1, color);
        Line(x1, y1, x2, y2, color);
        Line(x2, y2, x0, y0, color);
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
    void Fill_Triangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, const RGB& color) {
        int16_t a, b, y, last;
        if (y0 > y1) { swap(y0, y1); swap(x0, x1); }
        if (y1 > y2) { swap(y2, y1); swap(x2, x1); }
        if (y0 > y1) { swap(y0, y1); swap(x0, x1); }

        if (y0 == y2) return; 

        int16_t dx01 = x1 - x0, dy01 = y1 - y0;
        int16_t dx02 = x2 - x0, dy02 = y2 - y0;
        int16_t dx12 = x2 - x1, dy12 = y2 - y1;
        int32_t sa = 0, sb = 0;

        if (y1 == y2) last = y1; else last = y1 - 1; 

        for (y = y0; y <= last; y++) {
            a = x0 + sa / dy01;
            b = x0 + sb / dy02;
            sa += dx01;
            sb += dx02;
            if (a > b) swap(a, b);
            Fill_Rect(a, y, b - a + 1, 1, color);
        }

        sa = (int32_t)dx12 * (y - y1);
        sb = (int32_t)dx02 * (y - y0);
        for (; y <= y2; y++) {
            a = x1 + sa / dy12;
            b = x0 + sb / dy02;
            sa += dx12;
            sb += dx02;
            if (a > b) swap(a, b);
            Fill_Rect(a, y, b - a + 1, 1, color);
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
    void Ellipse(int16_t x0, int16_t y0, int16_t rw, int16_t rh, const RGB& color) {
        // Bresenham's ellipse algorithm
        int16_t x = 0, y = rh;
        int32_t rw2 = rw * rw, rh2 = rh * rh;
        int32_t twoRw2 = 2 * rw2, twoRh2 = 2 * rh2;
        int32_t decision = rh2 - (rw2 * rh) + (rw2 / 4);
        // region 1
        while ((twoRh2 * x) < (twoRw2 * y)) {
            Draw_Pixe(x0 + x, y0 + y, color);
            Draw_Pixe(x0 - x, y0 + y, color);
            Draw_Pixe(x0 + x, y0 - y, color);
            Draw_Pixe(x0 - x, y0 - y, color);
            x++;
            if (decision < 0) {
            decision += rh2 + (twoRh2 * x);
            } else {
            decision += rh2 + (twoRh2 * x) - (twoRw2 * y);
            y--;
            }
        }
        // region 2
        decision = ((rh2 * (2 * x + 1) * (2 * x + 1)) >> 2) +
                    (rw2 * (y - 1) * (y - 1)) - (rw2 * rh2);
        while (y >= 0) {
            Draw_Pixe(x0 + x, y0 + y, color);
            Draw_Pixe(x0 - x, y0 + y, color);
            Draw_Pixe(x0 + x, y0 - y, color);
            Draw_Pixe(x0 - x, y0 - y, color);
            y--;
            if (decision > 0) {
            decision += rw2 - (twoRw2 * y);
            } else {
            decision += rw2 + (twoRh2 * x) - (twoRw2 * y);
            x++;
            }
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
    void Fill_Ellipse(int16_t x0, int16_t y0, int16_t rw, int16_t rh, uint16_t color) {
        // Bresenham's ellipse algorithm
        int16_t x = 0, y = rh;
        int32_t rw2 = rw * rw, rh2 = rh * rh;
        int32_t twoRw2 = 2 * rw2, twoRh2 = 2 * rh2;
        int32_t decision = rh2 - (rw2 * rh) + (rw2 / 4);
        // region 1
        while ((twoRh2 * x) < (twoRw2 * y)) {
            x++;
            if (decision < 0) {
                decision += rh2 + (twoRh2 * x);
            } else {
                decision += rh2 + (twoRh2 * x) - (twoRw2 * y);
                Fast_HLine(x0 - (x - 1), y0 + y, 2 * (x - 1) + 1, color);
                Fast_HLine(x0 - (x - 1), y0 - y, 2 * (x - 1) + 1, color);
                y--;
            }
        }
        // region 2
        decision = ((rh2 * (2 * x + 1) * (2 * x + 1)) >> 2) + (rw2 * (y - 1) * (y - 1)) - (rw2 * rh2);
        while (y >= 0) {
            Fast_HLine(x0 - x, y0 + y, 2 * x + 1, color);
            Fast_HLine(x0 - x, y0 - y, 2 * x + 1, color);
            y--;
            if (decision > 0) {
                decision += rw2 - (twoRw2 * y);
            } else {
                decision += rw2 + (twoRh2 * x) - (twoRw2 * y);
                x++;
            }
        }
    }

    /*!
    @brief    Print number int
        @param    num  Number
        @param    x    x coordinate
        @param    y    y coordinate
        @param    lenght  Number lenght (default 0)
        @param    filler  Filler (default ' ')
        @param    system  Number system (default 10)
    */
    void Print_Number_Int(long long num, int16_t length, uint8_t filler, int16_t system) {
        uint8_t st[27] = {0};
        uint8_t *p = st+26;
        bool flag = false;
        int16_t len = 0,nlen = 0,left_len = 0,i = 0;
        *p = '\0';
        if(0 == num) {
            *(--p) = '0';
            len = 1;
        }
        else if(num < 0) {
             num = -num;
             flag = true;
        }
        while((num > 0) && (len < 10)) {
            if(num%system > 9) { *(--p) = 'A' + (num%system-10); }
            else *(--p) = '0' + num%system;
            num = num/system;
            len++;
        }
        if(flag) { *(--p) = '-'; }
        if(length > (len + flag + 1)) {
            if(length > sizeof(st)) { nlen = sizeof(st) - len - flag - 1; }
            else nlen = length - len - flag - 1;
            for(i = 0;i< nlen;i++) *(--p) = filler;
            left_len = sizeof(st) - nlen - len - flag - 1;
        } 
        else left_len = sizeof(st) - len - flag - 1;
        for(i = 0; i < (sizeof(st)-left_len);i++) st[i] = st[left_len + i];
        st[i] = '\0';
        text_len = static_cast<size_t>(i-1);
        text=st;
        Print_Str();
    }

    /*!
    @brief    Print number int
        @param    num  Float Number
        @param    dec  Decimal lenght (default 2)
        @param    x    x coordinate
        @param    y    y coordinate
        @param    divider Float divider (default '.')
        @param    lenght  Number lenght (default 0)
        @param    filler  Filler (default ' ')
    */
    void Print_Number_Float(double num, uint8_t dec, uint8_t divider, int16_t length, uint8_t filler) {
        uint8_t st[27] = {0};
        uint8_t * p = st;
        bool flag = false;
        int16_t i = 0;
        if(dec<1) dec=1; else if(dec>5) dec=5;
        if(num<0) flag = true;
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
        size_t str_len = 0;
        while (str_len < sizeof(st) && st[str_len] != '\0') {
            str_len++;
        }
        text_len = str_len;
        text=st;
        Print_Str();
    }
    
	//protected:
	int16_t text_x, text_y;
	uint16_t text_fc, text_bc, draw_color; 
	uint8_t text_size;
    size_t text_len;
    const uint8_t *text;
	bool text_mode; 
};

