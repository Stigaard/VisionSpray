#include <QtGui/QApplication>
#include "VisionSpray.h"


int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    app.setAttribute(Qt::AA_X11InitThreads);
    app.setOrganizationName("University Of Southern Denmark");
    app.setOrganizationDomain("sdu.dk");
    app.setApplicationName("VisionSpray");
    VisionSpray foo;
    foo.show();
    return app.exec();
}
