package com.superbomb.plugins;

import android.app.Activity;
import android.content.Intent;

public interface PluginListener {

    public void init(Activity context);
    public void onStart();
    public void onStop();
    public void onResume();
    public void onPause();
    public void onDestroy();
    public boolean onBackPressed();
    public boolean onActivityResult(int requestCode, int resultCode, Intent data);
}
