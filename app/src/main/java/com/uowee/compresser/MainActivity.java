package com.uowee.compresser;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.widget.TextView;
import android.widget.Toast;

import com.missile.compress.CompressCore;

import java.io.File;
import java.util.List;

import pub.devrel.easypermissions.AppSettingsDialog;
import pub.devrel.easypermissions.EasyPermissions;

public class MainActivity extends AppCompatActivity implements EasyPermissions.PermissionCallbacks {

    private static final int REQUEST_EXTERNAL_STORAGE = 1;
    private static String[] PERMISSIONS_STORAGE = {
            "android.permission.READ_EXTERNAL_STORAGE",
            "android.permission.WRITE_EXTERNAL_STORAGE"};

    private static final String DEFAULT_PATH = Environment.getExternalStorageDirectory().getAbsolutePath() + File.separator;

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Example of a call to a native method
        TextView tv = (TextView) findViewById(R.id.sample_text);
        tv.setText(stringFromJNI());


        if (EasyPermissions.hasPermissions(this, PERMISSIONS_STORAGE)) {
            Bitmap bitmap = BitmapFactory.decodeFile(DEFAULT_PATH + "print.bmp");
            String result = DEFAULT_PATH + "compress.jpg";
            long time = System.currentTimeMillis();
            int qu = 20;
            int ret = CompressCore.compressBitmap(bitmap, qu, result, true);
            Log.e("jpeg-turbo", "Native " + (System.currentTimeMillis() - time) + ", Ret: " + ret);

            if (ret == 1) {
                Toast.makeText(this, "Compress Successful.", Toast.LENGTH_LONG).show();
            }
        } else {
            EasyPermissions.requestPermissions(this, "Read and Write SD Card",
                    REQUEST_EXTERNAL_STORAGE, PERMISSIONS_STORAGE);
        }

    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();

    public native int compressBitmap(Bitmap bitmap, int quality, String destFile, boolean optimize);


    @Override
    public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        // EasyPermissions handles the request result.
        EasyPermissions.onRequestPermissionsResult(requestCode, permissions, grantResults, this);
    }

    @Override
    public void onPermissionsGranted(int requestCode, List<String> perms) {
        Bitmap bitmap = BitmapFactory.decodeFile(DEFAULT_PATH + "Paxprint.bmp");
        String result = DEFAULT_PATH + "compress.jpg";
        long time = System.currentTimeMillis();
        int qu = 20;
        int ret = CompressCore.compressBitmap(bitmap, qu, result, true);
        Log.e("jpeg-turbo", "Native " + (System.currentTimeMillis() - time) + ", Ret: " + ret);

        if (ret == 1) {
            Toast.makeText(this, "Compress Successful.", Toast.LENGTH_LONG).show();
        }
    }

    @Override
    public void onPermissionsDenied(int requestCode, List<String> perms) {
        if (EasyPermissions.somePermissionPermanentlyDenied(this, perms)) {
            new AppSettingsDialog.Builder(this).build().show();
        }
    }
}
