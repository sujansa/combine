/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
///////////////////////////////Combine///////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
// Name        : Combine  Ver: Pad 24                            //
// Written by  : Sujan S. A.                                      //
// E-mail      : messagesujan@yahoo.co.in                        //
// Description  : Graphic editor                                  //
// Last revised : 15 JUN 2006 (23:32 hrs)                          //
/////////////////////////////////////////////////////////////////////

/*If  sys\stat.h  is not available then
    #define S_IWRITE 128
    #define S_IREAD 256
*/

#include <iostream.h>
#include <stdio.h>          //sprintf(), fopen() ...
#include <string.h>        //strcpy() ...
#include <ctype.h>          //toupper()
#include <math.h>          //atan() ...
#include <stdlib.h>        //system()
#include <dos.h>            //delay()
#include <fcntl.h>          //O_CREAT, O_TRUNC,...
#include <io.h>            //open(), lseek(), read(), write(), ...
#include <sys\stat.h>      //S_IWRITE, S_IREAD
#include <conio.h>          //getch(), kbhit()
#include <alloc.h>          //farmalloc(), farfree()
#include <errno.h>          //errno ...
#include <graphics.h>

#define D2R 3.1415926535/180.0
#define R2D 180.0/3.1415926535

#define IMG_PATH "C:\\img_cmb"          //Image buffer
#define SINK_PATH "C:\\cmb.tsj"        //Temporary file
#define RES_PATH "Combine.rsj"          //The resource file
#define DFL_PATH "dfl_cmb"              //Default file for opening
#define DRIVER_PATH "GDriver"          //Graphics device driver
#define LOGOSCR_PATH "logoscr.exe"      //Welcome screen
#define BSJ_JUN_2006 1                  //Image file version

#define DEAD 0        //Status(buttons)
#define ALIVE 1
#define ACTIVE 2

#define LINE 1        //Primitives
#define CIRCLE 2
#define BEZIER 3

#define PENCIL 1      //Operations
#define AIRBRUSH 2
#define FLOOD 3
#define ERASER 4
#define DRAWLINE 5
#define DRAWPOLY  6
#define DRAWBEZIER 7
#define DRAWRECT 8
#define DRAWCIRCLE 9
#define TRANSLATE 10
#define SCALE 11
#define ROTATE 12
#define BTEXT 13


#define TEMP 254    //Special colours (must be refined when
#define ERASE 255    //using 256 or more colours)

#define BACKSPACE 8  //ASCII codes
#define ESC_KEY 27

#define UpArrow 72  //Extended key codes
#define DownArrow 80
#define RightArrow 77
#define LeftArrow 75

typedef unsigned short DPixel; //Double pixel
//typedef unsigned char Pixel; //Pixel

////ImageInfo structure//////////////////////////////////////////////
typedef struct {
    char type[4];  //Default :  B S J (version)
    short wcsizex;  //Horizontal image size
    short wcsizey;  //vertical image size
} ImageInfo;

////Segment structure////////////////////////////////////////////////
typedef struct {
    int type;      //Type of primitive
    int a, b, c, d; //Coordinates
} Segment;

////Node structure for linked-list of coordinates////////////////////
typedef struct nodetype{
    int x,y;
    struct nodetype* next;
} Node;

////Mouse class//////////////////////////////////////////////////////
class Mouse{
    union REGS in, out;
    int x, y, left_button, right_button;
public:
    int init();
    int reset();
    void show(){in.x.ax = 0x0001;int86(0x33, &in, &out);}
    void hide(){in.x.ax = 0x0002; int86(0x33, &in, &out);}
    void read();
    int getx() {  return x; } //does not read()
    int gety() {  return y; } //does not read()
    int left() { read(); return left_button; }
    int right() { read(); return right_button; }
    int click();
    void set_position(int x, int y);
    void set_hbound(int x1, int x2);
    void set_vbound(int y1, int y2);
};

////Pad class////////////////////////////////////////////////////////
class Pad {
public:
    int img, state, modified, xmin, ymin,
        xmax, ymax,thickness;
    ImageInfo attr;
    int wcstartx, wcstarty, wcendx, wcendy;
    int vpsizex, vpsizey, vpstartx, vpstarty, vpendx, vpendy;
    DPixel bgc, fgc, boarder_colour, boarder;
    char curfile[50];
public:
    Pad();
    void setbgc(int c) { bgc=c;}
    void setfgc(int c) { fgc=c; }
    int getfgc() {return fgc;}
    int getbgc() {return bgc; }
    void setthickness(int t) {if(t<4) thickness=t;}
    int getthickness() {return thickness;}
    long Linear(int x, int y);
    void setstate(int st) { state=st; }
    int getstate() {return state;}
    void SetImageInfo(int sx, int sy, int value=WHITE);
    int GetImageInfo();
    int load(char *fname);
    int save(char *fname);
    int user_save();
    int user_open();
    void imgopen(int i);
    int imgclose();
    void show(int flag=0);
    void ScrnCapture();
    void position(int x, int y);
    void resize(int x, int y);
    int scroll_down(int value=40);
    int scroll_up(int value=40);
    int scroll_right(int value=40);
    int scroll_left(int value=40);
    DPixel getpix(int x, int y);
    void draw(int x, int y, DPixel pix);
    void setpix(int x, int y, DPixel pix);
    void pen(int x, int y, DPixel pix);
    void commit();
    void roll_back();
    void MpLine(int x0, int y0, int x1, int y1);
    void MpCircle(int xc,int yc, int r1, int r2);
    void circlepoints(int xc, int yc, int x, int y);
    void Bezier(float xa, float ya, float xb, float yb,
        float xc, float yc, float xd, float yd, int n);
    void polyline(int n, int *p);
    int getxy(int &x, int &y);
    int test();
    int perform();
    void preview();
    void new_Pad(int sx, int sy);
    void ffill(int x, int y);
};

////GraClass/////////////////////////////////////////////////////////
class GraClass {
    Segment sg[1000], aux[1000];
    int nSeg, xp, yp;
public:
    GraClass() {init(); xp=yp=0;}
    void setpivot(int x, int y) { xp=x; yp=y; }
    void init() { nSeg=0; }
    void store();
    void recall();
    void AddSeg(int t, int a, int b, int c, int d);
    void DelSeg();
    void show();
    void erase();
    void showlast(int n=1);
    void eraselast(int n=1);
    void translate(int x0, int y0, int x1, int y1);
    void scale(int x0, int y0, int x1, int y1);
    void rotate(int xo, int y0, int x1, int y1);
};

////Button class/////////////////////////////////////////////////////
class Button {
    int xsize, ysize, xp, yp, img_off,state, boarder;
    char desc[100];
public:
    Button() { state=ALIVE; }
    void setdead() {state=DEAD; show();}
    void setalive() {state=ALIVE; show();}
    void init(int x, int y, int ssx, int ssy,
        int off, char *dstr);
    void position(int x, int y) { xp=x; yp=y; }
    void size(int x, int y) { xsize=x; ysize=y; }
    void show();
    int test();
    int select();
};

////FeedBack class///////////////////////////////////////////////////
class FeedBack {
    char capstr[100], msgstr[100], loc_str[20];
    int x0, y0;
public:
    FeedBack() { *msgstr=*capstr='\0'; x0=y0=0;}
    void SetCaption(char *s) { strcpy(capstr, s); }
    void Msg(char *msg);
    void LineStatus();
    void ShowPrompt(char *pr);
    int GetInput(char *inp);
    int Update(int x, int y);
    void Caption();
};

////ColourBox class//////////////////////////////////////////////////
class ColourBox {
    int xs, ys, xp, yp;
public:
    void init(int x, int y, int xx, int yy)
        { xs=xx; ys=yy; xp=x; yp=y; }
    void show();
    int test();
    void select();
};

////Global Data//////////////////////////////////////////////////////

int Maxx, Maxy;        //Screen size
int res_fd;            //The resource file
void far *ptr[4];      //Store screen
int xnext=0, ynext=0;  //Next text output coordinates

Mouse mouse;
Pad pd;
GraClass gobject;
FeedBack fb;
ColourBox cb;

//The close button.
Button close_button;

//Help button
Button help_button;

//Scroll buttons.
Button scrlup;
Button scrldown;
Button scrlright;
Button scrlleft;

//File management buttons.
Button new_button;
Button save_button;
Button open_button;

//Memory management buttons.
Button commit_button;
Button roll_back_button;
Button segment_clear_button;

//Operation buttons
Button pencil_button;
Button airbrush_button;
Button flood_button;
Button eraser_button;
Button line_button;
Button poly_button;
Button bezier_button;
Button circle_button;
Button rect_button;
Button trans_button;
Button scale_button;
Button rotate_button;
Button text_button;

//Line-thickness buttons.
Button thick1_button;
Button thick2_button;
Button thick3_button;

Button logo_button;
////Non member routines////////////////////////////////////////////

int MinOf(int a, int b) {
    return (a<b) ? a : b;
}
int MaxOf(int a, int b) {
    return (a>b) ? a : b;
}
//Writes text at (x, y) in the default font.
void WriteText( const char *str, int x=-1, int y=-1,
    int tfgc=BLACK, int tbgc=LIGHTGRAY) {
    int len; 
    setcolor(tfgc);
    len=strlen(str);
    setfillstyle(SOLID_FILL,tbgc);
    if(x<0 || y<0) {
        x=xnext;
        y=ynext;
        xnext=x+len*8+1;
        if(xnext>Maxx-1) {
            x=0;
            y=y+9;
        }
    }
    xnext=x+len*8+1;
    ynext=y;
    bar(x, y, xnext, y+9);
    outtextxy(x+1,y+1, str);
}

inline void WritePixel(int x, int y, int value) {
    putpixel(x, y, value);
/*  An alternate method:
    char col;
    col=value&0x00ff;
    asm {
        mov al, col
        mov ah, 0x0c
        mov bx, 0x0
        mov cx, x
        mov dx, y
        int 0x10
    }  */
}

inline int ReadPixel(int x, int y) {
    return getpixel(x, y);
}

inline int WaitKey() {
    return getch();
}

void Beep(int f=440) {
    sound(f);
    delay(300);
    nosound();
}

int InitializeGraphics() {
    /* request autodetection */
    int gdriver = DETECT, gmode, errorcode;
    /* initialize graphics and local variables */
    initgraph(&gdriver, &gmode, DRIVER_PATH);
    /* read result of initialization */
    errorcode = graphresult();
    if (errorcode != grOk) {  /* an error occurred */
        printf("Graphics error: %s\n", grapherrormsg(errorcode));
        printf("Press any key to halt:");
        WaitKey();
        exit(1);              /* terminate with an error code */
    }
    Maxx=getmaxx();
    Maxy=getmaxy();
    if(!mouse.init()){
        cout << "Unable to access software inturrupt for mouse.\n";
        WaitKey();
        exit(1);
    }
    settextstyle(DEFAULT_FONT,HORIZ_DIR,1);
    return 1;
}

void CloseGraphics(int flag=1) {
    if(flag)
        mouse.hide();
    closegraph();
}

void open_resource() {
    if((res_fd=open(RES_PATH, O_BINARY|O_RDONLY))==-1) {
        cout << "Cannot open Resource file.";
        cout << "  errno = " << errno;
        if(errno==EACCES) cout << " EACCES " <<EACCES;
        else if(errno==EBADF) cout << " EBADF " <<EBADF;
        WaitKey();
        exit(1);
    }
}

//Produces an identity matrix.
void IdentityMatrix(double tm[3][3]) {
    int i, j;
    for(i=0; i<3; i++)
        for(j=0; j<3; j++)
        tm[i][j]=(i==j)? 1: 0;
}

//Applies translation by tx, ty on the transformation matrix
//tm[][].
inline void Translate(int tx, int ty, double tm[3][3]) {
    tm[0][2]+=(double)tx;
    tm[1][2]+=(double)ty;
}

//Transforms the point (xp, yp) using the transformation matrix
//tm[][].
inline void Transform(int &xp, int &yp, double tm[3][3]) {
    int x, y;
    x=xp; y=yp;
    xp=floor(0.5+x*tm[0][0]+y*tm[0][1]+tm[0][2]);
    yp=floor(0.5+x*tm[1][0]+y*tm[1][1]+tm[1][2]);
}

void ScaleAbout(int x, int y, double sx, double sy,
    double tm[3][3]) {
    Translate(-x, -y, tm);
    int i;
    for(i=0; i<3; i++) {
        tm[0][i]*=sx;
        tm[1][i]*=sy;
    }
    Translate(x, y, tm);
}

void Rotate(double a, double tm[3][3]) {
    int i;
    double c, s, temp;
    a=(360.0-a)*D2R;
    if(a>=360)
        a=a-360;
    c=cos(a); s=sin(a);
    for(i=0; i<3; i++) {
        temp = tm[0][i]*c-tm[1][i]*s;
        tm[1][i]=tm[0][i]*s+tm[1][i]*c;
        tm[0][i]=temp;
    }
}

inline void RotateAbout(int x, int y, double a, double tm[3][3]) {
    Translate(-x, -y, tm);
    Rotate(a, tm);
    Translate(x, y, tm);
}


void save_screen(void far *buf[4]) {
    unsigned size;
    int ystart=0, yend, yincr, block;
    yincr = (Maxy+1) / 4;
    yend = yincr;

    /* get byte size of image */
    size = imagesize(0, ystart, Maxx, yend);
    for (block=0; block<=3; block++) {
        if ((buf[block] = farmalloc(size)) == NULL) {
            Beep();
            printf("Error: not enough heap space in save_screen().\n");
            WaitKey();
            return;
        }
        mouse.hide();
        getimage(0, ystart, Maxx, yend, buf[block]);
        mouse.show();
        ystart = yend + 1;
        yend += yincr + 1;
    }
}

void restore_screen(void far *buf[4]) {
    int ystart=0, yend, yincr, block;
    yincr = (Maxy+1) / 4;
    yend = yincr;
    for (block=0; block<=3; block++) {
        mouse.hide();
        putimage(0, ystart, buf[block], COPY_PUT);
        mouse.show();
        farfree(buf[block]);
        ystart = yend + 1;
        yend += yincr + 1;
    }
}

//Displays the help screen.
void showhelp() {
    mouse.hide();
    save_screen(ptr);
    bar(0, 20, Maxx, Maxy);

    WriteText(
" Combine  Graphic Editor    developed by Sujan S. A. and R. Srikanth",
    5, 25);
    WriteText(" Feedback:  messagesujan@yahoo.co.in      ");
    WriteText("=====================================================");
    WriteText("<Help>    Press Escape to return to Combine editor.");
    WriteText("                                                    ");
    WriteText(
"Combine is a drawing tool you can use to create simple or elaborate ");
    WriteText("drawings.");

    while(WaitKey()!=ESC_KEY) Beep();
    restore_screen(ptr);
    mouse.show();
}

////Mouse class member functions/////////////////////////////////////

int Mouse::init() {
    if(!reset()) {
        CloseGraphics(0);
        cout <<"Mouse driver not found/loaded\n";
        return 0;
    }
    set_position(Maxx/2, Maxy/2);
    set_hbound(0, Maxx);
    set_vbound(0, Maxy);
    show();
    return 1;
}

int Mouse::reset() {
    in.x.ax = 0x0000;
    int86(0x33, &in, &out);
    return(out.x.ax);
}

void Mouse::read(){
    int button;
    in.x.ax = 0x0003;
    int86(0x33, &in, &out);
    button = out.x.bx;
    x = out.x.cx;
    y = out.x.dx;
    left_button = 0;
    right_button = 0;
    switch(button) {
        case 0x0001:
            left_button = 1;
            break;
        case 0x0002:
            right_button = 1;
            break;
        default:
            break;
    }
}

//Test for button-down and wait until button-release.
int Mouse::click() {
    if(left()) {
        while(left());
        return 1;
    }
    return 0;
}

//Position the mouse cursor.
inline void Mouse::set_position(int x, int y){
    in.x.ax = 0x0004;
    in.x.cx = x;
    in.x.dx = y;
    int86(0x33, &in, &out);
}

void Mouse::set_hbound( int x1, int x2 ){
    in.x.ax = 0x0007;
    in.x.cx = x1;
    in.x.dx = x2;
    int86( 0x33, &in, &out );
}

void Mouse::set_vbound( int y1, int y2 ){
    in.x.ax = 0x0008;
    in.x.cx = y1;
    in.x.dx = y2;
    int86( 0x33, &in, &out );
}

////Pad class member functions///////////////////////////////////////

Pad::Pad(){
    modified=0;
    state=PENCIL;
    thickness=1;
    imgopen(1);
}

//Open the temporary file for hold the image being manipulated.
void Pad::imgopen(int i=0) {
    if(i==1)
        img=open(IMG_PATH, O_CREAT | O_RDWR | O_BINARY | O_TRUNC,
            S_IREAD | S_IWRITE);
    else img=open(IMG_PATH, O_CREAT | O_RDWR | O_BINARY,
        S_IREAD | S_IWRITE);
    if(img==-1) {
        outtext("unable to open img");
        WaitKey();
    }
}

//Converts 2-D address into linear address.
inline long Pad::Linear(int x, int y) {
    return (long)((long)attr.wcsizex * (long)(y)+(long)x);
}

//Writes the ImageInfo header for an image of size(sx, sy)
//and background colour value.
void Pad::SetImageInfo(int sx, int sy, int value) {
    bgc=value; fgc=RED;
    attr.wcsizex=sx; attr.wcsizey=sy;
    wcstartx=0; wcstarty=0;
    wcendx=wcstartx+vpsizex; wcendy=wcstarty+vpsizey;
    xmin=0; ymin=0; xmax=attr.wcsizex-1; ymax=attr.wcsizey-1;
    attr.type[0]='B';
    attr.type[1]='S';
    attr.type[2]='J';
    attr.type[3]=BSJ_JUN_2006;
    lseek(img, 0L, SEEK_SET);
    write(img, &attr, sizeof(ImageInfo));
    if(vpsizex>attr.wcsizex)
        vpsizex=attr.wcsizex;
    if(vpsizey>attr.wcsizey)
        vpsizey=attr.wcsizey;
    resize(Maxx-119, Maxy-60);
}

int Pad::GetImageInfo() {
  lseek(img, 0L, SEEK_SET);
  read(img, &attr, sizeof(ImageInfo));
    if(vpsizex>attr.wcsizex)
        vpsizex=attr.wcsizex;
    if(vpsizey>attr.wcsizey)
        vpsizey=attr.wcsizey;
    bgc=WHITE; fgc=RED;
    wcstartx=0; wcstarty=0;
    wcendx=wcstartx+vpsizex; wcendy=wcstarty+vpsizey;
    xmin=0; ymin=0; xmax=attr.wcsizex-1; ymax=attr.wcsizey-1;
    if(attr.type[0]!='B'||attr.type[1]!='S'||attr.type[2]!='J')
      return -2;
    else if(attr.type[3]!=BSJ_JUN_2006)
        return -3;
    return 0;
}

//Load image from the file 'fname'
//Checks file type.and format version.
int Pad::load(char *fname=NULL) {
    FILE *item;
    char *newpath=DFL_PATH;
    int invalid;
    long isize, i;
    char ch;
    if(fname==NULL)
        fname=newpath;
    if((item=fopen(fname, "rb"))==NULL) {
        Beep();
        return -1;
    }
    fread(&attr, sizeof(ImageInfo), 1, item);
    invalid=0;
    if(attr.type[0]!='B'||attr.type[1]!='S'||attr.type[2]!='J')
        invalid=-2;
    if(attr.type[3]!=BSJ_JUN_2006)
        invalid=-3;
    if(invalid!=0) {
        GetImageInfo();
        Beep(); Beep();
        return invalid;
    }
    lseek(img, 0L, SEEK_SET);
    write(img, &attr, sizeof(ImageInfo));
    SetImageInfo(attr.wcsizex, attr.wcsizey);
    isize=(long)attr.wcsizex*attr.wcsizey;
    lseek(img, sizeof(ImageInfo), SEEK_SET);
    for(i=0; i<isize; i++) {
        fread(&ch, 1, 1, item);
        write(img, &ch, 1);
        write(img, &ch, 1);
    }
    fclose(item);
    return 0;
}

//Saves the image in the file 'fname'.
int Pad::save(char *fname) {
    FILE *item;
    long i, isize;
    char ch;
    DPixel pix;
    if((item=fopen(fname, "w+b"))==NULL) {
      Beep();
      return -1;
    }
    isize=(long)attr.wcsizex*attr.wcsizey;
    fwrite(&attr, sizeof(ImageInfo), 1, item);
    lseek(img, sizeof(ImageInfo), SEEK_SET);
    for(i=0; i<isize; i++) {
        read(img, &pix, sizeof(DPixel));
        ch=pix;
        fwrite((char *)&ch, 1, 1, item);
    }
    fclose(item);
    return 0;
}

//Closes the file associated with Pad.
int Pad::imgclose() {
    lseek(img,
        Linear(wcendx-1, wcendy-1)*sizeof(DPixel)+sizeof(ImageInfo),
        SEEK_SET);
    close(img);
    return 1;
}

//Display the visible section of the Pad(image).
void Pad::show(int flag) {
    int i, j;
    DPixel ch;
    mouse.hide();
    setfillstyle(XHATCH_FILL, 7);
    if(!flag)
        bar(vpstartx, vpstarty,
        MaxOf(vpendx, 101+Maxx-120), MaxOf(vpendy, 22+Maxy-61));
    lseek(img,
        Linear(wcstartx, wcstarty)*sizeof(DPixel)+sizeof(ImageInfo),
        SEEK_SET);
    for(i=0; i<vpsizey; i++){
        lseek(img,
            Linear(wcstartx,
                wcstarty+i)*sizeof(DPixel)+sizeof(ImageInfo), SEEK_SET);
        for(j=vpstartx; j<vpendx; j++) {
            read(img, &ch, sizeof(DPixel));
            WritePixel(j, vpstarty+i, ch&0x00ff);
        }
    }
    mouse.show();
}

//
void Pad::ScrnCapture() {
    int i, j;
  DPixel ch;
  mouse.hide();
  for(i=0; i<vpsizey; i++){
    for(j=0; j<vpsizex; j++) {
        ch=ReadPixel(vpstartx+j, vpstarty+i);
      setpix(wcstartx+j, wcstarty+i, ch);
    }
  }
  mouse.show();
}


//Resize the window displaying the visible section of Pad.
void Pad::resize(int x, int y) {
    if(x>attr.wcsizex)
        vpsizex=attr.wcsizex;
    else vpsizex=x;
    if(y>attr.wcsizey)
        vpsizey=attr.wcsizey;
    else vpsizey=y;
    vpendx=vpstartx+vpsizex;
    vpendy=vpstarty+vpsizey;
    wcendx=wcstartx+vpsizex;
    wcendy=wcstarty+vpsizey;
}

//Position the window displaying a section of Pad
void Pad::position(int x, int y) {
    vpstartx=x; vpstarty=y;
    vpendx=vpstartx+vpsizex;
    vpendy=vpstarty+vpsizey;
}

inline int Pad::scroll_down(int value) {
    if(wcendy>=attr.wcsizey){
        Beep();
        return 0;
    }
    if((wcendy + value)>=attr.wcsizey) {
        value=attr.wcsizey-wcendy;
    }
    mouse.hide();
    wcstarty+=value; wcendy+=value;
    show(1);
    mouse.show();
    return 1;
}

inline int Pad::scroll_up(int value) {
    if(wcstarty<=0){
        Beep();
        return 0;
    }
    if((wcstarty - value)<0) {
        value=wcstarty;
    }
    mouse.hide();
    wcstarty-=value; wcendy-=value;
    show(1);
    mouse.show();
    return 1;
}

inline int Pad::scroll_right(int value) {
    if(wcendx>=attr.wcsizex) {
        Beep();
        return 0;
    }
    if(wcendx+value>=attr.wcsizex) {
        value=attr.wcsizex-wcendx;
    }
    mouse.hide();
    wcstartx+=value; wcendx+=value;
    show(1);
    mouse.show();
    return 1;
}

inline int Pad::scroll_left(int value) {
    if(wcstartx<=0) {
        Beep();
        return 0;
    }
    if(wcstartx-value<0) {
        value=wcstartx;
    }
    mouse.hide();
    wcstartx-=value; wcendx-=value;
    show(1);
    mouse.show();
    return 1;
}

//Plots a pixel on the canvas(pad).
inline void Pad::draw(int x, int y, DPixel pix){
    DPixel pix1;
    if(x<0||y<0)
        return;
    if(x<attr.wcsizex && y<attr.wcsizey) {
        pix1=(getpix(x, y) & 0xff00)|(pix&0x00ff);
        lseek(img,
            Linear(x, y)*sizeof(DPixel)+sizeof(ImageInfo), SEEK_SET);
        write(img, &pix1, sizeof(DPixel));
        if(x<xmin) xmin=x;
        if(y<ymin) ymin=y;
        if(x>xmax) xmax=x;
        if(y>ymax) ymax=y;
    }
}

//Obtains the value of the DPixel(x, y) in the canvas.
//Error: Returns -1 for white.
//Result is ANDed with 0x0f0f for proper functioning. This trick will
//not work for 256 colour. Debugging pending!
inline DPixel Pad::getpix(int x, int y){
    DPixel pix;
    if(x>=0 && y>=0 && x<attr.wcsizex && y<attr.wcsizey) {
        lseek(img,
            Linear(x, y)*sizeof(DPixel)+sizeof(ImageInfo), SEEK_SET);
        read(img, &pix, sizeof(DPixel));
        return pix&0x0f0f;
    }
    return -1;
}

//Plots a dot(one DPixel) in the canvas and on the screen if that
//section of the canvas is visible on the screen.
inline void Pad::setpix(int x, int y, DPixel pix) {
    if(pix==ERASE)
        pix=(getpix(x, y)>>8);
    else if(pix==TEMP)
        pix=(getpix(x, y)& 0x00ff)^pix;
    pix=pix & 0x00ff;
    if(wcstartx<=x && x<wcendx &&
        wcstarty <= y && y<wcendy)
        WritePixel(x-wcstartx+vpstartx, y-wcstarty+vpstarty, pix);
    draw(x, y, pix);
}

//Plots a dot sized according to the current thickness value.
void Pad::pen(int x, int y, DPixel pix) {
    if(thickness==1) setpix(x, y, pix);
    else if(thickness==2) {
        setpix(x+1, y, pix);
        setpix(x, y+1, pix);
        setpix(x+1, y+1, pix);
        setpix(x, y, pix);
    } else if(thickness==3) {
        setpix(x-1, y-1, pix);
        setpix(x, y-1, pix);
        setpix(x+1, y-1, pix);
        setpix(x-1, y, pix);
        setpix(x, y, pix);
        setpix(x+1, y, pix);
        setpix(x-1, y+1, pix);
        setpix(x, y+1, pix);
        setpix(x+1, y+1, pix);
    } else {
        setpix(x, y, pix);
        Beep();
    }
}

//Saves the current image in the temporary store.
void Pad::commit() {
    int i, j;
    DPixel pix;
    union { DPixel p; char c[2]; };
    if(xmax<-1) return;
    for(i=ymin; i<=ymax; i++)
        for(j=xmin; j<=xmax; j++){
            lseek(img, Linear(j, i)*sizeof(DPixel)+sizeof(ImageInfo),
                SEEK_SET);
            read(img, &pix, sizeof(DPixel));
            c[0]=c[1];
            pix=(pix&0x00ff);
            pix=(pix<<8)|pix;
            lseek(img, Linear(j, i)*sizeof(DPixel)+sizeof(ImageInfo),
                SEEK_SET);
            write(img, &pix, sizeof(DPixel));
        }
    xmin=attr.wcsizex; ymin=attr.wcsizey;
    xmax=ymax=-1;
}

//Restores the current image to what it was at the last commit.
void Pad::roll_back() {
    int i, j;
    DPixel pix;
    union { DPixel p; char c[2]; };
    if(xmax<0)
        return;
    for(i=ymin; i<=ymax; i++)
        for(j=xmin; j<=xmax; j++){
            lseek(img, Linear(j, i)*sizeof(DPixel)+sizeof(ImageInfo),
                SEEK_SET);
            read(img, &pix, sizeof(DPixel));
            c[1]=c[0];
            pix=(pix&0xff00);
            pix=(pix>>8)|pix;
            lseek(img, Linear(j, i)*sizeof(DPixel)+sizeof(ImageInfo),
                SEEK_SET);
            write(img, &pix, sizeof(DPixel));
        }
    gobject.show();
}

//Draws a line using the midpoint line algirithm
void Pad::MpLine(int x0, int y0, int x1, int y1) {
    int x, y, dx, dy, incE, incNE, sx, sy, temp, exch=0, d, i;
    x=x0; y=y0;
    dx=abs(x0-x1); dy=abs(y0-y1);
    sx=(x1<x0)? -1:(x1>x0) ? 1: 0;
    sy=(y1<y0)? -1:(y1>y0) ? 1: 0;
    if(dy>dx) {
        temp=dx;
        dx=dy;
        dy=temp;
        exch=1;
    }
    incE=2*dy;
    incNE=2*(dy-dx);
    d=2*dy-dx;
    for(i=0; i<=dx; i++) {
        pen(x,y, fgc);
        if(exch==1)
            y=y+sy;
        else x=x+sx;
        if(d>=0) {
            if(exch==1)
                x=x+sx;
            else y=y+sy;
            d+=incNE;
        }
        else d+=incE;
    }
    return;
}

//Draws a circle using the midpoint circle algorithm.
//The fourth parameter is for use in future(draw ellipse).
void Pad::MpCircle(int xc, int yc, int r1, int r2) {
    int x, y, d;
    x=0; y=r1;    d=1-r1;
    circlepoints(xc, yc, x, y);
    while(y>x) {
        if(d<0)
            d+=2*x+3;
        else { d+=2*(x-y)+5; y--; }
        x++;
        circlepoints(xc, yc, x, y);
    }
}

inline void Pad::circlepoints(int xc, int yc, int x, int y){
    pen(xc+x, yc+y,fgc);
    pen(xc+x, yc-y,fgc);
    pen(xc-x, yc+y,fgc);
    pen(xc-x, yc-y,fgc);
    pen(xc+y, yc+x,fgc);
    pen(xc+y, yc-x,fgc);
    pen(xc-y, yc+x,fgc);
    pen(xc-y, yc-x,fgc);
}

void Pad::polyline(int n, int *p) {
    int i, a, b;
    a=p[0]; b=p[1];
    n=2*n;
    for(i=2; i<n; i+=2)  {
        MpLine(a, b, p[i], p[i+1]);
        a=p[i]; b=p[i+1];
    }
}

//Draws Bezier curve with a the specified control points and
//smoothness.
void Pad::Bezier(float xa, float ya, float xb, float yb,
                float xc, float yc, float xd, float yd, int n) {
    float xab, yab, xbc, ybc, xcd, ycd, xabc, yabc, xbcd, ybcd,
        xabcd, yabcd;
    if(n==0) {
        MpLine(xa, ya, xb, yb);
        MpLine(xb, yb, xc, yc);
        MpLine(xc, yc, xd, yd);
    } else {
        xab=(xa+xb)/2.0;  yab=(ya+yb)/2.0;
        xbc=(xb+xc)/2.0;  ybc=(yb+yc)/2.0;
        xcd=(xc+xd)/2.0;  ycd=(yc+yd)/2.0;
        xabc=(xab+xbc)/2.0;  yabc=(yab+ybc)/2.0;
        xbcd=(xbc+xcd)/2.0;  ybcd=(ybc+ycd)/2.0;
        xabcd=(xabc+xbcd)/2.0;  yabcd=(yabc+ybcd)/2.0;
        Bezier(xa, ya, xab, yab, xabc, yabc, xabcd, yabcd, n-1);
        Bezier(xabcd, yabcd, xbcd, ybcd, xcd, ycd, xd, yd, n-1);
    }
}

//Tests whether cursor is in the drawing area and obtains cursor
//position within the drawing area.
inline int Pad::getxy(int &x, int &y) {
    int mx, my;
    mouse.read();
    mx=mouse.getx();
    my=mouse.gety();
    if(vpstartx<=mx && mx <vpendx && vpstarty <= my && my <vpendy) {
        x=mx-vpstartx+wcstartx; y=my-vpstarty+wcstarty;
        return 1;
    }
    return 0;
}

//Floodfill
//Commented lines should not be deleted.
void Pad::ffill(int x, int y) {
//    long WarnValue, count;
    int backColor, fill_color=fgc;
    Node *temp, *lastnode,*head;
    if((backColor=getpix(x, y))==-1){ cout <<backColor; getch();return;}
    backColor=(0x00ff & backColor);
    if(backColor==fill_color)
        return;
    lastnode=head=(Node *)malloc(sizeof(Node));
    head->x=x;
    head->y=y;
    head->next=NULL;
//    count=0;    WarnValue=1;
    while(head!=NULL) {
        setpix(x,y, fgc);
        if((0x00ff & getpix(x,y-1))==backColor){
            setpix(x,y-1,fill_color);
//            if(count++>WarnValue)
//              { Beep(); WarnValue*=10; }//one screen takes 3e+5
//Insert(x, y-1)
            temp=(Node *)malloc(sizeof(Node));
            temp->x=x;
            temp->y=y-1;
            temp->next=NULL;
            lastnode->next=temp;
            lastnode=temp;
        }
        if((0x00ff & getpix(x+1,y))==backColor){
            setpix(x+1,y,fgc);
//            if(count++>WarnValue)
//              { Beep(); WarnValue*=10; }//one screen takes 3e+5
//Insert(x+1, y)
            temp=(Node *)malloc(sizeof(Node));
            temp->x=x+1;
            temp->y=y;
            temp->next=NULL;
            lastnode->next=temp;
            lastnode=temp;
        }
        if((0x00ff & getpix(x,y+1))==backColor){
            setpix(x,y+1,fgc);
//            if(count++>WarnValue)
//              { Beep(); WarnValue*=10; }//one screen takes 3e+5
//Insert(x, y+1)
            temp=(Node *)malloc(sizeof(Node));
            temp->x=x;
            temp->y=y+1;
            temp->next=NULL;
            lastnode->next=temp;
            lastnode=temp;
        }
        if((0x00ff & getpix(x-1,y))==backColor){
            setpix(x-1,y,fgc);
//            if(count++>WarnValue)
//              { Beep(); WarnValue*=10; }//one screen takes 3e+5
//Insert(x-1, y)
            temp=(Node *)malloc(sizeof(Node));
            temp->x=x-1;
            temp->y=y;
            temp->next=NULL;
            lastnode->next=temp;
            lastnode=temp;
        }
        if(head!=NULL) {
        temp=head;
        head=head->next;
        free(temp);
//        count--;
        x=head->x;
        y=head->y;
        }
    }
}

//Performs the operation corresponding to the value of 'state'.
int Pad::perform() {
    int i, j, x0, y0, x1, y1, x, y, xa[4], ya[4], flag;
    char str[10];
    if(!getxy(x0, y0))
        return 0;
    mouse.hide();
    gobject.show();
    mouse.show();
    switch(state) {
        case PENCIL:
            fb.Msg("<Pencil> Hold and drag for Free-form drawing.");
            while(!mouse.left() && test()){
                if(!getxy(x0, y0))
                    return 0;
                fb.Update(x0, y0);
            }
            mouse.hide();
            while(mouse.left()) {
                if(!getxy(x1, y1))
                    break;
                if (x0 != x1 || y0 != y1) {
                    MpLine(x0, y0, x1, y1);
                    fb.Update(x1, y1);
                    x0 = x1;
                    y0 = y1;
                }
            }
            mouse.show();
            break;
        case ERASER:
fb.Msg("<Eraser> Hold and drag to paint the selected colour.");
            while(!mouse.left() && test()){
                if(!getxy(x0, y0))
                    return 0;
                fb.Update(x0, y0);
            }
            mouse.hide();
            while(mouse.left()) {
                if(!getxy(x1, y1))
                    break;
                if (x0 != x1 || y0 != y1) {
                    fb.Update(x1, y1);
                    for(i=-4; i<4; i++)
                        for(j=-4; j<4; j++)
                            setpix(x1+i, y1+j, fgc);
                    x0 = x1;
                    y0 = y1;
                }
            }
            mouse.show();
            break;
        case AIRBRUSH:
            fb.Msg("<Airbrush> Hold and drag to spray the selected colour.");
            while(!mouse.left() && test()){
                if(!getxy(x0, y0))
                    return 0;
                fb.Update(x0, y0);
            }
            mouse.hide();
            while(mouse.left()) {
                if(!getxy(x1, y1))
                    break;
                if (x0 != x1 || y0 != y1) {
                    delay(40);
                    setpix(x1, y1-3, fgc);
                    setpix(x1+3, y1, fgc);
                    setpix(x1, y1+3, fgc);
                    setpix(x1-3, y1, fgc);
                    fb.Update(x1, y1);
                    x0 = x1;
                    y0 = y1;
                }
            }
            mouse.show();
            break;
        case FLOOD:
            fb.Msg(
                "<Paint can> Click to pour the selected colour.");
            while(!mouse.click()){
                if(!test())
                    return 0;
                if(!getxy(x0, y0)) return 0;
                fb.Update(x0, y0);
            }
              mouse.hide();
              ffill(x0, y0);
              mouse.show();
            break;
        case DRAWLINE:
            fb.Msg(
            "<Straight line> Hold left-button and drag.");
            while(!mouse.left() && test()){
                if(!getxy(x, y))
                    return 0;
                fb.Update(x, y);
            }
            if(!getxy(x0, y0))
                return 1;
            mouse.set_hbound(vpstartx, vpendx-1);
            mouse.set_vbound(vpstarty, vpendy-1);
            mouse.hide();
            gobject.AddSeg(LINE, x, y, x0, y0);
            gobject.showlast();
            while(mouse.left()) {
                if(!getxy(x1, y1))
                    break;
                if (x0 != x1 || y0 != y1) {
                    fb.Update(x1, y1);
                    gobject.eraselast();
                    gobject.DelSeg();
                    gobject.AddSeg(LINE, x, y, x1, y1);
                    gobject.showlast();
                    x0=x1; y0=y1;
                }
            }
            mouse.set_hbound(0, Maxx);
            mouse.set_vbound(0, Maxy);
            gobject.show();
            mouse.show();
            break;
        case DRAWPOLY:
            fb.Msg("<Polyline> Click to add line.");
            while(!mouse.left() && test()){
                if(!getxy(x0, y0))
                    return 0;
                fb.Update(x0, y0);
            }
            while(getxy(x1, y1)) {
                if(mouse.click()){
                    gobject.erase();
                    gobject.AddSeg(LINE, x0, y0, x1, y1);
                    mouse.hide();
                    gobject.show();
                    mouse.show();
                    x0=x1; y0=y1;
                }
                fb.Update(x1, y1);
            }
//      segment_clear_button.setalive();
            break;
        case DRAWBEZIER:
            fb.Msg(
                "<Bezier curve> Click to add four controll points.");
            while(!mouse.click()){
                if(!test())
                    return 0;
                getxy(x0, y0);
                fb.Update(x0, y0);
            }
            xa[0]=x0; ya[0]=y0;
            i=1;
            while(getxy(x1, y1)) {
                if(i>3)
                    break;
                if(mouse.click()){
                    mouse.hide();
                    gobject.AddSeg(LINE, x0, y0, x1, y1);
                    xa[i]=x1; ya[i]=y1;
                    gobject.showlast();
                    mouse.show();
                    x0=x1; y0=y1;
                    i++;
                }
                fb.Update(x1, y1);
            }
            mouse.hide();
            gobject.eraselast(3);
            gobject.DelSeg();
            gobject.DelSeg();
            gobject.DelSeg();
            gobject.AddSeg(BEZIER, xa[0], ya[0], xa[1],ya[1]);
            gobject.AddSeg(BEZIER, xa[2], ya[2], xa[3],ya[3]);
            gobject.showlast();
            mouse.show();
            //edit Bezier-curve
            mouse.set_hbound(vpstartx, vpstartx+vpsizex-1);
            mouse.set_vbound(vpstarty,  vpstarty+vpsizey-1);
            fb.Msg(
"<Bezier Curve> Edit: Click to switch b/w control points. Press Esc to finalize.");
            i=0;
            mouse.set_position(xa[i]-wcstartx+vpstartx,
                ya[i]-wcstarty+vpstarty);
            x0=xa[i]; y0=ya[i];
            while(test()&!kbhit()) {
                if(!getxy(x, y)) break;
                fb.Update(x, y);
                if(mouse.click()) {
                    i=(i+1)%4;;
                    mouse.set_position(xa[i]-wcstartx+vpstartx,
                        ya[i]-wcstarty+vpstarty);
                    x0=xa[i]; y0=ya[i];
                }
                while(test()&&!mouse.left()) {
                    if(kbhit()) break;
                    getxy(x1, y1);
                    if(x1!=x0||y1!=y0) {
                        fb.Update(x1, y1);
                        xa[i]=x1; ya[i]=y1;
                        mouse.hide();
                        gobject.eraselast();
                        gobject.DelSeg();
                        gobject.AddSeg(BEZIER, xa[0], ya[0], xa[1],ya[1]);
                        gobject.AddSeg(BEZIER, xa[2], ya[2], xa[3],ya[3]);
                        gobject.showlast();
                        mouse.show();
                        x0=x1; y0=y1;
                    }
                }
            }
            mouse.hide();
            gobject.show();
            mouse.show();
            mouse.set_hbound(0, Maxx);
            mouse.set_vbound(0, Maxy);
            pd.setstate(PENCIL);
            break;
        case DRAWCIRCLE:
            fb.Msg(
            "<Draw circle> Hold left-button and drag.");
            while(!mouse.left() && test()){
                if(!getxy(x, y))
                    return 0;
                fb.Update(x, y);
            }
            if(!getxy(x0, y0))
                return 0;
            mouse.set_hbound(vpstartx, vpendx-1);
            mouse.set_vbound(vpstarty, vpendy-1);
            gobject.AddSeg(CIRCLE, x, y,
                ((x0-x)>(y0-y))?(x0-x):(y0-y),0);
            gobject.showlast();
            mouse.hide();
            while(mouse.left()) {
                if(!getxy(x1, y1))
                    break;
                if (x0 != x1 || y0 != y1) {
                    fb.Update(x1, y1);
                    gobject.eraselast();
                    gobject.DelSeg();
                    gobject.AddSeg(CIRCLE, x, y,
                    (abs(x1-x)>abs(y1-y))?abs(x1-x):abs(y1-y), 0);
                    gobject.showlast();
                    x0=x1; y0=y1;
                }
            }
            mouse.set_hbound(0, Maxx);
            mouse.set_vbound(0, Maxy);
            gobject.show();
            mouse.show();
            break;
        case DRAWRECT:
            fb.Msg(
            "<Draw rectangle> Hold left-button and drag.");
            while(!mouse.left() && test()){
                if(!getxy(x, y))
                    return 0;
                fb.Update(x, y);
            }
            if(!getxy(x0, y0))
                return 0;
            mouse.set_hbound(vpstartx, vpendx-1);
            mouse.set_vbound(vpstarty, vpendy-1);
            mouse.hide();
            gobject.AddSeg(LINE, x, y, x0, y);
            gobject.AddSeg(LINE, x0, y, x0, y0);
            gobject.AddSeg(LINE, x0, y0, x, y0);
            gobject.AddSeg(LINE, x, y0, x, y);
            gobject.showlast(4);
            while(mouse.left()) {
                if(!getxy(x1, y1))
                    break;
                if (x0 != x1 || y0 != y1) {
                    fb.Update(x1, y1);
                    gobject.eraselast(4);
                    gobject.DelSeg();
                    gobject.DelSeg();
                    gobject.DelSeg();
                    gobject.DelSeg();
                    gobject.AddSeg(LINE, x, y, x1, y);
                    gobject.AddSeg(LINE, x1, y, x1, y1);
                    gobject.AddSeg(LINE, x1, y1, x, y1);
                    gobject.AddSeg(LINE, x, y1, x, y);
                    gobject.showlast(4);
                    x0=x1; y0=y1;
                }
            }
            mouse.set_hbound(0, Maxx);
            mouse.set_vbound(0, Maxy);
            gobject.show();
            mouse.show();
            break;
        case TRANSLATE:
            fb.Msg(
            "<Translate graphic layer> Hold left-button and drag.");
            while(!mouse.left() && test()){
                if(!getxy(x0, y0))
                    return 0;
                fb.Update(x0, y0);
            }
            while(mouse.left()) {
                if(!getxy(x1, y1))
                    return 1;
                if (x0 != x1 || y0 != y1) {
                    mouse.hide();
                    gobject.erase();
                    gobject.translate(x0, y0, x1, y1);
                    gobject.show();
                    fb.Update(x1, y1);
                    mouse.show();
                    x0 = x1; y0 = y1;
                }
            }
            break;
        case SCALE:
            fb.Msg(
"<Scale graphic layer> Hold left-button and drag to scale.");
            while(!mouse.left() && test()){
                getxy(x, y);
                fb.Update(x, y);
            }
            gobject.store();
            while(mouse.left()) {
                if(!getxy(x1, y1))
                    return 1;
                if (x0 != x1 || y0 != y1) {
                    mouse.hide();
                    gobject.erase();
                    gobject.recall();
                    gobject.scale(x, y, x1, y1);
                    gobject.show();
                    fb.Update(x1, y1);
                    mouse.show();
                    x0 = x1; y0 = y1;
                }
            }
            break;
        case ROTATE:
            fb.Msg(
"<Rotate graphic layer> Hold left-button and drag to rotate.");
            while(!mouse.left()  && test()){
                getxy(x, y);
                fb.Update(x, y);
            }
            gobject.store();
            while(mouse.left()) {
                if(!getxy(x1, y1))
                    return 1;
                if (x0 != x1 || y0 != y1) {
                    mouse.hide();
                    gobject.erase();
                    gobject.recall();
                    gobject.rotate(x, y, x1, y1);
                    gobject.show();
                    fb.Update(x1, y1);
                    mouse.show();
                    x0 = x1; y0 = y1;
                }
            }
            break;
        case BTEXT:
            fb.Msg(
            "<Text> Hold left-button and drag to insert text box.");
            while(!mouse.left() && test()){
                if(!getxy(x, y))
                    return 0;
                fb.Update(x, y);
            }
            if(!getxy(x0, y0))
                return 0;
            mouse.set_hbound(vpstartx, vpendx-1);
            mouse.set_vbound(vpstarty, vpendy-1);
            mouse.hide();
            gobject.AddSeg(LINE, x, y, x0, y);
            gobject.AddSeg(LINE, x0, y, x0, y0);
            gobject.AddSeg(LINE, x0, y0, x, y0);
            gobject.AddSeg(LINE, x, y0, x, y);
            gobject.showlast(4);
            while(mouse.left()) {
                if(!getxy(x1, y1))
                    break;
                if (x0 != x1 || y0 != y1) {
                    fb.Update(x1, y1);
                    gobject.eraselast(4);
                    gobject.DelSeg();
                    gobject.DelSeg();
                    gobject.DelSeg();
                    gobject.DelSeg();
                    gobject.AddSeg(LINE, x, y, x1, y);
                    gobject.AddSeg(LINE, x1, y, x1, y1);
                    gobject.AddSeg(LINE, x1, y1, x, y1);
                    gobject.AddSeg(LINE, x, y1, x, y);
                    gobject.showlast(4);
                    x0=x1; y0=y1;
                }
            }
            fb.Msg("<Text> Press Escape to exit text box.");
            flag=0;
            str[1]='\0';
            for(j=y+2; j<y1-7; j+=9){
                for(i=x+1; i<x1-7; i+=8) {
                    if((str[0]=WaitKey())==ESC_KEY) {
                        flag=1;
                        break;
                    }
                    if(str[0]==BACKSPACE &&(i>x)) {
                        //i=i-2*8;
                        Beep();
                    } else if(str[0]==13){
                        break;
                    } else outtextxy(i-wcstartx+vpstartx, j-wcstarty+vpstarty, str);
                }
                if(flag) break;
            }
            fb.Msg("<Text> Converting to image. Please wait...");
            gobject.eraselast(4);
            gobject.DelSeg();
            gobject.DelSeg();
            gobject.DelSeg();
            gobject.DelSeg();
            ScrnCapture();
            mouse.show();
            mouse.set_hbound(0, Maxx);
            mouse.set_vbound(0, Maxy);
            break;
        default:
            fb.Msg("Drawing Pad");
            break;
    }
    return 1;
}

//Displays fullscreen preview of the image file.
void Pad::preview() {
  int vpsizex1, vpsizey1, vpstartx1, vpstarty1, vpendx1, vpendy1;
  vpsizex1=vpsizex;
  vpsizey1=vpsizey;
  vpstartx1=vpstartx;
  vpstarty1=vpstarty;
  vpendx1=vpendx;
  vpendy1=vpendy;
  pd.position(0, 0);
  pd.resize(MinOf(Maxx, attr.wcsizex), MinOf(Maxy, attr.wcsizey));
  setfillstyle(XHATCH_FILL, LIGHTGRAY);
  bar(0, 0, Maxx, Maxy);
  pd.show();
  pd.resize(vpsizex1, vpsizey1);
  vpsizex=vpsizex1;
  vpsizey=vpsizey1;
  vpstartx=vpstartx1;
  vpstarty=vpstarty1;
  vpendx=vpendx1;
  vpendy=vpendy1;
}

//Creates a new pad of dimension (sx, sy).
void Pad::new_Pad(int sx, int sy) {
    long isize, i;
    DPixel pix;
    setbgc(WHITE);
    setfgc(RED);
    load(DFL_PATH);
    SetImageInfo(sx, sy);
    isize=(long)attr.wcsizex*attr.wcsizey;
    if(isize>640*480) {
        pix=0x0f0f;
        lseek(img, sizeof(ImageInfo)+640*480*sizeof(DPixel), SEEK_SET);
        for(i=640*480; i<isize; i++)
          write(img, &pix, sizeof(DPixel));
        position(101, 22);
        resize(Maxx-119, Maxy-60);
        strcpy(curfile, "\0");
    }
    commit();
}

//Allows the user to open an image file.
int Pad::user_open() {
    int ret_val;
    char fname[50];
    fb.Msg("<Open> Enter file name : ");
    if(fb.GetInput(fname)==0) return 0;
    if(strlen(fname)>2){
        fb.Msg("<Open> Loading file. Please wait...");
        ret_val=load(fname);
        if(ret_val==0){
            fb.Msg("<Open> File opened. Press any key to edit image.");
            save_screen(ptr);
            preview();
            WaitKey();
            restore_screen(ptr);
            show();
            return 0;
        }
        else if(ret_val==-1)
            fb.Msg("<Open> Error! Invalid file name. Press any key.");
        else if(ret_val==-2)
            fb.Msg("<Open> Error! Unknown file type. Press any key.");
        else if(ret_val==-3)
            fb.Msg("<Open> Error! Unknown BSJ version. Press any key.");
        Beep();
        WaitKey();
    }
    return -1;     
}

//Allows a user to save image.
int Pad::user_save() {
    char fname[50];
    fb.Msg("<Save> Enter file name : ");
    if(fb.GetInput(fname)==0) return 0;
    if(strlen(fname)>2&&strncmp(fname, "prn", 3)){
        fb.Msg("<Save> Saving file. Please wait...");
        commit();
        if(save(fname)==-1) {
            Beep();
            fb.Msg("<Save> Error! Invalid filename.");
            delay(1000);
            return -1;
        }
        fb.Msg("<Save> File saved");
        return 0;
    }
    Beep();
    return -1;
}

//Tests whether the cursor is in Pads active region.
inline int Pad::test() {
  mouse.read();
    if(vpstartx<=mouse.getx() && mouse.getx()<vpendx &&
        vpstarty<=mouse.gety()
        && mouse.gety()<vpendy)
        return 1;
    else return 0;
}

////GraClass member functions////////////////////////////////////////

//Adds a new segment to the graphic object.
void GraClass::AddSeg(int t, int a, int b, int c, int d) {
    sg[nSeg].type=t;
    sg[nSeg].a=a;
    sg[nSeg].b=b;
    sg[nSeg].c=c;
    sg[nSeg].d=d;
    nSeg++;
}

//Removes the last segment from the graphic object.
void GraClass::DelSeg() {
    if(nSeg<=0){
        Beep();
        return;
    }
    switch(sg[nSeg-1].type) {
        case LINE:
        case CIRCLE:
            nSeg=nSeg-1;
            break;
        case BEZIER:
            nSeg=nSeg-2;
            break;
    }
}

//Displays the last segment of the graphic object in TEMP colour.
void GraClass::showlast(int n) {
    int i, old;
    old=pd.getfgc();
    pd.setfgc(TEMP);
    for(i=nSeg;  i>0;) {
        if(n--<1)
            break;
        switch(sg[i-1].type) {
            case LINE:
            case CIRCLE:
                i=i-1;
                break;
            case BEZIER:
                i=i-2;
                break;
            default:
                i--;
                break;
        }
    }
    for(; i<nSeg; i++) {
        switch(sg[i].type) {
            case LINE:
                pd.MpLine(sg[i].a, sg[i].b, sg[i].c, sg[i].d);
                break;
            case CIRCLE:
                pd.MpCircle(sg[i].a, sg[i].b, sg[i].c, sg[i].d);
                break;
            case BEZIER:
                pd.Bezier(sg[i].a, sg[i].b, sg[i].c, sg[i].d,
                sg[i+1].a, sg[i+1].b, sg[i+1].c, sg[i+1].d, 4);
                i++;
                break;
            default:
                break;
        }
    }
    pd.setfgc(old);
  /*  pd.setpix(xp, yp, RED);
    pd.setpix(xp+1, yp, RED);
    pd.setpix(xp, yp+1, RED);
    pd.setpix(xp+1, yp+1, RED);
    pd.setpix(xp-1, yp, RED);
    pd.setpix(xp, yp-1, RED);
    pd.setpix(xp-1, yp-1, RED);
    pd.setpix(xp-1, yp, RED);
*/
}

//Displays all the segments of the graphic object.
void GraClass::show() {
    int i;
    for(i=0; i<nSeg; i++) {
        switch(sg[i].type) {
            case LINE:
                pd.MpLine(sg[i].a, sg[i].b, sg[i].c, sg[i].d);
                break;
            case CIRCLE:
                pd.MpCircle(sg[i].a, sg[i].b, sg[i].c, sg[i].d);
                break;
            case BEZIER:
                pd.Bezier(sg[i].a, sg[i].b, sg[i].c, sg[i].d,
                    sg[i+1].a, sg[i+1].b, sg[i+1].c, sg[i+1].d, 4);
                i++;
                break;
            default:
                break;
        }
  /*  pd.setpix(xp, yp, RED);
    pd.setpix(xp+1, yp, RED);
    pd.setpix(xp, yp+1, RED);
    pd.setpix(xp+1, yp+1, RED);
    pd.setpix(xp-1, yp, RED);
    pd.setpix(xp, yp-1, RED);
    pd.setpix(xp-1, yp-1, RED);
    pd.setpix(xp-1, yp, RED);
*/}
}

//Erases the graphic object on the screen(while relocating etc.)
void GraClass::erase() {
    int tmp;
    tmp=pd.getfgc();
    pd.setfgc(ERASE);
    show();
    pd.setfgc(tmp);
}

//Erases the segment displayed by showlast().
//Should be called once after every showlast() before another call to
//showlast().
void GraClass::eraselast(int n) {
    int tmp;
    tmp=pd.getfgc();
    pd.setfgc(TEMP);
    showlast(n);
    pd.setfgc(tmp);
}

//Translates the graphic object(all segments).
void GraClass::translate(int x0, int y0, int x1, int y1) {
    int i;
    double h[3][3];
    IdentityMatrix(h);
    Translate(x1-x0, y1-y0, h);
    for(i=0; i<nSeg; i++) {
        switch(sg[i].type) {
            case LINE:
                Transform(sg[i].a, sg[i].b, h);
                Transform(sg[i].c, sg[i].d, h);
                break;
            case CIRCLE:
                Transform(sg[i].a, sg[i].b, h);
                break;
            case BEZIER:
                Transform(sg[i].a, sg[i].b, h);
                Transform(sg[i].c, sg[i].d, h);
            default:
                break;
        }
    }
}

//Performs rotation of the graphic object(all segments).
void GraClass::rotate(int x0, int y0, int x1, int y1) {
    int i;
    double h[3][3], a0, a1;
    a0=R2D * atan2(y0, x0);
    if(a0<0)
        a0=360.0+a0;
    a1=R2D * atan2(y1, x1);
    if(a1<0)
        a1=360.0+a1;
    IdentityMatrix(h);
    Rotate(a0-a1, h);
    for(i=0; i<nSeg; i++) {
        switch(sg[i].type) {
            case LINE:
            case BEZIER:
                Transform(sg[i].a, sg[i].b, h);
                Transform(sg[i].c, sg[i].d, h);
                break;
            case CIRCLE:
                Transform(sg[i].a, sg[i].b, h);
                break;
            default:
                break;
        }
    }
}

//Performs scaling of the graphic object(all segments).
void GraClass::scale(int x0, int y0, int x1, int y1) {
    int i;
    double h[3][3];
    IdentityMatrix(h);
    Translate(-xp, -yp, h);
    ScaleAbout(xp, yp,
        ((double)(abs(x1-xp))/(double)(abs(x0-xp)+0.00000001))/1.000000,
        ((double)(abs(y1-yp))/(double)(abs(y0-yp)+0.00000001))/1.000000,
        h);
    Translate(xp, yp, h);
    for(i=0; i<nSeg; i++) {
        switch(sg[i].type) {
        case LINE:
            Transform(sg[i].a, sg[i].b, h);
            Transform(sg[i].c, sg[i].d, h);
            break;
        case BEZIER:
            Transform(sg[i].a, sg[i].b, h);
            Transform(sg[i].c, sg[i].d, h);
            break;
        case CIRCLE:
            Transform(sg[i].a, sg[i].b, h);
            Transform(sg[i].c, sg[i].d, h);
            break;
        default:
            break;
        }
    }
}

//Stores contents of the segment buffer in an auxiliary buffer.
void GraClass::store() {
    int i;
    for(i=0; i<nSeg; i++)
        aux[i]=sg[i];
}

//Restores the segment buffer with the contents stored in the
//auxiliary buffer.
void GraClass::recall() {
    int i;
    for(i=0; i<nSeg; i++)
        sg[i]=aux[i];
}

////Button class member functions////////////////////////////////////

//Initialize position(x,y), size(ssx, ssy), bitmap offset in
//resource file (off), description string (dstr)
void Button::init(int x, int y, int ssx, int ssy, int off,
    char *dstr) {
    img_off=off;
    xsize=ssx; ysize=ssy;
    strcpy(desc, dstr);
    position(x, y);
}

//Display the button.
void Button::show() {
    int i, j;
    char ch;
    if(state==DEAD) {
        for(i=0; i<ysize; i++)
            for(j=0; j<xsize; j++) {
                lseek(res_fd, i*xsize+j+img_off, SEEK_SET);
                read(res_fd, &ch, 1);
                if(ch!=BLACK)
                    ch=LIGHTGRAY;
                WritePixel(xp+j, yp+i, ch);
            }
        boarder=LIGHTGRAY;
        return;
    }
    for(i=0; i<ysize; i++)
        for(j=0; j<xsize; j++) {
            lseek(res_fd, i*xsize+j+img_off, SEEK_SET);
            read(res_fd, &ch, 1);
            WritePixel(xp+j, yp+i, ch);
        }
    boarder=ch;
}

//Test whether the cursor is in the buttons region.
inline int Button::test() {
    if(xp<=mouse.getx() && mouse.getx()<xp+xsize && yp<=mouse.gety()
        && mouse.gety()<yp+ysize)
        return 1;
    else return 0;
}

//Activate the button and wait for mouse click until the cursor
//leaves the buttons region.
int Button::select() {
    int flag=0;
    if(state==DEAD)
        return 0;
    mouse.hide();
    setcolor(15-boarder);
    rectangle(xp, yp, xp+xsize-1, yp+ysize-1);
    mouse.show();
    fb.Msg(desc);
    while(test())
        if(mouse.click()){
            flag=1;
            break;
        }
    mouse.hide();
    setcolor(boarder);
    rectangle(xp, yp, xp+ysize-1, yp+xsize-1);
    mouse.show();
    return flag;
}

////FeedBack class member functions////////////////////////////////

//Displays a message, msg in the notification area.
void FeedBack::Msg(char *msg) {
    if(!strcmp(msgstr, msg))
        return;
    strcpy(msgstr, msg);
    WriteText(
"                                                                              ",
    5, Maxy-20);
    WriteText(msgstr, 5, Maxy-20);
}
//Displays a prompt for keyboard inputs.
void FeedBack::ShowPrompt(char *pr) {
    WriteText(
"                                                                ",
    5, Maxy-10);
    WriteText(pr,5, Maxy-10);
}
//Read a string from the keyboard(with echo).
int FeedBack::GetInput(char *inp) {
    int i;
    inp[0]='\0';
    for(i=0; (inp[i]=WaitKey())!=13; i++) {
        if(inp[i]==BACKSPACE) i=i-2;
        else if(inp[i]==ESC_KEY) return 0;
        inp[i+1]='\0';
        WriteText(
"                                                                ",
        5, Maxy-9);
        WriteText(inp,5, Maxy-9);
    }
    inp[i]='\0';
    WriteText(
"                                                                ",
    5, Maxy-9);
    return i;
}

//Update the cursor location (x, y) displayed in the notification
//area.
int FeedBack::Update(int x, int y) {
    if(x0==x && y0==y)
        return 0;
    x0=x; y0=y;
    if(x==-1) {
        WriteText("        ",Maxx-80, Maxy-10);
        return 0;
        }
    sprintf(loc_str, " %d,%d \0", x, y);
    WriteText(loc_str,Maxx-80, Maxy-10);
    return 1;
}

//Displays the caption.
void FeedBack::Caption() {
    setfillstyle(SOLID_FILL, BLUE);
    bar(0,0, Maxx-1, 20);
//  settextstyle(TRIPLEX_FONT , HORIZ_DIR, 1);
//  setusercharsize(1, 2, 1, 2);
    WriteText(capstr,14,8,WHITE, BLUE);
//    settextstyle(DEFAULT_FONT,HORIZ_DIR,1);
    logo_button.setalive();
}

//Displays current thickness and colour settings for line drawing.
void FeedBack::LineStatus() {
    setfillstyle(SOLID_FILL, WHITE);
    bar(25, 260, 65, 280);
    setfillstyle(SOLID_FILL, pd.getfgc());
    bar(25, 270, 65, 270+pd.getthickness()-1);
}

////ColourBox class member functions/////////////////////////////////

void ColourBox::show() {
    int i, j, k;
    setcolor(BLACK);
    rectangle(xp-1, yp-1, xp+xs, yp+ys);
    for(i=0, k=0; i<4; i++)
        for(j=0; j<4; j++, k++) {
            setfillstyle(SOLID_FILL, k);
            bar(xp+j*xs/4, yp+ys*i/4,
                xp+(j+1)*xs/4-1, yp+ys*(i+1)/4-1);
        }
}

//Checks whether the mouse cursor is in the colourbox area.
inline int ColourBox::test() {
    if(xp<=mouse.getx() && mouse.getx()<xp+xs && yp<=mouse.gety()
        && mouse.gety()<yp+ys)
        return 1;
    else return 0;
}

//Selects forground colour on mouse-click.
void ColourBox::select() {
    int x, y;
    fb.Msg("Select colour.");
    while(test())
        if(mouse.click()){
            x=(mouse.getx()-xp)/(xs/4); y=(mouse.gety()-yp)/(ys/4);
            pd.setfgc(y*4+x);
            fb.LineStatus();
            break;
        }
}

/////////////////////////////////////////////////////////////////////
//Starts a user session /////////////////////////////////////////////
void session(char *fname=NULL) {
    int x,y;
    char str[100];
    open_resource();
    InitializeGraphics();
    mouse.hide();
    setfillstyle(SOLID_FILL, LIGHTGRAY);
    bar(0, 0, Maxx, Maxy);
    fb.Msg(
" Combine (c) Sujan S. A. & R. Srikanth      Initializing.  Please wait...");
    if(fname==NULL) pd.load(DFL_PATH);
    else {
        pd.load(fname);
        pd.preview();
        if(WaitKey()==ESC_KEY) {
            CloseGraphics();
            exit(0);
        }
    }
    setfillstyle(SOLID_FILL, LIGHTGRAY);
    bar(0, 0, Maxx, Maxy);
    pd.position(101, 22);
    pd.resize(Maxx-119, Maxy-60);
    setcolor(BLUE);
    rectangle(0, 0, Maxx, Maxy);
    setcolor(0);
    rectangle(101-1, 22-1, 101+Maxx-119, 22+Maxy-60);
    rectangle(101-1, 22-1,
        101+Maxx-119+15, 22+pd.vpsizey+15);
    scrlup.init(101+Maxx-119, 22,
        15, 15, 450, "Scroll up");
    scrldown.init(101+Maxx-119, 22+Maxy-60-15,
        15, 15, 225, "Scroll down");
    scrlright.init(101+Maxx-119-15, 22+Maxy-60,
        15, 15, 900, "Scroll right");
    scrlleft.init(101, 22+Maxy-60,
        15, 15, 675, "Scroll left");

    fb.SetCaption(" Combine");
    logo_button.init(10, 0, 20, 20, 10300, "Combine");
    fb.Caption();
    new_button.init(10, 40, 20, 20, 1140, "New");
    save_button.init(35, 40, 20, 20, 1940, "Save As");
    open_button.init(60, 40, 20, 20, 1540, "Open: a *.bsj file");
    commit_button.init(10, 70, 20, 20, 3140,
        "Commit: Makes the changes made so far irrevokable.");
    roll_back_button.init(35, 70, 20, 20, 2740,
        "Roll Back: the image to the previously committed image.");
    segment_clear_button.init(60,70, 20, 20, 3540,
"<New Graphic object> Clears segment buffer (define a new graphic object).");
    eraser_button.init(10, 100, 20, 20, 9500,
        "Eraser");
    airbrush_button.init(35, 100, 20, 20, 8700,
        "Airbrush");
    flood_button.init(60, 100, 20, 20, 8300, "Paint can");
    pencil_button.init(10, 130, 20, 20, 2340,
        "Pencil: Draws a free-form line.");
    bezier_button.init(35, 130, 20, 20, 6320, "Bezier curve");
    circle_button.init(60, 130, 20, 20, 5920, "Circle");
    line_button.init(10, 160, 20, 20, 5520,
        "Line: Draws a straight line.");
    poly_button.init(35, 160, 20, 20, 3940, "Polyline");
    rect_button.init(60, 160, 20, 20, 7900, "Rectangle");
    trans_button.init(10, 190, 20, 20, 4340, "Translate");
    scale_button.init(35, 190, 20, 20, 4720, "Scale");
    rotate_button.init(60, 190, 20, 20, 5120, "Rotate");
    help_button.init(35, 220, 20, 20, 9100, "Help");
    text_button.init(10, 220, 20, 20, 9900, "Text");
    thick1_button.init(10, 295, 20, 20, 6720, "Thin line");
    thick2_button.init(35, 295, 20, 20, 7100, "Medium thick line");
    thick3_button.init(60, 295, 20, 20, 7500, "Thick line");
    close_button.init(Maxx-20, 3, 15, 15, 0, "Exit Combine!");
    gobject.setpivot(0, 0);
    close_button.setalive();
    scrlup.setalive();
    scrldown.setalive();
    scrlright.setalive();
    scrlleft.setalive();
    scrlup.setalive();
    new_button.setalive();
    save_button.setalive();
    open_button.setalive();
    commit_button.setalive();
    roll_back_button.setalive();
    segment_clear_button.setalive();
    eraser_button.setalive();
    airbrush_button.setalive();
    flood_button.setalive();
    pencil_button.setalive();
    bezier_button.setalive();
    circle_button.setalive();
    line_button.setalive();
    poly_button.setalive();
    rect_button.setalive();
    trans_button.setalive();
    scale_button.setalive();
    rotate_button.setalive();
    help_button.setalive();
    text_button.setalive();
    thick1_button.setalive();
    thick2_button.setalive();
    thick3_button.setalive();
    fb.Msg(
" Combine  (c) Sujan S. A. & R. Srikanth      Initializing.  Please wait...");
    pd.setstate(DRAWLINE);
    fb.LineStatus();
    cb.init(10, 330, 80, 75);
    cb.show();
    pd.show();
    mouse.show();

    while(1) {
  LABEL1: fb.Update(-1,-1);
        mouse.read();
        if(pd.test()) {
            pd.perform();
        } else if(close_button.test()) {
            if(close_button.select()) {
                pd.imgclose();
                fb.Msg(
"Closing Combine OK.  Thank you.    Press any key.");
                //WaitKey();
                CloseGraphics();
                exit(0);
            }
        } else if(new_button.test()) {
            if(new_button.select()) {
                do{
                    x=640; y=480;
                    fb.Msg(
                    "<New> Image size : 640x480 pixel ? (y/n)");
                    if(tolower(WaitKey())=='n') {
                        fb.Msg(
                    "<New> Enter horizontal image size(in pixels):");
                        if(fb.GetInput(str)==0) goto LABEL1    ;
                        x=atoi(str);
                        fb.Msg(
                    "<New> Enter vertical image size(in pixels):");
                        fb.GetInput(str);
                        y=atoi(str);
                    }
                    fb.Msg("<New> Are you sure?");
                }while(tolower(WaitKey())=='n');
                fb.Msg("<New> Initialising.    Please wait...");
                pd.new_Pad(x, y);
                pd.show();
                delay(500);
                fb.Caption();
                close_button.setalive();
            }
        } else if(save_button.test()) {
            if(save_button.select()) {
                pd.user_save();
                delay(500);
                fb.Caption();
                close_button.setalive();
            }
        } else if(open_button.test()) {
            if(open_button.select()) {
                pd.user_open();
                delay(500);
                fb.Caption();
                close_button.setalive();
            }
        } else if(scrlup.test()) {
            if(scrlup.select())
                pd.scroll_up();
        } else if(scrldown.test()) {
            if(scrldown.select())
                pd.scroll_down();
        } else if(scrlright.test()) {
            if(scrlright.select())
                pd.scroll_right();
        } else if(scrlleft.test()) {
            if(scrlleft.select())
            pd.scroll_left();
        } else if(commit_button.test()) {
            if(commit_button.select()) {
                mouse.hide();
                fb.Msg("Performing Commit.  Please Wait...");
                pd.commit();
                mouse.show();
            }
        } else if(roll_back_button.test()) {
            if(roll_back_button.select()) {
                mouse.hide();
                fb.Msg("Performing Roll-back.  Please Wait...");
                pd.roll_back();
                pd.show();
                mouse.show();
            }
        } else if(segment_clear_button.test()) {
            if(segment_clear_button.select()){
                gobject.init();
                //not rolling back
                //segment_clear_button.setdead();
            }
        } else if(pencil_button.test()) {
            if(pencil_button.select())
                pd.setstate(PENCIL);
        } else if(airbrush_button.test()) {
            if(airbrush_button.select())
                pd.setstate(AIRBRUSH);
        } else if(flood_button.test()) {
            if(flood_button.select())
                pd.setstate(FLOOD);
        } else if(eraser_button.test()) {
            if(eraser_button.select())
                pd.setstate(ERASER);
        } else if(line_button.test()) {
            if(line_button.select())
                pd.setstate(DRAWLINE);
        } else if(bezier_button.test()) {
            if(bezier_button.select())
                pd.setstate(DRAWBEZIER);
        } else if(poly_button.test()) {
            if(poly_button.select())
                pd.setstate(DRAWPOLY);
        } else if(rect_button.test()) {
            if(rect_button.select())
                pd.setstate(DRAWRECT);
        } else if(circle_button.test()) {
            if(circle_button.select())
                pd.setstate(DRAWCIRCLE);
        } else if(trans_button.test()) {
            if(trans_button.select())
                pd.setstate(TRANSLATE);
        } else if(scale_button.test()) {
            if(scale_button.select())
                pd.setstate(SCALE);
        } else if(rotate_button.test()) {
            if(rotate_button.select())
                pd.setstate(ROTATE);
        } else if(text_button.test()) {
            if(text_button.select())
                pd.setstate(BTEXT);
        } else if(help_button.test()) {
            if(help_button.select()) {
                  showhelp();
            }
        } else if(thick1_button.test()) {
            if(thick1_button.select()) {
                pd.setthickness(1);
                fb.LineStatus();
            }
        } else if(thick2_button.test()) {
            if(thick2_button.select()) {
                pd.setthickness(2);
                fb.LineStatus();
            }
        } else if(thick3_button.test()) {
            if(thick3_button.select()) {
                pd.setthickness(3);
                fb.LineStatus();
            }
        } else if(cb.test())
            cb.select();
        else if(kbhit()) {
            switch(WaitKey()) {
                case 'F':
                    if(!pd.getxy(x, y)) break;
                    pd.ffill(x, y);
                    break;
                case 'p':
                case 'P':
                    fb.Msg(
                        "Are you sure you want to change pivot ?");
                    if(tolower(WaitKey())!='y') break;
                    fb.Msg("Choose pivot");
                    while(!mouse.click()){
                        pd.getxy(x, y);
                        fb.Update(x, y);
                    }
                    gobject.setpivot(x, y);
                    break;
                default:
                    break;
            }
        }//end if kbhit
        else fb.Msg(" ");
    }
}

////C main() function////////////////////////////////////////////////
void main(int argc, char *argv[]) {
    if(argc>1) session(argv[1]);
    else {
        if(system(LOGOSCR_PATH)<0) Beep();
        delay(100);  //Allow enough time for the monitor to restore
        session();
    }
}

////End of Pad24.cpp/////////////////////////////////////////////////
