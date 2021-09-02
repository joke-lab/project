#ifndef RELATED_H
#define RELATED_H

#include <QWidget>

namespace Ui {
class related;
}

class related : public QWidget
{
    Q_OBJECT

public:
    explicit related(QWidget *parent = nullptr);
    ~related();

private:
    Ui::related *ui;
};

#endif // RELATED_H
