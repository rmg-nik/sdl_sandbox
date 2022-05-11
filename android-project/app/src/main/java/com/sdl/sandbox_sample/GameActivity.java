package com.sdl.sandbox_sample;

import android.content.pm.ActivityInfo;
import android.os.Bundle;
import android.view.View;

public class GameActivity extends org.libsdl.app.SDLActivity {

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        getWindow().getDecorView().setSystemUiVisibility(
                // Set the content to appear under the system bars so that the
                // content doesn't resize when the system bars hide and show.
                View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                        | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                        | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                        // Hide the nav bar and status bar
                        | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                        | View.SYSTEM_UI_FLAG_FULLSCREEN
                        | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY);
    }

    @Override
    protected String getMainSharedObject() {
        return "libsandbox_sample.so";
    }

    @Override
    protected String[] getLibraries() {
        return new String[]{
                "sandbox_sample"
        };
    }
}
