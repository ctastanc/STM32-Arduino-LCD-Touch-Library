#pragma once

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
    //uint8_t char_spc = 2;
	int16_t Width, Height, rotation, rot_val;

    static LCD_KBV& getInstance() {
        static LCD_KBV instance(1); 
        return instance;
    }

    LCD_KBV(uint16_t m) {
        SET_PORTS();
        RS_DATA; CS_H; WR_H; RD_H; RST_L; RST_H;
        rotation  = 0;
        SET_WRITE_DIR();
        Width  = WIDTH;
        Height = HEIGHT;
    }

    void Init_LCD(void) {
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
        CS_H; RD_H; WR_H;
        RST_L; delay(2); RST_H;
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
                    Fill_Rect(x+col*scale, y+row*scale, scale, scale, color);
                }
            }
        }
    }

    void Set_Addr_Window(int16_t x1, int16_t y1, int16_t x2, int16_t y2) {
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

    uint32_t get_line(uint8_t *st, char* &line_ref) {
        uint32_t str_len = strlen((const char *)st);
        line_ref = (char*)malloc((str_len * 5 * text_size) + 2);
        char *temp = line_ref; // Temporary pointer to preserve the original address
        uint32_t line_len = 0;
        for(uint32_t i = 0; i < str_len; i++) {
            uint8_t ch = st[i];
            for(uint32_t cl = 0; cl < ((ch == ' ') ? 2 : 5); cl++) {
                for(uint32_t c = 0; c < text_size; c++) {
                    *temp++ = font[ch * 5 + cl]; line_len++;
                }
            } *temp++=0; line_len++;
        } *temp++=0; line_len++;
        return line_len;
    }

    void sc(char *line, uint32_t line_len, uint32_t x1, int16_t x2, int16_t y, uint32_t step ){
        uint32_t line_len1 = 0, total_w = 0;
        for(line_len1=step; line_len1 < line_len; line_len1++) {
            if(x1+total_w + 1 >= Width-x2) break;
            total_w += 1;
        }
        Set_Addr_Window(y, x1, y+text_size*8-1, x1 + total_w - 1 ); CMD8(MW);
        for(uint32_t i=step; i<line_len1; i++) {
            for (uint32_t row = 0; row < 8;) {
                uint32_t c = (line[i] >> row++ & 1) ? text_fc:text_bc;
                uint32_t p = text_size; do{DATA16(c);} while(--p);
            }
        } //delay(5);
    }

    /*!
        @brief    Horzintal scroll string.
        @param    st     String
        @param    x1     left margin
        @param    x2     right margin
        @param    y      y coordinate
        @param    size   text size
        @param    f      Fore color for 16 bit or RGB(r,g,b) color     
        @param    b      Back color for 16 bit or RGB(r,g,b) color
        @param    speed  Scroll speed
    */
    void Print_HScroll(const uint8_t *st,int16_t x1,int16_t x2,int16_t y,int16_t size,const RGB& f,const RGB& b, uint16_t speed){
        text_size=size;
        text_fc=f.val;
        text_bc=b.val;
        char *line;
        int add_spc_len = (Width-x1-x2)/(2*text_size+1);
        int old_len=strlen((const char *)st);
        char* new_arr = (char*)malloc(add_spc_len + old_len + 1);
        memset(new_arr, ' ', add_spc_len); 
        strcpy(new_arr + add_spc_len, (char*)st);
        uint32_t line_len = get_line((uint8_t*)new_arr, line);
        free(new_arr);
        CS_L; CMDDATA8(MD, rot_val ^ 0x20); 
        for(int step = 0; step < line_len; step++) {sc(line,line_len,x1,x2,y,step); delay(speed);}
        //delay(500); 
        //for(int step = line_len - 1; step >= 0; step--) {sc(line, line_len, x1, x2, y, step); delay(speed);}
        CMDDATA8(MD, rot_val); CS_H;
        free(line); 
    }

    __attribute__((optimize("O3")))
    void Print_fr() {
        uint _x=text_x-1;
        for(uint i=0; i<text_len; i++){
            uint ch = text[i]; _x++;
            for(uint c=0; c<((ch==' ')?2:5); c++, _x+=text_size) {
                uint l=font[ch*5+c];
                if constexpr(LCD_DRIVER != ID_932X && LCD_DRIVER != ID_7575) SET_X(_x, _x+text_size-1);
                for(uint r=0; r<8; r++, l>>=1) {
                    if (l&1) {
                        if constexpr(LCD_DRIVER == ID_932X || LCD_DRIVER == ID_7575) {
                            Set_Addr_Window(_x, text_y+r*text_size, _x+text_size-1, text_y+r*text_size+text_size-1);}
                        else { SET_Y( text_y+r*text_size, text_y+r*text_size+text_size-1); } CMD8(MW);
                        switch (text_size) {
                            case 1: DATA16(text_fc); break;
                            case 2: BLOCK4(text_fc); break;
                            default: BLOCK8(text_fc); uint p = (text_size*text_size)-8; do {DATA16(text_fc); } while (--p); break;
                        }
                    }
                }
            }
        }
    }

    #if(LCD_DRIVER == ID_932X || LCD_DRIVER == ID_7575)
    __attribute__((optimize("Ofast")))
    void Print_bg(uint total_w) {
        Set_Addr_Window(text_x, text_y, text_x+total_w-1, text_y+(text_size*8)-1); CMD8(MW);
        for (uint32_t r=0; r<8; r++) {
            for (uint32_t y=0; y<text_size; y++) {
                for (uint32_t i=0; i<text_len; i++) {
                    uint32_t ch = text[i];
                    for (uint32_t c=0; c<((ch==' ')?2:5); c++) {
                        uint32_t cl = (font[ch*5+c]>>r & 1) ? text_fc:text_bc;
                        if(text_size == 1) { DATA16(cl); }
                        else for (uint32_t x = 0; x < text_size; x++) DATA16(cl);
                    } DATA16(text_bc); // space 1
                }
            }
        }
    }
    #else
    __attribute__((optimize("O3")))
    void Print_bg(uint total_w) {
        uint32_t fc = text_fc;
        uint32_t bc = text_bc;
        uint32_t ts = text_size;
        uint32_t len = text_len;
        CMDDATA8(MD, rot_val ^ 0x20);
        Set_Addr_Window(text_y, text_x, text_y+ts*8-1, text_x+total_w-1); CMD8(MW);        
        for(uint i=0; i<len; i++){
            uint ch = text[i];
            for(uint c=0; c<((ch==' ')?2:5); c++) {
                uint l = font[ch*5+c];
                if(ts==1) {for(uint r=0;r<8;r++) DATA16(((l>>r&1)?fc:bc));}
                else{ for(uint r=0; r<ts*8; r++) 
                    for(uint p=0; p<ts; p++) DATA16(((l>>(r&7)&1)?fc:bc));}
            }  uint r = ts; do BLOCK8(bc) while(--r);
        } CMDDATA8(MD, rot_val);
    }
    #endif
    
    __attribute__((optimize("O3")))
    void Print_Str() {
        //Serial.print((char*)text);Serial.println(text_len);
        uint total_w=0, tw=0;
        uint x1=(text_x==CENTER||text_x==RIGHT) ? 0:text_x;
        for(uint i=0; i < text_len; i++) {
            tw = 1+((text[i]==' ' ? 2:5)*text_size);
            if(x1+total_w+tw > Width) { text_len=i; break; }
            total_w +=tw;
        }
        if (text_x == CENTER) text_x = (Width-total_w)/2;
        else if (text_x == RIGHT) text_x = Width-total_w-1;
        if(text_x>=Width||text_y>=Height||text_x+5*text_size<0||text_y+text_size*8-1<0) return;
        CS_L;
        if (text_mode == 0) { Print_fr(); } else { Print_bg(total_w); } 
        CS_H;
    }

    void Draw_Pixe(int16_t x, int16_t y, uint16_t color)  {
        if((uint32_t)x > Width || (uint32_t)y > Height) return;
        CS_L; Set_Addr_Window(x, y, x, y); CMD8(MW); DATA16(color); CS_H;
    }

    void Fill_Scree(uint16_t c) {
        uint8_t rt=PORTRAIT;
        if (rotation != PORTRAIT) {rt = rotation; Set_Rotation(PORTRAIT);} // Rotation 0 to reduce tearing
        CS_L; Set_Addr_Window(0, 0, Width, Height);	CMD8(MW);
        uint32_t n = (240UL * 320UL) / 48;
        while (n--) { BLOCK8(c); BLOCK8(c); BLOCK8(c); BLOCK8(c); BLOCK8(c); BLOCK8(c);}
        if constexpr(LCD_DRIVER == ID_932X) {Set_Addr_Window(0, 0, Width-1, Height-1);}
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
        CS_L; Set_Addr_Window(x, y, x+w-1, y+h-1); CMD8(MW);
        //while(n--){DATA16(c);}
        uint32_t batches = n >> 3;      //3: n / 8 
        uint8_t remainder = n & 0x07;   //7: n % 8 
        while (batches--) {	BLOCK8(c);}
        while (remainder--) { DATA16(c); }
        if constexpr(LCD_DRIVER == ID_932X) {Set_Addr_Window(0, 0, Width-1, Height-1);}
        else if constexpr(LCD_DRIVER == ID_7575) Set_LR(); 
        CS_H; 
    }
	protected: 
	private:
};
#define lcd LCD_KBV::getInstance()
