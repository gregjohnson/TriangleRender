#include <iostream>
#include <fstream>
#include <vector>
#include <float.h>

#ifdef __APPLE__
    #include <OpenGL/gl.h>
    #include <GLUT/glut.h>
#else
    #include <GL/gl.h>
    #include <GL/glut.h>
#endif

#define MAX_DISPLAYLIST_SIZE 999999

bool useDisplayLists = false;
float zoom = 1.;

std::vector<char *> datafiles;
std::vector<GLuint> displayLists;

struct Triangles
{
    int count; // vertex / normal vector count
    float * vertices;
    float * normals;
};

std::vector<Triangles> triangles;

// bounds of the entire geometry
float minX = FLT_MAX;
float minY = FLT_MAX;
float minZ = FLT_MAX;
float maxX = -FLT_MAX;
float maxY = -FLT_MAX;
float maxZ = -FLT_MAX;


void syntax(char * app);
void loadTriangles(char * filename);
void display();


int main(int argc, char **argv)
{
    // read command-line arguments
    for(int i=1; i<argc; i++)
    {
        if(argv[i][0] == '-')
        {
            switch(argv[i][1])
            {
                case 'l':
                    useDisplayLists = true;
                    break;
                case 'z':
                    if(i+1 < argc)
                    {
                        zoom = atof(argv[i+1]);
                        i++;
                    }

                    break;
                default:
                    syntax(argv[0]);
            }
        }
        else
        {
            datafiles.push_back(argv[i]);
        }
    }

    // setup GLUT / OpenGL
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(1280, 1024);

    glutCreateWindow("TriangleRender");

    // the display function will be called continuously
    glutDisplayFunc(display);
    glutIdleFunc(display);

    glClearColor(0.5, 0.5, 0.5, 1.);

    glEnable(GL_NORMALIZE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, true);

    // load triangle data files (and optionally create display lists)
    for(std::vector<char *>::iterator it=datafiles.begin(); it!=datafiles.end(); it++)
    {
        loadTriangles(*it);
    }

    // print total number of triangles
    int totalTriangles = 0;

    for(std::vector<Triangles>::iterator it=triangles.begin(); it!=triangles.end(); it++)
    {
        totalTriangles += (*it).count / 3;
    }

    std::cout << "total number of triangles: " << totalTriangles << std::endl;

    // enter the main loop
    glutMainLoop();

    return 0;
}

void syntax(char * app)
{
    std::cerr << "syntax: " << app << " [options] [geometry filenames...]" << std::endl;
    std::cerr << "options:" << std::endl;
    std::cerr << " -l   use display lists (default false)" << std::endl;
    std::cerr << " -z   zoom factor (default 1, must be >= 1)" << std::endl;

    exit(1);
}

void loadTriangles(char * filename)
{
    Triangles t;

    std::fstream inf(filename, std::ios::in | std::ios::binary);

    if(inf.fail())
    {
        std::cerr << "unable to open " << filename << std::endl;
        exit(1);
    }

    // vertex count
    inf.read((char *)&t.count, sizeof(t.count));

    std::cout << "reading " << t.count / 3 << " triangles..." << std::endl;

    // allocate memory for triangle vertices and normals
    // if we're using display lists we don't need to store these
    if(!useDisplayLists)
    {
        t.vertices = new float[3 * t.count];
        t.normals = new float[3 * t.count];
    }

    // save to global vector
    triangles.push_back(t);

    // allocate vertex and normal buffers and read from file
    float * vertexBuf = new float[4 * t.count];
    float * normalBuf = new float[4 * t.count];

    inf.read((char *)vertexBuf, 4 * t.count * sizeof(float));
    inf.read((char *)normalBuf, 4 * t.count * sizeof(float));

    inf.close();

    float * iv = vertexBuf, * ov = t.vertices;
    float * in = normalBuf, * on = t.normals;

    GLuint displayListId;

    if(useDisplayLists)
    {
        displayListId = glGenLists(1);
        glNewList(displayListId, GL_COMPILE);
        glBegin(GL_TRIANGLES);
    }

    for(int i=0; i<t.count; i++, iv+=4, in+=4)
    {
        if(minX > iv[0]) minX = iv[0];
        if(minY > iv[1]) minY = iv[1];
        if(minZ > iv[2]) minZ = iv[2];
        if(maxX < iv[0]) maxX = iv[0];
        if(maxY < iv[1]) maxY = iv[1];
        if(maxZ < iv[2]) maxZ = iv[2];

        if(useDisplayLists)
        {
            // start a new display list if necessary
            if(i % MAX_DISPLAYLIST_SIZE == 0)
            {
                glEnd();
                glEndList();
                displayLists.push_back(displayListId);

                displayListId = glGenLists(1);
                glNewList(displayListId, GL_COMPILE);
                glBegin(GL_TRIANGLES);
            }

            glNormal3f(in[0], in[1], in[2]);
            glVertex3f(iv[0], iv[1], iv[2]);
        }
        else
        {
            *ov++ = iv[0];
            *ov++ = iv[1];
            *ov++ = iv[2];

            *on++ = in[0];
            *on++ = in[1];
            *on++ = in[2];
        }
    }

    if(useDisplayLists)
    {
        glEnd();
        glEndList();
        displayLists.push_back(displayListId);
    }

    // free memory from file buffers
    delete [] vertexBuf;
    delete [] normalBuf;
}

void display()
{
    // angle of camera rotation
    static float angle = 0;

    // clear color / depth buffers and setup view
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glScalef(zoom, zoom, zoom);

    float size = 1. * (maxX - minX);
    glOrtho(-size, size, -size, size, -size, size);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glRotatef(angle, 0.0, 1.0, 1.0);

    glTranslatef(-(maxX + minX) / 2, -(maxY + minY) / 2, -(maxZ + minZ) / 2);

    // render using display lists or immediate mode
    if(useDisplayLists)
    {
        for(std::vector<GLuint>::iterator it=displayLists.begin(); it!=displayLists.end(); it++)
        {
            glCallList(*it);
        }
    }
    else
    {
        for(std::vector<Triangles>::iterator it=triangles.begin(); it!=triangles.end(); it++)
        {
            Triangles t = *it;

            glBegin(GL_TRIANGLES);

            float * v = t.vertices;
            float * n = t.normals;

            for(int i=0; i<t.count; i++, v+=3, n+=3)
            {
                glNormal3f(n[0], n[1], n[2]);
                glVertex3f(v[0], v[1], v[2]);
            }

            glEnd();
        }
    }

    glutSwapBuffers();

    // increment rotation angle
    angle += 1.;
}
