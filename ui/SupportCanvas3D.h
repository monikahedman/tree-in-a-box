#ifndef SUPPORTCANVAS3D_H
#define SUPPORTCANVAS3D_H

#include <memory>

#include "GL/glew.h"
#include <QGLWidget>
#include <QTimer>

#include "glm/glm.hpp"

class RGBA;
class Camera;
class OpenGLScene;
class FinalScene;
class OrbitingCamera;
class CS123XmlSceneParser;

/**
 * @class  SupportCanvas3D
 *
 * The SupportCanvas3D class holds a single active OpenGLScene, and either
 * calls upon that scene to draw itself using OpenGL or draws the scene
 * by directly calling upon OpenGL (getting the scene-specific information
 * from the OpenGLScene object). The details of the implementation are left
 * to the student; neither way is better than the other.
 *
 * The SupportCanvas3D also contains a default camera which can be used in
 * case the loaded scene does not specify a camera.
 */

class SupportCanvas3D : public QGLWidget {
    Q_OBJECT
public:
    SupportCanvas3D(QGLFormat format, QWidget *parent);

    virtual ~SupportCanvas3D();

    Camera *getCamera();
    OrbitingCamera *getOrbitingCamera();

    // Returns a pointer to the current scene. If no scene is loaded, this function returns nullptr.
    OpenGLScene *getScene() { return m_currentScene; }

    void loadSceneviewSceneFromParser(CS123XmlSceneParser &parser);

    // Copies pixels from the OpenGL render buffer into a standard bitmap image, using row-major
    // order and RGBA data format.
    void copyPixels(int width, int height, RGBA *data);

    // This function will be called by the UI when the settings have changed.
    virtual void settingsChanged(int options = 0);
    static constexpr int UPDATE_TREE = 1;
    static constexpr int UPDATE_LIGHTCOUNT = 2;
    static constexpr int UPDATE_LIGHTRAND = 4;

signals:
    void aspectRatioChanged();

protected:
    // Overridden from QGLWidget
    virtual void initializeGL() override;
    virtual void paintGL() override;

    // Overridden from QWidget
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void wheelEvent(QWheelEvent *event) override;
    virtual void resizeEvent(QResizeEvent *event) override;

    float m_oldPosX, m_oldPosY, m_oldPosZ;
    float m_oldRotU, m_oldRotV, m_oldRotN;

private:
    QTimer m_timer;
    void initializeGlew();
    void initializeOpenGLSettings();
    void initializeScenes();
    void setSceneFromSettings();


    glm::vec4      m_cameraEye;
    bool           m_isDragging;

    bool m_settingsDirty;

    std::unique_ptr<OrbitingCamera> m_defaultOrbitingCamera;
    OpenGLScene *m_currentScene;
    std::unique_ptr<FinalScene> m_finalScene;
};

#endif // SUPPORTCANVAS3D_H
