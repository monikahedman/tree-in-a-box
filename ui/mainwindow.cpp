#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Databinding.h"
#include "SupportCanvas3D.h"
#include "CS123XmlSceneParser.h"
#include "Settings.h"
//#include "camera/CamtransCamera.h"
#include "CS123XmlSceneParser.h"
#include <math.h>
#include <QFileDialog>
#include <QMessageBox>
#include "glm/common.hpp"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_oldLength(settings.branchLength),
    m_oldRadius(settings.branchRadius),
    m_oldLightCount(settings.lightCount),
    m_oldLightRand(settings.randomEnabled),
    m_oldAngle(settings.branchAngle),
    m_oldLeafSize(settings.leafSize),
    m_oldLeafWidth(settings.leafWidth)

{
    // Make sure the settings are loaded before the UI
    settings.loadSettingsOrDefaults();

    QGLFormat qglFormat;
    qglFormat.setVersion(4, 3);
    qglFormat.setProfile(QGLFormat::CoreProfile);
    qglFormat.setSampleBuffers(true);
    ui->setupUi(this);
    QGridLayout *gridLayout = new QGridLayout(ui->canvas3D);
    m_canvas3D = new SupportCanvas3D(qglFormat, this);
    gridLayout->addWidget(m_canvas3D, 0, 1);
    ui->tabWidget->setCurrentWidget(ui->tab3D);

    // Restore the UI settings
    QSettings qtSettings("CS123", "CS123");
    restoreGeometry(qtSettings.value("geometry").toByteArray());
    restoreState(qtSettings.value("windowState").toByteArray());

    // Resize the window because the window is huge since all docks were visible.
    resize(1000, 600);

    // Bind the UI elements to their properties in the global Settings object, using binding
    // objects to simplify the code.  Each binding object connects to its UI elements and keeps
    // the UI and its setting in sync.

    QList<QAction*> actions;

#define SETUP_ACTION(dock, key) \
    actions.push_back(ui->dock->toggleViewAction()); \
    actions.back()->setShortcut(QKeySequence(key));
#undef SETUP_ACTION

    dataBind();

    QWidget *widget = ui->tabWidget->currentWidget();
    ui->tabWidget->setCurrentWidget(ui->tab3D);
    show();
    ui->tabWidget->setCurrentWidget(widget);
    show();

}

MainWindow::~MainWindow()
{
    foreach (DataBinding *b, m_bindings)
        delete b;
    foreach (QButtonGroup *bg, m_buttonGroups)
        delete bg;
    delete ui;
}

void MainWindow::dataBind() {
    // Brush dock
#define BIND(b) { \
    DataBinding *_b = (b); \
    m_bindings.push_back(_b); \
    assert(connect(_b, SIGNAL(dataChanged()), this, SLOT(settingsChanged()))); \
}
    QButtonGroup *shapesButtonGroup = new QButtonGroup;
    m_buttonGroups.push_back(shapesButtonGroup);

    // Final dock
    BIND(FloatBinding::bindSliderAndTextbox(
       ui->branchLengthSlider, ui->branchLengthTextbox, settings.branchLength, 0.1f, 1.f))
            BIND(FloatBinding::bindSliderAndTextbox(
                     ui->branchRadiusSlider, ui->branchRadiusTextbox, settings.branchRadius, 0.1f, 1.f))
            BIND(BoolBinding::bindCheckbox(ui->raindropsOnWindowCheckbox, settings.raindropsOnWindowEnabled))

    BIND(FloatBinding::bindSliderAndTextbox(
        ui->branchRadiusSlider, ui->branchRadiusTextbox, settings.branchRadius, 0.5f, 1.f))
    BIND(FloatBinding::bindSliderAndTextbox(
        ui->branchAngleSlider, ui->branchAngleTextbox, settings.branchAngle, 0.01f, 0.5f))
    BIND(FloatBinding::bindSliderAndTextbox(
        ui->leafSizeSlider, ui->leafSizeTextbox, settings.leafSize, 0.1f, 2.f))
    BIND(FloatBinding::bindSliderAndTextbox(
        ui->leafWidthSlider, ui->leafWidthTextbox, settings.leafWidth, 0.05f, 0.4f))
    BIND(BoolBinding::bindCheckbox(ui->raindropsOnWindowCheckbox, settings.raindropsOnWindowEnabled))
            BIND(BoolBinding::bindCheckbox(ui->randomEnabled, settings.randomEnabled))
            BIND(IntBinding::bindSliderAndTextbox(
                     ui->lightCountSlider, ui->lightCountTextbox, settings.lightCount, 0, 100))
            BIND(FloatBinding::bindSliderAndTextbox(
                     ui->lightRadiusSlider, ui->lightRadiusTextbox, settings.lightRadius, 0.f, 16.f))
            BIND(IntBinding::bindSliderAndTextbox(
                     ui->lightColorRSlider, ui->lightColorRTextbox, settings.lightColorRed, 0, 255))
            BIND(IntBinding::bindSliderAndTextbox(
                     ui->lightColorGSlider, ui->lightColorGTextbox, settings.lightColorGreen, 0, 255))
            BIND(IntBinding::bindSliderAndTextbox(
                     ui->lightColorBSlider, ui->lightColorBTextbox, settings.lightColorBlue, 0, 255))
            BIND(FloatBinding::bindSliderAndTextbox(
                     ui->lightBrightnessSlider, ui->lightBrightnessTextbox, settings.lightBrightness, 0, 1))
            BIND(IntBinding::bindSliderAndTextbox(ui->rainSlider, ui->rainTextbox, settings.rainParticleCount, 1.0, 5000.0))
            BIND(BoolBinding::bindCheckbox(ui->rainCheckbox, settings.rainEnabled))
        #undef BIND

            // make sure the aspect ratio updates when m_canvas3D changes size
            connect(m_canvas3D, SIGNAL(aspectRatioChanged()), this, SLOT(updateAspectRatio()));
    updateRainUiEnabled();
}

void MainWindow::changeEvent(QEvent *e) {
    QMainWindow::changeEvent(e); // allow the superclass to handle this for the most part...

    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWindow::closeEvent(QCloseEvent *event) {
    // Save the settings before we quit
    settings.saveSettings();
    QSettings qtSettings("CS123", "CS123");
    qtSettings.setValue("geometry", saveGeometry());
    qtSettings.setValue("windowState", saveState());

    QMainWindow::closeEvent(event);
}

void MainWindow::updateAspectRatio() {
    // Update the aspect ratio text so the aspect ratio will be correct even if the
    // 3D canvas isn't visible (the 3D canvas isn't resized when it isn't visible)
    QSize activeTabSize = ui->tabWidget->currentWidget()->size();
    float aspectRatio = static_cast<float>(activeTabSize.width()) / static_cast<float>(activeTabSize.height());
}

void MainWindow::updateRainUiEnabled() {
    ui->rainSlider->setEnabled(settings.rainEnabled);
    ui->rainTextbox->setEnabled(settings.rainEnabled);
    ui->raindropsOnWindowCheckbox->setEnabled(settings.rainEnabled);
}


void MainWindow::settingsChanged() {
    updateRainUiEnabled();
    constexpr float EPSILON = 0.001f;
    if (glm::abs(m_oldLength - settings.branchLength) > EPSILON
            || glm::abs(m_oldRadius - settings.branchRadius) > EPSILON
            || glm::abs(m_oldAngle - settings.branchAngle) > EPSILON
            || glm::abs(m_oldLeafSize - settings.leafSize) > EPSILON
            || glm::abs(m_oldLeafWidth - settings.leafWidth) > EPSILON) {
        m_canvas3D->settingsChanged(SupportCanvas3D::UPDATE_TREE);
    } else if (glm::abs(m_oldLightCount - settings.lightCount) > EPSILON) {
        m_canvas3D->settingsChanged(SupportCanvas3D::UPDATE_LIGHTCOUNT);
    }  else if (!m_oldLightRand & settings.randomEnabled
                || m_oldLightRand & !settings.randomEnabled) {
        m_canvas3D->settingsChanged(SupportCanvas3D::UPDATE_LIGHTRAND);
    } else {
        m_canvas3D->settingsChanged();
    }
    m_oldLength = settings.branchLength;
    m_oldRadius = settings.branchRadius;

    m_oldLightCount = settings.lightCount;
    m_oldLightRand = settings.randomEnabled;

    m_oldAngle = settings.branchAngle;
    m_oldLeafSize = settings.leafSize;
    m_oldLeafWidth = settings.leafWidth;

}

void MainWindow::renderImage() {
    // Make sure OpenGL gets a chance to update the OrbitCamera, which can only be done when
    // that tab is active (because it needs the OpenGL context for its matrix transforms)
    m_canvas3D->update();
    QApplication::processEvents();

    OpenGLScene *glScene = m_canvas3D->getScene();
    if (glScene) {
    }
}
