package com.superbomb.plugins;

import android.app.Activity;
import android.content.Intent;

import com.superbomb.plugins.firebase.Analytics;
import com.superbomb.plugins.gps.GameHelper;

public class PluginManager {

    private static PluginManager instance;
    public static PluginManager getInstance() {
        if( instance == null ) {
            instance = new PluginManager();
        }

        return instance;
    }

    private Activity context;

    private PluginManager() {

    }

    public static void init(Activity context) {

        getInstance().context = context;

        Analytics.getInstance().init(context);
        GameHelper.getInstance().init(context);
    }

    public static void onActivityResult(int requestCode, int resultCode, Intent data) {

        GameHelper.getInstance().onActivityResult(requestCode, resultCode, data);
    }
}
