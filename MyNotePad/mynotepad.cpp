#include "MyNotePad.h"
#include "ui_MyNotePad.h"
#include "finddialog.h"
#include "ui_finddialog.h"
#include <QApplication>
#include <QtGui>
#include <QtCore>
#include <QAction>
#include <QMenu>
#include <QUrl>
#include <QIcon>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFont>
#include <QFontDialog>
#include <QFontDatabase>
#include <QColor>
#include <QColorDialog>
#include <QTextCursor>
#include <QTextCharFormat>
#include <QSettings>
#include <QStringList>
#include <QMessageBox>
#include <QDateTime>
#include <QTextStream>

MyNotePad::MyNotePad(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MyNotePad)
{
    ui->setupUi(this);

    readSize();

    for(int i = 0; i<MaxRecentFiles; i++)
    {
        recentFiles[i] = new QAction(this);
        recentFiles[i]->setVisible(false);
        connect(recentFiles[i], SIGNAL(triggered()), this, SLOT(openRecentFile()));
        ui->menuOpen_Recent_Files->addAction(recentFiles[i]);
    }
    updateRecentFiles();

    this->setCentralWidget(ui->textEdit);
    setWindowTitle("MyNotePad");
    setWindowIcon(QIcon(":/ico/Icons/Logo.png"));
    typeSearch = NoSearch;
}



MyNotePad::~MyNotePad()
{
    delete ui;
}

void MyNotePad::on_actionNew_triggered()
{
    findText.clear();
    findRegExp = QRegExp();
    typeSearch = NoSearch;

    if(isSave())
    {
        ui->textEdit->clear();
        setRecentFile(QString());
    }
}

void MyNotePad::on_actionOpen_triggered()
{
    findText.clear();
    findRegExp = QRegExp();
    typeSearch = NoSearch;

    if(isSave())
    {
        QString fileName = QFileDialog::getOpenFileName(this, tr("MyNotePad - Open File"), "", ("Text Files (*.txt);;All Files (*.*)"));
        loadFile(fileName);
    }
}

bool MyNotePad::on_actionSave_triggered()
{
    if(filePath.isEmpty())
        return on_actionSave_As_triggered();
    else
        return saveFile(filePath);
}

bool MyNotePad::on_actionSave_As_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("MyNotePad - Save File"),"untitled.txt", tr("Text Files (*.txt);;All Files (*.*)"));
    if(!fileName.isEmpty())
    {
        filePath = fileName;
        statusBar()->showMessage(tr("Saved \"%1\"").arg(QDir::toNativeSeparators(fileName)), 2500);
        return saveFile(fileName);
    }
    return false;
}

void MyNotePad::on_actionExit_triggered()
{
    if(isSave())
    {
        close();
    }
}

void MyNotePad::on_actionCut_triggered()
{
    ui->textEdit->cut();
}

void MyNotePad::on_actionCopy_triggered()
{
    ui->textEdit->copy();
}

void MyNotePad::on_actionPaste_triggered()
{
    ui->textEdit->paste();
}

void MyNotePad::on_actionUndo_triggered()
{
    ui->textEdit->undo();
}

void MyNotePad::on_actionRedo_triggered()
{
    ui->textEdit->redo();
}

void MyNotePad::on_actionFind_triggered()
{
    w_FindDialog = new FindDialog(this);
    connect(w_FindDialog, SIGNAL(find(QRegExp,QTextDocument::FindFlags)), SLOT(find(QRegExp,QTextDocument::FindFlags)));
    connect(w_FindDialog, SIGNAL(find(QString,QTextDocument::FindFlags)), SLOT(find(QString, QTextDocument::FindFlags)));
    connect(w_FindDialog, SIGNAL(findNext(QTextDocument::FindFlags)), SLOT(findNext(QTextDocument::FindFlags)));
    w_FindDialog->show();
    ui->actionFind_Next->setEnabled(true);
}

void MyNotePad::on_actionFind_Next_triggered()
{
    findNext(w_FindDialog->getOptions());
}

void MyNotePad::on_actionZoom_In_triggered()
{
    ui->textEdit->zoomIn(2);
}

void MyNotePad::on_actionZoom_Out_triggered()
{
    ui->textEdit->zoomOut(2);
}

void MyNotePad::on_actionSelect_All_triggered()
{
    ui->textEdit->selectAll();
}

void MyNotePad::on_actionClear_All_triggered()
{
    if(isSave())
    {
        ui->textEdit->clear();
    }
}

void MyNotePad::on_actionBold_triggered()
{
    QTextCharFormat syn;
    syn.setFontWeight(ui->actionBold->isChecked() ? QFont::Bold : QFont::Normal);
    selectionSync(syn);
}

void MyNotePad::on_actionItalic_triggered()
{
    QTextCharFormat syn;
    syn.setFontItalic(ui->actionItalic->isChecked());
    selectionSync(syn);
}

void MyNotePad::on_actionUnderline_triggered()
{
    QTextCharFormat syn;
    syn.setFontUnderline(ui->actionUnderline->isChecked());
    selectionSync(syn);
}

void MyNotePad::on_actionFont_triggered()
{
    bool value;
    QFont fontStyle = QFontDialog::getFont(&value, this);
    if(value)
    {
        QTextCharFormat format;
        format.setFont(fontStyle);
        QTextCursor txtCursor(ui->textEdit->textCursor());
        txtCursor.setCharFormat(format);
        txtCursor.clearSelection();
        ui->textEdit->setTextCursor(txtCursor);
    }else
    {
        return;
    }
}

void MyNotePad::on_actionStrikethrough_triggered()
{
    QTextCharFormat syn;
    syn.setFontStrikeOut(ui->actionStrikethrough->isChecked());
    selectionSync(syn);
}

void MyNotePad::on_actionSubscript_triggered()
{
    if(ui->actionSubscript->isChecked())
    {
        QTextCharFormat syn;
        syn.setVerticalAlignment(QTextCharFormat::AlignSubScript);
        ui->textEdit->mergeCurrentCharFormat(syn);
    }else
    {
        QTextCharFormat syn;
        syn.setVerticalAlignment(QTextCharFormat::AlignNormal);
        ui->textEdit->mergeCurrentCharFormat(syn);
    }
}

void MyNotePad::on_actionSuperscript_triggered()
{
    if(ui->actionSuperscript->isChecked())
    {
        QTextCharFormat syn;
        syn.setVerticalAlignment(QTextCharFormat::AlignSuperScript);
        ui->textEdit->mergeCurrentCharFormat(syn);
    }else
    {
        QTextCharFormat syn;
        syn.setVerticalAlignment(QTextCharFormat::AlignNormal);
        ui->textEdit->mergeCurrentCharFormat(syn);
    }
}

void MyNotePad::on_actionDisc_triggered()
{
    QTextCursor cursor = ui->textEdit->textCursor();
    QTextListFormat::Style lStyle = QTextListFormat::ListDisc;

    cursor.beginEditBlock();

    QTextBlockFormat bFormat = cursor.blockFormat();
    QTextListFormat liFormat;

    if(cursor.currentList())
    {
        liFormat = cursor.currentList()->format();
    }
    else
    {
        liFormat.setIndent(bFormat.indent() + 1);
        bFormat.setIndent(0);
        cursor.setBlockFormat(bFormat);
    }

    liFormat.setStyle(lStyle);
    cursor.createList(liFormat);
    cursor.endEditBlock();
}

void MyNotePad::on_actionCircle_triggered()
{
    QTextCursor cursor = ui->textEdit->textCursor();
    QTextListFormat::Style lStyle = QTextListFormat::ListCircle;

    cursor.beginEditBlock();

    QTextBlockFormat bFormat = cursor.blockFormat();
    QTextListFormat liFormat;

    if(cursor.currentList())
    {
        liFormat = cursor.currentList()->format();
    }
    else
    {
        liFormat.setIndent(bFormat.indent() + 1);
        bFormat.setIndent(0);
        cursor.setBlockFormat(bFormat);
    }

    liFormat.setStyle(lStyle);
    cursor.createList(liFormat);
    cursor.endEditBlock();
}

void MyNotePad::on_actionSquare_triggered()
{
    QTextCursor cursor = ui->textEdit->textCursor();
    QTextListFormat::Style lStyle = QTextListFormat::ListSquare;

    cursor.beginEditBlock();

    QTextBlockFormat bFormat = cursor.blockFormat();
    QTextListFormat liFormat;

    if(cursor.currentList())
    {
        liFormat = cursor.currentList()->format();
    }
    else
    {
        liFormat.setIndent(bFormat.indent() + 1);
        bFormat.setIndent(0);
        cursor.setBlockFormat(bFormat);
    }

    liFormat.setStyle(lStyle);
    cursor.createList(liFormat);
    cursor.endEditBlock();
}

void MyNotePad::on_actionArabic_triggered()
{
    QTextCursor cursor = ui->textEdit->textCursor();
    QTextListFormat::Style lStyle = QTextListFormat::ListDecimal;

    cursor.beginEditBlock();

    QTextBlockFormat bFormat = cursor.blockFormat();
    QTextListFormat liFormat;

    if(cursor.currentList())
    {
        liFormat = cursor.currentList()->format();
    }
    else
    {
        liFormat.setIndent(bFormat.indent() + 1);
        bFormat.setIndent(0);
        cursor.setBlockFormat(bFormat);
    }

    liFormat.setStyle(lStyle);
    cursor.createList(liFormat);
    cursor.endEditBlock();
}

void MyNotePad::on_actionUppercase_Roman_triggered()
{
    QTextCursor cursor = ui->textEdit->textCursor();
    QTextListFormat::Style lStyle = QTextListFormat::ListUpperRoman;

    cursor.beginEditBlock();

    QTextBlockFormat bFormat = cursor.blockFormat();
    QTextListFormat liFormat;

    if(cursor.currentList())
    {
        liFormat = cursor.currentList()->format();
    }
    else
    {
        liFormat.setIndent(bFormat.indent() + 1);
        bFormat.setIndent(0);
        cursor.setBlockFormat(bFormat);
    }

    liFormat.setStyle(lStyle);
    cursor.createList(liFormat);
    cursor.endEditBlock();
}

void MyNotePad::on_actionLowercase_Roman_triggered()
{
    QTextCursor cursor = ui->textEdit->textCursor();
    QTextListFormat::Style lStyle = QTextListFormat::ListLowerRoman;

    cursor.beginEditBlock();

    QTextBlockFormat bFormat = cursor.blockFormat();
    QTextListFormat liFormat;

    if(cursor.currentList())
    {
        liFormat = cursor.currentList()->format();
    }
    else
    {
        liFormat.setIndent(bFormat.indent() + 1);
        bFormat.setIndent(0);
        cursor.setBlockFormat(bFormat);
    }

    liFormat.setStyle(lStyle);
    cursor.createList(liFormat);
    cursor.endEditBlock();
}

void MyNotePad::on_actionUppercase_Alphabetic_triggered()
{
    QTextCursor cursor = ui->textEdit->textCursor();
    QTextListFormat::Style lStyle = QTextListFormat::ListUpperAlpha;

    cursor.beginEditBlock();

    QTextBlockFormat bFormat = cursor.blockFormat();
    QTextListFormat liFormat;

    if(cursor.currentList())
    {
        liFormat = cursor.currentList()->format();
    }
    else
    {
        liFormat.setIndent(bFormat.indent() + 1);
        bFormat.setIndent(0);
        cursor.setBlockFormat(bFormat);
    }

    liFormat.setStyle(lStyle);
    cursor.createList(liFormat);
    cursor.endEditBlock();
}

void MyNotePad::on_actionLowercase_Alphabetic_triggered()
{
    QTextCursor cursor = ui->textEdit->textCursor();
    QTextListFormat::Style lStyle = QTextListFormat::ListLowerAlpha;

    cursor.beginEditBlock();

    QTextBlockFormat bFormat = cursor.blockFormat();
    QTextListFormat liFormat;

    if(cursor.currentList())
    {
        liFormat = cursor.currentList()->format();
    }
    else
    {
        liFormat.setIndent(bFormat.indent() + 1);
        bFormat.setIndent(0);
        cursor.setBlockFormat(bFormat);
    }

    liFormat.setStyle(lStyle);
    cursor.createList(liFormat);
    cursor.endEditBlock();
}

void MyNotePad::on_actionColour_triggered()
{
    QColor textColor = QColorDialog::getColor();
    ui->textEdit->setTextColor(textColor);
}

void MyNotePad::on_actionToolbar_triggered()
{
    if(ui->actionToolbar->isChecked())
    {
        ui->mainToolBar->show();
    }else
    {
        ui->mainToolBar->hide();
    }
}

void MyNotePad::on_actionStatus_Bar_triggered()
{
    if(ui->actionStatus_Bar->isChecked())
    {
        ui->statusBar->show();
    }else
    {
        ui->statusBar->hide();
    }
}

void MyNotePad::on_actionAbout_triggered()
{
    QString text;
    text = "<h2>MyNotePad 1.0</h2>";
    text += "<p>Simple text editor application Developed using QT.</p>";
    text += "<p><center><font color = blue> Developed By </font> : Pium Sudhara Karunasena</center></p>";
    text += "<p><center><font color = blue> &copy; MyNotePad 2018 </font>  <font color = blue> All Rights Reserved </font></center>";
    QMessageBox::about(this, "About MyNotePad", text);
}

void MyNotePad::on_textEdit_textChanged()
{
    ui->statusBar->showMessage(QString("Word Count : %1").arg(ui->textEdit->toPlainText().split(QRegExp("(\\s|\\n|\\r)+"), QString::SkipEmptyParts).count()));
}

void MyNotePad::find(QString text, QTextDocument::FindFlags options)
{
    findText = text;
    typeSearch = TextSearch;
    findNext(options);
}

void MyNotePad::find(QRegExp exp, QTextDocument::FindFlags options)
{
    findRegExp = exp;
    typeSearch = RegSearch;
    findNext(options);
}

void MyNotePad::findNext(QTextDocument::FindFlags options)
{
    if(typeSearch == NoSearch)
        return;
    else if(typeSearch == TextSearch)
    {
        ui->textEdit->setTextCursor(ui->textEdit->document()->find(findText, ui->textEdit->textCursor(), options));
    }else
    {
        ui->textEdit->setTextCursor(ui->textEdit->document()->find(findRegExp, ui->textEdit->textCursor(), options));
    }
}

void MyNotePad::charFormatChanged(const QTextCharFormat &format)
{
    fontTypeChanged(format.font());
}

void MyNotePad::openRecentFile()
{
    if(isSave())
    {
        QAction *action = qobject_cast<QAction *>(sender());
        if(action)
            loadFile(action->data().toString());
    }
}

void MyNotePad::closeEvent(QCloseEvent *event)
{
    if(isSave())
    {
        writeSize();
        event->accept();
    }else
    {
        event->ignore();
    }
}

void MyNotePad::readSize()
{
    QSettings settings("HMJ", "MyNotePad");
    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("size", QSize(1000, 500)).toSize();
    resize(size);
    move(pos);
}

void MyNotePad::writeSize()
{
    QSettings settings("HMJ", "MyNotePad");
    settings.setValue("pos", pos());
    settings.setValue("size", size());
}

void MyNotePad::updateRecentFiles()
{
    QSettings settings("HMJ", "MyNotePad");
    QStringList recentFilesList = settings.value("recentFilesList").toStringList();

    int numberOfFiles = qMin(recentFilesList.size(),(int)MaxRecentFiles);
    for(int i = 0; i<numberOfFiles; ++i)
    {
        QString text = tr("&%1 %2").arg(i+1).arg(QFileInfo(recentFilesList[i]).fileName());
        recentFiles[i]->setText(text);
        recentFiles[i]->setData(recentFilesList[i]);
        recentFiles[i]->setVisible(true);
    }
    for(int j = numberOfFiles; j<MaxRecentFiles; ++j)
        recentFiles[j]->setVisible(false);
}

void MyNotePad::loadFile(const QString &fileName)
{
    if(!fileName.isEmpty())
    {
        QFile file(fileName);
        if(file.open(QFile::ReadOnly))
        {
            QTextStream inputData(&file);
            QString fileText = inputData.readAll();
            ui->textEdit->setHtml(fileText);
            file.close();
            setRecentFile(fileName);
            statusBar()->showMessage(tr("Opened \"%1\"").arg(QDir::toNativeSeparators(fileName)),2500);
        }else
        {
            QMessageBox::warning(this, "MyNotePad", tr("Cannot read file%1.\nError: %2").arg(fileName).arg(file.errorString()));
            statusBar()->showMessage(tr("Error Opening \"%1\"").arg(QDir::toNativeSeparators(fileName)),2500);
        }
    }
}

void MyNotePad::setRecentFile(const QString &fileName)
{
    filePath = fileName;
    setWindowTitle(tr("MyNotePad - %1").arg(QFileInfo(filePath).fileName()));

    QSettings settings("HMJ", "MyNotePad");
    QStringList recentFilesList = settings.value("recentFilesList").toStringList();
    recentFilesList.removeAll(fileName);
    recentFilesList.prepend(fileName);
    while(recentFilesList.size() > MaxRecentFiles)
        recentFilesList.removeLast();

    settings.setValue("recentFilesList", recentFilesList);
    updateRecentFiles();
}

void MyNotePad::selectionSync(const QTextCharFormat &format)
{
    QTextCursor cursor = ui->textEdit->textCursor();
    if(!cursor.hasSelection())
        cursor.select(QTextCursor::WordUnderCursor);
    cursor.mergeCharFormat(format);
    ui->textEdit->mergeCurrentCharFormat(format);
}

void MyNotePad::fontTypeChanged(const QFont &fnt)
{
    ui->actionBold->setChecked(fnt.Bold);
    ui->actionItalic->setChecked(fnt.italic());
    ui->actionUnderline->setChecked(fnt.underline());
    ui->actionStrikethrough->setChecked(fnt.strikeOut());
}

bool MyNotePad::saveFile(const QString &fileName)
{
    QFile file(fileName);
    if(file.open(QFile::WriteOnly))
    {
        QTextStream writeData(&file);
        QString fileText = ui->textEdit->toHtml();
        writeData << fileText;
        file.flush();
        file.close();
        setRecentFile(fileName);
        setWindowTitle(tr("MyNotePad - %1").arg(QFileInfo(filePath).fileName()));
        ui->textEdit->document()->setModified(false);
        return true;
    }else
    {
        QMessageBox::warning(this, "MyNotePad", tr("Cannot write file %1.\n Error: %2").arg(filePath).arg(file.errorString()));
        return false;
    }
}

bool MyNotePad::isSave()
{
    if(ui->textEdit->document()->isModified())
    {
        QMessageBox::StandardButton value = QMessageBox::warning(this, "MyNotePad", tr("The Document has been Modified\n" "Do you want to save your changes?"), QMessageBox::Yes | QMessageBox::Discard | QMessageBox::Cancel);
        if(value == QMessageBox::Yes)
        {
            return on_actionSave_triggered();
        }
        else if(value == QMessageBox::Cancel)
            return false;
    }
    return true;
}
