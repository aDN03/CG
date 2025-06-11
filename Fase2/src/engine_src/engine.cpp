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



using namespace std;

float camx = 5.0f, camy = 5.0f, camz = 5.0f;
float lookAtx = 0.0f, lookAty = 0.0f, lookAtz = 0.0f;
float upx = 0.0f, upy = 1.0f, upz = 0.0f;

float posx = 0.0f, posz = 0.0f, angle = 0.0f, scalex = 1.0f, scaley = 1.0f, scalez = 1.0f;
float fov = 0.0f, nearVal = 0.0f, farVal = 0.0f;

struct Point {
    float x, y, z;
};

vector<Point> vertexes;


struct Transformation {
    int type;  // 0 = translate, 1 = rotate, 2 = scale
    float values[4];  // Guarda os parâmetros necessários
};


struct Transform {
    vector<Transformation> transformations;  // Armazena transformações ordenadas
    float transX, transY, transZ;
    float rotateAngle, rotateX, rotateY, rotateZ;
    float scaleX, scaleY, scaleZ;
    vector<Point> vertexes;

    // Construtor padrão
    Transform()
        : transX(0.0f), transY(0.0f), transZ(0.0f),
          rotateAngle(0.0f), rotateX(0.0f), rotateY(0.0f), rotateZ(0.0f),
          scaleX(1.0f), scaleY(1.0f), scaleZ(1.0f),
          vertexes() {}
};

std::vector<Transform> transformations;





void readFile(const string& fich, Transform& transform) {
    ifstream file(fich);
    if (!file.is_open()) {
        cout << "Erro ao abrir o arquivo: " << fich << endl;
        return;
    }

    string linha;
    while (getline(file, linha)) {
        Point p;
        if (sscanf(linha.c_str(), "%f,%f,%f", &p.x, &p.y, &p.z) == 3) {
            transform.vertexes.push_back(p);  // Adiciona os vértices à estrutura de transformação
        }
    }
    file.close();

    cout << "Total de vértices carregados para o modelo " << fich << ": " << transform.vertexes.size() << endl;
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

void renderGroup(const Transform& t, int modelIndex) {
    glPushMatrix();  // Salva a matriz de transformação atual

    // Aplicar as transformações da matriz de transformação
    for (const auto& trans : t.transformations) {
        switch (trans.type) {
        case 0:  // Translação
            glTranslatef(trans.values[0], trans.values[1], trans.values[2]);
            break;
        case 1:  // Rotação
            glRotatef(trans.values[0], trans.values[1], trans.values[2], trans.values[3]);
            break;
        case 2:  // Escala
            glScalef(trans.values[0], trans.values[1], trans.values[2]);
            break;
        }
    }

    // Paleta de cores para modelos diferentes
    float colors[][3] = {
        {1.0, 0.0, 0.0}, // Vermelho
        {0.0, 1.0, 0.0}, // Verde
        {0.0, 0.0, 1.0}, // Azul
        {1.0, 1.0, 0.0}, // Amarelo
        {1.0, 0.0, 1.0}, // Magenta
        {0.0, 1.0, 1.0}  // Ciano
    };

    // Escolher uma cor com base no índice do modelo
    int colorIndex = modelIndex % (sizeof(colors) / sizeof(colors[0]));
    

    // Desenha os modelos associados a essa transformação
        glBegin(GL_TRIANGLES);
        glColor3f(colors[colorIndex][0], colors[colorIndex][1], colors[colorIndex][2]);
         
        for (size_t i = 0; i < t.vertexes.size(); i += 3) {
            glVertex3f(t.vertexes[i].x, t.vertexes[i].y, t.vertexes[i].z);
            glVertex3f(t.vertexes[i + 1].x, t.vertexes[i + 1].y, t.vertexes[i + 1].z);
            glVertex3f(t.vertexes[i + 2].x, t.vertexes[i + 2].y, t.vertexes[i + 2].z);
        }
        glEnd();


    glPopMatrix();  // Restaura a matriz de transformação anterior
}




void renderScene(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glLoadIdentity();  // Reseta a matriz de transformação

    gluLookAt(camx, camy, camz, lookAtx, lookAty, lookAtz, upx, upy, upz);
    draw_axis();

    // Itera sobre todas as transformações e renderiza cada grupo corretamente
    for (size_t i = 0; i < transformations.size(); ++i) {
        renderGroup(transformations[i], i);
    }

    glutSwapBuffers();
}





void keyboardspecial(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_UP:
        camy += 1;
        break;
    case GLUT_KEY_DOWN:
        camy -= 1;
        break;
    case GLUT_KEY_LEFT:
        camx -= 1;
        break;
    case GLUT_KEY_RIGHT:
        camx += 1;
        break;
    }
    glutPostRedisplay();
}

// write function to process keyboard events
void keyboardFunc(unsigned char key, int x, int y) {
    switch (key) {
    case 'a':
        posx -= 0.1;
        break;
    case 'd':
        posx += 0.1;
        break;
    case 's':
        posz += 0.1;
        break;
    case 'w':
        posz -= 0.1;
        break;
    case 'q':
        angle -= 15;
        break;
    case 'e':
        angle += 15;
        break;
    case 'i':
        scalez += 0.1;
        break;
    case 'k':
        scalez -= 0.1;
        break;
    case 'j':
        scalex -= 0.1;
        break;
    case 'l':
        scalex += 0.1;
        break;
    case 'u':
        scaley -= 0.1;
        break;
    case 'o':
        scaley += 0.1;
        break;
    case '+':
        scalex += 0.1;
        scaley += 0.1;
        scalez += 0.1;
        break;
    case '-':
        scalex -= 0.1;
        scaley -= 0.1;
        scalez -= 0.1;
        break;
    }
    glutPostRedisplay();
}


void processGroup(tinyxml2::XMLElement* groupElem, const Transform& parentTransform) {
    if (!groupElem) return;

    // Criar um novo Transform que herda as transformações do pai
    Transform currentTransform = parentTransform;

    tinyxml2::XMLElement* transformElem = groupElem->FirstChildElement("transform");

    if (transformElem) {
        for (tinyxml2::XMLElement* elem = transformElem->FirstChildElement(); elem; elem = elem->NextSiblingElement()) {
            std::string name = elem->Value();
            Transformation t;
            t.values[0] = t.values[1] = t.values[2] = t.values[3] = 0.0f;  // Inicializa os valores

            if (name == "translate") {
                t.type = 0;
                elem->QueryFloatAttribute("x", &t.values[0]);
                elem->QueryFloatAttribute("y", &t.values[1]);
                elem->QueryFloatAttribute("z", &t.values[2]);

                currentTransform.transX += t.values[0];
                currentTransform.transY += t.values[1];
                currentTransform.transZ += t.values[2];
            } 
            else if (name == "rotate") {
                t.type = 1;
                elem->QueryFloatAttribute("angle", &t.values[0]);
                elem->QueryFloatAttribute("x", &t.values[1]);
                elem->QueryFloatAttribute("y", &t.values[2]);
                elem->QueryFloatAttribute("z", &t.values[3]);

                currentTransform.rotateAngle = t.values[0];
                currentTransform.rotateX = t.values[1];
                currentTransform.rotateY = t.values[2];
                currentTransform.rotateZ = t.values[3];
            } 
            else if (name == "scale") {
                t.type = 2;
                elem->QueryFloatAttribute("x", &t.values[0]);
                elem->QueryFloatAttribute("y", &t.values[1]);
                elem->QueryFloatAttribute("z", &t.values[2]);

                currentTransform.scaleX *= t.values[0];
                currentTransform.scaleY *= t.values[1];
                currentTransform.scaleZ *= t.values[2];
            }

            // Adiciona ao vetor mantendo a ordem original do XML
            currentTransform.transformations.push_back(t);
        }
    }

    // Criar um Transform separado para armazenar apenas os modelos desse grupo
    Transform modelTransform = currentTransform;

    // Processar modelos (arquivos) dentro do grupo (apenas para este grupo)
    tinyxml2::XMLElement* modelsElem = groupElem->FirstChildElement("models");
    if (modelsElem) {
        for (tinyxml2::XMLElement* modelElem = modelsElem->FirstChildElement("model"); modelElem; modelElem = modelElem->NextSiblingElement("model")) {
            const char* fileAttr = modelElem->Attribute("file");
            if (fileAttr) {
                std::cout << "Carregando modelo: " << fileAttr << std::endl;
                readFile(fileAttr, modelTransform);  // Só adiciona ao transform de modelos
            }
        }
    }

    // Armazenar apenas transformações com modelos na lista
    if (!modelTransform.vertexes.empty()) {
        transformations.push_back(modelTransform);
    }

    // Processar subgrupos recursivamente (sem copiar modelos do pai)
    for (tinyxml2::XMLElement* subGroupElem = groupElem->FirstChildElement("group"); subGroupElem; subGroupElem = subGroupElem->NextSiblingElement("group")) {
        processGroup(subGroupElem, currentTransform);  // Passa apenas as transformações para os filhos
    }
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
        processGroup(groupElem, initialTransform);
    } else {
        cout << "Aviso: Nenhum grupo encontrado no XML." << endl;
    }

    cout << "=== Fim da Leitura ===" << endl;
}



// Função corrigida
int main(int argc, char** argv) {
    if (argc > 1) {
        lerXML(argv[1]);
    } else {
        std::cerr << "Erro: Arquivo XML não fornecido!" << std::endl;
    }

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(800, 800);
    glutCreateWindow("Projeto_CG");

    glEnable(GL_CULL_FACE);  // Habilita o Culling
    glCullFace(GL_BACK);     // Descartar faces traseiras
    glFrontFace(GL_CCW);     // Definir sentido anti-horário como frente


    glutDisplayFunc(renderScene); // Desenha a cena
    glutReshapeFunc(changeSize);  // Alteração do tamanho da janela
    glutIdleFunc(renderScene);    // Atualiza a cena continuamente
    glutSpecialFunc(keyboardspecial); // Teclas especiais (seta)
    glutKeyboardFunc(keyboardFunc);   // Teclas normais (a, d, s, w, etc.)

    glEnable(GL_DEPTH_TEST);
    // glEnable(GL_CULL_FACE);

    glutMainLoop();

    return 0;
}