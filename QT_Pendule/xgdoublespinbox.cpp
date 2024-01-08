#include "xgdoublespinbox.h"
#include "qevent.h"

xgdoublespinbox::xgdoublespinbox(QWidget *parent)
    : QDoubleSpinBox(parent)
    , m_validator(nullptr)
    , m_current_suffix(QString())
{
    // validate positive or negative number written with "." or ","
    // and that may have a suffix at the end
    // also matches empty string "(?![\\s\\S]\r\n)" to allow user to clear the whole field
    const QString regex = QStringLiteral("[+-]?(\\d+([.,]\\d*)?(e[+-]?\\d+)?|[.,]\\d+(e[+-]?\\d+)?)");                //   ("((?![\\s\\S]\r\n)|-?\\d{1,}(?:[,.]{1})?\\d*\\s*)");
    m_validator = new QRegularExpressionValidator(QRegularExpression(regex), this);

    // check if a suffix is present
    // connect is triggered when setSuffix() is called
    connect(this, &xgdoublespinbox::textChanged,this,&xgdoublespinbox::xgtextChanged); /*[this](){
                if(suffix().isEmpty())
                    return;

                if(m_current_suffix.localeAwareCompare(suffix()) == 0)
                    return;

                m_current_suffix = suffix();


                QString previous_regex = m_validator->regularExpression().pattern();
                // remove the ending ")"
                previous_regex.chop(1);

                QString new_regex = previous_regex +
                                    QStringLiteral("(?:") + m_current_suffix + QStringLiteral(")?)");
                m_validator->setRegularExpression(QRegularExpression(new_regex));
            });*/
}

xgdoublespinbox::~xgdoublespinbox()
{
    delete m_validator;
}

void xgdoublespinbox::xgtextChanged()
{
    // if(suffix().isEmpty())
    //     return;

    // if(m_current_suffix.localeAwareCompare(suffix()) == 0)
    //     return;

    // m_current_suffix = suffix();


    QString previous_regex = m_validator->regularExpression().pattern();
    // remove the ending ")"
    previous_regex.chop(1);

    QString new_regex = previous_regex +
                        QStringLiteral("(?:") + m_current_suffix + QStringLiteral(")?)");
    m_validator->setRegularExpression(QRegularExpression(new_regex));
}

QValidator::State xgdoublespinbox::validate(QString &text, int &pos) const
{
    return m_validator->validate(text, pos);
}


qreal xgdoublespinbox::valueFromText(const QString &text) const
{
    QString temp = text;
    temp.replace(QStringLiteral(","), QStringLiteral(".")); // replace comma with dot before toDouble()
    temp.remove(suffix()); // remove "°" at the end of the value

    return temp.toDouble();
}

void xgdoublespinbox::showEvent(QShowEvent *event)
{
    // need to call manually textChanged(const QString &text)
    // otherwise the new regex may not be taken into account directly
    Q_EMIT textChanged(text());
    QDoubleSpinBox::showEvent(event);
}

void xgdoublespinbox::wheelEvent(QWheelEvent *event)
{
    // Prevent value from changing when user scrolls
    event->ignore();
}
