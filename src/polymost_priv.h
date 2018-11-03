#ifdef POLYMOST

#define PI 3.14159265358979323

enum {
	METH_SOLID   = 0,
	METH_MASKED  = 1,
	METH_TRANS   = 2,
	METH_INTRANS = 3,

	METH_CLAMPED = 4,
	METH_LAYERS  = 8,	    // when given to drawpoly, renders the additional texture layers
    METH_POW2XSPLIT = 16,   // when given to drawpoly, splits polygons for non-2^x-capable GL devices
    METH_ROTATESPRITE = 32, // when given to drawpoly, use the rotatesprite projection matrix
};

typedef struct { unsigned char r, g, b, a; } coltype;
typedef struct { GLfloat r, g, b, a; } coltypef;

extern int rendmode;
extern float gtang;
extern float glox1, gloy1;
extern double gxyaspect, grhalfxdown10x;
extern double gcosang, gsinang, gcosang2, gsinang2;
extern double gchang, gshang, gctang, gstang;
extern int gfogpalnum;
extern float gfogdensity;

struct glfiltermodes {
	char *name;
	int min,mag;
};
#define numglfiltermodes 6
extern struct glfiltermodes glfiltermodes[numglfiltermodes];

extern int gltexcomprquality;	// 0 = fast, 1 = slow and pretty, 2 = very slow and pretty
extern int gltexmaxsize;	// 0 means autodetection on first run
extern int gltexmiplevel;	// discards this many mipmap levels

#ifdef DEBUGGINGAIDS
struct polymostcallcounts {
    int drawpoly_glcall;
    int drawaux_glcall;
    int drawpoly;
    int domost;
};
extern struct polymostcallcounts polymostcallcounts;
#endif

extern const GLfloat gidentitymat[4][4];
extern GLfloat gdrawroomsprojmat[4][4];      // Proj. matrix for drawrooms() calls.
extern GLfloat grotatespriteprojmat[4][4];   // Proj. matrix for rotatesprite() calls.
extern GLfloat gorthoprojmat[4][4];          // Proj. matrix for 2D (aux) calls.

struct polymostvboitem {
    struct {    // Vertex
        GLfloat x, y, z;
    } v;
    struct {    // Texture
        GLfloat s, t;
    } t;
};

struct polymostdrawpolycall {
    int mode;               // 0 = ART, 1 = Hightile

    // Common
    GLuint texture0;
    GLuint texture1;        // Ignored by ART; auto set to palette.
    coltypef fogcolour;
    GLfloat fogdensity;
    coltypef colour;

    // ART
    GLushort palnum;
    GLint ismasked;

    // Hightile
    GLfloat alphacut;

    const GLfloat *modelview;     // 4x4 matrices.
    const GLfloat *projection;

    GLuint indexbuffer;     // Buffer object identifier, or 0 for the global index buffer.
    GLuint indexcount;      // Number of index items.

    GLuint elementbuffer;   // Buffer object identifier. >0 ignores elementvbo.
    GLuint elementcount;    // Number of elements in the element buffer. Ignored if elementbuffer >0.
    const struct polymostvboitem *elementvbo; // Elements. elementbuffer must be 0 to recognise this.
};

// Smallest initial size for the global index buffer.
#define MINVBOINDEXES 16

struct polymostdrawauxcall {
    GLuint texture0;
    coltypef colour;
    coltypef bgcolour;
    int mode;

    GLuint indexcount;      // Number of index items.
    GLushort *indexes;      // Array of indexes, or NULL to use the global index buffer.

    int elementcount;
    struct polymostvboitem *elementvbo;
};

void polymost_drawpoly_glcall(GLenum mode, struct polymostdrawpolycall *draw);

int polymost_texmayhavealpha (int dapicnum, int dapalnum);
void polymost_texinvalidate (int dapicnum, int dapalnum, int dameth);
void polymost_texinvalidateall (void);
void polymost_glinit(void);
void polymost_nextpage(void);
int polymost_palfade(void);
void polymost_drawrooms (void);
void polymost_drawmaskwall (int damaskwallcnt);
void polymost_drawsprite (int snum);
void polymost_dorotatesprite (int sx, int sy, int z, short a, short picnum,
	signed char dashade, unsigned char dapalnum, unsigned char dastat, int cx1, int cy1, int cx2, int cy2, int uniqid);
void polymost_fillpolygon (int npoints);
int polymost_printext256(int xpos, int ypos, short col, short backcol, const char *name, char fontsize);
int polymost_drawline256(int x1, int y1, int x2, int y2, unsigned char col);
int polymost_plotpixel(int x, int y, unsigned char col);
void polymost_setpalette(coltype *pal);
void polymost_initosdfuncs(void);
void polymost_setview(void);

#endif
