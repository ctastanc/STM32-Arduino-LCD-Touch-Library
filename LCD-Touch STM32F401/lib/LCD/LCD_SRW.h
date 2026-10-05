#pragma once

#include <LCD_GUI.h>
#include <mcu_regs.h>
#include <lcd_regs.h>
#include <LCD_font.h>

#define PORTRAIT  0
#define LANDSCAPE 1
#define PORTRAIT_REV 2
#define LANDSCAPE_REV 3

class LCD_SRW:public LCD_GUI<LCD_SRW>
{
	public:
    using LCD_GUI<LCD_SRW>::text_x;
    using LCD_GUI<LCD_SRW>::text_y;
    using LCD_GUI<LCD_SRW>::text_fc;
    using LCD_GUI<LCD_SRW>::text_bc;
    using LCD_GUI<LCD_SRW>::text_size;
    using LCD_GUI<LCD_SRW>::text_len;
    using LCD_GUI<LCD_SRW>::text;
    using LCD_GUI<LCD_SRW>::text_mode;
    using LCD_GUI<LCD_SRW>::draw_color;
    //uint8_t char_spc = 2;
	int16_t Width, Height, rotation, rot_val;
    
    LCD_SRW() {
        SET_PORTS();
        RS_DATA; CS_H; WR_H; RD_H; RST_L; RST_H;
        rotation  = 0;
        SET_WRITE_DIR();
        Width  = WIDTH;
        Height = HEIGHT;
    }

    void Init(void) {
        reset(); delay(200);
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
        CS_L;
        while (size > 0) {
            uint16_t cmd = *p++;
            uint16_t d   = *p++;
            if (cmd == TFTLCD_DELAY16) { delay(d); }
            else { CMDDATA16(cmd, d); }
            size -= 2 * sizeof(int16_t);
        } CS_H;
    }

    void reset(void) {
        CS_H; RD_H; WR_H; RST_L; delay(2); RST_H;
        CS_L;
        CMD8(0x00);
        for(uint8_t i=0; i<3; i++) { WR_L; WR_H; }
        CS_H;
    }

    void Push_Command(uint8_t cmd, uint8_t *block, int8_t N) {
        CS_L;
        CMD16(cmd);
        while (N-- > 0) {
            uint8_t u8 = *block++;
            DATA8(u8);
            if(N && (LCD_DRIVER == ID_7575)) { cmd++; CMD16(cmd); }
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
            //if constexpr(LCD_DRIVER == ID_932X) {CMD8(ILI932X_START_OSC);} 
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
            //if constexpr(LCD_DRIVER == ID_932X) {CMD8(ILI932X_START_OSC);} 
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
        //delay(1);
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
        Set_Addr_Window(x, y, x+w-1, y+h-1);
        while (n > 0) {
            CMD16(MR);
            SET_READ_DIR();
            #if(LCD_DRIVER == ID_932X) 
                while(n) {
                    for(int i=0; i<2; i++) { READ8(r); READ8(r); READ8(r); READ8(g); }
                    *block++ = (r<<8 | g); n--;
                }
                Set_Addr_Window(0, 0, Width-1, Height-1);
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
        Set_Addr_Window(0, 0, Width-1, Height-1);
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
        else {CMD8(val ? ILI9341_INVERTON : ILI9341_INVERTOFF);}
        CS_H;
    }

    void Draw_Bit_Map(int16_t x, int16_t y, int16_t sx, int16_t sy, const uint16_t *data, int16_t scale) {
        int16_t color;
        CS_L; Set_Addr_Window(x, y, x + sx*scale - 1, y + sy*scale - 1); CS_H;
        if(1 == scale) { Push_Any_Color((uint16_t *)data, sx * sy, 1, 0); }
        else {
            for (int16_t row = 0; row < sy; row++) {
                for (int16_t col = 0; col < sx; col++) {
                    color = *(data + (row*sx + col)*1);
                    Fill_Rectangle(x+col*scale, y+row*scale, scale, scale, color);
                }
            }
        }
    }

    __attribute__((always_inline)) 
    inline void Set_Addr_Window(int16_t x1, int16_t y1, int16_t x2, int16_t y2) {
        #if(LCD_DRIVER == ID_932X)
            int x, y, t;
            switch(rotation) {
                default: x = x1; y = y1; break;
                case 1:
                t=y1; y1=x1; x1=WIDTH-1-y2; y2=x2; x2=WIDTH-1-t; x=x2; y=y1; break;
                case 2:
                t=x1; x1=WIDTH-1-x2; x2=WIDTH-1-t; t=y1; y1=HEIGHT-1-y2; y2=HEIGHT-1-t; x=x2; y=y2; break;
                case 3:
                t=x1; x1=y1; y1=HEIGHT-1-x2; x2=y2; y2=HEIGHT-1-t; x=x1; y=y2; break;
            }
            CMDDATA16(ILI932X_HOR_START_AD, x1); 
            CMDDATA16(ILI932X_HOR_END_AD,   x2); 
            CMDDATA16(ILI932X_VER_START_AD, y1); 
            CMDDATA16(ILI932X_VER_END_AD,   y2); 
            CMDDATA16(ILI932X_GRAM_HOR_AD,   x); 
            CMDDATA16(ILI932X_GRAM_VER_AD,   y); 
            CMD8(ILI932X_START_OSC);
        #elif(LCD_DRIVER == ID_7575)
            CMDDATA8(HX8347G_COLADDRSTART_HI, (x1) >> 8); 
            CMDDATA8(HX8347G_COLADDRSTART_LO, (x1)); 
            CMDDATA8(HX8347G_ROWADDRSTART_HI, (y1) >> 8); 
            CMDDATA8(HX8347G_ROWADDRSTART_LO, (y1)); 
            CMDDATA8(HX8347G_COLADDREND_HI,   (x2) >> 8); 
            CMDDATA8(HX8347G_COLADDREND_LO,   (x2)); 
            CMDDATA8(HX8347G_ROWADDREND_HI,   (y2) >> 8); 
            CMDDATA8(HX8347G_ROWADDREND_LO,   (y2));
        #else 
            CMD8(XS); DATA16(x1); DATA16(x2); 
            CMD8(YS); DATA16(y1); DATA16(y2);
        #endif   
    }
    
    /*!
        @brief    Horzintal scroll string.
        @param    st     String
        @param    x      x coordinate
        @param    y      y coordinate
        @param    size   text size
        @param    f      Fore color for 16 bit or RGB(r,g,b) color     
        @param    b      Back color for 16 bit or RGB(r,g,b) color
        @param    speed  Scroll delay
    */
    void Print_LeftScroll(const char *st, int16_t x, int16_t y, int16_t size, const RGB f, const RGB b, uint16_t speed) {
        uint ts = size, fc = f, bc = b;
        const int char_spacing = 1;                 // Character spacing (pixels)
        const int char_w = 5 * ts + char_spacing;   // Total width of 1 character (e.g., 5*2 + 1 = 11)
        int16_t cur_x_start = x;
        CS_L;
        while (true) {
            cur_x_start--; // The text shifts exactly 1 pixel to the left in each cycle.
            if (cur_x_start + char_w <= 0) {  // If the first character has completely exited from the left side of the screen, shift the text by one character.
                cur_x_start += char_w; // Reset/shift coordinates
                st++; if (*st == '\0') break; // End of text
            }
            int char_x = cur_x_start;
            int len = strlen(st);
            for (int i = 0; i < len && char_x < Width; i++, char_x += char_w) {
                if (char_x + char_w <= 0) continue; // Do not perform the action if the character is completely off-left.
                const uint8_t *ch = &font[st[i] * 5];
                int col_x = char_x;
                for (int c = 0; c < 5; c++, col_x += ts) { // 1. Draw 5 columns of font.
                    if (col_x + (int)ts <= 0 || col_x >= Width) continue; // Skip if the column is outside the screen boundaries.
                    int draw_x1 = (col_x < 0) ? 0 : col_x; // Clip the left and right edges
                    int draw_x2 = (col_x + (int)ts - 1 >= Width) ? (Width - 1) : (col_x + (int)ts - 1);
                    int draw_w = draw_x2 - draw_x1 + 1;
                    if (draw_w <= 0) continue;
                    uint8_t l = ch[c];
                    for (int r = 0; r < 7; r++, l >>= 1) {
                        uint cl = (l & 1) ? fc : bc;
                        Set_Addr_Window(draw_x1, y + r * ts, draw_x2, y + r * ts + ts - 1); CMD8(MW);
                        uint p = draw_w * ts; do { DATA16(cl); } while (--p);
                    }
                }
                if (col_x >= 0 && col_x < Width) {
                    Set_Addr_Window(col_x, y, col_x, y + 7 * ts - 1); CMD8(MW);
                    uint p = 7 * ts; do { DATA16(bc); } while (--p);
                }
            }
            delay(speed);
            if (*st == '\0') break;
        } CS_H;
    }

    __attribute__((optimize("O3"), noinline))
    void Print_Str() {
        uint ts = text_size, ty = text_y, fc = text_fc, bc = text_bc;
        uint32_t fc_hi = DATA_MASK1 | (fc >> 8), fc_lo = DATA_MASK1 | (fc & 0xFF),
                 bc_hi = DATA_MASK1 | (bc >> 8), bc_lo = DATA_MASK1 | (bc & 0xFF);
        uint x1 = (text_x == CENTER || text_x == RIGHT) ? 0 : text_x;
        uint char_w = 5 * ts + 1;
        if (text_len > (Width - x1) / char_w) text_len = (Width - x1) / char_w;  
        uint total_w = text_len * char_w;
        if (text_x == CENTER) text_x = (Width - total_w)/2; else if (text_x == RIGHT) text_x = Width - total_w-1;
        uint tx = text_x;
        CS_L;
        if (text_mode == 0) { // FOREGROUND ONLY
            uint32_t ts2 = ts * ts;
            for (uint i = 0; i < text_len; i++) {
                const uint8_t *ch = &font[text[i] * 5];
                for (uint c = 0; c < 5; c++, tx += ts) {
                    uint8_t v_line = *ch++; if (!v_line) continue; uint8_t pix = 0;
                    while (v_line) {
                        while (!(v_line & 1)) { v_line >>= 1; pix++; }
                        uint8_t pix_len = 0;
                        while (v_line & 1) { v_line >>= 1; pix_len++; }
                        Set_Addr_Window(tx, ty + pix * ts, tx + ts - 1, ty + (pix + pix_len) * ts - 1); CMD8(MW);
                        uint32_t p = pix_len * ts2;
                        do DATA_COLOR(fc_hi,fc_lo); while (--p);
                        pix += pix_len;
                    }
                } tx++; // Character spacing
            }
        } else { // BACKGROUND INCLUDED
            Set_Addr_Window(tx, ty, tx + total_w - 1, ty + (ts * 7) - 1); CMD8(MW);
            if (ts == 1) { 
                for (uint32_t r = 0; r < 7; r++) {
                    for (uint32_t i = 0; i < text_len; i++) {
                        const uint8_t *ch = &font[text[i] * 5];
                        for (uint32_t c = 0; c < 5; c++) {
                            if (*ch++ & (1 << r)) DATA_COLOR(fc_hi,fc_lo); else DATA_COLOR(bc_hi,bc_lo);
                        } DATA_COLOR(bc_hi,bc_lo); // Character spacing
                    }
                }
            } else { 
                for (uint32_t r = 0; r < 7; r++) {
                    for (uint32_t y = 0; y < ts; y++) {
                        for (uint32_t i = 0; i < text_len; i++) {
                            const uint8_t *ch = &font[text[i] * 5];
                            for (uint32_t c = 0; c < 5; c++) {
                                uint32_t p = ts;
                                if (*ch++ & (1 << r)) do DATA_COLOR(fc_hi,fc_lo); while (--p); 
                                    else do DATA_COLOR(bc_hi,bc_lo); while (--p);
                            } DATA_COLOR(bc_hi,bc_lo); // Character spacing
                        }
                    }
                }
            }
        }
        CS_H;
    }

    __attribute__((always_inline)) 
    inline void Pixe(int16_t x, int16_t y, uint16_t c)  {
        //if((uint32_t)x >= Width || (uint32_t)y >= Height) return;
        CS_L; Set_Addr_Window(x, y, x, y); CMD8(MW); DATA16(c); CS_H;
    }

    void Fill_Scree(uint16_t c) {
        CS_L; Set_Addr_Window(0, 0, Width-1, Height-1);	CMD8(MW);
        uint32_t n = (uint32_t)Width * Height /64; 
        while (n--) { BLOCK_D16(c); BLOCK_D16(c); BLOCK_D16(c); BLOCK_D16(c); }
        if constexpr(LCD_DRIVER == ID_932X) {Set_Addr_Window(0, 0, Width-1, Height-1);}
        else if constexpr(LCD_DRIVER == ID_7575) Set_LR(); 
        CS_H;
    }

    __attribute__((optimize("unroll-loops"), always_inline)) 
    inline void Fill_Rect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t c) {
        if (w <= 0 || h <= 0) return; 
        uint32_t hi = DATA_MASK1 | (c >> 8), lo = DATA_MASK1 | (c & 0xFF); 
        uint32_t n = h * w; 
        CS_L; Set_Addr_Window(x, y, x+w-1, y+h-1); CMD8(MW);
        uint32_t batches = n >> 4;    //3: n / 8 4: n/16
        uint8_t remainder = n & 15;   //7: n % 8 15: n % 16
        while (batches--) {	BLOCK_C8(hi,lo); BLOCK_C8(hi,lo); }
        while (remainder--) { DATA_COLOR(hi,lo); }
        if constexpr(LCD_DRIVER == ID_932X) {Set_Addr_Window(0, 0, Width-1, Height-1);}
        else if constexpr(LCD_DRIVER == ID_7575) Set_LR(); 
        CS_H; 
    }
	protected: 
	private:
};

inline LCD_SRW lcd;
