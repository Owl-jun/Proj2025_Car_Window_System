#include <QApplication>
#include <QPushButton>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QPushButton btn("Hello from Qt6!");
    btn.resize(300, 100);

    
    btn.show();
    return app.exec();
}
