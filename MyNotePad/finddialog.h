#ifndef FINDDIALOG_H
#define FINDDIALOG_H

#include <QWidget>
#include <QTextDocument>
#include <QPushButton>

namespace Ui {
class FindDialog;
}

class FindDialog : public QWidget
{
    Q_OBJECT

public:
    explicit FindDialog(QWidget *parent = 0);
    ~FindDialog();

    QTextDocument::FindFlags getOptions();

    enum Option
    {
        TextCaseSensitive = 1,
        TextRegExp = 2
    };
    Q_DECLARE_FLAGS(Options, Option)

signals:
    void find(QString text, QTextDocument::FindFlags options);
    void find(QRegExp exp, QTextDocument::FindFlags options);
    void findNext(QTextDocument::FindFlags options);

private slots:
    void mCancel();
    void mFindNext();

    void on_sText_textChanged();

private:
    Ui::FindDialog *ui;
    bool firstSearch;
    QPushButton *bCancel, *bFindNext;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(FindDialog::Options)

#endif
