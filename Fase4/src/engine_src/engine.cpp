#define STB_IMAGE_IMPLEMENTATION
#include "../utils/stb_image.h"

#define NOCRT_STDIO_INLINE
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <cmath>
#include <cstring>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#undef _NO_CRT_STDIO_INLINE
#include "../tinyXML/tinyxml2.hpp"
#include "../utils/matrix.hpp"

using namespace std;

struct Light
{
    enum Type
    {
        POINT,
        DIRECTIONAL
    } type;
    float position[4]; // w = 1 para point, 0 para directional
    float ambient[4];
    float diffuse[4];
    float specular[4];
};

vector<Light> lights;

struct Vertex
{
    float x, y, z;
    float nx, ny, nz;
    float u, v;
};

struct Material
{
    float diffuse[4] = {0.8f, 0.8f, 0.8f, 1.0f};
    float ambient[4] = {0.2f, 0.2f, 0.2f, 1.0f};
    float specular[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    float emissive[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    float shininess = 0;
};

struct Model
{
    vector<Vertex> vertices; // guarda os dados em CPU
    GLuint vboID = 0;
    GLuint texID = 0;
    Material mat;
    size_t vertexCount = 0;
    string texturePath;
};

struct Transform
{
    enum Type
    {
        TRANSLATE,
        ROTATE,
        SCALE
    } type;
    float values[4];
};

struct Group
{
    vector<Transform> transforms;
    vector<Model> models;
    vector<Group> children;
};

Group scene;
float camx = 5, camy = 5, camz = 5;
float lookx = 0, looky = 0, lookz = 0;
float upx = 0, upy = 1, upz = 0;
float fov = 60, nearVal = 1, farVal = 1000;
bool wireframe = false;

void applyMaterial(const Material &m)
{
    glMaterialfv(GL_FRONT, GL_DIFFUSE, m.diffuse);
    glMaterialfv(GL_FRONT, GL_AMBIENT, m.ambient);
    glMaterialfv(GL_FRONT, GL_SPECULAR, m.specular);
    glMaterialfv(GL_FRONT, GL_EMISSION, m.emissive);
    glMaterialf(GL_FRONT, GL_SHININESS, m.shininess);
}

void setupLights()
{
    int maxLights = 8; // GL_MAX_LIGHTS geralmente é 8
    int count = (int)min(lights.size(), (size_t)maxLights);

    for (int i = 0; i < count; ++i)
    {
        GLenum lightEnum = GL_LIGHT0 + i;
        glEnable(lightEnum);

        glLightfv(lightEnum, GL_POSITION, lights[i].position);
        glLightfv(lightEnum, GL_AMBIENT, lights[i].ambient);
        glLightfv(lightEnum, GL_DIFFUSE, lights[i].diffuse);
        glLightfv(lightEnum, GL_SPECULAR, lights[i].specular);
    }

    // Desliga luzes extras
    for (int i = count; i < maxLights; ++i)
    {
        glDisable(GL_LIGHT0 + i);
    }
}

void createVBOs(Group &g)
{
    for (auto &m : g.models)
    {
        if (!m.vertices.empty())
        {
            glGenBuffers(1, &m.vboID);
            glBindBuffer(GL_ARRAY_BUFFER, m.vboID);
            glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * m.vertices.size(), m.vertices.data(), GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            m.vertices.clear(); // já pode limpar o vetor para liberar memória
            m.vertices.shrink_to_fit();
        }
    }
    for (auto &child : g.children)
    {
        createVBOs(child);
    }
}

GLuint loadTexture(const string &file)
{
    cout << "[Info] Carregando textura: " << file << endl;
    int w, h, channels;
    unsigned char *data = stbi_load(file.c_str(), &w, &h, &channels, 0);
    if (!data)
    {
        cerr << "[Erro] Falha ao carregar textura: " << file << endl;
        return 0;
    }

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;

    glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
    cerr << "[Info] Textura carregada com sucesso: " << file << " (ID: " << tex << ")" << endl;
    return tex;
}

void setupTextures(Group &g)
{
    for (auto &m : g.models)
    {
        if (!m.texturePath.empty())
        {
            m.texID = loadTexture(m.texturePath);
        }
    }
    for (auto &child : g.children)
    {
        setupTextures(child);
    }
}

Model loadModel(const string &path, const string &texPath, const Material &mat)
{
    Model m;

    ifstream in(path);
    if (!in.is_open())
    {
        cerr << "[Erro] Não foi possível abrir o ficheiro: " << path << endl;
        return m;
    }

    string line;
    if (!getline(in, line))
    {
        cerr << "[Erro] Ficheiro vazio: " << path << endl;
        return m;
    }

    int totalVertices = 0;
    try
    {
        totalVertices = stoi(line);
    }
    catch (...)
    {
        cerr << "[Erro] Primeira linha inválida: " << path << endl;
        return m;
    }

    vector<Vertex> vertices;
    while (getline(in, line))
    {
        Vertex v;
        int n = sscanf(line.c_str(), "%f,%f,%f,%f,%f,%f,%f,%f",
                       &v.x, &v.y, &v.z, &v.nx, &v.ny, &v.nz, &v.u, &v.v);
        if (n == 8)
        {
            vertices.push_back(v);
        }
        else
        {
            cerr << "[Aviso] Linha inválida: " << line << endl;
        }
    }

    if (vertices.empty())
    {
        cerr << "[Erro] Nenhum vértice válido encontrado no modelo." << endl;
        return m;
    }

    m.vertexCount = vertices.size();
    m.vertices = std::move(vertices);
    m.mat = mat;

    if (!texPath.empty())
    {
        m.texturePath = texPath;
    }

    cerr << "[Info] Modelo carregado: " << path << " com " << m.vertexCount << " vértices." << endl;
    return m;
}

void processLights(tinyxml2::XMLElement *lightsElem)
{
    for (auto *lightElem = lightsElem->FirstChildElement("light"); lightElem; lightElem = lightElem->NextSiblingElement("light"))
    {
        Light light;
        const char *typeStr = lightElem->Attribute("type");
        if (!typeStr)
            continue;
        string t(typeStr);

        if (t == "directional")
        {
            light.type = Light::DIRECTIONAL;
            light.position[0] = lightElem->FloatAttribute("dirx");
            light.position[1] = lightElem->FloatAttribute("diry");
            light.position[2] = lightElem->FloatAttribute("dirz");
            light.position[3] = 0.0f;
        }
        else if (t == "point")
        {
            light.type = Light::POINT;
            light.position[0] = lightElem->FloatAttribute("posx");
            light.position[1] = lightElem->FloatAttribute("posy");
            light.position[2] = lightElem->FloatAttribute("posz");
            light.position[3] = 1.0f;
        }
        else
            continue;

        // Podes definir cores padrão, ou ler atributos no XML se quiseres
        light.ambient[0] = 0.2f;
        light.ambient[1] = 0.2f;
        light.ambient[2] = 0.2f;
        light.ambient[3] = 1.0f;
        light.diffuse[0] = 0.8f;
        light.diffuse[1] = 0.8f;
        light.diffuse[2] = 0.8f;
        light.diffuse[3] = 1.0f;
        light.specular[0] = 1.0f;
        light.specular[1] = 1.0f;
        light.specular[2] = 1.0f;
        light.specular[3] = 1.0f;

        lights.push_back(light);
    }
}

Group processGroup(tinyxml2::XMLElement *groupElem)
{
    Group g;

    // Transforms
    auto *transformElem = groupElem->FirstChildElement("transform");
    if (transformElem)
    {
        for (auto *t = transformElem->FirstChildElement(); t; t = t->NextSiblingElement())
        {
            Transform tf;
            string type = t->Value();
            if (type == "translate")
            {
                tf.type = Transform::TRANSLATE;
                t->QueryFloatAttribute("x", &tf.values[0]);
                t->QueryFloatAttribute("y", &tf.values[1]);
                t->QueryFloatAttribute("z", &tf.values[2]);
            }
            else if (type == "rotate")
            {
                tf.type = Transform::ROTATE;
                t->QueryFloatAttribute("angle", &tf.values[0]);
                t->QueryFloatAttribute("x", &tf.values[1]);
                t->QueryFloatAttribute("y", &tf.values[2]);
                t->QueryFloatAttribute("z", &tf.values[3]);
            }
            else if (type == "scale")
            {
                tf.type = Transform::SCALE;
                t->QueryFloatAttribute("x", &tf.values[0]);
                t->QueryFloatAttribute("y", &tf.values[1]);
                t->QueryFloatAttribute("z", &tf.values[2]);
            }
            g.transforms.push_back(tf);
        }
    }

    // Models
    auto *modelsElem = groupElem->FirstChildElement("models");
    if (modelsElem)
    {
        for (auto *m = modelsElem->FirstChildElement("model"); m; m = m->NextSiblingElement("model"))
        {
            const char *file = m->Attribute("file");
            if (!file)
            {
                cerr << "[Erro] Modelo sem atributo 'file'." << endl;
                continue;
            }

            string texFile = "";
            Material mat;

            // Textura (atribuição direta)
            auto *texElem = m->FirstChildElement("texture");
            if (texElem)
            {
                const char *texFileRaw = texElem->Attribute("file");
                if (texFileRaw)
                    texFile = texFileRaw;
            }

            // Cores
            auto *color = m->FirstChildElement("color");
            if (color)
            {
                auto *d = color->FirstChildElement("diffuse");
                if (d)
                {
                    mat.diffuse[0] = d->FloatAttribute("R") / 255.0f;
                    mat.diffuse[1] = d->FloatAttribute("G") / 255.0f;
                    mat.diffuse[2] = d->FloatAttribute("B") / 255.0f;
                    mat.diffuse[3] = 1.0f;
                }
                auto *a = color->FirstChildElement("ambient");
                if (a)
                {
                    mat.ambient[0] = a->FloatAttribute("R") / 255.0f;
                    mat.ambient[1] = a->FloatAttribute("G") / 255.0f;
                    mat.ambient[2] = a->FloatAttribute("B") / 255.0f;
                    mat.ambient[3] = 1.0f;
                }
                auto *s = color->FirstChildElement("specular");
                if (s)
                {
                    mat.specular[0] = s->FloatAttribute("R") / 255.0f;
                    mat.specular[1] = s->FloatAttribute("G") / 255.0f;
                    mat.specular[2] = s->FloatAttribute("B") / 255.0f;
                    mat.specular[3] = 1.0f;
                }
                auto *e = color->FirstChildElement("emissive");
                if (e)
                {
                    mat.emissive[0] = e->FloatAttribute("R") / 255.0f;
                    mat.emissive[1] = e->FloatAttribute("G") / 255.0f;
                    mat.emissive[2] = e->FloatAttribute("B") / 255.0f;
                    mat.emissive[3] = 1.0f;
                }
                auto *sh = color->FirstChildElement("shininess");
                if (sh)
                {
                    sh->QueryFloatAttribute("value", &mat.shininess);
                }
            }

            g.models.push_back(loadModel(file, texFile, mat));
        }
    }

    // Grupos filhos
    for (auto *child = groupElem->FirstChildElement("group"); child; child = child->NextSiblingElement("group"))
    {
        g.children.push_back(processGroup(child));
    }

    return g;
}

void readXML(const string &file)
{
    tinyxml2::XMLDocument doc;
    if (doc.LoadFile(file.c_str()) != tinyxml2::XML_SUCCESS)
    {
        cerr << "Erro ao ler XML: " << file << endl;
        return;
    }
    auto *world = doc.FirstChildElement("world");
    if (!world)
        return;

    // Lê as luzes
    auto *lightsElem = world->FirstChildElement("lights");
    if (lightsElem)
    {
        processLights(lightsElem);
    }

    // Continua como antes para câmera e grupo
    auto *cam = world->FirstChildElement("camera");
    if (cam)
    {
        auto *pos = cam->FirstChildElement("position");
        if (pos)
        {
            pos->QueryFloatAttribute("x", &camx);
            pos->QueryFloatAttribute("y", &camy);
            pos->QueryFloatAttribute("z", &camz);
        }
        auto *la = cam->FirstChildElement("lookAt");
        if (la)
        {
            la->QueryFloatAttribute("x", &lookx);
            la->QueryFloatAttribute("y", &looky);
            la->QueryFloatAttribute("z", &lookz);
        }
        auto *up = cam->FirstChildElement("up");
        if (up)
        {
            up->QueryFloatAttribute("x", &upx);
            up->QueryFloatAttribute("y", &upy);
            up->QueryFloatAttribute("z", &upz);
        }
        auto *proj = cam->FirstChildElement("projection");
        if (proj)
        {
            proj->QueryFloatAttribute("fov", &fov);
            proj->QueryFloatAttribute("near", &nearVal);
            proj->QueryFloatAttribute("far", &farVal);
        }
    }

    auto *group = world->FirstChildElement("group");
    if (group)
    {
        scene = processGroup(group);
    }
}

void renderGroup(const Group &g)
{
    glPushMatrix();
    for (const auto &t : g.transforms)
    {
        if (t.type == Transform::TRANSLATE)
            glTranslatef(t.values[0], t.values[1], t.values[2]);
        else if (t.type == Transform::ROTATE)
            glRotatef(t.values[0], t.values[1], t.values[2], t.values[3]);
        else if (t.type == Transform::SCALE)
            glScalef(t.values[0], t.values[1], t.values[2]);
    }
    for (const auto &m : g.models)
    {
        applyMaterial(m.mat);
        if (m.texID)
        {
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, m.texID);
        }
        else
        {
            glDisable(GL_TEXTURE_2D);
        }
        glBindBuffer(GL_ARRAY_BUFFER, m.vboID);
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, sizeof(Vertex), (void *)0);
        glEnableClientState(GL_NORMAL_ARRAY);
        glNormalPointer(GL_FLOAT, sizeof(Vertex), (void *)(3 * sizeof(float)));
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), (void *)(6 * sizeof(float)));
        glDrawArrays(GL_TRIANGLES, 0, m.vertexCount);
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    for (const auto &child : g.children)
        renderGroup(child);
    glPopMatrix();
}

void renderScene()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    gluLookAt(camx, camy, camz, lookx, looky, lookz, upx, upy, upz);

    // Atualiza as posições das luzes (OpenGL fixa luzes à matriz atual)
    for (int i = 0; i < (int)lights.size() && i < 8; ++i)
    {
        GLenum lightEnum = GL_LIGHT0 + i;
        glLightfv(lightEnum, GL_POSITION, lights[i].position);
    }

    renderGroup(scene);

    glutSwapBuffers();
}

void changeSize(int w, int h)
{
    if (h == 0)
        h = 1;
    float ratio = w * 1.0 / h;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fov, ratio, nearVal, farVal);
    glViewport(0, 0, w, h);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char **argv)
{
    std::cout << "[Info] Início do programa" << std::endl;

    if (argc < 2)
    {
        std::cerr << "[Erro] Ficheiro XML em falta" << std::endl;
        return 1;
    }
    else
    {
        std::cout << "[Info] Ficheiro XML fornecido: " << argv[1] << std::endl;
    }

    std::cout << "[Info] A ler o XML..." << std::endl;
    readXML(argv[1]);
    std::cout << "[Info] XML lido com sucesso" << std::endl;

    std::cout << "[Info] Inicializando GLUT..." << std::endl;
    glutInit(&argc, argv);

    std::cout << "[Info] Criando janela OpenGL..." << std::endl;
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA);
    glutInitWindowSize(800, 800);
    glutCreateWindow("Fase 4 - Engine");

    std::cout << "[Info] Criando VBOs..." << std::endl;
    setupTextures(scene);
    createVBOs(scene);
    glEnable(GL_TEXTURE_2D);
    std::cout << "[fInfo] Configurando OpenGL..." << std::endl;
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glEnable(GL_LIGHTING);
    glEnable(GL_RESCALE_NORMAL);

    float globalAmbient[4] = {0.2f, 0.2f, 0.2f, 1.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);

    setupLights();

    glutDisplayFunc(renderScene);
    glutReshapeFunc(changeSize);

    std::cout << "[Info] Entrando no ciclo principal GLUT..." << std::endl;
    glutMainLoop();

    return 0;
}
