#ifndef QPRIVATE_H
#define QPRIVATE_H

#include <QDialog>
#include <QBoxLayout>
#include <QButtonGroup>
#include <QDialogButtonBox>
#include <QSplitter>
#include "qTypes.h"

class qPrivate : public QDialog
{
    Q_OBJECT
public:
    explicit qPrivate(QWidget *parent = 0);

signals:

public slots:

protected:

};

#endif // QPRIVATE_H
