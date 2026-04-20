#ifndef _LCD_KBV_H_
#define _LCD_KBV_H_

#include <LCD_GUI.h>
#include <mcu_regs.h>
#include <lcd_regs.h>
#include <LCD_font.h>

#define PORTRAIT  0
#define LANDSCAPE 1
#define PORTRAIT_REV 2
#define LANDSCAPE_REV 3

class LCD_KBV:public LCD_GUI
{
	public:
    static LCD_KBV& getInstance() {
        static LCD_KBV instance(1); 
        return instance;
    }
    uint8_t char_spc = 2;
	int16_t Width, Height, rotation, rot_val;

    LCD_KBV(uint16_t m) {  
        ENABLE_AND_SET_PORTS();
        RS_DATA; CS_H; WR_H; RD_H; RST_L; RST_H;
        rotation  = 0;
        SET_WRITE_DIR();
        Width  = WIDTH;
        Height = HEIGHT;
    }

    void Init_LCD(void) {
        reset(); delay(100);//200
        if constexpr( LCD_DRIVER == ID_932X ) {	init_table16(_regValues, sizeof(_regValues));}
        else if constexpr(LCD_DRIVER == ID_9341 || LCD_DRIVER == ID_HX8357D || LCD_DRIVER == ID_7575 || LCD_DRIVER 
                        == ID_9486 || LCD_DRIVER == ID_7735    || LCD_DRIVER == ID_9488 || LCD_DRIVER == ID_9481 ) 
            { init_table8(_regValues, sizeof(_regValues)); }
        Set_Rotation(rotation);
        Invert_Display(false);
    }

    void init_table8(const void *table, int16_t size) {
        uint8_t i;
        uint8_t *p = (uint8_t *) table, dat[MAX_REG_NUM];
        while (size > 0) {
            uint8_t cmd = *p++;
            uint8_t len = *p++;
            if (cmd == TFTLCD_DELAY8) { delay(len); len = 0; }
            else {
                for (i=0; i<len; i++) { dat[i] = *p++;}
                Push_Command(cmd, dat, len);
            }
            size -= len + 2;
        }
    }

    void init_table16(const void *table, int16_t size) {
        uint16_t *p = (uint16_t *) table;
        while (size > 0) {
            uint16_t cmd = *p++;
            uint16_t d   = *p++;
            if (cmd == TFTLCD_DELAY16) { delay(d); }
            else { CMDDATA16(cmd, d); }
            size -= 2 * sizeof(int16_t);
        }
    }

    void reset(void) {
        CS_H; RD_H; WR_H;
        RST_L;	delay(2); RST_H;
        CS_L;
        CMD8(0x00);
        for(uint8_t i=0; i<3; i++) { WR_L; WR_H; }
        CS_H;
    }

    void Push_Command(uint8_t cmd, uint8_t *block, int8_t N) {
        CS_L;
        CMD8(cmd);
        while (N-- > 0) {
            uint8_t u8 = *block++;
            DATA8(u8);
            if(N && (LCD_DRIVER == ID_7575)) { cmd++; CMD8(cmd); }
        }
        CS_H;
    }

    void Set_LR(void) {
        CMDDATA8(HX8347G_COLADDREND_HI,(Width-1)>>8);
        CMDDATA8(HX8347G_COLADDREND_LO,Width-1);
        CMDDATA8(HX8347G_ROWADDREND_HI,(Height-1)>>8);
        CMDDATA8(HX8347G_ROWADDREND_LO,Height-1);
    }

    void Push_Any_Color(uint16_t * block, int16_t n, bool first, uint8_t flags) {
        uint16_t color;
        CS_L;
        if (first) { 
            if constexpr(LCD_DRIVER == ID_932X) {CMD8(ILI932X_START_OSC);} 
            CMD8(MW); 
        }
        while (n-- > 0) {
            color = *block++;
            DATA16(color);
        }
        CS_H;
    }

    void Push_Any_Color(uint8_t * block, int16_t n, bool first, uint8_t flags) {
        uint16_t color;
        bool isbigend = (flags & 2) != 0;
        CS_L;
        if (first) { 
            if constexpr(LCD_DRIVER == ID_932X) {CMD8(ILI932X_START_OSC);} 
            CMD8(MW); 
        }
        while (n-- > 0) {
            uint8_t h, l;
            h = *block++; l = *block++; 
            color = isbigend ? (h << 8 | l) : (l << 8 | h);
            DATA16(color);
        }
        CS_H;
    }

    uint16_t Read_Reg(uint16_t reg, int8_t index) {
        uint16_t ret;
        CS_L;
        CMD16(reg);
        SET_READ_DIR();
        delay(1);
        do { READ16(ret); } while (--index >= 0);
        CS_H;
        SET_WRITE_DIR();
        return ret;
    }

    uint16_t Read_ID(void) {
        uint16_t ret;
        if ((Read_Reg(0x04,0) == 0x00)&&(Read_Reg(0x04,1) == 0x8000)) {
            uint8_t buf[] = {0xFF, 0x83, 0x57};
            Push_Command(HX8357D_SETC, buf, sizeof(buf));
            ret = (Read_Reg(0xD0,0) << 16) | Read_Reg(0xD0,1);
            if((ret == 0x990000) || (ret == 0x900000)) { return 0x9090; }
        }
        ret = Read_Reg(0xD3,1);
        if     (ret == 0x9341) { return 0x9341; }
        else if(ret == 0x9486) { return 0x9486; }
        else if(ret == 0x9488) { return 0x9488; }
        else                   { return Read_Reg(0, 0); }
    }

    int16_t Read_GRAM(int16_t x, int16_t y, uint16_t *block, int16_t w, int16_t h) {
        uint16_t ret, dummy;
        int16_t n = w * h;
        uint8_t r, g, b;
        CS_L;
        SET_ADDR_WINDOW(x, y, x+w-1, y+h-1);
        while (n > 0) {
            CMD16(MR);
            SET_READ_DIR();
            #if(LCD_DRIVER == ID_932X) 
            while(n) {
                for(int i=0; i<2; i++) { READ8(r); READ8(r); READ8(r); READ8(g); }
                *block++ = (r<<8 | g); n--;
            }
            SET_ADDR_WINDOW(0, 0, Width-1, Height-1);
            #else 
            READ8(r);
            while (n) {
                if(R24BIT == 1) {
                    READ8(r); READ8(g); READ8(b);
                    ret = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3);
                }
                else { READ16(ret); }
                *block++ = ret; n--;
            }
            #endif
            SET_WRITE_DIR();
        }
        CS_H;
        return 0;
    }

    void Vert_Scroll(int16_t top, int16_t scrollines, int16_t offset) {
        int16_t bfa = HEIGHT - top - scrollines;
        int16_t vsp;
        if (offset <= -scrollines || offset >= scrollines) offset = 0;
        vsp = top + offset;
        if (offset < 0) vsp += scrollines;
        if constexpr(LCD_DRIVER == ID_932X) {CMDDATA8(SC1, (1<<1)|0x1); CMDDATA8(SC2, vsp);}
        else { 
            uint8_t d[6];
            d[0]=top>>8; d[1]=top; d[2]=scrollines>>8; d[3]=scrollines; d[4]=bfa>>8; d[5]=bfa;
            Push_Command(SC1, d, 6);
            d[0]=vsp>>8; d[1]=vsp;
            Push_Command(SC2, d, 2);
            if constexpr(LCD_DRIVER == ID_7575) { d[0]=(offset!=0)?0x08:0; Push_Command(0x01,d,1); }
            else if (offset == 0) { Push_Command(0x13, NULL, 0); }
        }
    }

    void Set_Rotation(uint8_t r) {
        rotation = r & 3;
        Width  = (rotation & 1) ? HEIGHT : WIDTH;
        Height = (rotation & 1) ? WIDTH  : HEIGHT;
        CS_L;
        #if(LCD_DRIVER == ID_932X)
        switch(rotation) {
            default:rot_val=0x1030;break; case 1:rot_val=0x1028;break; 
            case 2:rot_val=0x1000;break; case 3:rot_val=0x1018;break;}
        CMDDATA16(MD, rot_val);
        #elif(LCD_DRIVER == ID_7735) 
        switch(rotation) {
            case 0:rot_val=0xD0;break; case 1:rot_val=0xA0; break; 
            case 2:rot_val=0x00; break; case 3:rot_val=0x60; break;}
        CMDDATA8(MD, rot_val);
        #elif(LCD_DRIVER == ID_9481) 
        switch(rotation) {
            case 0:rot_val=0x09;break; case 1:rot_val=0x2B;break; 
            case 2:rot_val=0x0A;break; case 3:rot_val=0x28;break;}
        CMDDATA8(MD, rot_val);
        #elif(LCD_DRIVER == ID_9486) 
        switch(rotation) {
            case 0: rot_val=ILI9341_MADCTL_BGR; break;
            case 1: rot_val=ILI9341_MADCTL_MX|ILI9341_MADCTL_MV|ILI9341_MADCTL_ML|ILI9341_MADCTL_BGR; break;
            case 2: rot_val=ILI9341_MADCTL_MY|ILI9341_MADCTL_MX|ILI9341_MADCTL_BGR; break;
            case 3: rot_val=ILI9341_MADCTL_MY|ILI9341_MADCTL_MV|ILI9341_MADCTL_BGR; break; }
        CMDDATA8(MD, rot_val);
        #elif(LCD_DRIVER == ID_9488) 
        switch(rotation) {
            case 0: rot_val=ILI9341_MADCTL_MX|ILI9341_MADCTL_MY|ILI9341_MADCTL_BGR; break;
            case 1: rot_val=ILI9341_MADCTL_MV|ILI9341_MADCTL_MY|ILI9341_MADCTL_BGR; break;
            case 2: rot_val=ILI9341_MADCTL_ML|ILI9341_MADCTL_BGR; break;
            case 3: rot_val=ILI9341_MADCTL_MX|ILI9341_MADCTL_ML|ILI9341_MADCTL_MV|ILI9341_MADCTL_BGR; break; }
        CMDDATA8(MD, rot_val);
        #else 
        switch(rotation) {
            case 0: rot_val=ILI9341_MADCTL_MX|ILI9341_MADCTL_BGR; break;
            case 1: rot_val=ILI9341_MADCTL_MV|ILI9341_MADCTL_BGR; break;
            case 2: rot_val=ILI9341_MADCTL_MY|ILI9341_MADCTL_ML|ILI9341_MADCTL_BGR; break;
            case 3: rot_val=ILI9341_MADCTL_MX|ILI9341_MADCTL_MY|ILI9341_MADCTL_ML|ILI9341_MADCTL_MV|ILI9341_MADCTL_BGR;break;}
        CMDDATA8(MD, rot_val);
        #endif
        SET_ADDR_WINDOW(0, 0, Width-1, Height-1);
        Vert_Scroll(0, HEIGHT, 0);
        CS_H;
    }

    int16_t Get_Width(void) const { return Width; }
    int16_t Get_Height(void) const { return Height; }
    uint8_t Get_Rotation(void) const { return rotation; }

    void Invert_Display(bool i) {
        CS_L;
        uint8_t val = VL^i;
        if constexpr(LCD_DRIVER == ID_932X) {CMDDATA8(0x61, val);}
        else if constexpr(LCD_DRIVER == ID_7575) {CMDDATA8(0x01, val ? 8 : 10);}
        else {CMD8(val ? 0x21 : 0x20);}
        CS_H;
    }

    void Draw_Bit_Map(int16_t x, int16_t y, int16_t sx, int16_t sy, const uint16_t *data, int16_t scale) {
        int16_t color;
        CS_L; SET_ADDR_WINDOW(x, y, x + sx*scale - 1, y + sy*scale - 1); CS_H;
        if(1 == scale) { Push_Any_Color((uint16_t *)data, sx * sy, 1, 0); }
        else {
            for (int16_t row = 0; row < sy; row++) {
                for (int16_t col = 0; col < sx; col++) {
                    color = *(data + (row*sx + col)*1);
                    Fill_Rect(x+col*scale, y+row*scale, scale, scale, color);
                }
            }
        }
    }

    uint32_t convert_tr(uint8_t c) {
        switch(c) {
            case 0xC7: return 128; break; // 
            case 0xD0: return 129; break; // 
            case 0xDD: return 130; break; // 
            case 0xD6: return 131; break; // 
            case 0xDE: return 132; break; // 
            case 0xDC: return 133; break; // 
            case 0xE7: return 134; break; // 
            case 0xF0: return 135; break; // 
            case 0xFD: return 136; break; // 
            case 0xF6: return 137; break; //
            case 0xFE: return 138; break; // 
            case 0xFC: return 139; break; // 
            default: return c;   }
    }

    __attribute__((optimize("unroll-loops")))
    __attribute__((optimize("Ofast")))
    void Print_fr(uint8_t *st, uint32_t str_len, uint32_t x, uint32_t y, uint32_t f, uint32_t t) {
        uint32_t x_curr = x; const uint32_t tt = t*t;
        while(str_len--) {
            uint32_t ch = *st++; uint32_t cl = 0;//ch = convert_tr(ch); if (ch >= 176) ch++; 
            for(uint32_t col = (ch == ' ') ? 2 : 5; col > 0;col--) {
                uint32_t line = lcd_font[ch*5 + cl++];
                if constexpr(LCD_DRIVER != ID_932X && LCD_DRIVER != ID_7575) SET_X(x_curr, x_curr + t-1);
                for(uint32_t row = 0; row < 8; row++) {
                    if (line & 1) {
                        if constexpr(LCD_DRIVER == ID_932X || LCD_DRIVER == ID_7575) {
                            SET_ADDR_WINDOW(x_curr, y+row*t , x_curr + t-1, y+row*t  + t-1);}
                        else { SET_Y( y+row*t,  y+(row+1)*t -1);} CMD8(MW);
                        switch (t) { case 1: DATA16(f);break; case 2: BLOCK4(f);break;
                        default: uint32_t p = tt; do {DATA16(f); } while (--p);break;}
                    } line >>= 1;
                } x_curr += t;
            }  x_curr +=char_spc;
        }
    }

    //__attribute__((optimize("unroll-loops")))
    __attribute__((optimize("Ofast")))
    void Print_bg(uint8_t *st, uint32_t str_len) {
        const uint32_t t8 = 8* text_size; 
        while(str_len--) {
            uint32_t ch = *st++; uint32_t cl = 0; //ch = convert_tr(ch); if (ch >= 176) ch++; 
            for(uint32_t col = (ch == ' ') ? 2 : 5; col > 0;col--) {
                uint32_t line = lcd_font[ch*5 + cl++]; uint32_t row= 8;
                if (text_size==1) {
                    while(row--) { DATA16(((line & 1) ? text_color:text_bgcolor));line >>= 1;}
                }else { 
                    for (uint32_t row = 0; row < t8;) {
                    uint32_t c = (line >> (row++ & 7) & 1) ? text_color : text_bgcolor;
                    if(text_size >=2){DATA16(c);DATA16(c);} if(text_size >=3) DATA16(c);
                    if(text_size >=4) DATA16(c); if(text_size >=5) DATA16(c);}} 
            }  {uint32_t r = text_size*char_spc; do {BLOCK8(text_bgcolor)} while(--r);}
        }
    }

    __attribute__((optimize("Ofast")))
    void Print_Str(uint8_t *st, int16_t x, int16_t y) {
        uint32_t str_len = strlen((const char *)st); 
        uint32_t total_w = 0, tw = 0;
        for(uint32_t i=0; i < str_len; i++) {
            tw = char_spc+((st[i] == ' ' ? 2 : 5) * text_size); 
            if(total_w + tw > Width) { str_len = i; break;} 
            total_w += tw;}
        if (x == CENTER) x = (Width - total_w) / 2; else if (x == RIGHT) x = Width - total_w - 1;
        if((x >= Width)||(y >= Height)||((x+5*text_size) < 0)||((y+text_size*8-1) < 0)) return;
        //uint32_t ym = (y + text_size*8 -1 > Height) ? Height-y : 8; 
        Set_Text_Cursor(x, y); 
        CS_L;
        if (text_mode ==0) {
            Print_fr(st, str_len, text_x, text_y, text_color, text_size);
        } else {
            CMDDATA8(MD, rot_val ^ 0x20); 
            SET_ADDR_WINDOW(text_y, text_x, text_y + (text_size*8) - 1, text_x + total_w - 1); CMD8(MW); 
            Print_bg(st, str_len);
            CMDDATA8(MD, rot_val); 
        } CS_H;
    }

    void Draw_Pixe(int16_t x, int16_t y, uint16_t color)  {
        if((uint32_t)x > Width || (uint32_t)y > Height) return;
        CS_L; SET_ADDR_WINDOW(x, y, x, y); CMD8(MW); DATA16(color); CS_H;
    }

    void Fill_Scree(uint16_t c) {
        uint8_t rt=PORTRAIT;
        if (rotation != PORTRAIT) {rt = rotation; Set_Rotation(PORTRAIT);} // Rotation 0 to reduce tearing
        CS_L; SET_ADDR_WINDOW(0, 0, 239, 319);	CMD8(MW);
        uint32_t n = (240UL * 320UL) / 48;
        while (n--) { BLOCK8(c); BLOCK8(c); BLOCK8(c); BLOCK8(c); BLOCK8(c); BLOCK8(c);}
        if constexpr(LCD_DRIVER == ID_932X) {SET_ADDR_WINDOW(0, 0, Width-1, Height-1);}
        else if constexpr(LCD_DRIVER == ID_7575) Set_LR(); 
        CS_H;
        if (rotation != rt) Set_Rotation(rt);
    }

    __attribute__((optimize("unroll-loops")))
    void Fill_Rect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t c) {
        int16_t end;
        if (w < 0) { w = -w; x -= w; } end = x + w;
        if (x < 0) { x = 0; }
        if (end > Width) { end = Width; } w = end - x;
        if (h < 0) { h = -h; y -= h; } end = y + h;
        if (y < 0) { y = 0; }
        if (end > Height) { end = Height; } h = end - y;
        uint32_t n = h * w;
        CS_L; SET_ADDR_WINDOW(x, y, x+w-1, y+h-1);	CMD8(MW);
        //while(n--){DATA16(c);}
        uint32_t batches = n >> 3;      //3: n / 8 
        uint8_t remainder = n & 0x07;   //7: n % 8 
        while (batches--) {	BLOCK8(c);}
        while (remainder--) { DATA16(c); }
        if constexpr(LCD_DRIVER == ID_932X) {SET_ADDR_WINDOW(0, 0, Width-1, Height-1);}
        else if constexpr(LCD_DRIVER == ID_7575) Set_LR(); 
        CS_H; 
    }
	protected: 
	private:
};
#define lcd LCD_KBV::getInstance()

#endif
