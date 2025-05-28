#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QTranslator>
#include <QLibraryInfo>
#include <QLocalServer>
#include <QLocalSocket>
#include <QMessageBox>
#include <QBuffer>
#include <QFile>
#include "application.h"
#ifdef _WIN32
#include <QSharedMemory>
#include <windows.h>
//#include <QWindowsXPStyle.h>
#endif

#include <regex>

#ifdef QT_DEBUG
#define LOCALSOCKETNAME "kaihitsu_devel"
#else
#define LOCALSOCKETNAME "kaihitsu"
#endif


#include "mainwindow.h"


int main(int argc, char *argv[])
{
    QCoreApplication::addLibraryPath(QCoreApplication::applicationDirPath());
    Application app(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "kaihitsu_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            app.installTranslator(&translator);
            break;
        }
    }

    //他のプロセスが起動していてかつ引数があればそのプロセスで起動する
    //さもなくばローカルサーバーを起動
    if(app.registerLocalServer())
        return app.exec();
    return 0;
}



Application::Application(int &argc, char *argv[])
    :QApplication(argc,argv)
{

}

Application::~Application()
{
    localsv.close();
    sharedsvlist.detach();
}

bool Application::registerLocalServer()
{
    const int shmemsize = 1024;
    QBuffer shbuf;
    QStringList args;
    //args.append("kaihitsu.exe");
    //args.append("d:/cl.txt");
    args = arguments();

    //ローカルサーバー一覧
    //一覧は'\n'で区切られ、1行ごとサーバー名、空行で終端
    sharedsvlist.setKey(LOCALSOCKETNAME);
    QStringList svlist;
    QString newsvname;
    if(!sharedsvlist.create(shmemsize)){
        //他のプロセスがクラッシュしていた時のために一度attach,detachする
        //detachは共有メモリを使用しているプロセスが0になると解放してくれる
        sharedsvlist.attach();
        sharedsvlist.detach();
        //再度createが失敗したら本当に共有メモリがある
        sharedsvlist.create(shmemsize);
    }
    //createが成功していたときはすでにattachされているので共有メモリを初期化する
    if(sharedsvlist.isAttached()){
        sharedsvlist.lock();
        memcpy(sharedsvlist.data(), "\n", 2);
        sharedsvlist.unlock();
    }
    else{
        sharedsvlist.attach();
    }


    sharedsvlist.lock();
    shbuf.setData((const char*)sharedsvlist.constData(), shmemsize);
    sharedsvlist.unlock();
    shbuf.open(QBuffer::ReadOnly);
    //共有メモリからローカルサーバー一覧を読み出す
    while(true){
        QString svname = QString(shbuf.readLine()).replace('\n',"");
        if(svname=="")//空行で終わり
            break;
        else
            svlist.append(svname);
    }
    shbuf.close();
    //重複しない名前を付ける
    for(int i=0;i<99;i++){
        newsvname = QString("%1%2").arg(LOCALSOCKETNAME).arg(i);
        int j;
        for(j=0; j<svlist.size(); j++){
            if(newsvname==svlist.at(j))
                break;
        }
        if(j==svlist.size())
            break;
    }
    //コマンド引数あり、かつ起動中のプロセスがあれば、そのプロセスに引数を送り
    //このプロセスはローカルサーバーを起動せずプロセスを終了する
    if(args.length()>=2 && svlist.size()>0){
        QString params;
        QByteArray data;
        QLocalSocket localsoc;
        //接続失敗のサーバーは除外されるのでループは常に0番要素にアクセスする
        while(svlist.size()>0){
            localsoc.connectToServer(svlist.first());
            if(localsoc.error()!=QLocalSocket::ServerNotFoundError){
                for(int i=1; i<args.length(); i++){
                    params.append(args.at(i));
                    params.append("\n");
                }
                params.append("\n");  //空行を終端とする
                data = params.toUtf8();
                localsoc.waitForConnected(5000);
                localsoc.write(data);
                localsoc.flush();
                localsoc.waitForBytesWritten(5000);
                localsoc.waitForDisconnected(5000);
                return false;
            }
            else{
                svlist.removeFirst();
            }
        }
    }
    //他プロセスの接続が失敗した場合、または引数なしの場合は単独起動する
    //ローカルサーバー登録、起動
    svlist.append(newsvname);
    if(!localsv.listen(newsvname)){
        //共有メモリで起動中プロセスのチェックをしているにもかかわらず
        //listenが失敗するのは、前回のプロセスが異常終了した可能性がある
        //そのためローカルソケットの削除を試みる
        //UNIX系OSでは/tmp/nameがソケットのパス
        QString socpath = QString("/tmp/%1").arg(newsvname);
        QFile socfile(socpath);
        socfile.remove();
        localsv.listen(newsvname);
    }
    if(localsv.isListening()){
        connect(&localsv, SIGNAL(newConnection()), SLOT(onLocalServerNewConnection()));
        QString joinedsvlist = svlist.join("\n");
        joinedsvlist.append("\n");
        sharedsvlist.lock();
        memcpy(sharedsvlist.data(), joinedsvlist.toLatin1().constData(), joinedsvlist.size());
        sharedsvlist.unlock();
    }
    return true;
}

int Application::exec()
{
    //メインウィンドウ
    mainwindow = new MainWindow();
    mainwindow->show();
    //setActiveWindow(mainwindow);

    return QApplication::exec();
}


void Application::onLocalServerNewConnection()
{
    QLocalSocket *soc;
    QByteArray data;
    QString line;
    soc = localsv.nextPendingConnection();
    if(!soc->waitForReadyRead(5000))
        return;
    while(true){
        data = soc->readLine();
        line = QString::fromUtf8(data).replace('\n',"");
        if(line.length()==0)
            break;
        //QMessageBox::warning(mainwindow,"",line);
        mainwindow->openFile(line);
    }
    soc->close();

    setActiveWindow(mainwindow);
    mainwindow->raise();
    mainwindow->activateWindow();

#ifdef _WIN32
    /*
     * SetForegroundWindow()はNT系ではタスクバーのタスクが点滅するのみ、9x系ではトップに来る
     * フォアグラウンドにするには現在のアクティブなウィンドウのスレッドから実行する必要がある
    */
    HWND hWnd = (HWND)mainwindow->winId();
    int targetThread, selfThread;

    // Threadのインプットのアタッチ
    targetThread = GetWindowThreadProcessId(GetForegroundWindow(), NULL);
    selfThread = GetCurrentThreadId();
    AttachThreadInput(selfThread, targetThread, TRUE );

    // ウィンドウをフォアグラウンドに持ってくる
    SetForegroundWindow(hWnd);

    // Threadを切り離す
    AttachThreadInput(selfThread, targetThread, FALSE );

    //Zオーダーの最前面に持ってくる(vista以降で必要)
    BringWindowToTop(hWnd);
#endif
    mainwindow->editorview->setFocus();



}

