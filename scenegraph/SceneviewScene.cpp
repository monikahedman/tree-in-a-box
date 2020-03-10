#include "SceneviewScene.h"
#include "GL/glew.h"
#include <QGLWidget>
#include "Camera.h"

#include "Settings.h"
#include "SupportCanvas3D.h"
#include "ResourceLoader.h"
#include "gl/shaders/CS123Shader.h"

#include "CS123SceneData.h"

using namespace CS123::GL;


SceneviewScene::SceneviewScene():
    m_cube(nullptr),
    m_cone(nullptr),
    m_cylinder(nullptr),
    m_sphere(nullptr)
{
    loadPhongShader();
    loadWireframeShader();
    loadNormalsShader();
    loadNormalsArrowShader();
    settingsChanged();
}

SceneviewScene::~SceneviewScene()
{
}

void SceneviewScene::loadPhongShader() {
    std::string vertexSource = ResourceLoader::loadResourceFileToString(":/shaders/default.vert");
    std::string fragmentSource = ResourceLoader::loadResourceFileToString(":/shaders/default.frag");
    m_phongShader = std::make_unique<CS123Shader>(vertexSource, fragmentSource);
}

void SceneviewScene::loadWireframeShader() {
    std::string vertexSource = ResourceLoader::loadResourceFileToString(":/shaders/wireframe.vert");
    std::string fragmentSource = ResourceLoader::loadResourceFileToString(":/shaders/wireframe.frag");
    m_wireframeShader = std::make_unique<Shader>(vertexSource, fragmentSource);
}

void SceneviewScene::loadNormalsShader() {
    std::string vertexSource = ResourceLoader::loadResourceFileToString(":/shaders/normals.vert");
    std::string geometrySource = ResourceLoader::loadResourceFileToString(":/shaders/normals.gsh");
    std::string fragmentSource = ResourceLoader::loadResourceFileToString(":/shaders/normals.frag");
    m_normalsShader = std::make_unique<Shader>(vertexSource, geometrySource, fragmentSource);
}

void SceneviewScene::loadNormalsArrowShader() {
    std::string vertexSource = ResourceLoader::loadResourceFileToString(":/shaders/normalsArrow.vert");
    std::string geometrySource = ResourceLoader::loadResourceFileToString(":/shaders/normalsArrow.gsh");
    std::string fragmentSource = ResourceLoader::loadResourceFileToString(":/shaders/normalsArrow.frag");
    m_normalsArrowShader = std::make_unique<Shader>(vertexSource, geometrySource, fragmentSource);
}

void SceneviewScene::render(SupportCanvas3D *context) {
    setClearColor();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_phongShader->bind();
    setSceneUniforms(context);
    setLights();
    renderGeometry();
    glBindTexture(GL_TEXTURE_2D, 0);
    m_phongShader->unbind();

}

void SceneviewScene::setSceneUniforms(SupportCanvas3D *context) {
    Camera *camera = context->getCamera();
    m_phongShader->setUniform("useLighting", settings.useLighting);
    m_phongShader->setUniform("useArrowOffsets", false);
    m_phongShader->setUniform("p" , camera->getProjectionMatrix());
    m_phongShader->setUniform("v", camera->getViewMatrix());
}

void SceneviewScene::setMatrixUniforms(Shader *shader, SupportCanvas3D *context) {
    shader->setUniform("p", context->getCamera()->getProjectionMatrix());
    shader->setUniform("v", context->getCamera()->getViewMatrix());
}

/*
 * Goes through the list of lights and sends each light's data to the phongShader.
 */
void SceneviewScene::setLights() {
    int num_lights = m_lights.size();
    for (int i = 0; i < num_lights; i++) {
        m_phongShader->setLight(m_lights[i]);
    }
}

/*
 * Goes through the lists of primitives and transformations side by side and sends the
 * transformation to the phongShader. The primitive is then drawn based on what shape it is.
 */
void SceneviewScene::renderGeometry() {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    int num_primitives = m_primitives.size();
    for (int i = 0; i < num_primitives; i++) {
        // gets the current primitive and its transformation
        CS123ScenePrimitive primitive = m_primitives[i];
        glm::mat4x4 transformation = m_transformations[i];

        // applies the material based on the global data and material data
        primitive.material.cDiffuse *= m_global.kd;
        primitive.material.cAmbient *= m_global.ka;
        primitive.material.cSpecular *= m_global.ks;
        primitive.material.cTransparent *= m_global.kt;
        m_phongShader->applyMaterial(primitive.material);
        // send the cumulative transformation to the phongShader's model matrix
        m_phongShader->setUniform("m", transformation);

        // draws the shape based on the primitive's type
        switch (primitive.type) {
            case PrimitiveType::PRIMITIVE_CUBE:
                m_cube->renderShape();
                break;
            case PrimitiveType::PRIMITIVE_CONE:
                m_cone->renderShape();
                break;
            case PrimitiveType::PRIMITIVE_CYLINDER:
                m_cylinder->renderShape();
                break;
            case PrimitiveType::PRIMITIVE_SPHERE:
                m_sphere->renderShape();
                break;
            default:
                m_cube->renderShape();
                break;
        }
    }
}

/*
 * Is called with the settings change, mainly to update the scene based on a change in the
 * shape parameters. All the shapes reload their OpenGLShape renderers based on their new coordinates.
 */
void SceneviewScene::settingsChanged(int options) {
    m_cube = std::make_unique<Cube>(std::max(1, settings.shapeParameter1), std::max(1, settings.shapeParameter1));
    m_cone = std::make_unique<Cone>(std::max(1, settings.shapeParameter1), std::max(1, settings.shapeParameter2));
    m_cylinder = std::make_unique<Cylinder>(std::max(1, settings.shapeParameter1), std::max(1, settings.shapeParameter2));
    m_sphere = std::make_unique<Sphere>(std::max(1, settings.shapeParameter1), std::max(1, settings.shapeParameter2));

    m_cube->loadRenderer();
    m_cone->loadRenderer();
    m_cylinder->loadRenderer();
    m_sphere->loadRenderer();
}

