#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <emscripten.h>
#include <emscripten/bind.h>
#include "libjpeg/jpeglib.h"
#include "lensfun_webassembly/lensfun/lensfun.h"
#include "LibRaw-0.18.4/libraw/libraw.h"

using namespace emscripten;

void asdf(LibRaw* raw, void* buffer, size_t size) {
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

void qwer() {
    lfLens* lens = new lfLens();
    lfModifier* mod = new lfModifier(lens, 1, 10, 10);

    mod->ApplyColorModification(NULL, 0.0, 1, 1, 1, LF_CR_3 (RED, GREEN, BLUE), 0);

    int lwidth = 1 * 2 * 3;
    
    float *pos = new float [lwidth];

    bool ok = mod->ApplySubpixelGeometryDistortion (0.0, 1, 1, 1, pos);
    if (ok)
    {
        float *src = pos;
        for (unsigned x = 0; x < /*img->width*/0; x++)
        {
            // dst->red   = img->GetR (src [0], src [1]);
            // dst->green = img->GetG (src [2], src [3]);
            // dst->blue  = img->GetB (src [4], src [5]);
            src += 2 * 3;
            
        }
    }
    

    delete lens; 
}

std::string test() {
    return "test";
}


void createJpeg(JSAMPLE* image_buffer, int quality) {
    jpeg_compress_struct cinfo;
    jpeg_error_mgr jerr;
    JSAMPROW row_pointer[1];	/* pointer to JSAMPLE row[s] */
    int row_stride;		/* physical row width in image buffer */
    int image_width = 1;
    //int quality = 95;
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

    row_stride = image_width * 3;
    
    while (cinfo.next_scanline < cinfo.image_height) {
        row_pointer[0] = & image_buffer[cinfo.next_scanline * row_stride];
        (void) jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }
    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);
}

EMSCRIPTEN_BINDINGS(my_module) {
    function("qwer", &qwer);
    function("test", &test);
    function("createJpeg", &createJpeg, allow_raw_pointers());
    function("asdf", &asdf, allow_raw_pointers());

    value_object<libraw_processed_image_t>("libraw_processed_image_t")
    .field("data", &libraw_processed_image_t::data)
    .field("data_size", &libraw_processed_image_t::data_size)
    .field("height", &libraw_processed_image_t::height)
    .field("width", &libraw_processed_image_t::width)
    ;

    enum_<lfDistortionModel>("lfDistortionModel")
        .value("LF_DIST_MODEL_NONE", LF_DIST_MODEL_NONE)
        .value("LF_DIST_MODEL_POLY3", LF_DIST_MODEL_POLY3)
        .value("LF_DIST_MODEL_POLY5", LF_DIST_MODEL_POLY5)
        .value("LF_DIST_MODEL_PTLENS", LF_DIST_MODEL_PTLENS)
        .value("LF_DIST_MODEL_ACM", LF_DIST_MODEL_ACM)
        ;
    
    enum_<lfPixelFormat>("lfPixelFormat")
        .value("LF_PF_U8", LF_PF_U8)
        .value("LF_PF_U16", LF_PF_U16)
        .value("LF_PF_U32", LF_PF_U32)
        .value("LF_PF_F32", LF_PF_F32)
        .value("LF_PF_F64", LF_PF_F64)
        ;
    
    enum_<lfLensType>("lfLensType")
        .value("LF_UNKNOWN", LF_UNKNOWN)
        .value("LF_RECTILINEAR", LF_RECTILINEAR)
        .value("LF_FISHEYE", LF_FISHEYE)
        .value("LF_PANORAMIC", LF_PANORAMIC)
        .value("LF_EQUIRECTANGULAR", LF_EQUIRECTANGULAR)
        .value("LF_FISHEYE_ORTHOGRAPHIC", LF_FISHEYE_ORTHOGRAPHIC)
        .value("LF_FISHEYE_STEREOGRAPHIC", LF_FISHEYE_STEREOGRAPHIC)
        .value("LF_FISHEYE_EQUISOLID", LF_FISHEYE_EQUISOLID)
        .value("LF_FISHEYE_THOBY", LF_FISHEYE_THOBY)
        ;

    enum_<lfCropMode>("lfCropMode")
        .value("LF_NO_CROP", LF_NO_CROP)
        .value("LF_CROP_RECTANGLE", LF_CROP_RECTANGLE)
        .value("LF_CROP_CIRCLE", LF_CROP_CIRCLE)
        ;

    enum_<lfVignettingModel>("lfVignettingModel")
        .value("LF_VIGNETTING_MODEL_NONE", LF_VIGNETTING_MODEL_NONE)
        .value("LF_VIGNETTING_MODEL_PA", LF_VIGNETTING_MODEL_PA)
        .value("LF_VIGNETTING_MODEL_ACM", LF_VIGNETTING_MODEL_ACM)
        ;

    value_object<lfLensCalibCrop>("lfLensCalibCrop")
        .field("Focal", &lfLensCalibCrop::Focal)
        .field("CropMode", &lfLensCalibCrop::CropMode)
        .field("Crop", &lfLensCalibCrop::Crop)
        ;

    value_object<lfLensCalibDistortion>("lfLensCalibDistortion")
        .field("Model", &lfLensCalibDistortion::Model)
        .field("Focal", &lfLensCalibDistortion::Focal)
        .field("RealFocal", &lfLensCalibDistortion::RealFocal)
        .field("RealFocalMeasured", &lfLensCalibDistortion::RealFocalMeasured)
        .field("Terms", &lfLensCalibDistortion::Terms)
        ;

    class_<lfLens>("lfLens")
    .constructor()
    .function("SetMaker", &lfLens::SetMaker, allow_raw_pointers())
    .function("SetModel", &lfLens::SetModel, allow_raw_pointers())
    .function("AddMount", &lfLens::AddMount, allow_raw_pointers())
    .function("SetMaker", &lfLens::SetMaker, allow_raw_pointers())
    .function("AddCalibCrop", &lfLens::AddCalibCrop, allow_raw_pointers())
    .function("AddCalibDistortion", &lfLens::AddCalibDistortion, allow_raw_pointers())
    .function("AddCalibFov", &lfLens::AddCalibFov, allow_raw_pointers())
    .function("AddCalibTCA", &lfLens::AddCalibTCA, allow_raw_pointers())
    .function("AddCalibVignetting", &lfLens::AddCalibVignetting, allow_raw_pointers())
    ;

    class_<lfModifier>("lfModifier")
    .constructor<const lfLens*, float, long, long>()
    .function("Initialize", &lfModifier::Initialize, allow_raw_pointers())
    .function("ApplyColorModification", &lfModifier::ApplyColorModification, allow_raw_pointers())
    .function("ApplyGeometryDistortion", &lfModifier::ApplyGeometryDistortion, allow_raw_pointers())
    .function("ApplySubpixelDistortion", &lfModifier::ApplySubpixelDistortion, allow_raw_pointers())
    .function("ApplySubpixelGeometryDistortion", &lfModifier::ApplySubpixelGeometryDistortion, allow_raw_pointers())
    ;
    
    class_<LibRaw>("LibRaw")
    .constructor()
    .function("dcraw_clear_mem", &LibRaw::dcraw_clear_mem, allow_raw_pointers())
    .function("unpack", &LibRaw::unpack, allow_raw_pointers())
    .function("open_buffer", &LibRaw::open_buffer, allow_raw_pointers())
    .function("dcraw_process", &LibRaw::dcraw_process, allow_raw_pointers())
    .function("dcraw_make_mem_image", &LibRaw::dcraw_make_mem_image, allow_raw_pointers())
    .function("dcraw_clear_mem", &LibRaw::dcraw_clear_mem, allow_raw_pointers())
    .function("recycle", &LibRaw::recycle, allow_raw_pointers())
    ;
}

