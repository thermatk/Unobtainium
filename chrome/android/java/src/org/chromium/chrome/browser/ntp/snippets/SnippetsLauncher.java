// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package org.chromium.chrome.browser.ntp.snippets;

import android.content.Context;
import android.net.ConnectivityManager;

import org.chromium.base.ContextUtils;
import org.chromium.base.Log;
import org.chromium.base.VisibleForTesting;
import org.chromium.base.annotations.CalledByNative;
import org.chromium.chrome.browser.externalauth.ExternalAuthUtils;

/**
 * The {@link SnippetsLauncher} singleton is created and owned by the C++ browser.
 *
 * Thread model: This class is to be run on the UI thread only.
 */
public class SnippetsLauncher {
    private static final String TAG = "SnippetsLauncher";

    // Task tags for fetching snippets.
    public static final String TASK_TAG_WIFI = "FetchSnippetsWifi";
    public static final String TASK_TAG_FALLBACK = "FetchSnippetsFallback";

    // The amount of "flex" to add around the fetching periods, as a ratio of the period.
    private static final double FLEX_FACTOR = 0.1;

    @VisibleForTesting
    public static final String PREF_IS_SCHEDULED = "ntp_snippets.is_scheduled";

    // The instance of SnippetsLauncher currently owned by a C++ SnippetsLauncherAndroid, if any.
    // If it is non-null then the browser is running.
    private static SnippetsLauncher sInstance;

    private boolean mGCMEnabled = true;

    /**
     * Create a SnippetsLauncher object, which is owned by C++.
     */
    @VisibleForTesting
    @CalledByNative
    public static SnippetsLauncher create() {
        if (sInstance != null) {
            throw new IllegalStateException("Already instantiated");
        }

        sInstance = new SnippetsLauncher();
        return sInstance;
    }

    /**
     * Called when the C++ counterpart is deleted.
     */
    @VisibleForTesting
    @CalledByNative
    public void destroy() {
        assert sInstance == this;
        sInstance = null;
    }

    /**
     * Returns true if the native browser has started and created an instance of {@link
     * SnippetsLauncher}.
     */
    public static boolean hasInstance() {
        return sInstance != null;
    }

    protected SnippetsLauncher() {
        checkGCM();
    }

    private void checkGCM() {
        // Check to see if Play Services is up to date, and disable GCM if not.
        if (!ExternalAuthUtils.canUseGooglePlayServices()) {
            mGCMEnabled = false;
            Log.i(TAG, "Disabling SnippetsLauncher because Play Services is not up to date.");
        }
    }

    private void scheduleOrCancelFetchTask(String taskTag, long period, int requiredNetwork) {
    }

    @CalledByNative
    private boolean schedule(long periodWifiSeconds, long periodFallbackSeconds) {
        if (!mGCMEnabled) return false;
        Log.i(TAG, "Scheduling: " + periodWifiSeconds + " " + periodFallbackSeconds);

        boolean isScheduled = periodWifiSeconds != 0 || periodFallbackSeconds != 0;
        ContextUtils.getAppSharedPreferences()
                .edit()
                .putBoolean(PREF_IS_SCHEDULED, isScheduled)
                .apply();
            // Disable GCM for the remainder of this session.
            mGCMEnabled = false;

            ContextUtils.getAppSharedPreferences().edit().remove(PREF_IS_SCHEDULED).apply();
            // Return false so that the failure will be logged.
            return false;
    }

    @CalledByNative
    private boolean unschedule() {
        if (!mGCMEnabled) return false;
        Log.i(TAG, "Unscheduling");
        return schedule(0, 0);
    }

    @CalledByNative
    public boolean isOnUnmeteredConnection() {
        Context context = ContextUtils.getApplicationContext();
        ConnectivityManager manager =
                (ConnectivityManager) context.getSystemService(Context.CONNECTIVITY_SERVICE);
        return !manager.isActiveNetworkMetered();
    }

    public static boolean shouldNotifyOnBrowserUpgraded() {
        // If there was no schedule previously, we do not need to react to upgrades.
        return ContextUtils.getAppSharedPreferences().getBoolean(PREF_IS_SCHEDULED, false);
    }
}

