#include "Entity.hpp"

#include "../opengl.hpp"

Entity::Entity(Transform t) : GameObject(t) {}
Entity::Entity() : GameObject() {}

Entity::~Entity() {}

void Entity::Render(ShaderProgram *sp, const glm::mat4 *view, const glm::mat4 *projection, Transform *cameraTransform, const FogRenderSettings *fogRenderSettings)
{
    if (!enableRender) return;

    sp->UseThisProgram();

    sp->SetUniformMatrix4x4("projection", *projection);
    sp->SetUniformMatrix4x4("view", *view);

    sp->SetUniformInteger("texture", 0);
    glActiveTexture(GL_TEXTURE0);

    sp->SetUniformVector3("cameraPosition", cameraTransform->GetPosition());
    sp->SetUniformVector3("cameraRotation", glm::eulerAngles(cameraTransform->GetRotation()));

    sp->SetUniformVector3("cameraFront", cameraTransform->GetFront());
    sp->SetUniformVector3("cameraUp", cameraTransform->GetUp());
    sp->SetUniformVector3("cameraRight", cameraTransform->GetRight());

    sp->SetUniformInteger("fogEnabled", fogRenderSettings->fogEnabled ? GL_TRUE : GL_FALSE);
    sp->SetUniformFloat("fogStartDistance", fogRenderSettings->fogStartDistance);
    sp->SetUniformFloat("fogEndDistance", fogRenderSettings->fogEndDistance);
    sp->SetUniformVector3("fogColor", fogRenderSettings->fogColor);

    for (Surface surface : surfaces)
    {
        if (!surface.enableRender) continue;

        Texture *texture = surface.GetTexture();
        Mesh *mesh = surface.GetMesh();
        if (!(mesh && mesh->HasBuffers() && surface.culling != BothFaces)) continue;

        if (surface.culling == NoCulling) glDisable(GL_CULL_FACE);
        else
        {
            glEnable(GL_CULL_FACE);

            switch (surface.culling)
            {
                case BackFace:
                    glCullFace(GL_BACK);
                    break;

                case FrontFace:
                    glCullFace(GL_FRONT);
                    break;
            }
        }

        if (texture && texture->HasTexture())
        {
            sp->SetUniformInteger("hasTexture", GL_TRUE);
            texture->BindTexture();
        }
        else sp->SetUniformInteger("hasTexture", GL_FALSE);

        sp->SetUniformMatrix4x4("model", GetGlobalTransform().GetTransformationMatrix() * surface.transform.GetTransformationMatrix());
        sp->SetUniformVector4("color", color * surface.color);

        mesh->RenderMesh();
    }
}