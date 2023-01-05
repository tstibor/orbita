#include "renderwindow.h"

constexpr GLdouble lightGrayColor  = 220 / 255.0;
constexpr GLdouble darkGrayColor   = 150 / 255.0;
constexpr GLdouble grayColor       = 185 / 255.0;
constexpr GLdouble lightBlackColor = 105 / 255.0;

RenderWindow::RenderWindow(QWidget *parent, SolarSystem *solarSystem)
    : QOpenGLWidget(parent), m_SolarSystem(*solarSystem)
{
    m_SolarSystem.computePlanetsPosition();
    m_SolarSystem.computePlanetsOrbit();

    connectActions();

    resetView();
}

RenderWindow::~RenderWindow() { }

void RenderWindow::connectActions()
{
    connect(&m_SolarSystem, &SolarSystem::positionsChanged, this, [&]() { update(); });
    connect(&m_SolarSystem, &SolarSystem::asteroidsPositionChanged, this, [&]() { update(); });
    connect(&m_SolarSystem, &SolarSystem::cometsPositionChanged, this, [&]() { update(); });

    connect(this, &RenderWindow::radiusFactorPlanetsSunChanged, this, [&]() { update(); });
    connect(this, &RenderWindow::radiusFactorAsteroidsChanged, this, [&]() { update(); });
    connect(this, &RenderWindow::renderSettingAsteroidChanged, this, [&]() { update(); });
    connect(this, &RenderWindow::renderSettingCometChanged, this, [&]() { update(); });

    connect(this, &RenderWindow::xzRotationChanged, this, [&]() { update(); });
    connect(this, &RenderWindow::xRotationChanged, this, [&]() { update(); });
    connect(this, &RenderWindow::yRotationChanged, this, [&]() { update(); });
    connect(this, &RenderWindow::zRotationChanged, this, [&]() { update(); });
    connect(this, &RenderWindow::xyTranslationChanged, this, [&]() { update(); });
    connect(this, &RenderWindow::zTranslationChanged, this, [&]() { update(); });
    connect(this, &RenderWindow::topViewChanged, this, [&]() { update(); });
    connect(this, &RenderWindow::resetViewChanged, this, [&]() { update(); });
    connect(this, &RenderWindow::projectionChanged, this, [&]() { update(); });
}

int RenderWindow::xRot() const
{
    return m_xRot;
}

int RenderWindow::yRot() const
{
    return m_yRot;
}

int RenderWindow::zRot() const
{
    return m_zRot;
}

void RenderWindow::resetView()
{
    m_xRot = m_yRot = m_zRot = 0;
    m_xTrans = m_yTrans = .0;
    m_zTrans = -7.5;
    m_zFar = 5000.0;
    m_zNear = -500.0;
    m_radiusFactorPlanetsSun = 50 * 40.0;
    m_radiusFactorAsteroid = 50 * 75000.0;
    m_orthographic = true;
    m_sameRadiusPlanetsSun = true;
    m_renderSettingAsteroid = 0;
    m_renderSettingComet = 0;

    emit resetViewChanged();
    emit projectionChanged(m_orthographic);
}

void RenderWindow::topView()
{
    m_xRot = m_yRot = m_zRot = 0;

    emit topViewChanged();
}

void RenderWindow::switchProjection()
{
    m_orthographic = !m_orthographic;

    emit projectionChanged(m_orthographic);
}

void RenderWindow::setRadiusFactorPlanetsSun(int value)
{
    m_radiusFactorPlanetsSun = value * 40.0;

    emit radiusFactorPlanetsSunChanged();
}

void RenderWindow::setRadiusSamePlanetsSun(int value)
{
    m_sameRadiusPlanetsSun = value;

    emit radiusFactorPlanetsSunChanged();
}

void RenderWindow::setRadiusFactorAsteroids(int value)
{
    m_radiusFactorAsteroid = value * 75000.0;

    emit radiusFactorAsteroidsChanged();
}

void RenderWindow::renderSettingAsteroid(quint16 fields)
{
    m_renderSettingAsteroid = fields;

    emit renderSettingAsteroidChanged();
}

void RenderWindow::renderSettingComet(quint16 fields)
{
    m_renderSettingComet = fields;

    emit renderSettingCometChanged();
}

static void qNormalizeAngle(int &angle)
{
    while (angle < 0)
        angle += 360 * 16;
    while (angle > 360 * 16)
        angle -= 360 * 16;
}

void RenderWindow::setXRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != m_xRot) {
        m_xRot = angle;
        emit xRotationChanged(angle);
    }
}

void RenderWindow::setYRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != m_yRot) {
        m_yRot = angle;
        emit yRotationChanged(angle);
    }
}

void RenderWindow::setZRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != m_zRot) {
        m_zRot = angle;
        emit zRotationChanged(angle);
    }
}

void RenderWindow::setXZRotation(int angleX, int angleZ)
{
    bool emitRequired = false;

    qNormalizeAngle(angleX);
    if (angleX != m_xRot) {
        m_xRot = angleX;
        emitRequired = true;
    }

    qNormalizeAngle(angleZ);
    if (angleZ != m_zRot) {
        m_zRot = angleZ;
        emitRequired = true;
    }

    if (emitRequired)
        emit xzRotationChanged(angleX, angleZ);
}

void RenderWindow::setXYTranslation(float x, float y)
{
    float aspect = width() / (float)height();
    float fovRad = 5.0 * M_PI / 180.0;
    float tanFov = tan(fovRad / 2.0);

    m_xTrans += x * 10.0 * tanFov * aspect;
    m_yTrans += y * 10.0 * tanFov;

    emit xyTranslationChanged();
}

void RenderWindow::setZTranslation(float z)
{
    z > 0 ? m_zTrans -= m_zTrans * 0.1f : m_zTrans += m_zTrans * 0.1f;

    emit zTranslationChanged();
}

void RenderWindow::resizeGL(int w, int h)
{
    doProjection(w, h);
}

void RenderWindow::mousePressEvent(QMouseEvent *event)
{
    m_lastPos = event->pos();
}

void RenderWindow::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->position().x() - m_lastPos.x();
    int dy = event->position().y() - m_lastPos.y();

    if (event->buttons() & Qt::LeftButton) {
        setXYTranslation((dx * m_zTrans) / (float)width(), (dy * m_zTrans) / (float)height());
    } else if (event->buttons() & Qt::RightButton) {
        setXZRotation(m_xRot + 8 * dy, m_zRot + 8 * dx);
    } else if (event->buttons() & Qt::MiddleButton) {
        setYRotation(m_yRot + 8 * dx);
    }

    m_lastPos = event->pos();
}

void RenderWindow::wheelEvent(QWheelEvent *event)
{
    setZTranslation(event->angleDelta().y());
}

void RenderWindow::initializeGL()
{
    glClearColor(1, 1, 1, 1);
}

void RenderWindow::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    doProjection(width(), height());

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(m_xTrans, -m_yTrans, -m_zTrans, /* eye */
	      m_xTrans, -m_yTrans, 0,	      /* center */
	      0, 1, 0);			      /* up */
    glRotatef(m_xRot / 16.0, 1, 0, 0);
    glRotatef(m_yRot / 16.0, 0, 1, 0);
    glRotatef(m_zRot / 16.0, 0, 0, 1);

    renderCoordSystem();
    renderSun();
    renderPlanets();
    renderPlanetsOrbit();
    renderAsteroids();
    renderAsteroidsOrbit();
    renderComets();
    renderCometsOrbit();
}

void RenderWindow::doProjection(int w, int h)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    const float m_zoom = qAbs(m_zTrans) / 4.5;
    const float aspectRatio = GLfloat(w) / h;

    if (m_orthographic) {
        if (w <= h)
            glOrtho(m_zoom * -1.0, m_zoom * 1.0, m_zoom * -1.0 / aspectRatio,
                    m_zoom * 1.0 / aspectRatio, m_zNear, m_zFar);
        else
            glOrtho(m_zoom * -1.0 * aspectRatio, m_zoom * 1.0 * aspectRatio, m_zoom * -1.0,
                    m_zoom * 1.0, m_zNear, m_zFar);
    } else
        gluPerspective(25, aspectRatio, 0, m_zFar * m_zoom);
}

void RenderWindow::renderCoordSystem()
{
    glColor3d(lightGrayColor, lightGrayColor, lightGrayColor);

    /* X-Y-Z coordinate system. */
    glBegin(GL_LINES);

    glVertex3d(0.0, 0.0, 0.0);
    glVertex3d(1.0, 0.0, 0.0);
    glVertex3d(0.0, 0.0, 0.0);
    glVertex3d(-1.0, 0.0, 0.0);

    glVertex3d(0.0, 0.0, 0.0);
    glVertex3d(0.0, 1.0, 0.0);
    glVertex3d(0.0, 0.0, 0.0);
    glVertex3d(0.0, -1.0, 0.0);

    glVertex3d(0.0, 0.0, 0.0);
    glVertex3d(0.0, 0.0, 1.0);
    glVertex3d(0.0, 0.0, 0.0);
    glVertex3d(0.0, 0.0, -1.0);

    glEnd();
}

void RenderWindow::renderSphere(const struct ln_rect_posn position, GLfloat radius,
                                const QColor &col, const QString &text = nullptr)
{
    GLUquadricObj *quadricObj = gluNewQuadric();
    const GLfloat x = position.X;
    const GLfloat y = position.Y;
    const GLfloat z = position.Z;

    glPushMatrix();
    glTranslatef(x, y, z);
    glColor3d(col.red() / 255.0, col.green() / 255.0, col.blue() / 255.0);
    gluSphere(quadricObj, radius, 20, 20);
    glPopMatrix();

    if (text != nullptr)
        renderText(x, y, z, text);

    gluDeleteQuadric(quadricObj);
}

void RenderWindow::renderPlanets()
{
    for (const auto &planet : m_SolarSystem.planets()) {
        renderSphere(planet.position(),
		     m_sameRadiusPlanetsSun ?  m_radiusFactorPlanetsSun * 4.25875e-05 : m_radiusFactorPlanetsSun * planet.radiusAU(),
		     planet.color(),
                     planet.name());
    }
}

void RenderWindow::renderSun()
{
    constexpr double radiusMeter = 696342000;
    constexpr double radiusAU = radiusMeter / ONE_AU_IN_METER;
    const double radiusScaledAU = m_radiusFactorPlanetsSun * radiusAU * 1e-2; // Sun is still too large, scale it more down.
    constexpr QColor color(255, 204, 51);

    renderSphere((struct ln_rect_posn) { .X = 0, .Y = 0, .Z = 0 },
		 m_sameRadiusPlanetsSun ?  m_radiusFactorPlanetsSun * 4.25875e-05 : radiusScaledAU, color, "sun");
}

void RenderWindow::renderAsteroids()
{
    constexpr double radiusMeter = 1000; // Assume average asteroid has radius of 1000 meters.
    constexpr double radiusAU = radiusMeter / ONE_AU_IN_METER;
    const double radiusScaledAU = m_radiusFactorAsteroid * radiusAU;
    constexpr QColor color(160, 160, 160);

    for (const auto &asteroid : m_SolarSystem.asteroids()) {
        renderSphere(asteroid.position(),
		     radiusScaledAU,
		     color,
		     nullptr);

	QString renderString = "";

	if (m_renderSettingAsteroid & RENDER_SETTING_NAME)
	    renderString = asteroid.name();

	if (m_renderSettingAsteroid & RENDER_SETTING_DATE)
	    renderString += " " + JdToQStringDate(asteroid.JD());

	if (m_renderSettingAsteroid & RENDER_SETTING_MAG)
	    renderString += " " + QString::number(asteroid.magnitude(), 'f', 2);

	if (m_renderSettingAsteroid & RENDER_SETTING_DIST)
	    renderString += " " + QString::number(asteroid.distEarth(), 'f', 2) + " "
		+ QString::number(asteroid.distSun(), 'f', 2);

	renderText(asteroid.position().X, asteroid.position().Y, asteroid.position().Z, renderString);
    }
}

void RenderWindow::renderComets()
{
    constexpr double radiusMeter = 2000; // Assume average comet has radius of 2000 meters.
    constexpr double radiusAU = radiusMeter / ONE_AU_IN_METER;
    const double radiusScaledAU = m_radiusFactorAsteroid * radiusAU;
    constexpr QColor color(153, 204, 255);

    for (const auto &comet : m_SolarSystem.comets()) {
        renderSphere(comet.position(),
		     radiusScaledAU,
		     color,
		     nullptr);

	QString renderString = "";

	if (m_renderSettingComet & RENDER_SETTING_NAME)
	    renderString = comet.name();

	if (m_renderSettingComet & RENDER_SETTING_DATE)
	    renderString += " " + JdToQStringDate(comet.JD());

	if (m_renderSettingComet & RENDER_SETTING_MAG)
	    renderString += " " + QString::number(comet.magnitude(), 'f', 2);

	if (m_renderSettingComet & RENDER_SETTING_DIST)
	    renderString += " " + QString::number(comet.distEarth(), 'f', 2) + " "
		+ QString::number(comet.distSun(), 'f', 2);

	renderText(comet.position().X, comet.position().Y, comet.position().Z, renderString);
    }
}

void RenderWindow::renderPlanetsOrbit()
{
    glColor3d(lightBlackColor, lightBlackColor, lightBlackColor);
    glLineWidth(1.25);

    for (const auto &planet : m_SolarSystem.planets()) {
        glBegin(GL_LINE_LOOP);
        for (const auto &orbit : planet.orbit())
            glVertex3f(orbit.X, orbit.Y, orbit.Z);
        glEnd();
    }
}

void RenderWindow::renderAsteroidsOrbit()
{
    if (!(m_renderSettingAsteroid & RENDER_SETTING_ORBIT))
	return;

    glColor3d(grayColor, grayColor, grayColor);
    glLineWidth(1.25);

    for (const auto &asteroid : m_SolarSystem.asteroids()) {
        glBegin(GL_LINE_LOOP);
        for (const auto &orbit : asteroid.orbit())
            glVertex3f(orbit.X, orbit.Y, orbit.Z);
        glEnd();
    }
}

void RenderWindow::renderCometsOrbit()
{
    if (!(m_renderSettingComet & RENDER_SETTING_ORBIT))
	return;

    glColor3d(grayColor, grayColor, grayColor);
    glLineWidth(1.25);

    for (const auto &comet : m_SolarSystem.comets()) {
        glBegin(GL_LINE_STRIP);
        for (const auto &orbit : comet.orbit())
            glVertex3f(orbit.X, orbit.Y, orbit.Z);
        glEnd();
    }
}

void RenderWindow::renderText(GLfloat x, GLfloat y, GLfloat z, const QString &text)
{
    GLdouble model[4][4], proj[4][4];
    GLint view[4];
    GLdouble textPosX = 0, textPosY = 0, textPosZ = 0;

    glGetDoublev(GL_MODELVIEW_MATRIX, &model[0][0]);
    glGetDoublev(GL_PROJECTION_MATRIX, &proj[0][0]);
    glGetIntegerv(GL_VIEWPORT, &view[0]);

    project(x, y, z, &model[0][0], &proj[0][0], &view[0], &textPosX, &textPosY, &textPosZ);
    textPosY = height() - textPosY;

    QPainter painter(this);
    painter.setPen(Qt::black);
    painter.setFont(QFont("Helvetica", 10));
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    painter.drawText(textPosX, textPosY, text);
    painter.end();
}

inline GLint RenderWindow::project(GLdouble objx, GLdouble objy, GLdouble objz,
                                   const GLdouble model[16], const GLdouble proj[16],
                                   const GLint viewport[4], GLdouble *winx, GLdouble *winy,
                                   GLdouble *winz)
{
    GLdouble in[4], out[4];

    in[0] = objx;
    in[1] = objy;
    in[2] = objz;
    in[3] = 1.0;
    transformPoint(out, model, in);
    transformPoint(in, proj, out);

    if (in[3] == 0.0)
        return GL_FALSE;

    in[0] /= in[3];
    in[1] /= in[3];
    in[2] /= in[3];

    *winx = viewport[0] + (1 + in[0]) * viewport[2] / 2;
    *winy = viewport[1] + (1 + in[1]) * viewport[3] / 2;
    *winz = (1 + in[2]) / 2;

    return GL_TRUE;
}

inline void RenderWindow::transformPoint(GLdouble out[4], const GLdouble m[16],
                                         const GLdouble in[4])
{
#define M(row, col) m[col * 4 + row]
    out[0] = M(0, 0) * in[0] + M(0, 1) * in[1] + M(0, 2) * in[2] + M(0, 3) * in[3];
    out[1] = M(1, 0) * in[0] + M(1, 1) * in[1] + M(1, 2) * in[2] + M(1, 3) * in[3];
    out[2] = M(2, 0) * in[0] + M(2, 1) * in[1] + M(2, 2) * in[2] + M(2, 3) * in[3];
    out[3] = M(3, 0) * in[0] + M(3, 1) * in[1] + M(3, 2) * in[2] + M(3, 3) * in[3];
#undef M
}
