#include "Scene.h"
#include "Camera.h"
#include "CS123ISceneParser.h"

#include "glm/gtx/transform.hpp"


Scene::Scene()
{
}

Scene::Scene(Scene &scene)

{
    // We need to set the global constants to one when we duplicate a scene,
    // otherwise the global constants will be double counted (squared)
    CS123SceneGlobalData global = { 1, 1, 1, 1};
    setGlobal(global);

    // Make sure to copy over the lights and the scenegraph from the old scene,
    // as well as any other member variables your new scene will need.
    m_lights = scene.m_lights;
    m_global = scene.m_global;
    m_primitives = scene.m_primitives;
    m_transformations = scene.m_transformations;
    m_textures = scene.m_textures;
    m_texture_indices = scene.m_texture_indices;
}

Scene::~Scene()
{
    // Do not delete m_camera, it is owned by SupportCanvas3D
}

/*
 * Parses the scenefile by getting the light data, global data, and traversing the
 * scenegraph tree to get primitives and transformations.
 *
 * @param sceneToFill {*Scene} Pointer to the scene we want to fill.
 * @param parser {*CS123ISceneParser} Pointer to the parser which can access the lights,
 * global data, and root of the scenegraph.
 */
void Scene::parse(Scene *sceneToFill, CS123ISceneParser *parser) {
    // load scene into sceneToFill using setGlobal(), addLight(), addPrimitive()
    // lights
    int num_lights = parser->getNumLights();
    for (int i = 0; i < num_lights; i++) {
        CS123SceneLightData light_data;
        parser->getLightData(i, light_data);
        sceneToFill->addLight(light_data);
    }
    // global data
    CS123SceneGlobalData global_data;
    parser->getGlobalData(global_data);
    sceneToFill->setGlobal(global_data);
    // primitives with transformations
    CS123SceneNode *root = parser->getRootNode();
    glm::mat4x4 identity;
    sceneToFill->preorder(root, identity);
}

/*
 * Recursively traverses the scenegraph starting from the root node in a preorder fashion
 * and fills the member variable QLists of primitives and cumulative transformations.
 *
 * @param node {*CS123SceneNode} Pointer to the current node we are on during the traversal.
 * @param matrix {glm::mat4x4} Transformation matrix of the parent of the current node (root's
 * is just the identity matrix).
 */
void Scene::preorder(CS123SceneNode *node, glm::mat4x4 matrix) {
    // gets the current node's transformations
    std::vector<CS123SceneTransformation*> node_transformations = node->transformations;
    glm::mat4x4 composite = matrix;
    // accumulates the transformation matrix by multiplying by the correct matrix.
    for (size_t i = 0; i < node_transformations.size(); i++) {
        glm::mat4x4 current_transform;
        glm::vec3 vector;
        float angle;
        switch(node_transformations[i]->type) {
            case TRANSFORMATION_SCALE:
                vector = node_transformations[i]->scale;
                current_transform = glm::scale(vector);
                break;
            case TRANSFORMATION_ROTATE:
                vector = node_transformations[i]->rotate;
                angle = node_transformations[i]->angle;
                current_transform = glm::rotate(angle, vector);
                break;
            case TRANSFORMATION_TRANSLATE:
                vector = node_transformations[i]->translate;
                current_transform = glm::translate(vector);
                break;
            case TRANSFORMATION_MATRIX:
                current_transform = node_transformations[i]->matrix;
                break;
        }
        composite *= current_transform;
    }
    // adds the primitives and their cumulative transformation to the QLists
    std::vector<CS123ScenePrimitive*> node_primitives = node->primitives;
    for (size_t i = 0; i < node_primitives.size(); i++) {
        CS123ScenePrimitive *scene_primitive = node_primitives[i];
        addPrimitive(*scene_primitive, composite);
    }
    // preorder traverses all the node's children
    std::vector<CS123SceneNode*> children = node->children;
    for (size_t i = 0; i < children.size(); i++) {
        preorder(children[i], composite);
    }
}

/*
 * Adds a primitive and its cumulative transformation to the same index in their
 * respective QLists.
 *
 * @param scenePrimitive {&CS123ScenePrimitive} Address of the primitive we want to add.
 * @param matrix {&glm::mat4x4} Address of the cumulative transformation matrix we want to add.
 */
void Scene::addPrimitive(const CS123ScenePrimitive &scenePrimitive, const glm::mat4x4 &matrix) {
    m_primitives.push_back(scenePrimitive);
    m_transformations.push_back(matrix);
    CS123SceneFileMap texture = scenePrimitive.material.textureMap;
    if (texture.isUsed) {
        QImage image(QString::fromStdString(texture.filename));
        m_texture_indices.push_back(m_textures.size());
        m_textures.push_back(image);
    } else {
        m_texture_indices.push_back(-1);
    }
}

/*
 * Adds a light's data to the light data list.
 * @param sceneLight {&CS123SceneLightData} Address of the data of one light.
 */
void Scene::addLight(const CS123SceneLightData &sceneLight) {
    m_lights.push_back(sceneLight);
}

/*
 * Sets m_global to the global data
 * @param global {&CS123SceneGlobalData} Address of the global data.
 */
void Scene::setGlobal(const CS123SceneGlobalData &global) {
    m_global = global;
}

