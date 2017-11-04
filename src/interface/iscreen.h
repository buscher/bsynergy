
#ifndef ISCREEN_H
#define ISCREEN_H

#include <QObject>
#include <QRect>
#include <QString>

#include "base/bscreenconfigglobal.h"

//TODO fix the ISreen mess to allow new connect syntax
class IScreen : public QObject
{
public:
    virtual ~IScreen() {}

    virtual bool open(QString *errorMessage = nullptr) = 0;
    virtual bool close() = 0;

    virtual QRect screenSize() = 0;

    virtual bool grabControls(BScreenConfigGlobal::ScreenLayout) = 0;
    virtual bool ungrabControls(BScreenConfigGlobal::ScreenLayout) = 0;

    virtual void setHostScreenSize(int width, int height) = 0;

    virtual bool host() = 0;

public slots:
    virtual void fakeMouseMove(int x, int y) = 0;
    virtual void fakeMousePressed(int button) = 0;
    virtual void fakeMouseReleased(int button) = 0;

    virtual void fakeKeyPressed(int keyCode, int modifiers = 0) = 0;
    virtual void fakeKeyReleased(int keyCode, int modifiers = 0) = 0;

signals:
    virtual void mouseMoved(int x, int y) = 0;
    virtual void mousePressed(int button) = 0;
    virtual void mouseReleased(int button) = 0;

    virtual void keyPressed(int keyCode, int modifiers = 0) = 0;
    virtual void keyReleased(int keyCode, int modifiers = 0) = 0;
};

Q_DECLARE_INTERFACE(IScreen, "IScreen")

#endif //ISCREEN_H
