package com.missile.compress;

import android.graphics.Bitmap;

public class CompressCore {
    static {
        System.loadLibrary("compress");
    }

    public static int saveBitmap(Bitmap bit, int quality, String fileName, boolean optimize) {
        return compressBitmap(bit, quality, fileName, optimize);
    }

    public static native int compressBitmap(Bitmap bitmap, int quality, String destFile, boolean optimize);


}
