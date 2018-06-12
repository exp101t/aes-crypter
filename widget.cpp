#include "widget.h"
#include "filelistwindow.h"

#include <openssl/aes.h>
#include <openssl/md5.h>

#include <QFileDialog>
#include <QMessageBox>
#include <QObject>
#include <QFile>
#include <QDir>

Widget::Widget(QWidget *parent) : QWidget(parent){
    generateWindow();
}

Widget::~Widget(){

}

void Widget::generateWindow(){
    chooseFiles    = new QPushButton("Choose files...");
    chooseFolders  = new QPushButton("Choose folder...");
    fileCount      = new QLabel("0 Files selected");
    fileListButton = new QPushButton("List of files");
    row[0] = new QHBoxLayout;
    row[0] -> addWidget(chooseFiles);
    row[0] -> addWidget(chooseFolders);
    row[0] -> addWidget(fileCount);
    row[0] -> addWidget(fileListButton);

    enterPassLabel = new QLabel("Enter password:    ");
    enterPass      = new QLineEdit;
    enterPass -> setEchoMode(QLineEdit::Password);
    row[1] = new QHBoxLayout;
    row[1] -> addWidget(enterPassLabel);
    row[1] -> addWidget(enterPass);

    confirmPassLabel = new QLabel("Confirm password:");
    confirmPass      = new QLineEdit;
    confirmPass -> setEchoMode(QLineEdit::Password);
    row[2] = new QHBoxLayout;
    row[2] -> addWidget(confirmPassLabel);
    row[2] -> addWidget(confirmPass);

    encryptButton = new QPushButton("Encrypt!");
    decryptButton = new QPushButton("Decrypt!");
    deleteAfter   = new QCheckBox("Delete after encrypting / decrypting");
    recursive     = new QCheckBox("Recursive encrypting / decrypting of folders");
    checkBoxList  = new QVBoxLayout;
    checkBoxList -> addWidget(deleteAfter);
    checkBoxList -> addWidget(recursive);
    row[3] = new QHBoxLayout;
    row[3] -> addWidget(encryptButton);
    row[3] -> addWidget(decryptButton);
    row[3] -> addLayout(checkBoxList);

    warning = new QLabel("<u><b>Warning!</b></u> Program doesn't check validity of password<br>so decrypting with incorrect key will corrupt your files!");
    row[4] = new QHBoxLayout;
    row[4] -> addWidget(warning);

    mainLayout = new QVBoxLayout;
    for(auto r: row) mainLayout -> addLayout(r);
    setLayout(mainLayout);

    connect(chooseFiles,    SIGNAL(clicked()), this, SLOT(fChooseFiles()));
    connect(encryptButton,  SIGNAL(clicked()), this, SLOT(processFiles()));
    connect(decryptButton,  SIGNAL(clicked()), this, SLOT(processFiles()));
    connect(fileListButton, SIGNAL(clicked()), this, SLOT(showFileList()));
    connect(chooseFolders,  SIGNAL(clicked()), this, SLOT(fChooseFolders()));
    setWindowTitle(tr("AES File Crypter by Ritter German (GitHub: exp101t)"));
}

void Widget::processFiles(){
    QString password = enterPass -> text();
    QString confirm  = confirmPass -> text();

    if(password == confirm && password != ""){
        if(!fileList.empty() || folder != ""){
            MD5((const unsigned char*) (password + "!").toLatin1().data(), password.length(), ckey);
            MD5((const unsigned char*) (password + "#").toLatin1().data(), password.length(), ivec);
            bool isEncrypt = (QObject::sender() -> property("text").toString()) == "Encrypt!";
            AES_set_encrypt_key(ckey, 128, &key);

            encryptButton -> setEnabled(false);
            decryptButton -> setEnabled(false);
            enterPass     -> setEnabled(false);
            confirmPass   -> setEnabled(false);

            if(!fileList.empty()) encrypt(isEncrypt, fileList, "");
            if(folder != "")      encryptFolder(isEncrypt, folder);

            encryptButton -> setEnabled(true);
            decryptButton -> setEnabled(true);
            enterPass     -> setEnabled(true);
            confirmPass   -> setEnabled(true);

            FileListWindow* flw = new FileListWindow; flw -> show();

            for(auto f: fileList)
                flw -> fileListEdit -> append((isEncrypt ? "[CRYPTED] " : "[DECRYPTED] ") + f);

            if(folder != "")
                flw -> fileListEdit -> append((isEncrypt ? "[CRYPTED] " : "[DECRYPTED] ") + folder);

            folder = ""; fileList.clear();
            fileCount -> setText(tr("0 Files selected"));
        }
    }else QMessageBox::warning(this, tr("Warning!"), tr("Passwords you entered are different or empty!"));
}

void Widget::encrypt(bool isEncrypt, QStringList files, QString folder){
    for(auto f: files){
        if(!isEncrypt && !f.endsWith(".crypted")) break;
        QFile ifp(folder + f), ofp(folder + (isEncrypt ? (f + ".crypted") : (f.remove(".crypted"))));
        ifp.open(QIODevice::ReadOnly); ofp.open(QIODevice::WriteOnly);

        int bytes_read, num = 0;

        while(true){
            QByteArray temp  = ifp.read(AES_BLOCK_SIZE);
            memcpy(indata, temp.data(), AES_BLOCK_SIZE);
            bytes_read = temp.length();

            AES_cfb128_encrypt(indata, outdata, bytes_read, &key, ivec, &num, isEncrypt ? AES_ENCRYPT : AES_DECRYPT);
            ofp.write(QByteArray::fromRawData((const char*) outdata, bytes_read));

            if (bytes_read < AES_BLOCK_SIZE) break;
        }

        if(deleteAfter -> isChecked()) ifp.remove();
    }
}

void Widget::fChooseFiles(){
    fileList = QFileDialog::getOpenFileNames(this, tr("Choose files to encrypt / decrypt"), NULL, tr("All (*.*);;Encrypted Files (*.crypted)"));
    fileCount -> setText(tr("%1 Files selected").arg(fileList.length() + (folder != "")));
}

void Widget::fChooseFolders(){
    folder = QFileDialog::getExistingDirectory(0, tr("Select a directory to encrypt / decrypt"));
    fileCount -> setText(tr("%1 Files selected").arg(fileList.length() + (folder != "")));
}

void Widget::encryptFolder(bool isEncrypt, QString toEncrypt){
    QDir handle = QDir(toEncrypt);
    QStringList dirs  = handle.entryList(QDir::NoDotAndDotDot | QDir::AllDirs);
    QStringList files = handle.entryList(QDir::Files);
    encrypt(isEncrypt, files, toEncrypt + "/");

    if(recursive -> isChecked())
        for(auto dir: dirs)
            encryptFolder(isEncrypt, toEncrypt + "/" + dir);
}

void Widget::showFileList(){
    FileListWindow* w = new FileListWindow; w -> show();
    for(auto f: fileList) w -> fileListEdit -> append(f);
    w -> fileListEdit -> append(folder);
}
