#include "websearchview.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    WebSearchView w;
    w.show();

    return a.exec();
}
