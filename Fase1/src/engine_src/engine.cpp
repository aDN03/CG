#define NOCRT_STDIO_INLINE
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <list>
//#include <GL/glew.h>
#include <GLUT/glut.h>
#undef _NO_CRT_STDIO_INLINE
#include "../tinyXML/tinyxml2.hpp"



using namespace std;

float alpha = 0.0f;  // Angulo de rotação horizontal (em torno do eixo Y)
float beta = 0.0f;   // Angulo de rotação vertical (em torno do eixo X)
float radius = 10.0f;  // Distância da câmera ao objeto

float camx = 5.0f;
float camy = 5.0f;
float camz = 5.0f;
float lookAtx = 0.0f;
float lookAty = 0.0f;
float lookAtz = 0.0f;
float posx = 0, posz = 0, angle = 0, scalex = 1, scaley = 1, scalez = 1, upx = 0, upy = 1, upz = 0;
float fov = 0, near = 0, far = 0;
struct Point {
    float x, y, z;
};

vector<Point> vertexes;



void spherical2Cartesian() {
    // Convertendo coordenadas esféricas para cartesianas
    camx = radius * cos(beta) * sin(alpha);
    camy = radius * sin(beta);
    camz = radius * cos(beta) * cos(alpha);
}

// Função corrigida
void readFile(string fich) {
    std::cout << "\n" << fich << "\n";
    string linha;
    string novo;
    string delimiter = ",";
    int pos;
    float xx, yy, zz;
    int totalPontos = 0;

    ifstream file(fich);

    if (file.is_open()) {

        if (getline(file, linha)) {
            totalPontos = stoi(linha);
            std::cout << "Total de pontos: " << totalPontos << std::endl;
        }

        while (getline(file, linha)) {
            Point p;
            pos = linha.find(delimiter);
            novo = linha.substr(0, pos);
            xx = stof(novo);
            linha.erase(0, pos + delimiter.length());
            p.x = xx;

            pos = linha.find(delimiter);
            novo = linha.substr(0, pos);
            yy = stof(novo);
            linha.erase(0, pos + delimiter.length());
            p.y = yy;

            pos = linha.find(delimiter);
            novo = linha.substr(0, pos);
            zz = stof(novo);
            linha.erase(0, pos + delimiter.length());
            p.z = zz;

            vertexes.push_back(p);
        }

        file.close();

        if ((int)vertexes.size() != totalPontos) {
            std::cout << "⚠️ AVISO: Número de pontos lidos (" << vertexes.size() 
                      << ") diferente do esperado (" << totalPontos << ")." << std::endl;
        }
    }
    else {
        cout << "ERRO AO LER FICHEIRO" << endl;
    }
}


// Função corrigida
void changeSize(int w, int h) {
    if (h == 0)
        h = 1;
    float ratio = w * 1.0 / h;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, w, h);
    gluPerspective(45.0f, ratio, 1.0f, 1000.0f);
    glMatrixMode(GL_MODELVIEW);
}

//Função corrigida
void draw_axis() {
    glBegin(GL_LINES);

    // X axis in red
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(-100.0f, 0.0f, 0.0f);
    glVertex3f(100.0f, 0.0f, 0.0f);

    // Y axis in green
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, -100.0f, 0.0f);
    glVertex3f(0.0f, 100.0f, 0.0f);

    // Z axis in blue
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, -100.0f);
    glVertex3f(0.0f, 0.0f, 100.0f);
    glEnd();
}

//Função corrigida
void renderScene(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glLoadIdentity();

    gluLookAt(camx, camy, camz,
              lookAtx, lookAty, lookAtz,
              upx, upy, upz);

    draw_axis();
    
    glTranslatef(posx, 0.0, posz);
    glRotatef(angle, 0.0, 1.0, 0.0);
    glScalef(scalex, scaley, scalez);

    // Renderizar as figuras
    if (!vertexes.empty() && vertexes.size() % 3 == 0) {  // Evitar problemas de memória
        glBegin(GL_TRIANGLES);
        glColor3f(1.0, 1.0, 1.0);

        for (size_t i = 0; i < vertexes.size(); i += 3) {
            glVertex3f(vertexes[i].x, vertexes[i].y, vertexes[i].z);
            glVertex3f(vertexes[i + 1].x, vertexes[i + 1].y, vertexes[i + 1].z);
            glVertex3f(vertexes[i + 2].x, vertexes[i + 2].y, vertexes[i + 2].z);
        }

        glEnd();
    }

    glutSwapBuffers();
}


void keyboardspecial(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_UP:   // Aumenta o ângulo beta (rotação vertical para cima)
        beta += 0.1f;
        if (beta > 1.5f) beta = 1.5f; // Limita o ângulo para não ultrapassar 90 graus
        break;
    case GLUT_KEY_DOWN: // Diminui o ângulo beta (rotação vertical para baixo)
        beta -= 0.1f;
        if (beta < -1.5f) beta = -1.5f; // Limita o ângulo para não ultrapassar -90 graus
        break;
    case GLUT_KEY_LEFT: // Aumenta o ângulo alpha (rotação horizontal para a esquerda)
        alpha -= 0.1f;
        break;
    case GLUT_KEY_RIGHT: // Diminui o ângulo alpha (rotação horizontal para a direita)
        alpha += 0.1f;
        break;
    }

    // Atualiza a posição da câmera com base nos novos ângulos
    spherical2Cartesian();

    // Redesenha a cena
    glutPostRedisplay();
}

void keyboardFunc(unsigned char key, int x, int y) {
    // Outras teclas para controlar o comportamento, se necessário
    if (key == '-') {
        radius += 0.1f; // Aumenta a distância da câmera ao objeto
        if (radius > 20.0f) radius = 20.0f; // Limita a distância máxima
    }
    if (key == '+') {
        radius -= 0.1f; // Diminui a distância da câmera ao objeto
        if (radius < 5.0f) radius = 5.0f; // Limita a distância mínima
    }

    // Atualiza a posição da câmera com base no novo raio
    spherical2Cartesian();

    // Redesenha a cena
    glutPostRedisplay();
}



void lerXML(const string& fich) {
    tinyxml2::XMLDocument doc;
    tinyxml2::XMLElement* root;

    if (doc.LoadFile(fich.c_str()) == tinyxml2::XML_SUCCESS) {
        root = doc.FirstChildElement("world");
        if (root) {
            // Process window element
            tinyxml2::XMLElement* windowElem = root->FirstChildElement("window");
            if (windowElem) {
                int width, height;
                windowElem->QueryIntAttribute("width", &width);
                windowElem->QueryIntAttribute("height", &height);
                cout << "Window Width: " << width << ", Height: " << height << endl;
            }

            // Process camera element
            tinyxml2::XMLElement* cameraElem = root->FirstChildElement("camera");
            if (cameraElem) {
            
                tinyxml2::XMLElement* positionElem = cameraElem->FirstChildElement("position");
                if (positionElem) {
                positionElem->QueryFloatAttribute("x", &camx);
                positionElem->QueryFloatAttribute("y", &camy);
                positionElem->QueryFloatAttribute("z", &camz);
                cout << "Camera Position: x=" << camx << ", y=" << camy << ", z=" << camz << endl;
                }

                tinyxml2::XMLElement* lookAtElem = cameraElem->FirstChildElement("lookAt");
                if (lookAtElem) {
                lookAtElem->QueryFloatAttribute("x", &lookAtx);
                lookAtElem->QueryFloatAttribute("y", &lookAty);
                lookAtElem->QueryFloatAttribute("z", &lookAtz);
                cout << "Camera LookAt: x=" << lookAtx << ", y=" << lookAty << ", z=" << lookAtz << endl;
                }

                tinyxml2::XMLElement* upElem = cameraElem->FirstChildElement("up");
                if (upElem) {
                upElem->QueryFloatAttribute("x", &upx);
                upElem->QueryFloatAttribute("y", &upy);
                upElem->QueryFloatAttribute("z", &upz);
                cout << "Camera Up: x=" << upx << ", y=" << upy << ", z=" << upz << endl;
                }

                tinyxml2::XMLElement* projectionElem = cameraElem->FirstChildElement("projection");
                if (projectionElem) {
                projectionElem->QueryFloatAttribute("fov", &fov);
                projectionElem->QueryFloatAttribute("near", &near);
                projectionElem->QueryFloatAttribute("far", &far);
                cout << "Camera Projection: FOV=" << fov << ", Near=" << near << ", Far=" << far << endl;
                }
            }

            // Process group element
            tinyxml2::XMLElement* groupElem = root->FirstChildElement("group");
            if (groupElem) {
                tinyxml2::XMLElement* modelsElem = groupElem->FirstChildElement("models");
                if (modelsElem) {
                    for (tinyxml2::XMLElement* modelElem = modelsElem->FirstChildElement("model"); modelElem; modelElem = modelElem->NextSiblingElement("model")) {
                        const char* fileAttr = modelElem->Attribute("file");
                        if (fileAttr) {
                            string ficheiro(fileAttr);
                            cout << "Model File: " << ficheiro << " read successfully" << endl;
                            readFile(ficheiro);
                        }
                    }
                }
            }
        }
    } else {
        cout << "XML file not found" << endl;
    }
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