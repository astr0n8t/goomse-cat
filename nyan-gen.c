#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


char *header = "#ifndef __NYAN_%s_INC\n#define __NYAN_%s_INC\n// automatically generated by %s.\n\
static EFI_GRAPHICS_OUTPUT_BLT_PIXEL %s[] = { \n// each chunk is pixel is a pixel %d*%d\n";

char *footer = "};\n\
#endif\n\
";

FILE *out;

typedef struct __attribute__((__packed__)) {
    unsigned char    fType[2];
    unsigned char    newline1;
    unsigned char    w[2];
    unsigned char    newline2;
    unsigned char    h[2];
    unsigned char    newline3;
    unsigned char    bDepth[3];
    unsigned char    newline4;
} INFOHEADER;

typedef struct __attribute__((__packed__)) {
    uint8_t    r;
    uint8_t    g;
    uint8_t    b;
} IMAGE;

void hexDump (char *desc, IMAGE *pc, int len) {
    int i;

    // Process every pixel in the data.
    for (i = 0; i < len/3; i++) {
        // Multiple of 16 means new line (with line offset).
        if ((i % 8) == 0) {
            // Just don't print ASCII for the zeroth line.
            if (i != 0) fputs("\n", out);
        }

        fprintf(out, "{0x%02x,0x%02x,0x%02x,0}", pc[i].b, pc[i].g, pc[i].r);
        
        if (i != len/3-1) {
            fputs(",", out);
        }
    }
}

int main(int argc, char *argv[]) {
    INFOHEADER ih;
    char garbage[3];
    int width, height, bitdepth;
    IMAGE *im;
    FILE *img;

    if (argc != 4) return -1;

    img = fopen(argv[1], "rb");
    out = fopen(argv[2], "w");

    fread(&ih, sizeof(unsigned char), sizeof(INFOHEADER), img);
    sscanf((unsigned char *)&ih, "%s %d %d %d", garbage, &width, &height, &bitdepth);
    printf("w = %d, h = %d\n", width, height);

    im = malloc(sizeof(IMAGE) * width * height);
    fread(im, sizeof(IMAGE), width * height, img);

    fprintf(out, header, argv[3], argv[3], argv[0], argv[3], width, height);
    hexDump("image", im, sizeof(IMAGE) * width * height);
    fputs(footer, out);

    fclose(out);
    return 0;
}
