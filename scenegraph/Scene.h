#ifndef SCENE_H
#define SCENE_H

#include "CS123SceneData.h"
#include <QList>
#include <memory>
#include <QImage>

class Camera;
class CS123ISceneParser;


/**
 * @class Scene
 *
 * @brief This is the base class for all scenes. Modify this class if you want to provide
 * common functionality to all your scenes.
 */
class Scene {
public:
    Scene();
    Scene(Scene &scene);
    virtual ~Scene();

    virtual void settingsChanged(int options = 0) {}

    // Gets all the data from the scenegraph by parsing the scenefile
    static void parse(Scene *sceneToFill, CS123ISceneParser *parser);

    QList<CS123SceneLightData> m_lights;
    CS123SceneGlobalData m_global;
    QList<CS123ScenePrimitive> m_primitives;
    QList<glm::mat4x4> m_transformations;
    QList<QImage> m_textures;
    QList<int> m_texture_indices;

protected:

    // Adds a primitive to the scene.
    virtual void addPrimitive(const CS123ScenePrimitive &scenePrimitive, const glm::mat4x4 &matrix);

    // Adds a light to the scene.
    virtual void addLight(const CS123SceneLightData &sceneLight);

    // Sets the global data for the scene.
    virtual void setGlobal(const CS123SceneGlobalData &global);

    // Traverses the scenegraph in a preorder fashion
    void preorder(CS123SceneNode *node, glm::mat4x4 matrix);
};

#endif // SCENE_H
