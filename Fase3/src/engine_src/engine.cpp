#define NOCRT_STDIO_INLINE
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <list>
//#include <GL/glew.h>
#ifdef __APPLE
#include <GLUT/glut.h>
#else
#include <glut.h>
#endif
#undef _NO_CRT_STDIO_INLINE
#include "../tinyXML/tinyxml2.hpp"
#include "../utils/matrix.hpp"


using namespace std;

float camx = 5.0f, camy = 5.0f, camz = 5.0f;
float lookAtx = 0.0f, lookAty = 0.0f, lookAtz = 0.0f;
float upx = 0.0f, upy = 1.0f, upz = 0.0f;

float posx = 0.0f, posz = 0.0f, angle = 0.0f, scalex = 1.0f, scaley = 1.0f, scalez = 1.0f;
float fov = 0.0f, nearVal = 0.0f, farVal = 0.0f;
bool wireframe = true;
float camDist = 500.0f;



// --- Estruturas definidas pelo utilizador ---
struct Point { float x, y, z; };
struct Transformation {
    enum Type { TRANSLATE_STATIC, TRANSLATE_CURVE, ROTATE_STATIC, ROTATE_TIME, SCALE } type;
    float values[4] = {0,0,0,0};
    bool align = false;
    vector<Point> points;
};
struct Transform {
    vector<Transformation> transformations;
    vector<Point> vertexes;
    GLuint vboID = 0;
    GLsizei vertexCount = 0;
};
vector<Transform> transformations;

struct GroupNode {
    Transform transform;
    vector<GroupNode> children;
};
GroupNode rootGroup;


void draw_orbit(float radius) {
    glBegin(GL_LINE_LOOP);
    glColor3f(0.7f, 0.7f, 0.7f);  // Cor da órbita
    for (int i = 0; i < 100; ++i) {
        float angle = 2 * M_PI * i / 100;
        glVertex3f(radius * cos(angle), 0.0f, radius * sin(angle));
    }
    glEnd();
}



void readFile(const string& path, Transform& tf) {
    ifstream file(path);
    if (!file) { cerr << "Erro a abrir " << path << endl; return; }
    string line;
    while (getline(file, line)) {
        Point p;
        if (sscanf(line.c_str(), "%f,%f,%f", &p.x, &p.y, &p.z) == 3)
            tf.vertexes.push_back(p);
    }
    tf.vertexCount = tf.vertexes.size();
}

void initVBOsRecursive(GroupNode& node) {
    if (!node.transform.vertexes.empty()) {
        glGenBuffers(1, &node.transform.vboID);
        glBindBuffer(GL_ARRAY_BUFFER, node.transform.vboID);
        glBufferData(GL_ARRAY_BUFFER,
                     node.transform.vertexCount * sizeof(Point),
                     node.transform.vertexes.data(),
                     GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    for (auto& child : node.children) {
        initVBOsRecursive(child);
    }
}



void draw_curve(const vector<Point>& points) {
    glBegin(GL_LINE_STRIP);
    glColor3f(1.0f, 0.0f, 0.0f); // Cor da curva
    for (auto& p : points) {
        glVertex3f(p.x, p.y, p.z);
    }
    glEnd();
}


void changeSize(int w, int h) {
    if (h == 0) h = 1;
    float ratio = w * 1.0 / h;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fov, ratio, nearVal, farVal);
    glViewport(0, 0, w, h);
    glMatrixMode(GL_MODELVIEW);
}

void draw_axis() {
    glBegin(GL_LINES);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(-100.0f, 0.0f, 0.0f);
    glVertex3f(100.0f, 0.0f, 0.0f);
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, -100.0f, 0.0f);
    glVertex3f(0.0f, 100.0f, 0.0f);
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, -100.0f);
    glVertex3f(0.0f, 0.0f, 100.0f);
    glEnd();
}

// Variáveis para armazenar as transformações do XML
float transX = 0.0f, transY = 0.0f, transZ = 0.0f;
float rotateAngle = 0.0f, rotateX = 0.0f, rotateY = 0.0f, rotateZ = 0.0f;
float scaleX = 1.0f, scaleY = 1.0f, scaleZ = 1.0f;


void renderGroup(const GroupNode& node) {
    glPushMatrix();

    // Aplica transformações
    for (const auto& tr : node.transform.transformations) {
        switch (tr.type) {
            case Transformation::TRANSLATE_STATIC:
                glTranslatef(tr.values[0], tr.values[1], tr.values[2]);
                break;

            case Transformation::TRANSLATE_CURVE: {
                float elapsed = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
                float duration = tr.values[0];
                float gt = fmod(elapsed, duration) / duration;
                vector<vector<float>> ctrl;
                for (auto& p : tr.points)
                    ctrl.push_back({p.x, p.y, p.z});
                float pos[3], deriv[3];
                getGlobalCatmullRomPoint(gt, ctrl, pos, deriv);
                glTranslatef(pos[0], pos[1], pos[2]);

                if (tr.align) {
                    float up[3] = {0,1,0}, right[3];
                    cross(deriv, up, right); normalize(right);
                    float up2[3]; cross(right, deriv, up2); normalize(up2);
                    float m[16]; buildRotMatrix(deriv, up2, right, m);
                    glMultMatrixf(m);
                }
                break;
            }

            case Transformation::ROTATE_STATIC:
                glRotatef(tr.values[0], tr.values[1], tr.values[2], tr.values[3]);
                break;

            case Transformation::ROTATE_TIME: {
                float elapsed = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
                float duration = tr.values[0];
                float ang = fmod(elapsed * 360.0f / duration, 360.0f);
                glRotatef(ang, tr.values[1], tr.values[2], tr.values[3]);
                break;
            }

            case Transformation::SCALE:
                glScalef(tr.values[0], tr.values[1], tr.values[2]);
                break;
        }
    }

    // Desenha geometria (se houver)
    if (node.transform.vboID) {
        glColor3f(0.8f, 0.5f, 0.1f); // Altere esta linha conforme o planeta
        glEnableClientState(GL_VERTEX_ARRAY);
        glBindBuffer(GL_ARRAY_BUFFER, node.transform.vboID);
        glVertexPointer(3, GL_FLOAT, 0, 0);
        glDrawArrays(GL_TRIANGLES, 0, node.transform.vertexCount);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDisableClientState(GL_VERTEX_ARRAY);
    }
    

    // Desenha os filhos
    for (const auto& child : node.children) {
        renderGroup(child);
    }

    glPopMatrix();
}

void renderScene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
    glLoadIdentity();

    gluLookAt(
        camx, camy, camz,  // posição da câmera
        lookAtx, lookAty, lookAtz,  // ponto para onde a câmera olha
        upx, upy, upz  // vetor para cima
      );
    
    draw_axis();
    renderGroup(rootGroup);

    glutSwapBuffers();
}



void keyboardSpecial(int key, int x, int y) {
    switch(key) {
      case GLUT_KEY_LEFT:  angle -= 5; break;
      case GLUT_KEY_RIGHT: angle += 5; break;
      case GLUT_KEY_UP:    camDist = max(10.0f, camDist - 10.0f); break;
      case GLUT_KEY_DOWN:  camDist += 10.0f; break;
    }
    glutPostRedisplay();
  }
  

  void keyboardFunc(unsigned char key,int x,int y){
    switch(key){
      case 'w': posz -= 5; break;
      case 's': posz += 5; break;
      case 'a': posx -= 5; break;
      case 'd': posx += 5; break;
      case 'q': angle -= 5; break;
      case 'e': angle += 5; break;
      case 'i': camy   += 5; break;
      case 'k': camy   -= 5; break;
      case '+': camDist -= 10; break;
      case '-': camDist += 10; break;
      case 'm': wireframe = !wireframe; break;
    }
    glutPostRedisplay();
  }
  



GroupNode processGroup(tinyxml2::XMLElement* groupElem, const Transform& parentTransform) {
    GroupNode node;
    node.transform = parentTransform;

    tinyxml2::XMLElement* transformElem = groupElem->FirstChildElement("transform");

    if (transformElem) {
        for (auto *elem = transformElem->FirstChildElement(); elem; elem = elem->NextSiblingElement()) {
            std::string name = elem->Value();
            Transformation t;

            if (name == "translate") {
                if (elem->QueryFloatAttribute("time", &t.values[0]) == tinyxml2::XML_SUCCESS) {
                    // animação por curva
                    t.type  = Transformation::TRANSLATE_CURVE;
                    elem->QueryBoolAttribute("align", &t.align);
                    for (auto *pt = elem->FirstChildElement("point"); pt; pt = pt->NextSiblingElement("point")) {
                        Point p;
                        pt->QueryFloatAttribute("x", &p.x);
                        pt->QueryFloatAttribute("y", &p.y);
                        pt->QueryFloatAttribute("z", &p.z);
                        t.points.push_back(p);
                    }
                } else {
                    t.type = Transformation::TRANSLATE_STATIC;
                    elem->QueryFloatAttribute("x", &t.values[0]);
                    elem->QueryFloatAttribute("y", &t.values[1]);
                    elem->QueryFloatAttribute("z", &t.values[2]);
                }
            }
            else if (name == "rotate") {
                if (elem->QueryFloatAttribute("time", &t.values[0]) == tinyxml2::XML_SUCCESS) {
                    t.type = Transformation::ROTATE_TIME;
                    elem->QueryFloatAttribute("x", &t.values[1]);
                    elem->QueryFloatAttribute("y", &t.values[2]);
                    elem->QueryFloatAttribute("z", &t.values[3]);
                } else {
                    t.type = Transformation::ROTATE_STATIC;
                    elem->QueryFloatAttribute("angle", &t.values[0]);
                    elem->QueryFloatAttribute("x", &t.values[1]);
                    elem->QueryFloatAttribute("y", &t.values[2]);
                    elem->QueryFloatAttribute("z", &t.values[3]);
                }
            }
            else if (name == "scale") {
                t.type = Transformation::SCALE;
                elem->QueryFloatAttribute("x", &t.values[0]);
                elem->QueryFloatAttribute("y", &t.values[1]);
                elem->QueryFloatAttribute("z", &t.values[2]);
            }

            node.transform.transformations.push_back(t);
        }
    }

    // Modelos deste grupo
    tinyxml2::XMLElement* modelsElem = groupElem->FirstChildElement("models");
    if (modelsElem) {
        for (tinyxml2::XMLElement* modelElem = modelsElem->FirstChildElement("model"); modelElem; modelElem = modelElem->NextSiblingElement("model")) {
            const char* fileAttr = modelElem->Attribute("file");
            if (fileAttr) {
                std::cout << "Carregando modelo: " << fileAttr << std::endl;
                readFile(fileAttr, node.transform);
            }
        }
    }

    // Subgrupos (filhos)
    for (tinyxml2::XMLElement* subGroupElem = groupElem->FirstChildElement("group"); subGroupElem; subGroupElem = subGroupElem->NextSiblingElement("group")) {
        node.children.push_back(processGroup(subGroupElem, Transform())); // começa de novo (não herda transformações acumuladas)
    }

    return node;
}



void lerXML(const string& fich) {
    tinyxml2::XMLDocument doc;
    if (doc.LoadFile(fich.c_str()) != tinyxml2::XML_SUCCESS) {
        cout << "Erro ao carregar o XML: " << fich << endl;
        return;
    }

    tinyxml2::XMLElement* root = doc.FirstChildElement("world");
    if (!root) {
        cout << "Erro: Elemento <world> não encontrado no XML." << endl;
        return;
    }

    cout << "=== Lendo XML ===" << endl;

    tinyxml2::XMLElement* cameraElem = root->FirstChildElement("camera");
    if (cameraElem) {
        cout << "Camera:" << endl;

        tinyxml2::XMLElement* positionElem = cameraElem->FirstChildElement("position");
        if (positionElem) {
            positionElem->QueryFloatAttribute("x", &camx);
            positionElem->QueryFloatAttribute("y", &camy);
            positionElem->QueryFloatAttribute("z", &camz);
            cout << "  Position: x=" << camx << ", y=" << camy << ", z=" << camz << endl;
        }

        tinyxml2::XMLElement* lookAtElem = cameraElem->FirstChildElement("lookAt");
        if (lookAtElem) {
            lookAtElem->QueryFloatAttribute("x", &lookAtx);
            lookAtElem->QueryFloatAttribute("y", &lookAty);
            lookAtElem->QueryFloatAttribute("z", &lookAtz);
            cout << "  LookAt: x=" << lookAtx << ", y=" << lookAty << ", z=" << lookAtz << endl;
        }

        tinyxml2::XMLElement* upElem = cameraElem->FirstChildElement("up");
        if (upElem) {
            upElem->QueryFloatAttribute("x", &upx);
            upElem->QueryFloatAttribute("y", &upy);
            upElem->QueryFloatAttribute("z", &upz);
            cout << "  Up: x=" << upx << ", y=" << upy << ", z=" << upz << endl;
        }

        tinyxml2::XMLElement* projectionElem = cameraElem->FirstChildElement("projection");
        if (projectionElem) {
            projectionElem->QueryFloatAttribute("fov", &fov);
            projectionElem->QueryFloatAttribute("near", &nearVal);
            projectionElem->QueryFloatAttribute("far", &farVal);
            cout << "  Projection: FOV=" << fov << ", Near=" << nearVal << ", Far=" << farVal << endl;
        }
    }

    tinyxml2::XMLElement* groupElem = root->FirstChildElement("group");
    if (groupElem) {
        Transform initialTransform;
        rootGroup = processGroup(groupElem, initialTransform);

    } else {
        cout << "Aviso: Nenhum grupo encontrado no XML." << endl;
    }

    cout << "=== Fim da Leitura ===" << endl;
}




int main(int argc, char** argv) {
    if (argc < 2) {
        cerr << "Erro: XML não fornecido\n";
        return 1;
    }
    lerXML(argv[1]);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA);
    glutInitWindowSize(800, 800);
    glutCreateWindow("Projeto_CG - VBO");

    initVBOsRecursive(rootGroup);

    
    glutDisplayFunc(renderScene);
    glutIdleFunc(renderScene);
    glutReshapeFunc(changeSize);
    glutKeyboardFunc(keyboardFunc);
    glutSpecialFunc(keyboardSpecial);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    glutMainLoop();
    return 0;
}

