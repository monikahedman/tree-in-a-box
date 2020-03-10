/*!

 Settings.h
 CS123 Support Code

 @author  Evan Wallace (edwallac)
 @date    9/1/2010

 This file contains various settings and enumerations that you will need to
 use in the various assignments. The settings are bound to the GUI via static
 data bindings.

**/

#include "Settings.h"
#include <QFile>
#include <QSettings>

Settings settings;


/**
 * Loads the application settings, or, if no saved settings are available, loads default values for
 * the settings. You can change the defaults here.
 */
void Settings::loadSettingsOrDefaults() {
    // Set the default values below
    QSettings s("CS123", "CS123");

    // Final
    branchLength = s.value("branchLength", 0.5f).toDouble();
    branchRadius = s.value("branchRadius", 0.7f).toDouble();
    branchAngle = s.value("branchAngle", 0.5f).toDouble();
    leafSize = s.value("leafSize", 1.f).toDouble();
    leafWidth = s.value("leafWidth", 0.2f).toDouble();

    raindropsOnWindowEnabled = s.value("raindropsOnWindowEnabled", 1).toBool();
    rainParticleCount = s.value("rainParticleCount", 1).toInt();
    rainEnabled = s.value("rainEnabled", 1).toBool();

    // Lights
    randomEnabled = s.value("randomEnabled", 1).toBool();
    lightCount = s.value("lightCount", 50).toInt();
    lightColorRed = s.value("lightColorRed", 255).toInt();
    lightColorBlue = s.value("lightColorBlue", 255).toInt();
    lightColorGreen = s.value("lightColorGreen", 255).toInt();
    lightBrightness = s.value("lightBrightness", 1).toDouble();
    lightRadius = s.value("lightRadius", 8).toDouble();

    // Camtrans
    useOrbitCamera = s.value("useOrbitCamera", true).toBool();
    cameraFov = s.value("cameraFov", 55).toDouble();
    cameraNear = s.value("cameraNear", 0.1).toDouble();
    cameraFar = s.value("cameraFar", 50).toDouble();

    // These are for computing deltas and the values don't matter, so start all dials in the up
    // position
    cameraPosX = 0;
    cameraPosY = 0;
    cameraPosZ = 0;
    cameraRotU = 0;
    cameraRotV = 0;
    cameraRotN = 0;
}

void Settings::saveSettings() {
    QSettings s("CS123", "CS123");

    // Final
    s.setValue("branchLength", branchLength);
    s.setValue("branchRadius", branchRadius);
    s.setValue("branchAngle", branchAngle);
    s.setValue("leafSize", leafSize);
    s.setValue("leafWidth", leafWidth);
    s.setValue("raindropsOnWindowEnabled", raindropsOnWindowEnabled);
    s.setValue("rainParticleCount", rainParticleCount);
    s.setValue("rainEnabled", rainEnabled);
}

int Settings::getSceneMode() {
    return SCENEMODE_FINAL;
}

int Settings::getCameraMode() {
    return CAMERAMODE_ORBIT;
}
