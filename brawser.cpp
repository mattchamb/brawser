#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <emscripten.h>
#include "libjpeg/jpeglib.h"
#include "lensfun_webassembly/lensfun/lensfun.h"
#include "LibRaw-0.18.4/libraw/libraw.h"


void EMSCRIPTEN_KEEPALIVE asdf(void* buffer, size_t size) {
    LibRaw *raw = new LibRaw();
    raw->open_buffer(buffer, size);
    raw->unpack();

    raw->imgdata.params.use_camera_wb = 1;
    raw->imgdata.params.use_auto_wb = 0;
    raw->imgdata.params.no_auto_bright = 1;

    raw->dcraw_process();
    libraw_processed_image_t* img = raw->dcraw_make_mem_image();

    raw->dcraw_clear_mem(img);
    raw->recycle();
    delete raw;
}

void EMSCRIPTEN_KEEPALIVE qwer() {
    lfLens* lens = new lfLens();

    lfModifier* mod = new lfModifier(lens, 1, 10, 10);

    mod->ApplyColorModification(NULL, 0.0, 1, 1, 1, LF_CR_3 (RED, GREEN, BLUE), 0);
    mod->ApplySubpixelGeometryDistortion (0.0, 1, 1, 1, NULL);

    delete lens;
}

void EMSCRIPTEN_KEEPALIVE aaaa(JSAMPLE* image_buffer) {
    jpeg_compress_struct cinfo;
    jpeg_error_mgr jerr;
    JSAMPROW row_pointer[1];	/* pointer to JSAMPLE row[s] */
    int row_stride;		/* physical row width in image buffer */
    int image_width = 1;
    int quality = 95;
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);
    unsigned char* outbuff;
    unsigned long outSize;
    jpeg_mem_dest(&cinfo, &outbuff, &outSize);

    cinfo.image_width = 1; 	/* image width and height, in pixels */
    cinfo.image_height = 1;
    cinfo.input_components = 3;		/* # of color components per pixel */
    cinfo.in_color_space = JCS_RGB; 	/* colorspace of input image */
    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, quality, TRUE /* limit to baseline-JPEG values */);
    jpeg_start_compress(&cinfo, TRUE);

    row_stride = image_width * 3;	/* JSAMPLEs per row in image_buffer */
    
    while (cinfo.next_scanline < cinfo.image_height) {
        row_pointer[0] = & image_buffer[cinfo.next_scanline * row_stride];
        (void) jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }
    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);
}