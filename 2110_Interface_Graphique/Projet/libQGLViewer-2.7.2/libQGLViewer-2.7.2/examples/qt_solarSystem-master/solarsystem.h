#ifndef SOLARSYSTEM_H
#define SOLARSYSTEM_H

#include <QGLViewer/qglviewer.h>
#include "ishapefactory.h"
#include "iorbit.h"

#ifdef __gnu_linux__
    #include <GL/freeglut.h>
    #include <GL/gl.h>
#else
    #include <OpenGL/gl.h>
    #include <GLUT/glut.h>
#endif
class SolarSystem : public QGLViewer {
    Q_OBJECT
    public:
        explicit SolarSystem(QWidget *parent, IShapeFactory *shapeFactory);
        ~SolarSystem();

        void init();
        void draw();

    signals:
    
    public slots:

    private:
        void setRadiusAndCenter();

        TimeCounter *_counter;

        IShapeFactory *_shapeFactory;
        IOrbit *_sun;
};

#endif // SOLARSYSTEM_H
