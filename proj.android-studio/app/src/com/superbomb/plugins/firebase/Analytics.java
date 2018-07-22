package com.superbomb.plugins.firebase;

import android.app.Activity;
import android.os.Bundle;
import android.text.TextUtils;

import com.google.firebase.analytics.FirebaseAnalytics;

import java.util.Hashtable;
import java.util.Iterator;

public class Analytics {
    private static Analytics instance = null;
    public static Analytics getInstance() {
        if( instance == null ) {
            instance = new Analytics();
        }

        return instance;
    }

    private Activity context;
    private FirebaseAnalytics firebaseAnalytics;

    private Analytics() {

    }

    public void init(Activity context) {

        this.context = context;
        firebaseAnalytics = FirebaseAnalytics.getInstance(context);
    }

    public static FirebaseAnalytics getFirebaseAnalytics() {
        return getInstance().firebaseAnalytics;
    }

    public static Activity getContext() {
        return getInstance().context;
    }

    public static void setScreenName(final String screen, final String screenClass) {

        getContext().runOnUiThread(new Runnable() {
            @Override
            public void run() {
                getFirebaseAnalytics().setCurrentScreen(getContext(), screen,
                        !TextUtils.isEmpty(screenClass) ? screenClass : null);
            }
        });
    }

    public static void logEvent(String event, Hashtable<String, String> params) {

        Bundle bundle = new Bundle();
        Iterator<String> it = params.keySet().iterator();

        while( it.hasNext() ) {
            String key = it.next();
            bundle.putString(key, params.get(key));
        }

        getFirebaseAnalytics().logEvent(event, bundle);
    }
}
