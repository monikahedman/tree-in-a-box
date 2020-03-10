#include "OpenGLScene.h"

#include <GL/glew.h>

#include "Settings.h"

OpenGLScene::~OpenGLScene()
{
}

void OpenGLScene::setClearColor() {
    glClearColor(0.2f, 0.2f, 0.2f, 1.f);
}

void OpenGLScene::settingsChanged(int options) {

}
