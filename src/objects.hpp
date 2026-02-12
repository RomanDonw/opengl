#include "utils.hpp"

class ShaderProgram
{
  private:
    GLuint shprog = 0;

  public:
    ShaderProgram(std::string vshader, std::string fshader)
    {
        const char *vsrc = vshader.c_str();
        GLuint vs = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vs, 1, &vsrc, NULL);
        glCompileShader(vs);

        const char *fsrc = fshader.c_str();
        GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fs, 1, &fsrc, NULL);
        glCompileShader(fs);

        shprog = glCreateProgram();
        glAttachShader(shprog, vs);
        glAttachShader(shprog, fs);
        glLinkProgram(shprog);

        glDeleteShader(vs);
        glDeleteShader(fs);
    }

    ~ShaderProgram()
    {
        glDeleteProgram(shprog);
    }

    inline GLuint GetShaderProgram() { return shprog; }
};

class Transform
{
  private:
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(1.0f);

    bool lock_cache = false;
    inline void updatecache() { if (!lock_cache) UpdateCache(); }

    // cache:
    glm::quat rot_quat;
    glm::mat4 rot_mat;

    glm::vec3 front, up, right;

    void callback_onchange() { for (std::function<void (Transform *)> callback : onTransformChangeCallbacks) callback(this); }

  public:
    //std::vector<void (*)(Transform *)> onTransformChangeCallbacks = std::vector<void (*)(Transform *)>();
    std::vector<std::function<void (Transform *)>> onTransformChangeCallbacks = std::vector<std::function<void (Transform *)>>();

    Transform(glm::vec3 pos, glm::vec3 rot, glm::vec3 scl)
    {
        position = pos;
        rotation = rot;
        scale = scl;
        UpdateCache();
    }

    Transform(glm::vec3 pos, glm::vec3 rot)
    {
        position = pos;
        rotation = Utils::wrapangles(rot);
        UpdateCache();
    }

    Transform(glm::vec3 pos)
    {
        position = pos;
        UpdateCache();
    }

    Transform() { UpdateCache(); }

    inline Transform Copy() { return *this; }

    inline bool IsCacheLocked() { return lock_cache; }
    inline void SetLockCache(bool lock) { lock_cache = lock; }
    inline void LockCache() { lock_cache = true; }
    inline void UnlockCache() { lock_cache = false; }

    void UpdateCache()
    {
        rotation = Utils::wrapangles(rotation);
        rot_quat = glm::quat(rotation);
        rot_mat = glm::toMat4(rot_quat);

        front = rot_quat * glm::vec3(0.0f, 0.0f, -1.0f);
        up = rot_quat * glm::vec3(0.0f, 1.0f, 0.0f);
        right = rot_quat * glm::vec3(1.0f, 0.0f, 0.0f);

        callback_onchange();
    }

    inline glm::vec3 GetPosition() { return position; }
    
    inline glm::vec3 GetRotation() { return rotation; }
    inline glm::quat GetRotationQuaternion() { return rot_quat; }
    inline glm::mat4 GetRotationMatrix() { return rot_mat; }

    inline glm::vec3 GetScale() { return scale; }

    inline glm::vec3 GetFront() { return front; }
    inline glm::vec3 GetUp() { return up; }
    inline glm::vec3 GetRight() { return right; }

    inline glm::mat4 GetTransformationMatrix() 
    { return glm::scale(glm::translate(glm::mat4(1), position) * rot_mat, scale); }

    inline void SetPosition(glm::vec3 v) { position = v; callback_onchange(); }
    inline void SetRotation(glm::vec3 v) { rotation = v; updatecache(); }
    inline void SetScale(glm::vec3 v) { scale = v; callback_onchange(); }

    inline void Translate(glm::vec3 v) { position += v; callback_onchange(); }
    inline void Rotate(glm::vec3 v) { rotation += v; updatecache(); }
    inline void Scale(glm::vec3 v) { scale += v; callback_onchange(); }
};

enum
{
    UNKNOWN = 0,
    ENTITY = 1,
    CAMERA = 2,
    AUDIOSOURCE = 3
} typedef GameObjectType;

class GameObject
{
  private:
    GameObject *parent = nullptr;
    std::vector<GameObject *> children = std::vector<GameObject *>();

  public:
    const GameObjectType type = UNKNOWN;
    Transform transform = Transform();

    GameObject(Transform t) { transform = t; }
    GameObject() {}

    ~GameObject() { SetParent(nullptr); }

    inline GameObject Copy() { return *this; }

    void SetParent(GameObject *new_parent)
    {
        if (parent) parent->children.erase(std::remove(parent->children.begin(), parent->children.end(), this), parent->children.end());
        if (new_parent) new_parent->children.push_back(this);
        parent = new_parent;
    }

    inline glm::mat4 GetParentGlobalTransformationMatrix()
    { return parent ? parent->GetParentGlobalTransformationMatrix() * parent->transform.GetTransformationMatrix() : glm::mat4(1.0f); }
};

struct
{
    float x, y, z;
    float u, v;
} typedef UCMESHVertexInfo;

struct
{
    unsigned int v0, v1, v2;
} typedef UCMESHTriangleInfo;

struct
{
    unsigned int v0, v1, v2, v3;
} typedef UCMESHQuadInfo;

class Mesh
{
  private:
    std::vector<glm::vec3> vertices;
    std::vector<unsigned int> indices;
    std::vector<glm::vec2> uvs;

    bool hasbuffers = false;
    GLuint VAO, VBO_VERTEX, VBO_UV, EBO;

    bool lockbuffers = false;
    inline void updatebuffers() { if (!lockbuffers) RegenerateBuffers(); }

  public:
    Mesh(std::vector<glm::vec3> _vertices, std::vector<unsigned int> _indices, std::vector<glm::vec2> _uvs)
    {
        vertices = _vertices;
        indices = _indices;
        uvs = _uvs;

        GenerateBuffers();
    }

    Mesh() {}
    ~Mesh() { DeleteBuffers(); }

    inline Mesh Copy() { return *this; }

    inline void ClearVertices() { vertices.clear(); DeleteBuffers(); }
    inline void ClearIndices() { indices.clear(); DeleteBuffers(); }
    inline void ClearUVs() { uvs.clear(); DeleteBuffers(); }
    inline void ClearMesh() { ClearVertices(); ClearIndices(); ClearUVs(); }

    inline void AddVertexWithUV(glm::vec3 vertex, glm::vec2 uv) { vertices.push_back(vertex); uvs.push_back(uv); }
    inline void AddVertexWithUV(float x, float y, float z, float u, float v) { AddVertexWithUV(glm::vec3(x, y, z), glm::vec2(u, v)); }

    void AddTriangle(unsigned int v0, unsigned int v1, unsigned int v2)
    {
        indices.push_back(v0);
        indices.push_back(v1);
        indices.push_back(v2);

        updatebuffers();
    }

    /*
    Vertices sequence:

        v0 ... v1
        .       .
        .       .
        v3 ... v2

    */
    inline void AddQuad(unsigned int v0, unsigned int v1, unsigned int v2, unsigned int v3) { AddTriangle(v3, v0, v1); AddTriangle(v1, v2, v3); }

    inline std::vector<glm::vec3> GetVertices() { return vertices; }
    inline std::vector<unsigned int> GetIndices() { return indices; }
    inline size_t GetIndicesCount() { return indices.size(); }
    inline std::vector<glm::vec2> GetUVs() { return uvs; }

    inline bool IsBuffersLocked() { return lockbuffers; }
    inline void SetBuffersLock(bool state) { lockbuffers = state; }
    inline void LockBuffers() { lockbuffers = true; }
    inline void UnlockBuffers() { lockbuffers = false; }

    inline bool HasBuffers() { return hasbuffers; }
    inline GLuint GetVAO() { return VAO; }

    bool GenerateBuffers()
    {
        if (hasbuffers /*|| vertices.size() == 0 || uvs.size() == 0 || indices.size() == 0*/) return false;

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO_VERTEX);
        glGenBuffers(1, &VBO_UV);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO_VERTEX);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, VBO_UV);
        glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), uvs.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        hasbuffers = true;
        return true;
    }

    bool DeleteBuffers()
    {
        if (!hasbuffers) return false;

        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO_VERTEX);
        glDeleteBuffers(1, &VBO_UV);
        glDeleteBuffers(1, &EBO);

        hasbuffers = false;
        return true;
    }

    inline void RegenerateBuffers() { GenerateBuffers(); DeleteBuffers(); }

    void ApplyTransformation(glm::mat4 mat)
    {
        for (size_t i = 0; i < vertices.size(); i++)
        {
            glm::vec4 v = mat * glm::vec4(vertices[i].x, vertices[i].y, vertices[i].z, 0);
            vertices[i] = glm::vec3(v.x, v.y, v.z);
        }

        updatebuffers();
    }
    inline void ApplyTransformation(Transform t) { ApplyTransformation(t.GetTransformationMatrix()); }

    bool LoadFromUCMESHFile(std::string filename)
    {
        if (!std::filesystem::is_regular_file(filename)) return false;

        FILE *f = fopen(filename.c_str(), "rb");
        if (!f) return false;

        char sig[6];
        fread(&sig, sizeof(char), 6, f);
        if (feof(f) || strncmp(sig, "UCMESH", 6)) goto fileerrorquit;

        uint16_t version;
        fread(&version, sizeof(version), 1, f);
        if (feof(f) || version != 0) goto fileerrorquit;

        uint32_t vertices_count;
        fread(&vertices_count, sizeof(vertices_count), 1, f);
        if (feof(f)) goto fileerrorquit;

        uint32_t primitives_count;
        fread(&primitives_count, sizeof(primitives_count), 1, f);
        if (feof(f)) goto fileerrorquit;

        ClearMesh();
        LockBuffers();

        UCMESHVertexInfo v;
        for (uint32_t i = 0; i < vertices_count; i++)
        {
            fread(&v, sizeof(v), 1, f);
            if (feof(f)) goto readmesherrorquit;

            AddVertexWithUV(v.x, v.y, v.z, v.u, v.v);
        }

        uint8_t prim_type;
        for (uint32_t i = 0; i < primitives_count; i++)
        {
            fread(&prim_type, sizeof(prim_type), 1, f);
            if (feof(f)) goto readmesherrorquit;

            switch (prim_type)
            {
                case 0: // triangle.
                    UCMESHTriangleInfo tri;
                    fread(&tri, sizeof(tri), 1, f);
                    if (feof(f)) goto readmesherrorquit;

                    if (tri.v0 >= vertices_count || tri.v1 >= vertices_count || tri.v2 >= vertices_count) continue;

                    AddTriangle(tri.v0, tri.v1, tri.v2);
                    break;

                case 1: // quad.
                    UCMESHQuadInfo quad;
                    fread(&quad, sizeof(quad), 1, f);
                    if (feof(f)) goto readmesherrorquit;

                    if (quad.v0 >= vertices_count || quad.v1 >= vertices_count || quad.v2 >= vertices_count || quad.v3 >= vertices_count) continue;

                    AddQuad(quad.v0, quad.v1, quad.v2, quad.v3);
                    break;

                default:
                    goto readmesherrorquit;
            }
        }

        UnlockBuffers();
        GenerateBuffers();

        return true;

        readmesherrorquit:
            ClearMesh();
            UnlockBuffers();
        fileerrorquit:
            fclose(f);
        return false;
    }
};

class Texture
{
  private:
    bool hasTexture = false;
    GLuint texture;

  public:
    Texture() {}
    ~Texture() { DeleteTexture(); }

    inline Texture Copy() { return *this; }

    inline bool HasTexture() { return hasTexture; } /*{ return glIsTexture(texture) == GL_TRUE; }*/
    inline GLuint GetTexture() { return texture; }

    bool DeleteTexture()
    {
        if (!HasTexture()) return false;
        glDeleteTextures(1, &texture);
        hasTexture = false;
        return true;
    }

    bool LoadFromUCTEXFile(std::string filename)
    {
        if (!std::filesystem::is_regular_file(filename)) return false;

        FILE *f = fopen(filename.c_str(), "rb");
        if (!f) return false;

        char sig[5];
        fread(&sig, sizeof(char), 5, f);
        if (feof(f) || strncmp(sig, "UCTEX", 5)) { fclose(f); return false; }

        uint16_t version;
        fread(&version, sizeof(version), 1, f);
        if (feof(f) || version != 0) { fclose(f); return false; }

        uint8_t type;
        fread(&type, sizeof(type), 1, f);
        if (feof(f) || (type > 3)) { fclose(f); return false; }

        uint16_t width16, height16;
        fread(&width16, sizeof(width16), 1, f);
        fread(&height16, sizeof(height16), 1, f);
        if (feof(f)) { fclose(f); return false; }
        uint32_t width = width16 + 1;
        uint32_t height = height16 + 1;

        DeleteTexture();

        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        hasTexture = true;

        bool texmiss_y = false;
        std::vector<uint32_t> pixels;
        switch (type)
        {
            case 0: // RGBA (0xAABBGGRR).
                for (uint32_t y = 0; y < height; y++)
                {
                    bool texmiss_x = texmiss_y;
                    for (uint32_t x = 0; x < width; x++)
                    {
                        uint32_t pixel;
                        fread(&pixel, sizeof(uint32_t), 1, f);
                        if (feof(f))
                        {
                            if (texmiss_x) pixel = 0xFFFF00FF;
                            else pixel = 0xFF000000;
                        }
                        pixels.push_back(pixel);

                        texmiss_x = !texmiss_x;
                    }
                    texmiss_y = !texmiss_y;
                }
                break;

            case 1: // RGB (0xBBGGRR).
                for (uint32_t y = 0; y < height; y++)
                {
                    bool texmiss_x = texmiss_y;
                    for (uint32_t x = 0; x < width; x++)
                    {
                        uint8_t r, g, b;
                        fread(&r, sizeof(uint8_t), 1, f);
                        fread(&g, sizeof(uint8_t), 1, f);
                        fread(&b, sizeof(uint8_t), 1, f);
                        if (feof(f))
                        {
                            if (texmiss_x) { r = 0; g = 255; b = 0; }
                            else { r = 255; g = 255; b = 255; }
                        }
                        pixels.push_back((0xFF << 24) | (b << 16) | (g << 8) | r);

                        texmiss_x = !texmiss_x;
                    }
                    texmiss_y = !texmiss_y;
                }
                break;
            
            case 2: // 16-bit depth RGBA (1 bit alpha). (0bABBBBBGGGGGRRRRR)
                for (uint32_t y = 0; y < height; y++)
                {
                    bool texmiss_x = texmiss_y;
                    for (uint32_t x = 0; x < width; x++)
                    {
                        uint16_t pixel;
                        fread(&pixel, sizeof(uint16_t), 1, f);
                        if (feof(f))
                        {
                            if (texmiss_x) pixel = 0b1000001111111111;
                            else pixel = 0b1111110000000000;
                        }
                        pixels.push_back((((pixel >> 15) * 255) << 24) | ((pixel & 0b11111) << 3) | ((pixel & (0b11111 << 5)) << 6) | ((pixel & (0b11111 << 10)) << 9));

                        texmiss_x = !texmiss_x;
                    }
                    texmiss_y = !texmiss_y;
                }
                break;

            case 3: // 16-bit depth RGB (0b0BBBBBGGGGGRRRRR).
                for (uint32_t y = 0; y < height; y++)
                {
                    bool texmiss_x = texmiss_y;
                    for (uint32_t x = 0; x < width; x++)
                    {
                        uint16_t pixel;
                        fread(&pixel, sizeof(uint16_t), 1, f);
                        if (feof(f))
                        {
                            if (texmiss_x) pixel = 0b0111111111100000;
                            else pixel = 0b0000000000011111;
                        }
                        pixels.push_back((0xFF << 24) | ((pixel & 0b11111) << 3) | ((pixel & (0b11111 << 5)) << 6) | ((pixel & (0b11111 << 10)) << 9));

                        texmiss_x = !texmiss_x;
                    }
                    texmiss_y = !texmiss_y;
                }
                break;
        }

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());

        glBindTexture(GL_TEXTURE_2D, 0);
        fclose(f);

        return true;
    }

    bool SetTextureIntParameter(GLenum param, GLint value)
    {
        if (!HasTexture()) return false;

        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, param, value);
        glBindTexture(GL_TEXTURE_2D, 0);

        return true;
    }

    void SetDefaultParametres()
    {
        SetTextureIntParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        SetTextureIntParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        SetTextureIntParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
        SetTextureIntParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
    }
};

enum
{
    NoCulling = 0,
    BackFace = 1,
    FrontFace = 2,
    BothFaces = 3
} typedef FaceCullingType;

class Surface
{
  private:
    Texture *texture = nullptr;
    Mesh *mesh = nullptr;
    FaceCullingType culling = BackFace;

  public:
    Transform transform = Transform();
    glm::vec4 color = glm::vec4(1.0f);
    bool enableRender = true;

    Surface(Transform tr, Texture *t, Mesh *m, FaceCullingType c)
    { transform = tr; texture = t; mesh = m; culling = c; }
    Surface(Texture *t, Mesh *m, FaceCullingType c)
    { texture = t; mesh = m; culling = c; }

    Surface(Transform tr, Mesh *m, Texture *t, FaceCullingType c)
    { transform = tr; texture = t; mesh = m; culling = c; }
    Surface(Mesh *m, Texture *t, FaceCullingType c)
    { texture = t; mesh = m; culling = c; }

    Surface(Transform tr, Texture *t, Mesh *m)
    { transform = tr; texture = t; mesh = m; }
    Surface(Texture *t, Mesh *m)
    { texture = t; mesh = m; }

    Surface(Transform tr, Mesh *m, Texture *t)
    { transform = tr; texture = t; mesh = m; }
    Surface(Mesh *m, Texture *t)
    { texture = t; mesh = m; }

    Surface(Transform tr, Mesh *m) { transform = tr; mesh = m; }
    Surface(Mesh *m) { mesh = m; }

    ~Surface() {}

    inline Surface Copy() { return *this; }

    inline Texture *GetTexture() { return texture; }
    inline Mesh *GetMesh() { return mesh; }
    inline FaceCullingType GetFaceCullingType() { return culling; }

    inline void SetTexture(Texture *t) { texture = t; }
    inline void SetMesh(Mesh *m) { mesh = m; }
    inline void SetFaceCullingType(FaceCullingType c) { culling = c; }
};

/*
class AABB
{
  private:
    glm::vec3 min = glm::vec3(0.0f);
    glm::vec3 max = glm::vec3(0.0f);

  public:
    AABB(glm::vec3 point1, glm::vec3 point2) { SetBounds(point1, point2); }

    inline AABB Copy() { return *this; }

    inline glm::vec3 GetMinPoint() { return min; }
    inline glm::vec3 GetMaxPoint() { return max; }
    inline glm::vec3 GetSize() { return max - min; }
    inline glm::vec3 GetCenterOffset() { return (min + max) / 2.0f; }

    inline void Translate(glm::vec3 offset) { min += offset; max += offset; }
    void SetBounds(glm::vec3 point1, glm::vec3 point2)
    {
        min = glm::vec3(glm::min(point1.x, point2.x), glm::min(point1.y, point2.y), glm::min(point1.z, point2.z));
        max = glm::vec3(glm::max(point1.x, point2.x), glm::max(point1.y, point2.y), glm::max(point1.z, point2.z));
    }

    inline bool PointIntersects(glm::vec3 point)
    { return (point.x >= min.x && point.y >= min.y && point.z >= min.z) && (point.x <= max.x && point.y <= max.y && point.z <= max.z); }

    bool AABBIntersects(AABB *aabb)
    {
        glm::vec3 b_min = aabb->GetMinPoint();
        glm::vec3 b_max = aabb->GetMaxPoint();
        return (min.x <= b_max.x && min.y <= b_max.y && min.z <= b_max.z) && (max.x >= b_min.x && max.y >= b_min.y && max.z >= b_min.z);
    }
    inline bool AABBIntersects(AABB aabb) { return AABBIntersects(&aabb); }

    inline AABB GetAABBOverlapRegion(AABB *target) { return AABB(glm::max(target->GetMinPoint(), min), glm::min(target->GetMaxPoint(), max)); }
    inline AABB GetAABBOverlapRegion(AABB target) { return GetAABBOverlapRegion(&target); }

    inline glm::vec3 GenRightSideNormal() { return Utils::normalize(glm::vec3(max.x, min.y, min.z)); }
    inline glm::vec3 GenUpSideNormal() { return Utils::normalize(glm::vec3(min.x, max.y, min.z)); }
    inline glm::vec3 GenFrontSideNormal() { return -Utils::normalize(glm::vec3(min.x, min.y, max.z)); } // negative because in OpenGL front is -Z direction instead of +Z.

    glm::vec3 GetAABBPenetration(AABB *target)
    {
        if (!AABBIntersects(target)) return glm::vec3(0.0f);

        glm::vec3 b_min = target->GetMinPoint();
        glm::vec3 b_max = target->GetMaxPoint();

        glm::vec3 ret;
        ret.x = glm::min(max.x - b_min.x, b_max.x - min.x);
        ret.y = glm::min(max.y - b_min.y, b_max.y - min.y);
        ret.z = glm::min(max.z - b_min.z, b_max.z - min.z);
        return ret;
    }

    inline glm::vec3 GetAABBWeightedPenetration(AABB *target) { return (target->GetCenterOffset() - GetCenterOffset()) * GetAABBPenetration(target); }
    
    glm::vec3 GetAABBSingleDirectionPenetration(AABB *target)
    {
        glm::vec3 p = GetAABBPenetration(target);
        glm::vec3 a_center = GetCenterOffset();
        glm::vec3 b_center = target->GetCenterOffset();

        if (p.x > p.y && p.x > p.z) return p * glm::vec3(Utils::sign(a_center.x - b_center.x), 0.0f, 0.0f);
        else if (p.y > p.z) return p * glm::vec3(0.0f, Utils::sign(a_center.y - b_center.y), 0.0f);
        return p * glm::vec3(0.0f, Utils::sign(a_center.z - b_center.z), 0.0f);
    }
};*/

struct
{
    bool fogEnabled;
    float fogStartDistance;
    float fogEndDistance;
    glm::vec3 fogColor;
} typedef FogRenderSettings;

class Entity : public GameObject
{
  public:
    const GameObjectType type = ENTITY;

    bool enableRender = true;
    glm::vec4 color = glm::vec4(1.0f);
    std::vector<Surface> surfaces = std::vector<Surface>();

    Entity(Transform t) : GameObject(t) {}
    Entity() : GameObject() {}

    ~Entity() {}

    void Render(ShaderProgram *sp, glm::mat4 *view, glm::mat4 *projection, Transform *cameraTransform, FogRenderSettings *fogRenderSettings)
    {
        if (!enableRender) return;

        glUseProgram(sp->GetShaderProgram());

        glUniformMatrix4fv(glGetUniformLocation(sp->GetShaderProgram(), "projection"), 1, GL_FALSE, glm::value_ptr(*projection));
        glUniformMatrix4fv(glGetUniformLocation(sp->GetShaderProgram(), "view"), 1, GL_FALSE, glm::value_ptr(*view));

        glUniform1i(glGetUniformLocation(sp->GetShaderProgram(), "texture"), 0);
        glActiveTexture(GL_TEXTURE0);

        glUniform3fv(glGetUniformLocation(sp->GetShaderProgram(), "cameraPosition"), 1, glm::value_ptr(cameraTransform->GetPosition()));
        glUniform3fv(glGetUniformLocation(sp->GetShaderProgram(), "cameraRotation"), 1, glm::value_ptr(cameraTransform->GetRotation()));

        glUniform3fv(glGetUniformLocation(sp->GetShaderProgram(), "cameraFront"), 1, glm::value_ptr(cameraTransform->GetFront()));
        glUniform3fv(glGetUniformLocation(sp->GetShaderProgram(), "cameraUp"), 1, glm::value_ptr(cameraTransform->GetUp()));
        glUniform3fv(glGetUniformLocation(sp->GetShaderProgram(), "cameraRight"), 1, glm::value_ptr(cameraTransform->GetRight()));

        glUniform1i(glGetUniformLocation(sp->GetShaderProgram(), "fogEnabled"), fogRenderSettings->fogEnabled ? GL_TRUE : GL_FALSE);
        glUniform1f(glGetUniformLocation(sp->GetShaderProgram(), "fogStartDistance"), fogRenderSettings->fogStartDistance);
        glUniform1f(glGetUniformLocation(sp->GetShaderProgram(), "fogEndDistance"), fogRenderSettings->fogEndDistance);
        glUniform3fv(glGetUniformLocation(sp->GetShaderProgram(), "fogColor"), 1, glm::value_ptr(fogRenderSettings->fogColor));

        for (Surface surface : surfaces)
        {
            if (!surface.enableRender) continue;

            Texture *texture = surface.GetTexture();
            Mesh *mesh = surface.GetMesh();
            FaceCullingType culling = surface.GetFaceCullingType();
            if (!(mesh && mesh->HasBuffers() && culling != BothFaces)) continue;

            if (culling == NoCulling) glDisable(GL_CULL_FACE);
            else
            {
                glEnable(GL_CULL_FACE);

                switch (culling)
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
                glUniform1i(glGetUniformLocation(sp->GetShaderProgram(), "hasTexture"), GL_TRUE);
                glBindTexture(GL_TEXTURE_2D, texture->GetTexture());
            }
            else glUniform1i(glGetUniformLocation(sp->GetShaderProgram(), "hasTexture"), GL_FALSE);

            glm::mat4 modelmat = GetParentGlobalTransformationMatrix() * transform.GetTransformationMatrix() * surface.transform.GetTransformationMatrix();
            glUniformMatrix4fv(glGetUniformLocation(sp->GetShaderProgram(), "model"), 1, GL_FALSE, glm::value_ptr(modelmat));

            glUniform4fv(glGetUniformLocation(sp->GetShaderProgram(), "color"), 1, glm::value_ptr(color * surface.color));

            glBindVertexArray(mesh->GetVAO());
            glDrawElements(GL_TRIANGLES, mesh->GetIndicesCount(), GL_UNSIGNED_INT, nullptr);
        }
    }
};

class Camera : public GameObject
{
  private:
    float neardist = 0.05;//0.1;
    float fardist = 1000;
    float fov = glm::radians(60.0f);
    
  public:
    const GameObjectType type = CAMERA;

    Camera(float _fov, float _neardist, float _fardist)
    {
        fov = _fov;
        neardist = _neardist;
        fardist = _fardist;
    }

    Camera(float _neardist, float _fardist)
    {
        neardist = _neardist;
        fardist = _fardist;
    }

    Camera(float _fov) { fov = _fov; }

    Camera() {}

    inline float GetNearDistance() { return neardist; }
    inline float GetFarDistance() { return fardist; }
    inline float GetFOV() { return fov; }

    inline void SetNearDistance(float _neardist) { neardist = _neardist; }
    inline void SetFarDistance(float _fardist) { fardist = _fardist; }
    inline void SetFOV(float _fov) { fov = _fov; }

    inline glm::mat4 GetViewMatrix()
    { return glm::lookAt(transform.GetPosition(), transform.GetPosition() + transform.GetFront(), transform.GetUp()); }
    inline glm::mat4 GetProjectionMatrix(unsigned int screen_width, unsigned int screen_height)
    { return glm::perspective(fov, (float)screen_width / (float)screen_height, neardist, fardist); }
};

/*
enum
{
    NODATA = 0,

    UNSIGNED_PCM_8_MONO = 1,
    UNSIGNED_PCM_8_STEREO = 2,

    SIGNED_PCM_16_MONO = 3,
    SIGNED_PCM_16_STEREO = 4
} typedef AudioClipType;

class AudioClip
{
  private:
    void *data = nullptr;
    size_t size = 0;

    AudioClipType type = NODATA;
    uint16_t frequency = 0;
    size_t length = 0; // in (both channel) samples.

  public:
    AudioClip() {}
    ~AudioClip()
    {
        if (HasBuffer()) free(data);
    }

    inline bool HasBuffer() { return data; }
    inline AudioClipType GetType() { return type; }

    static uint8_t GetTypeSampleSizeInBytes(AudioClipType t)
    {
        switch (t)
        {
            case UNSIGNED_PCM_8_MONO:
                return sizeof(uint8_t);

            case UNSIGNED_PCM_8_STEREO:
                return 2 * sizeof(uint8_t);

            case SIGNED_PCM_16_MONO:
                return sizeof(int16_t);

            case SIGNED_PCM_16_STEREO:
                return 2 * sizeof(int16_t);
        }

        return 0;
    }

    inline size_t GetBufferSizeInBytes() { return GetTypeSampleSizeInBytes(type) * length * frequency; }

    bool CreateBuffer(size_t len, AudioClipType t, uint16_t freq)
    {
        if (HasBuffer() || t == NODATA || len == 0 || freq == 0) return false;

        void *new_data = malloc(len * GetTypeSampleSizeInBytes(t) * freq);
        if (!new_data) return false;
        data = new_data;

        type = t;
        length = len;
        frequency = freq;

        return true;
    }

    bool DeleteBuffer()
    {
        if (!data) return false;

        free(data);

        type = NODATA;
        length = 0;
        frequency = 0;

        return true;
    }

    bool SetBufferData(size_t srcbuffsize, void *srcbuff)
    {
        if (!HasBuffer()) return false;

        memcpy(data, srcbuff, min(srcbuffsize, GetBufferSizeInBytes()));

        return true;
    }
};*/

class AudioClip
{
  private:
    ALuint buffer;
    bool hasBuffer = false;

  public:
    AudioClip() {}
    ~AudioClip() { DeleteBuffer(); }

    inline bool HasBuffer() { return hasBuffer; }
    inline ALuint GetBuffer() { return buffer; }
    bool DeleteBuffer()
    {
        if (!HasBuffer()) return false;
        alDeleteBuffers(1, &buffer);
        hasBuffer = false;
        return true;
    }
    bool LoadUCSOUNDFromFile(std::string filename)
    {
        if (!std::filesystem::is_regular_file(filename)) return false;

        FILE *f = fopen(filename.c_str(), "rb");
        if (!f) return false;

        char sig[7];
        fread(&sig, sizeof(char), 7, f);
        if (feof(f) || strncmp(sig, "UCSOUND", 7)) { fclose(f); return false; }

        uint16_t version;
        fread(&version, sizeof(version), 1, f);
        if (feof(f) || version != 0) { fclose(f); return false; }

        uint8_t type;
        fread(&type, sizeof(type), 1, f);
        if (feof(f)) { fclose(f); return false; }

        ALenum altype;
        switch (type)
        {
            case 0: // mono 8 bit/sample (unsigned 8-bit).
                altype = AL_FORMAT_MONO8;
                break;

            case 1: // mono 16 bit/sample (signed 16-bit).
                altype = AL_FORMAT_MONO16;
                break;

            case 2: // stereo 8 bit/sample (unsigned 8-bit).
                altype = AL_FORMAT_STEREO8;
                break;

            case 3: // stereo 16 bit/sample (signed 16-bit).
                altype = AL_FORMAT_STEREO16;
                break;

            default:
                return false;
        }

        uint16_t frequency;
        fread(&frequency, sizeof(frequency), 1, f);
        if (feof(f)) { fclose(f); return false; }

        alGenBuffers(1, &buffer);
        hasBuffer = true;

        std::vector<uint8_t> data = std::vector<uint8_t>();
        while (true)
        {
            uint8_t byte;
            fread(&byte, sizeof(byte), 1, f);
            if (feof(f)) break;

            data.push_back(byte);
        }

        alBufferData(buffer, altype, data.data(), data.size(), frequency);

        return true;
    }
};

class AudioSource : public GameObject
{
  private:
    ALuint source;

    void callback(Transform *t)
    {
    }

    void constructor()
    {
        alGenSources(1, &source);
        transform.onTransformChangeCallbacks.push_back([this](Transform *t)
        {
           alSourcefv(source, AL_POSITION, glm::value_ptr(t->GetPosition())); 
        });
    }

  public:
    const GameObjectType type = AUDIOSOURCE;

    AudioSource(glm::vec3 pos) : GameObject(pos) { constructor(); }
    AudioSource() : GameObject() { constructor(); }

    ~AudioSource() { alDeleteSources(1, &source); }

    float GetPitch()
    {
        float ret;
        alGetSourcef(source, AL_PITCH, &ret);
        return ret;
    }
    inline void SetPitch(float value) { alSourcef(source, AL_PITCH, value); }

    float GetGain()
    {
        float ret;
        alGetSourcef(source, AL_GAIN, &ret);
        return ret;
    }
    inline void SetGain(float value) { alSourcef(source, AL_GAIN, value); }

    float GetMinGain()
    {
        float ret;
        alGetSourcef(source, AL_MIN_GAIN, &ret);
        return ret;
    }
    inline void SetMinGain(float value) { alSourcef(source, AL_MIN_GAIN, value); }

    float GetMaxGain()
    {
        float ret;
        alGetSourcef(source, AL_MAX_GAIN, &ret);
        return ret;
    }
    inline void SetMaxGain(float value) { alSourcef(source, AL_MAX_GAIN, value); }

    float GetMaxDistance()
    {
        float ret;
        alGetSourcef(source, AL_MAX_DISTANCE, &ret);
        return ret;
    }
    inline void SetMaxDistance(float value) { alSourcef(source, AL_MAX_DISTANCE, value); }
};