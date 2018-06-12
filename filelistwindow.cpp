#include "filelistwindow.h"

FileListWindow::FileListWindow(QWidget *parent) : QWidget(parent){
    generateWindow();
}

void FileListWindow::generateWindow(){
    mainLayout   = new QVBoxLayout;
    label        = new QLabel("List of the files: ");
    fileListEdit = new QTextEdit();
    fileListEdit -> setReadOnly(true);

    mainLayout -> addWidget(label);
    mainLayout -> addWidget(fileListEdit);

    setLayout(mainLayout);
}
