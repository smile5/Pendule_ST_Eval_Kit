#ifndef XGDOUBLESPINBOX_H
#define XGDOUBLESPINBOX_H

#include <QDoubleSpinBox>
#include <QRegularExpressionValidator>

class xgdoublespinbox : public QDoubleSpinBox
{
    Q_OBJECT
public:
    explicit xgdoublespinbox(QWidget* parent = nullptr);
    virtual ~xgdoublespinbox();

    QValidator::State validate(QString & text, int & pos) const override;

    qreal valueFromText(const QString &text) const override;

    void showEvent(QShowEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void xgtextChanged();

private:
    QRegularExpressionValidator* m_validator;

    QString m_current_suffix;
};

#endif // XGDOUBLESPINBOX_H
