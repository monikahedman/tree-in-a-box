#ifndef FINALSCENE_H
#define FINALSCENE_H

#include "OpenGLScene.h"

#include <QSize>
#include <memory>
#include <QTime>

#include <GL/glew.h>

#include "gl/datatype/FBO.h"
#include "Settings.h"

#include "gl/datatype/VBO.h"
#include "gl/datatype/VAO.h"
#include "gl/openglshape.h"
#include "gl/shaders/ShaderAttribLocations.h"

#include "shape/Shape.h"
#include "shape/Cube.h"
#include "shape/Sphere.h"
#include "shape/Cone.h"
#include "shape/Cylinder.h"
#include "shape/RoundedTaperedCylinder.h"

#include "tree/Tree.h"
#include "tree/Leaf.h"
#include "gl/textures/Texture2D.h"

namespace CS123 { namespace GL {

    class Shader;
    class CS123Shader;
    class Texture2D;
}}

class FinalScene : public OpenGLScene {
public:
    FinalScene();
    virtual ~FinalScene();

    virtual void render(SupportCanvas3D *context) override;
    virtual void settingsChanged(int options = 0) override;

    // Use this method to set an internal selection, based on the (x, y) position of the mouse
    // pointer.  This will be used during the "modeler" lab, so don't worry about it for now.
    void setSelection(int x, int y);

private:
    void initRain();
    void initFullscreenQuad();
    void resizeRainFBOs(int numParticles);
    std::unique_ptr<CS123::GL::FBO> &getCurrentRainFBO();
    std::unique_ptr<CS123::GL::FBO> &getNextRainFBO();
    void updateRainFBOs();
    void drawRainFBO(SupportCanvas3D *context);
    void loadLeafShader();
    void loadRainShaders();
    void loadFirstPassShader() ;
    void loadPuddleShader();
    void loadWindowShader();
    void loadPhongShader();
    void loadWireframeShader();
    void loadNormalsShader();
    void loadNormalsArrowShader();
    void loadTextures();
    void loadDeferredTextures();
    void setSceneUniforms(SupportCanvas3D *context, Shader *shader);
    void setMatrixUniforms(CS123::GL::Shader *shader, SupportCanvas3D *context);
    void setLights(CS123Shader *shader);
    void renderPhongGeometry();
    void renderLightVolumes();
    void renderGround();
    void renderTree();
    void renderLeaves(SupportCanvas3D *context);
    void resizeFBO(const SupportCanvas3D *context);
    int adjustedRainParticleCount();
    void initializeLightList();
    void colorLights();
    glm::vec3 getRandomColor();
    glm::vec4 getRandomPosition();
    glm::vec4 getCameraEye(SupportCanvas3D *context);
    glm::vec4 getLightColors();

    /**
     * @brief m_startTime This is used to pass the iTime uniform to the window shader.
     */
    QTime m_startTime;

    /**
     * @brief m_quad This is the fullscreen quad used for the window shader.
     */
    std::unique_ptr<OpenGLShape> m_quad;

    /**
     * @brief m_FBO This FBO is used in conjunction with the window shader.
     * The scene is rendered to the FBO, and the window shader reads from the FBO and draws to the screen.
     */
    std::unique_ptr<CS123::GL::FBO> m_FBO1;
    std::unique_ptr<CS123::GL::FBO> m_FBO2;

    /**
     * @brief m_evenRainFBO This FBO is used in conjunction with the rain update shader.
     * These FBOs hold particle information.
     */
    std::unique_ptr<CS123::GL::FBO> m_evenRainFBO;
    std::unique_ptr<CS123::GL::FBO> m_oddRainFBO;

    /**
     * @brief m_useEvenRainFBO This keeps track of which FBO is currently being used.
     */
    bool m_useEvenRainFBO;

    /**
     * @brief m_resetRainFBO If this is true, the rain simulation is reset during the next update.
     */
    bool m_resetRainFBO;

    /**
     * @brief m_particlesVAO This VAO is used to draw the rain particles.
     */
    GLuint m_particlesVAO;

    /**
     * @brief m_FBOsize This is the FBO's old size.
     * If the FBO's size has changed between render calls, a new FBO is created.
     */
    QSize m_FBOsize;

    /**
     * @brief m_grassTexture the grass texture
     */
    std::unique_ptr<Texture2D> m_grassTexture;

    /**
     * @brief m_barkTexture the bark texture
     */
    std::unique_ptr<Texture2D> m_barkTexture;

    /**
     * @brief m_colorTexture the position texture for the scene
     */
    std::unique_ptr<Texture2D> m_positionTexture;

    /**
     * @brief m_colorTexture the normal texture for the scene
     */
    std::unique_ptr<Texture2D> m_normalTexture;

    /**
     * @brief m_colorTexture the color texture for the scene
     */
    std::unique_ptr<Texture2D> m_colorTexture;

    std::unique_ptr<CS123::GL::Shader> m_rainUpdateShader;
    std::unique_ptr<CS123::GL::Shader> m_rainDrawShader;
    std::unique_ptr<CS123::GL::CS123Shader> m_puddleShader;
    std::unique_ptr<CS123::GL::CS123Shader> m_windowShader;
    std::unique_ptr<CS123::GL::CS123Shader> m_phongShader;
    std::unique_ptr<CS123::GL::CS123Shader> m_deferredShader;
    std::unique_ptr<CS123::GL::CS123Shader> m_firstPassShader;
    std::unique_ptr<CS123::GL::Shader> m_wireframeShader;
    std::unique_ptr<CS123::GL::Shader> m_normalsShader;
    std::unique_ptr<CS123::GL::Shader> m_normalsArrowShader;
    std::unique_ptr<CS123::GL::CS123Shader> m_leafShader;

    // primitive shapes that contain a method to draw themselves in the scene
    std::unique_ptr<Shape> m_cube;
    std::unique_ptr<Shape> m_reverseCube;
    std::unique_ptr<Shape> m_cone;
    std::unique_ptr<Shape> m_cylinder;
    std::unique_ptr<Shape> m_sphere;
    std::unique_ptr<Shape> m_tapered;

    std::unique_ptr<Tree> m_tree;
    std::unique_ptr<Leaf> m_leaf;

    std::vector<CS123SceneLightData> m_deferredLights;
};

#endif // FINALSCENE_H
