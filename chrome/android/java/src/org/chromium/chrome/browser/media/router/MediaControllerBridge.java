// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package org.chromium.chrome.browser.media.router;

import org.chromium.base.annotations.CalledByNative;
import org.chromium.base.annotations.JNINamespace;

/**
 * A wrapper around a MediaController that allows the native code to use it.
 * See chrome/browser/media/android/remote/media_controller_bridge.h for the corresponding native
 * code.
 */
@JNINamespace("media_router")
public class MediaControllerBridge {
    @CalledByNative
    public void play() {
    }

    @CalledByNative
    public void pause() {
    }

    @CalledByNative
    public void setMute(boolean mute) {
    }

    @CalledByNative
    public void setVolume(float volume) {
    }

    @CalledByNative
    public void seek(long positionInMs) {
    }
}
