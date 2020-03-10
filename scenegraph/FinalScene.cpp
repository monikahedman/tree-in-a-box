#include "FinalScene.h"
#include "GL/glew.h"
#include <QGLWidget>
#include "Camera.h"
#include "gl/GLDebug.h"
#include <iostream>

#include "Settings.h"
#include "SupportCanvas3D.h"
#include "ResourceLoader.h"
#include "gl/shaders/CS123Shader.h"

#include "gl/textures/Texture2D.h"
#include "CS123SceneData.h"
#include <QDebug>
#include <glm/gtc/matrix_transform.hpp>

#include <ctime>
#include <cstdlib>

using namespace CS123::GL;
using namespace std;


FinalScene::FinalScene():
    m_quad(nullptr),
    m_FBO1(nullptr),
    m_FBO2(nullptr),
    m_evenRainFBO(nullptr),
    m_oddRainFBO(nullptr),
    m_useEvenRainFBO(true),
    m_resetRainFBO(true),
    m_particlesVAO(0),
    m_FBOsize(0, 0),
    m_grassTexture(nullptr),
    m_barkTexture(nullptr),
    m_rainUpdateShader(nullptr),
    m_rainDrawShader(nullptr),
    m_deferredShader(nullptr),
    m_firstPassShader(nullptr),
    m_leafShader(nullptr),
    m_cube(std::make_unique<Cube>(25, 25)),
    m_reverseCube(std::make_unique<Cube>(25, 25, true)),
    m_cone(std::make_unique<Cone>(25, 25)),
    m_cylinder(std::make_unique<Cylinder>(25, 35)),
    m_sphere(std::make_unique<Sphere>(40, 40)),
    m_tapered(std::make_unique<RoundedTaperedCylinder>(26, 26)),
    m_tree(std::make_unique<Tree>()),
    m_leaf(std::make_unique<Leaf>()),
    m_deferredLights(std::vector<CS123SceneLightData>())
{
    m_cube->loadRenderer();
    m_reverseCube->loadRenderer();
    m_cone->loadRenderer();
    m_cylinder->loadRenderer();
    m_sphere->loadRenderer();
    m_tapered->loadRenderer();
    m_leaf->loadRenderer();

    initFullscreenQuad();
    initRain();
    loadLeafShader();
    loadPuddleShader();
    loadWindowShader();
    loadFirstPassShader();
    loadPhongShader();
    loadWireframeShader();
    loadNormalsShader();
    loadNormalsArrowShader();
    settingsChanged();
    initializeLightList();

    m_startTime = QTime::currentTime();
    loadTextures();
    m_tree->createTree();
}

FinalScene::~FinalScene() {
    glDeleteVertexArrays(1, &m_particlesVAO);
}

void FinalScene::initRain() {
    loadRainShaders();
    resizeRainFBOs(adjustedRainParticleCount());
    glGenVertexArrays(1, &m_particlesVAO);
}

void FinalScene::loadLeafShader() {
    std::string vertexSource = ResourceLoader::loadResourceFileToString(":/shaders/default.vert");
    std::string fragmentSource = ResourceLoader::loadResourceFileToString(":/shaders/leaf.frag");
    m_leafShader = std::make_unique<CS123Shader>(vertexSource, fragmentSource);
}

/**
 * @brief loadRainShaders Loads the rain update and draw shaders.
 */
void FinalScene::loadRainShaders() {
    std::string updateVertexSource = ResourceLoader::loadResourceFileToString(":/shaders/rainupdate.vert");
    std::string updateFragmentSource = ResourceLoader::loadResourceFileToString(":/shaders/rainupdate.frag");
    m_rainUpdateShader = std::make_unique<CS123Shader>(updateVertexSource, updateFragmentSource);
    std::string drawVertexSource = ResourceLoader::loadResourceFileToString(":/shaders/raindraw.vert");
    std::string drawFragmentSource = ResourceLoader::loadResourceFileToString(":/shaders/raindraw.frag");
    m_rainDrawShader = std::make_unique<CS123Shader>(drawVertexSource, drawFragmentSource);
}

/**
 * @brief FinalScene::initRainFBOs Sets up the rain FBOs based on the number of particles.
 */
void FinalScene::resizeRainFBOs(int numParticles) {
    m_evenRainFBO = std::make_unique<CS123::GL::FBO>(2, FBO::DEPTH_STENCIL_ATTACHMENT::NONE, numParticles, 1, TextureParameters::WRAP_METHOD::CLAMP_TO_EDGE, TextureParameters::FILTER_METHOD::NEAREST, GL_FLOAT);
    m_oddRainFBO = std::make_unique<CS123::GL::FBO>(2, FBO::DEPTH_STENCIL_ATTACHMENT::NONE, numParticles, 1, TextureParameters::WRAP_METHOD::CLAMP_TO_EDGE, TextureParameters::FILTER_METHOD::NEAREST, GL_FLOAT);
}

void FinalScene::initializeLightList() {

    m_deferredLights.clear();
    srand(time(0));

    for(int i = 0; i < settings.lightCount; ++i){
        CS123SceneLightData light = CS123SceneLightData();
        light.id = i;
        light.type = LightType::LIGHT_POINT;
        light.pos = getRandomPosition();
        if(settings.randomEnabled){
            light.color = glm::vec4(getRandomColor(), 0.f);
        } else {
            light.color = getLightColors();
        }
        m_deferredLights.push_back(light);
    }
}

/**
 * @brief makeRandomLights changes the color of the lights if the random toggle changes
 */
void FinalScene::colorLights() {
    for(int i = 0; i < settings.lightCount; ++i){
        CS123SceneLightData light = m_deferredLights[i];
        if(settings.randomEnabled){
            light.color = glm::vec4(getRandomColor(), 0.f);
        } else {
            light.color = getLightColors();
        }
    }
}

/**
 * @brief getRandomPosition returns a vec4 for the initial light position
 */
glm::vec4 FinalScene::getRandomPosition() {
    float x, y, z;
    x = (rand() % 12) + 1.f - 6.f;
    y = (rand() % 12) + 1.f - 6.f;
    z = (rand() % 12) + 1.f - 6.f;
    return glm::vec4(x, y, z, 1.0);
}

/**
 * @brief getRandomColor returns a vec3 with 3 random colors in range 0 to 1 for the lights to use.
 */
glm::vec3 FinalScene::getRandomColor() {
    int r255, g255, b255;
    float r01, g01, b01;

    r255 = (rand() % 255) + 1;
    g255 = (rand() % 255) + 1;
    b255 = (rand() % 255) + 1;

    r01 = (float)r255/255.f;
    g01 = (float)g255/255.f;
    b01 = (float)b255/255.f;

    return glm::vec3(r01, g01, b01);
}

/**
 * @brief loadGrassTexture Sets up the textures.
 */
void FinalScene::loadTextures() {
    QImage grassImage(":/textures/grass.jpg");
    QImage barkImage(":/textures/bark.jpg");
    m_grassTexture = std::make_unique<Texture2D>(static_cast<GLubyte *>(grassImage.bits()), grassImage.width(), grassImage.height());
    m_barkTexture = std::make_unique<Texture2D>(static_cast<GLubyte *>(barkImage.bits()), barkImage.width(), barkImage.height());
}

/**
 * @brief FinalScene::initFullscreenQuad Creates the fullscreen quad used by the window shader.
 */
void FinalScene::initFullscreenQuad() {
    std::vector<GLfloat> quadData = {
        -1.f,   1.f,    0.f,    0.f,    1.f,
        -1.f,   -1.f,   0.f,    0.f,    0.f,
        1.f,    1.f,    0.f,    1.f,    1.f,
        1.f,    -1.f,   0.f,    1.f,    0.f,
    };
    m_quad = std::make_unique<OpenGLShape>();
    m_quad->setVertexData(&quadData[0], int(quadData.size()), VBO::GEOMETRY_LAYOUT::LAYOUT_TRIANGLE_STRIP, 4);
    m_quad->setAttribute(ShaderAttrib::POSITION, 3, 0, VBOAttribMarker::DATA_TYPE::FLOAT, false);
    m_quad->setAttribute(ShaderAttrib::TEXCOORD0, 2, 3*sizeof(GLfloat), VBOAttribMarker::DATA_TYPE::FLOAT, false);
    m_quad->buildVAO();
}

void FinalScene::loadFirstPassShader() {
    std::string vertexSource = ResourceLoader::loadResourceFileToString(":/shaders/firstPass.vert");
    std::string fragmentSource = ResourceLoader::loadResourceFileToString(":/shaders/firstPass.frag");
    m_firstPassShader = std::make_unique<CS123Shader>(vertexSource, fragmentSource);
}

void FinalScene::loadPuddleShader() {
    std::string vertexSource = ResourceLoader::loadResourceFileToString(":/shaders/puddle.vert");
    std::string fragmentSource = ResourceLoader::loadResourceFileToString(":/shaders/puddle.frag");
    m_puddleShader = std::make_unique<CS123Shader>(vertexSource, fragmentSource);
}

void FinalScene::loadWindowShader() {
    std::string vertexSource = ResourceLoader::loadResourceFileToString(":/shaders/window.vert");
    std::string fragmentSource = ResourceLoader::loadResourceFileToString(":/shaders/window.frag");
    m_windowShader = std::make_unique<CS123Shader>(vertexSource, fragmentSource);
}

void FinalScene::loadPhongShader() {
    std::string vertexSource = ResourceLoader::loadResourceFileToString(":/shaders/default.vert");
    std::string fragmentSource = ResourceLoader::loadResourceFileToString(":/shaders/default.frag");
    m_phongShader = std::make_unique<CS123Shader>(vertexSource, fragmentSource);
}

void FinalScene::loadWireframeShader() {
    std::string vertexSource = ResourceLoader::loadResourceFileToString(":/shaders/wireframe.vert");
    std::string fragmentSource = ResourceLoader::loadResourceFileToString(":/shaders/wireframe.frag");
    m_wireframeShader = std::make_unique<Shader>(vertexSource, fragmentSource);
}

void FinalScene::loadNormalsShader() {
    std::string vertexSource = ResourceLoader::loadResourceFileToString(":/shaders/normals.vert");
    std::string geometrySource = ResourceLoader::loadResourceFileToString(":/shaders/normals.gsh");
    std::string fragmentSource = ResourceLoader::loadResourceFileToString(":/shaders/normals.frag");
    m_normalsShader = std::make_unique<Shader>(vertexSource, geometrySource, fragmentSource);
}

void FinalScene::loadNormalsArrowShader() {
    std::string vertexSource = ResourceLoader::loadResourceFileToString(":/shaders/normalsArrow.vert");
    std::string geometrySource = ResourceLoader::loadResourceFileToString(":/shaders/normalsArrow.gsh");
    std::string fragmentSource = ResourceLoader::loadResourceFileToString(":/shaders/normalsArrow.frag");
    m_normalsArrowShader = std::make_unique<Shader>(vertexSource, geometrySource, fragmentSource);
}

/**
 * @brief FinalScene::resizeFBO Creates a new FBO if the canvas' size has changed since the last render call.
 * @param context the canvas
 */
void FinalScene::resizeFBO(const SupportCanvas3D *context) {
    if (m_FBOsize.width() != context->width() || m_FBOsize.height() != context->height()) {
        m_FBOsize.setWidth(context->width());
        m_FBOsize.setHeight(context->height());
        m_FBO1 = std::make_unique<CS123::GL::FBO>(3, FBO::DEPTH_STENCIL_ATTACHMENT::DEPTH_ONLY, m_FBOsize.width(), m_FBOsize.height(), TextureParameters::WRAP_METHOD::CLAMP_TO_EDGE);
        m_FBO2 = std::make_unique<CS123::GL::FBO>(1, FBO::DEPTH_STENCIL_ATTACHMENT::NONE, m_FBOsize.width(), m_FBOsize.height(), TextureParameters::WRAP_METHOD::CLAMP_TO_EDGE);
    }

}

/**
 * @brief FinalScene::updateRainFBOs Updates the rain FBOs.
 * The rain FBOs hold particle information. They have two attachments:
 * 1) Attachment 0 holds particle positions.
 * 2) Attachment 1 holds particle velocities.
 * This function reads from one FBO and writes to the other.
 * It alternates which one it reads to and writes from.
 * The rain particles are reset when m_resetRainFBO is true.
 */
void FinalScene::updateRainFBOs() {
    std::unique_ptr<CS123::GL::FBO> &currentFBO = getCurrentRainFBO();
    std::unique_ptr<CS123::GL::FBO> &nextFBO = getNextRainFBO();

    // Configures the shader to write to the next FBO.
    nextFBO->bind();
    m_rainUpdateShader->bind();

    // Sets the shader's uniforms.
    m_rainUpdateShader->setUniform("reset", m_resetRainFBO);
    m_rainUpdateShader->setUniform("numParticles", adjustedRainParticleCount());
    if (!m_resetRainFBO) {
        m_rainUpdateShader->setTexture("previousPositions", currentFBO->getColorAttachment(0));
        m_rainUpdateShader->setTexture("previousVelocities", currentFBO->getColorAttachment(1));
    }

    // Draws and unbinds.
    m_quad->draw();
    nextFBO->unbind();
    m_rainUpdateShader->unbind();

    // Updates FBO state.
    m_useEvenRainFBO = !m_useEvenRainFBO;
    m_resetRainFBO = false;
}

/**
 * @brief FinalScene::getCurrentRainFBO Gets the current rain FBO.
 * @return the current rain FBO (even FBO for even frame, odd FBO for odd frame)
 */
std::unique_ptr<CS123::GL::FBO> &FinalScene::getCurrentRainFBO() {
    return m_useEvenRainFBO ? m_evenRainFBO : m_oddRainFBO;
}

/**
 * @brief FinalScene::getCurrentRainFBO Gets the next rain FBO.
 * @return the current rain FBO (odd FBO for even frame, even FBO for odd frame)
 */
std::unique_ptr<CS123::GL::FBO> &FinalScene::getNextRainFBO() {
    return m_useEvenRainFBO ? m_oddRainFBO : m_evenRainFBO;
}

/**
 * @brief FinalScene::drawRainFBO Draws the rain FBO's contents to the screen.
 */
void FinalScene::drawRainFBO(SupportCanvas3D *context) {
    std::unique_ptr<CS123::GL::FBO> &rainFBO = getCurrentRainFBO();

    m_rainDrawShader->bind();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    m_rainDrawShader->setTexture("positions", rainFBO->getColorAttachment(0));
    setSceneUniforms(context, m_rainDrawShader.get());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(m_particlesVAO);
    glDrawArrays(GL_TRIANGLES, 0, 3 * adjustedRainParticleCount());
    glBindVertexArray(0);
    glDisable(GL_BLEND);
    m_rainDrawShader->unbind();
}

void FinalScene::render(SupportCanvas3D *context) {
    // Makes sure the FBO has the correct dimensions for the context.
    resizeFBO(context);
    glm::vec4 eyeWorldSpace = getCameraEye(context);

    //get size
    float width = context->width();
    float height = context->height();

    // Updates the rain FBOs, which contain particle information.
    updateRainFBOs();

    // Constructs the iResolution uniform.
    const glm::vec3 iResolution = glm::vec3(float(context->width()), float(context->height()), float(0));

    // Gets delta time for the iTime uniform.
    QTime currentTime = QTime::currentTime();
    float secondsPassed = float(currentTime.msecsSinceStartOfDay() - m_startTime.msecsSinceStartOfDay()) * 0.001f;

    // Binds the FBO (the scene is now drawn to the FBO instead of the screen).
    m_FBO1->bind();
    setClearColor();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // Renders the tree using the Phong shader.
    m_phongShader->bind();
    setSceneUniforms(context, m_phongShader.get());
    setLights(m_phongShader.get());

    m_phongShader->setUniform("useTexture", 1);
    m_phongShader->setUniform("repeatUV", glm::vec2(1.f, 1.f));
    m_phongShader->setTexture("tex", *m_barkTexture.get());

    renderPhongGeometry();

    m_phongShader->unbind();

    renderLeaves(context);

    // Renders the ground using the puddle shader.
    m_puddleShader->bind();
    m_puddleShader->setUniform("enabled", settings.rainEnabled);
    m_puddleShader->setUniform("iTime", secondsPassed);
    m_puddleShader->setUniform("repeatUV", glm::vec2(1.f, 1.f));
    setSceneUniforms(context, m_puddleShader.get());
    setLights(m_puddleShader.get());
    m_puddleShader->setTexture("tex", *m_grassTexture.get());
    renderGround();
    m_puddleShader->unbind();

    // Renders the rain.
    drawRainFBO(context);

    // Unbinds the FBO (the scene is now drawn to the screen).
    m_FBO1->unbind();

    // Binds the FBO's color attachments to make textures.
     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    m_FBO2->bind();


    m_firstPassShader->bind();
    setSceneUniforms(context, m_firstPassShader.get());
    m_firstPassShader->setUniform("screenWidth", width);
    m_firstPassShader->setUniform("screenHeight", height);
        m_firstPassShader->setUniform("eyePositionWorldspace", eyeWorldSpace);
    m_firstPassShader->setTexture("colorTexture", m_FBO1->getColorAttachment(0));
    m_firstPassShader->setTexture("positionTexture", m_FBO1->getColorAttachment(1));
    m_firstPassShader->setTexture("normalTexture", m_FBO1->getColorAttachment(2));
    renderLightVolumes();
    m_firstPassShader->unbind();

    m_FBO2->unbind();

   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Lets the window shader draw to the screen.
    m_windowShader->bind();
    m_windowShader->setTexture("tex", m_FBO2->getColorAttachment(0));
    m_windowShader->setUniform("iTime", secondsPassed);
    m_windowShader->setUniform("iResolution", iResolution);
    m_windowShader->setUniform("disableBlur", false);
    m_windowShader->setUniform("disableBlurColor", false);
    m_windowShader->setUniform("disableShader", !(settings.raindropsOnWindowEnabled && settings.rainEnabled));
    glViewport(0, 0, context->width(), context->height());
    m_quad->draw();
    m_windowShader->unbind();

    // Keep this here so that OpenGL errors don't pile up.
    checkError();
}

/**
 * @brief FinalScene::setSceneUniforms Technically, since setUniform wraps an OpenGL call, using a specific shader isn't necessary.
 * @param context the canvas
 * @param shader the shader (could be any shader, but it's nice to use the correct one)
 */
void FinalScene::setSceneUniforms(SupportCanvas3D *context, Shader *shader) {
    Camera *camera = context->getCamera();
    shader->setUniform("p" , camera->getProjectionMatrix());
    shader->setUniform("v", camera->getViewMatrix());
}

void FinalScene::setMatrixUniforms(Shader *shader, SupportCanvas3D *context) {
    shader->setUniform("p", context->getCamera()->getProjectionMatrix());
    shader->setUniform("v", context->getCamera()->getViewMatrix());
}

glm::vec4 FinalScene::getCameraEye(SupportCanvas3D *context){
    glm::vec4 eyeCameraSpace = glm::vec4(0,0,0,1);
    Camera *camera = context->getCamera();
    glm::mat4 view = camera->getViewMatrix();
    glm::vec4 eyeWorldSpace = glm::inverse(view) * eyeCameraSpace;
    return eyeWorldSpace;
}

/*
 * Goes through the list of lights and sends each light's data to the shader.
 */
void FinalScene::setLights(CS123Shader *shader) {
    CS123SceneLightData new_light = CS123SceneLightData();
    new_light.id = 0;
    new_light.type = LightType::LIGHT_DIRECTIONAL;
    new_light.color = glm::vec4(1.f, 1.f, 1.f, 1.f);
    new_light.dir = glm::normalize(glm::vec4(1.f, -0.8f, -1.2f, 0.f));
    shader->setLight(new_light);
}

void FinalScene::renderLightVolumes() {
    CS123SceneLightData light;

    // clear buffer and enable blending
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);
    glBlendEquation(GL_FUNC_ADD);

    // loop through lights to render volumes
    for(int i = 0; i < m_deferredLights.size(); i++){
        light = m_deferredLights[i];

        if(!settings.randomEnabled){
            light.color = getLightColors();
        }

        glm::mat4x4 transMatrix = glm::translate(glm::vec3(light.pos.xyz()));
        float scaleval = settings.lightRadius;
        glm::vec3 scaleVec = glm::vec3(scaleval, scaleval, scaleval);

        glm::mat4x4 transformation = glm::scale(transMatrix, scaleVec);
        m_firstPassShader->setUniform("m", transformation);
        m_firstPassShader->setUniform("lightIntensity", light.color.rgb());
        m_firstPassShader->setUniform("lightPos", light.pos.xyz());
        m_firstPassShader->setUniform("lightBrightness", settings.lightBrightness);
        m_sphere->renderShape();
    }
    glDisable(GL_BLEND);
}

/*
 * Goes through the lists of primitives and transformations side by side and sends the
 * transformation to the phongShader. The primitive is then drawn based on what shape it is.
 */
void FinalScene::renderPhongGeometry() {
    renderTree();
}

glm::vec4 FinalScene::getLightColors() {
    float red = (float)settings.lightColorRed/255.f;
    float green = (float)settings.lightColorGreen/255.f;
    float blue = (float)settings.lightColorBlue/255.f;
    return glm::vec4(red, green, blue, 0.0);
}

void FinalScene::renderGround() {
    CS123SceneMaterial material;

    // Sets the material's color.
    CS123SceneColor color;
    color.r = 0.05f;
    color.g = 0.3f;
    color.b = 0.1f;
    color.a = 0.5f;
    material.cAmbient = color;
    material.cDiffuse = color;
    material.cSpecular = color;

    // Sets the material's texture.
    material.blend = 0.5f;
    CS123SceneFileMap texture;
    texture.repeatU = 1.f;
    texture.repeatV = 1.f;
    texture.isUsed = true;
    texture.filename = ResourceLoader::loadResourceFileToString(":/textures/grass.jpg");
    material.textureMap = texture;

    glm::mat4x4 m = glm::translate(glm::scale(glm::mat4(1.f), glm::vec3(12.f, 12.f, 12.f)), glm::vec3(0.f, 0.0f, 0.f));
    m_puddleShader->setUniform("m", m);
    m_puddleShader->setUniform("ambient_color", glm::vec3(material.cAmbient.r, material.cAmbient.g, material.cAmbient.b));
    m_puddleShader->setUniform("diffuse_color", glm::vec3(material.cDiffuse.r, material.cDiffuse.g, material.cDiffuse.b));
    m_reverseCube->renderShape();
}

/**
 * @brief FinalScene::adjustedRainParticleCount Adjusts the raininess based on whether we're in Portland, OR.
 * @return the slider value if rain is enabled (i.e. in Portland), otherwise 1
 */
int FinalScene::adjustedRainParticleCount() {
    return settings.rainEnabled ? settings.rainParticleCount : 1;
}

void FinalScene::renderTree() {
    CS123ScenePrimitive primitive = m_tree->m_branch;

    primitive.material.blend = 0.5f;
    CS123SceneFileMap texture;
    texture.repeatU = 1.f;
    texture.repeatV = 1.f;
    texture.isUsed = true;
    texture.filename = ResourceLoader::loadResourceFileToString(":/textures/bark.jpg");
    primitive.material.textureMap = texture;

    int num_cylinders = m_tree->m_transformations.size();
    for (int i = 0; i < num_cylinders; i++) {
        glm::mat4x4 transformation = m_tree->m_transformations[i];

        m_phongShader->applyMaterial(primitive.material);
        // send the cumulative transformation to the phongShader's model matrix
        m_phongShader->setUniform("m", transformation);
        m_phongShader->setUniform("ambient_color", glm::vec3(primitive.material.cAmbient.r, primitive.material.cAmbient.g, primitive.material.cAmbient.b));
        m_phongShader->setUniform("diffuse_color", glm::vec3(primitive.material.cDiffuse.r, primitive.material.cDiffuse.g, primitive.material.cDiffuse.b));

        m_tapered->renderShape();

    }
}

void FinalScene::renderLeaves(SupportCanvas3D *context) {
    // Defines the leaves' material.
    CS123SceneMaterial material;

    // Sets the material's color.
    CS123SceneColor color;
    color.r = 0.6f;
    color.g = 0.7f;
    color.b = 0.1f;
    color.a = 1.0f;
    material.cAmbient = color;
    material.cDiffuse = color;
    material.cSpecular = color;

    // Sets up the shader.
    glDisable(GL_CULL_FACE);
    m_leafShader->bind();

    // Sets the uniforms.
    setSceneUniforms(context, m_leafShader.get());
    setLights(m_leafShader.get());
    m_leafShader->setUniform("useTexture", 0);
    m_leafShader->setUniform("repeatUV", glm::vec2(1.f, 1.f));
    m_leafShader->setTexture("tex", *m_barkTexture.get());
    m_leafShader->setUniform("ambient_color", glm::vec3(material.cAmbient.r, material.cAmbient.g, material.cAmbient.b));
    m_leafShader->setUniform("diffuse_color", glm::vec3(material.cDiffuse.r, material.cDiffuse.g, material.cDiffuse.b));
    m_leafShader->setUniform("leafWidth", settings.leafWidth);

    // Renders the leaves.
    // TODO: Actually render the tree's leaves.
    int num_leaves = m_tree->m_leaf_transforms.size();
    for (int i = 0; i < num_leaves; i++) {
        glm::mat4 m = m_tree->m_leaf_transforms[i];
        // glm::mat4 m = glm::mat4(1.f);
        m_leafShader->setUniform("m", m);
        m_leaf->renderShape();
    }

    // Cleans up the shader.
    m_leafShader->unbind();
    glEnable(GL_CULL_FACE);
}


/*
 * Is called with the settings change, mainly to update the scene based on a change in the
 * shape parameters. All the shapes reload their OpenGLShape renderers based on their new coordinates.
 */
void FinalScene::settingsChanged(int options) {
    if (options & SupportCanvas3D::UPDATE_TREE) {
        m_tree->createTree();
    }
    if (options & SupportCanvas3D::UPDATE_LIGHTCOUNT) {
        initializeLightList();
    }
    if (options & SupportCanvas3D::UPDATE_LIGHTRAND) {
        colorLights();
    }
    resizeRainFBOs(adjustedRainParticleCount());
    m_resetRainFBO = true;
}

