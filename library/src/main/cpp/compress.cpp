#include <jni.h>
#include <string>
#include <android/bitmap.h>
#include <android/log.h>
#include <csetjmp>
#include <cstdlib>
#include <cstring>

extern "C" {
#include "include/jpeglib.h"
}


#define LOG_TAG "jpeg-turbo"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

typedef u_int8_t BYTE;

extern "C"
int generateJPEG(BYTE *data, int w, int h, jint quality, const char *location, jboolean optimize) {
    int nComponent = 3;
    struct jpeg_compress_struct jcs;
    struct jpeg_error_mgr jem;

    jcs.err = jpeg_std_error(&jem);

    jpeg_create_compress(&jcs);
    LOGE("The File's location = %s", location);
    FILE *f = fopen(location, "wb");
    if (f == NULL) {
        LOGE("The File is NULL");
        return 0;
    }

    jpeg_stdio_dest(&jcs, f);
    jcs.image_height = h;
    jcs.image_width = w;
    jcs.arith_code = false;
    jcs.input_components = nComponent;
    if (nComponent == 1)
        jcs.in_color_space = JCS_GRAYSCALE;
    else
        jcs.in_color_space = JCS_RGB;

    jpeg_set_defaults(&jcs);
    jcs.optimize_coding = optimize;

    jpeg_set_quality(&jcs, quality, true);

    jpeg_start_compress(&jcs, TRUE);

    JSAMPROW row_pointer[1];
    int row_stride;
    row_stride = jcs.image_width * nComponent;
    while (jcs.next_scanline < jcs.image_height) {
        row_pointer[0] = &data[jcs.next_scanline * row_stride];
        jpeg_write_scanlines(&jcs, row_pointer, 1);
    }

    if (jcs.optimize_coding) {
        LOGE("Huffman algorithm is used to complete compression");
    } else {
        LOGE("Unused Huffman algorithm");
    }

    jpeg_finish_compress(&jcs);

    jpeg_destroy_compress(&jcs);
    fclose(f);
    return 1;
}

const char *jstringToString(JNIEnv *env, jstring jstr) {
    char *ret;
    const char *tempStr = env->GetStringUTFChars(jstr, NULL);
    jsize len = env->GetStringUTFLength(jstr);
    if (len > 0) {
        ret = (char *) malloc(len + 1);
        memcpy(ret, tempStr, len);
        ret[len] = 0;
    }
    env->ReleaseStringUTFChars(jstr, tempStr);
    return ret;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_missile_compress_CompressCore_compressBitmap(JNIEnv *env, jobject /* this*/,
                                                      jobject bitmap,
                                                      jint quality, jstring destFile_,
                                                      jboolean optimize) {
    AndroidBitmapInfo androidBitmapInfo;
    BYTE *pixelsColor;
    int ret;
    BYTE *data;
    BYTE *tmpData;

    const char *dstFileName = jstringToString(env, destFile_);
    LOGD("The DST File Name = %s", dstFileName);
    // decode AndroidBitmap Info
    if ((ret = AndroidBitmap_getInfo(env, bitmap, &androidBitmapInfo)) < 0) {
        LOGD("AndroidBitmap_getInfo() failed error = %d", ret);
        return ret;
    }

    if ((ret = AndroidBitmap_lockPixels(env, bitmap, reinterpret_cast<void **>(&pixelsColor))) <
        0) {
        LOGD("AndroidBitmap_lockPixels() failed error = %d", ret);
        return ret;
    }
    LOGI("bitmap: width=%d,height=%d,size=%d , format=%d ",
         androidBitmapInfo.width, androidBitmapInfo.height,
         androidBitmapInfo.height * androidBitmapInfo.width,
         androidBitmapInfo.format);

    BYTE r, g, b;
    int color;

    int w, h, format;
    w = androidBitmapInfo.width;
    h = androidBitmapInfo.height;
    format = androidBitmapInfo.format;

    data = (BYTE *) malloc(androidBitmapInfo.width * androidBitmapInfo.height * 3);
    tmpData = data;

    // convert bitmap to RGB
    for (int i = 0; i < h; ++i) {
        for (int j = 0; j < w; ++j) {
            color = *((int *) (pixelsColor));
            r = ((color & 0x00FF0000) >> 16);
            g = ((color & 0x0000FF00) >> 8);
            b = color & 0X000000FF;

            *data = b;
            *(data + 1) = g;
            *(data + 2) = r;
            data += 3;
            pixelsColor += 4;
        }
    }

    AndroidBitmap_unlockPixels(env, bitmap);
    ret = generateJPEG(tmpData, w, h, quality, dstFileName, optimize);
    free((void *) dstFileName);
    free((void *) tmpData);

    return ret;
}