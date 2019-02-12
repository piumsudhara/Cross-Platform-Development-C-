#include "finddialog.h"
#include "ui_finddialog.h"
#include <QPushButton>
#include <QDialogButtonBox>
#include <QTextDocument>

FindDialog::FindDialog(QWidget *parent) :
    QWidget(parent, Qt::Window),
    ui(new Ui::FindDialog)
{
    ui->setupUi(this);
    ui->downButton->setChecked(true);
    firstSearch = true;

    bFindNext = new QPushButton("Find Next");
    bCancel = new QPushButton("Cancel");

    ui->buttonBox->addButton(bFindNext, QDialogButtonBox::ActionRole);
    ui->buttonBox->addButton(bCancel, QDialogButtonBox::RejectRole);

    connect(bFindNext, SIGNAL(clicked(bool)), SLOT(mFindNext()));
    connect(bCancel, SIGNAL(clicked(bool)), SLOT(mCancel()));

    this->setWindowTitle("Find Text");
}

FindDialog::~FindDialog()
{
    delete bFindNext;
    delete bCancel;
    delete ui;
}

QTextDocument::FindFlags FindDialog::getOptions()
{
    QTextDocument::FindFlags options = 0x00000;
    if(ui->caseSensitive->isChecked())
        options |= QTextDocument::FindCaseSensitively;
    if(ui->upButton->isChecked())
        options |= QTextDocument::FindBackward;

    return options;
}

void FindDialog::mCancel()
{
    close();
}

void FindDialog::mFindNext()
{
    if(firstSearch)
    {
        if(ui->useRegExp->isChecked())
            emit find(QRegExp(ui->sText->text()), getOptions());
        else
            emit find(ui->sText->text(), getOptions());
        firstSearch = false;
    }
    else
        emit findNext(getOptions());
}

void FindDialog::on_sText_textChanged()
{
    firstSearch = true;
}
