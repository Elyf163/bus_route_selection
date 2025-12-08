#include "bus_route_selection.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    bus_route_selection window;
    window.show();
    return app.exec();
}
