#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QQmlComponent>
#include <QQmlEngine>
#include <QQuickItem>
#include <QQmlContext>

int main(int argc, char *argv[])
{

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    engine.load(QUrl(QStringLiteral("qrc:///src/main.qml")));
    QObject *root = engine.rootObjects().first();
    QObject *temp = root->findChild<QObject *>("ZoneEngine");

    for (int var = 0; var < temp->children().size(); ++var) {
        qDebug() << "childOFZoneEngine" << temp->children().at(var)->metaObject()->className();
    }

    QQuickWindow *window = qobject_cast<QQuickWindow *>(root);
    if (!window) {
        qFatal("Error: Your root item has to be a window.");
        return -1;
    }
    qDebug() << "Number children of window" << root->children().size();
    qDebug() << "Number children of zoneEngine" << temp->children().size();

    for (int var = 0; var < root->children().size(); ++var) {
        qDebug() << root->children().at(var)->metaObject()->className();
    }
    return app.exec();
}
