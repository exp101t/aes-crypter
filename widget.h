#ifndef WIDGET_H
#define WIDGET_H

#include <openssl/aes.h>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QCheckBox>
#include <QWidget>
#include <QLabel>

class Widget : public QWidget{
    Q_OBJECT

    public slots:
        void fChooseFiles();
        void processFiles();
        void showFileList();
        void fChooseFolders();

    private:
        QPushButton* chooseFiles; QPushButton* encryptButton; QPushButton* chooseFolders;
        QPushButton* decryptButton; QPushButton* fileListButton;
        QCheckBox* deleteAfter; QCheckBox* recursive;
        QHBoxLayout* row[5]; QVBoxLayout* mainLayout; QVBoxLayout* checkBoxList;
        QLabel* fileCount; QLabel* enterPassLabel; QLabel* confirmPassLabel; QLabel* warning;
        QLineEdit* enterPass; QLineEdit* confirmPass;
        QStringList fileList; QString folder;

    public:
        Widget(QWidget *parent = 0); ~Widget();

        void generateWindow();
        void encryptFolder(bool, QString);
        void encrypt(bool, QStringList, QString);

        unsigned char indata[AES_BLOCK_SIZE], outdata[AES_BLOCK_SIZE], ckey[16], ivec[16];
        AES_KEY key;
};

#endif // WIDGET_H
