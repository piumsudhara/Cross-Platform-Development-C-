#ifndef MyNotePad_H
#define MyNotePad_H

#include <finddialog.h>
#include <QMainWindow>
#include <QtCore>
#include <QAction>
#include <QTextCharFormat>
#include <QPrinter>
#include <QTextDocument>


enum SearchType
{
    NoSearch,TextSearch,RegSearch
};
typedef enum SearchType searchType;

namespace Ui {
class MyNotePad;
}

class MyNotePad : public QMainWindow
{
    Q_OBJECT

public:
    explicit MyNotePad(QWidget *parent = 0);
    ~MyNotePad();

private slots:
    void on_actionNew_triggered();
    void on_actionOpen_triggered();
    bool on_actionSave_triggered();
    bool on_actionSave_As_triggered();
    void on_actionExit_triggered();
    void on_actionCut_triggered();
    void on_actionCopy_triggered();
    void on_actionPaste_triggered();
    void on_actionUndo_triggered();
    void on_actionRedo_triggered();
    void on_actionFind_triggered();
    void on_actionFind_Next_triggered();
    void on_actionZoom_In_triggered();
    void on_actionZoom_Out_triggered();
    void on_actionSelect_All_triggered();
    void on_actionClear_All_triggered();
    void on_actionBold_triggered();
    void on_actionItalic_triggered();
    void on_actionUnderline_triggered();
    void on_actionFont_triggered();
    void on_actionStrikethrough_triggered();
    void on_actionSubscript_triggered();
    void on_actionSuperscript_triggered();
    void on_actionDisc_triggered();
    void on_actionCircle_triggered();
    void on_actionSquare_triggered();
    void on_actionArabic_triggered();
    void on_actionUppercase_Roman_triggered();
    void on_actionLowercase_Roman_triggered();
    void on_actionUppercase_Alphabetic_triggered();
    void on_actionLowercase_Alphabetic_triggered();
    void on_actionColour_triggered();
    void on_actionToolbar_triggered();
    void on_actionStatus_Bar_triggered();
    void on_actionAbout_triggered();
    void on_textEdit_textChanged();
    void find(QString text, QTextDocument::FindFlags options);
    void find(QRegExp exp, QTextDocument::FindFlags options);
    void findNext(QTextDocument::FindFlags options);
    void charFormatChanged(const QTextCharFormat &format);
    void openRecentFile();
    void closeEvent(QCloseEvent *event);
private:
    Ui::MyNotePad *ui;
    QString filePath;
    QString findText;
    void readSize();
    void writeSize();
    bool isSave();
    void updateRecentFiles();
    void loadFile(const QString &fileName);
    void setRecentFile(const QString &fileName);
    void selectionSync(const QTextCharFormat &format);
    void fontTypeChanged(const QFont &fnt);
    bool saveFile(const QString &fileName);

    enum {MaxRecentFiles = 5};
    QAction *recentFiles[MaxRecentFiles];
    FindDialog *w_FindDialog;
    SearchType typeSearch;
    QRegExp findRegExp;
};

#endif
