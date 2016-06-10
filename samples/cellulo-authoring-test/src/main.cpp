#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QQmlComponent>
#include <QQmlEngine>
#include <QQuickItem>
#include <QQmlContext>

int main(int argc, char *argv[])
{

    //JSON
    //http://doc.qt.io/qt-5/qtcore-json-savegame-example.html
    //http://stackoverflow.com/questions/15893040/how-to-create-read-write-json-files-in-qt5
    //http://stackoverflow.com/questions/19822211/qt-parsing-json-using-qjsondocument-qjsonobject-qjsonarray
    //https://forum.qt.io/topic/60694/save-load-json-using-qml/3
    //Is that possible to Save/Load JSON data with only QML ??? Yes but only from network as QML doesnot have direct access to local file system.
    //https://qmlbook.github.io/en/ch16/index.html -> alternative JSON.parse (seems older way -> post forum 5 years ago)

    //MOUSE AREA
    //http://doc.qt.io/qt-5/qml-qtquick-mousearea.html

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    engine.load(QUrl(QStringLiteral("qrc:///src/main.qml")));
    QObject *root = engine.rootObjects().first();

    return app.exec();
}
