#include "mainwindow.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTcpSocket *socket = new QTcpSocket();
    // 读取数据库，尝试连接服务器（静默）
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "main_conn");
    db.setDatabaseName("user.db");
    QString ip;
    quint16 port = 0;
    if (db.open()) {
        QSqlQuery query(db);
        query.exec("SELECT ip, port FROM setting ORDER BY id DESC LIMIT 1");
        if (query.next()) {
            ip = query.value(0).toString();
            port = query.value(1).toUInt();
        }
    }
    if (!ip.isEmpty() && port > 0) {
        socket->connectToHost(ip, port);
    }
    MainWindow w;
    w.setSocket(socket); // 让主窗口持有socket
    w.show();
    return a.exec();
}
