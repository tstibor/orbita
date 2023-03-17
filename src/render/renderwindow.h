#ifndef RENDERWINDOW_H
#define RENDERWINDOW_H

#include <QOpenGLWidget>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>
#include <QPainter>
#include <QtMath>
#include <GL/glu.h>
#include "solarsystem.h"
#include "common.h"

enum RENDER_OPTIONS : quint16 {
    RENDER_ORBIT = 0x1,
    RENDER_NAME  = 0x2,
    RENDER_DATE  = 0x4,
    RENDER_MAG   = 0x8,
    RENDER_DIST  = 0x10,
};

class RenderWindow : public QOpenGLWidget
{
    Q_OBJECT

public:
    RenderWindow(QWidget *parent = nullptr, SolarSystem *solarSystem = nullptr);
    ~RenderWindow();
    int xRot() const;
    int yRot() const;
    int zRot() const;

public slots:
    void setXRotation(int angle);
    void setYRotation(int angle);
    void setZRotation(int angle);
    void setXZRotation(int angleX, int angleZ);
    void setXYTranslation(float x, float y);
    void setZTranslation(float z);
    void resetView();
    void topView();
    void switchProjection();
    void setRadiusFactorPlanetsSun(int value);
    void setRadiusSamePlanetsSun(int value);
    void setRadiusFactorAsteroids(int value);
    void renderSettingAsteroid(quint16 fields);
    void renderSettingComet(quint16 fields);

signals:
    void xRotationChanged(int angle);
    void yRotationChanged(int angle);
    void zRotationChanged(int angle);
    void xzRotationChanged(int angleX, int angleZ);
    void xyTranslationChanged();
    void zTranslationChanged();
    void projectionChanged(bool isOrthographic);
    void topViewChanged();
    void resetViewChanged();
    void radiusFactorPlanetsSunChanged();
    void radiusFactorAsteroidsChanged();
    void renderSettingAsteroidChanged();
    void renderSettingCometChanged();

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int width, int height) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    int m_xRot;
    int m_yRot;
    int m_zRot;
    float m_xTrans;
    float m_yTrans;
    float m_zTrans;
    float m_radiusFactorPlanetsSun;
    float m_radiusFactorAsteroid;
    float m_zFar;
    float m_zNear;
    QPoint m_lastPos;
    bool m_orthographic;
    bool m_sameRadiusPlanetsSun;
    quint16 m_renderSettingAsteroid;
    quint16 m_renderSettingComet;

    SolarSystem &m_SolarSystem;

    void connectActions();
    void renderSun();
    void renderPlanets();
    void renderAsteroids();
    void renderComets();
    void renderPlanetsOrbit();
    void renderAsteroidsOrbit();
    void renderCometsOrbit();

    void doProjection(int w, int h);
    void renderCoordSystem();
    void renderSphere(const struct ln_rect_posn position, GLfloat radius,
		      const QColor &col, const QString &text);
    /* Taken from: https://stackoverflow.com/questions/28216001/how-to-render-text-with-qopenglwidget */
    void renderText(GLfloat x, GLfloat y, GLfloat z, const QString &text);
    inline void transformPoint(GLdouble out[4], const GLdouble m[16], const GLdouble in[4]);
    inline GLint project(GLdouble objx, GLdouble objy, GLdouble objz,
			 const GLdouble model[16], const GLdouble proj[16], const GLint viewport[4],
			 GLdouble *winx, GLdouble *winy, GLdouble *winz);
};

#endif // RENDERWINDOW_H
