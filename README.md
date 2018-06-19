[![Donate](https://liberapay.com/assets/widgets/donate.svg)](https://liberapay.com/thermatk/)

# ![Logo](chrome/app/theme/chromium/product_logo_64.png) Unobtainium

Unobtainium is a libre fork of Chromium. Do not expect it to be ungoogled, it's unblobbed!

## Changes:

*Standard set of changes for F-Droid, removing proprietary blobs:*
- GMS libraries are removed
- VR SDK is removed
- Some visual changes to hide disabled functionality
- New package name, icon and app name

*Paranoid FLOSS set, removing free software prebuilts:*
- Custom SDK and NDK are removed, will use provided standard ones
- Custom Clang and LLVM are removed, will use provided system ones(require 7.0)
- Custom binutils are removed, will use system ones
- Custom NodeJS and modules are removed, redownload from upstream
- Prebuilt GN is removed, rebuild from source at compile time
- A lot of other unused binaries are removed

## Building

**Important:**
1. You need the [Android NDK, Revision 16b](https://developer.android.com/ndk/downloads/older_releases) and a preconfigured Android SDK with some build and platform tools.
	- It will fail and tell you what you lack

2. Don't forget to include the submodules when you clone:
      - `git clone --recursive https://github.com/thermatk/Unobtainium.git`

4. Install required build dependencies:
      - If you're on a supported Debian/Ubuntu, this should work:

      ```
      build/install-build-deps-android.sh
      ```
      - On other distros you'll have to find out what to install yourself.
      	- This may be enough on Arch: 
      	```
      	sudo pacman -S --needed python perl gcc gcc-libs bison flex gperf pkgconfig nss alsa-lib glib2 gtk2 nspr freetype2 cairo dbus libgnome-keyring bsdiff python-pexpect xorg-server-xvfb lighttpd
      	```
5. You also need a nightly build of LLVM and Clang.
	- On Debian/Ubuntu [this might help](https://apt.llvm.org/)
	- On Arch, use AUR or [binary repos](https://github.com/kerberizer/llvm-svn#binary-packages)
		- clang-svn, llvm-svn, lld-svn

6. Build it:
      - Execute the following (define stuff in brackets):

      ```
      export ANDROID_NDK=[PATH_TO_NDK]
      export ANDROID_HOME=[PATH_TO_SDK]
      ./build.sh [version-code] [arch] [ninja-target]
      ```
      - where [version-code] must be a growing number, [arch] can be x86, arm or arm64 and [ninja-target] can be:
      	- *monochrome_public_apk*: Contains both WebView and Unobtainium, sdk24+
      	- *chrome_modern_public_apk*: Unobtainium, sdk21+
      	- *chrome_public_apk*: Unobtainium, sdk16+
      	- *system_webview_apk*: WebView, sdk21+

7. Don't forget to sign the apk.

# Chromium

Chromium is an open-source browser project that aims to build a safer, faster,
and more stable way for all users to experience the web.

The project's web site is https://www.chromium.org.

Documentation in the source is rooted in [docs/README.md](docs/README.md).

Learn how to [Get Around the Chromium Source Code Directory Structure
](https://www.chromium.org/developers/how-tos/getting-around-the-chrome-source-code).
