#!/bin/bash

CURRENT_DIR=$(cd $(dirname $0); pwd)
PROJECT_DIR=$(cd $(dirname $CURRENT_DIR); pwd)

cd ${PROJECT_DIR}/src
autoninja -C out/Chromium/ chrome_public_apk

echo "${PROJECT_DIR}/src/out/Chromium/apks/ChromePublic.apk"

adb install out/Chromium/apks/ChromePublic.apk
adb shell am start -n org.chromium.chrome/com.google.android.apps.chrome.Main
