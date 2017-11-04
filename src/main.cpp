
#include <QApplication>
#include "gui/bsynergywidget.h"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    BSynergyWidget wid;
    wid.show();

    return app.exec();
}
