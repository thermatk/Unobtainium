// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package org.chromium.chrome.browser.media.router;

import android.support.v7.app.MediaRouteChooserDialogFragment;
import android.support.v7.app.MediaRouteControllerDialogFragment;
import android.support.v7.media.MediaRouteSelector;

import org.chromium.base.annotations.CalledByNative;
import org.chromium.base.annotations.JNINamespace;

/**
 * Implements the JNI interface called from the C++ Media Router dialog controller implementation
 * on Android.
 */
@JNINamespace("media_router")
public class ChromeMediaRouterDialogController {

    private static final String MEDIA_ROUTE_CONTROLLER_DIALOG_FRAGMENT =
            "android.support.v7.mediarouter:MediaRouteControllerDialogFragment";

    private final long mNativeDialogController;

    /**
     * Returns a new initialized {@link ChromeMediaRouterDialogController}.
     * @param nativeDialogController the handle of the native object.
     * @return a new dialog controller to use from the native side.
     */
    @CalledByNative
    public static ChromeMediaRouterDialogController create(long nativeDialogController) {
        return new ChromeMediaRouterDialogController(nativeDialogController);
    }

    /**
     * Shows the {@link MediaRouteChooserDialogFragment} if it's not shown yet.
     * @param sourceUrns the URNs identifying the media sources to filter the devices with.
     */
    @CalledByNative
    public void openRouteChooserDialog(String[] sourceUrns) {
    }

    /**
     * Shows the {@link MediaRouteControllerDialogFragment} if it's not shown yet.
     * @param sourceUrn the URN identifying the media source of the current media route.
     * @param mediaRouteId the identifier of the route to be controlled.
     */
    @CalledByNative
    public void openRouteControllerDialog(String sourceUrn, String mediaRouteId) {
    }

    /**
     * Closes the currently open dialog if it's open.
     */
    @CalledByNative
    public void closeDialog() {
    }

    /**
     * @return if any media route dialog is currently open.
     */
    @CalledByNative
    public boolean isShowingDialog() {
        return false;
    }

    private ChromeMediaRouterDialogController(long nativeDialogController) {
        mNativeDialogController = nativeDialogController;
    }

    native void nativeOnDialogCancelled(long nativeMediaRouterDialogControllerAndroid);
    native void nativeOnSinkSelected(
            long nativeMediaRouterDialogControllerAndroid, String sourceUrn, String sinkId);
    native void nativeOnRouteClosed(long nativeMediaRouterDialogControllerAndroid, String routeId);
    native void nativeOnMediaSourceNotSupported(long nativeMediaRouterDialogControllerAndroid);
}
