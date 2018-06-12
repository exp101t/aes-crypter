#ifndef FILELISTWINDOW_H
#define FILELISTWINDOW_H

#include <QVBoxLayout>
#include <QTextEdit>
#include <QWidget>
#include <QLabel>

class FileListWindow : public QWidget{
    Q_OBJECT
    public:
        explicit FileListWindow(QWidget *parent = 0);
        void generateWindow();
        QLabel* label; QTextEdit* fileListEdit;
        QVBoxLayout* mainLayout;

    signals:

    public slots:
};

#endif // FILELISTWINDOW_H
