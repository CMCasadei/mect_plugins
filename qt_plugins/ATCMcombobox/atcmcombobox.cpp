/**
 * @file
 * @author Emiliano Bergamaschini <ebergamaschini@mect.it>
 *
 * @section LICENSE
 * Copyright Mect s.r.l. 2013
 *
 * @brief ATCM combobox base class
 */
#include <QPainter>
#include <QVBoxLayout>
#include <QStyleOption>
#include <QMessageBox>
#include <QStyledItemDelegate>

#include "atcmcombobox.h"
#include "common.h"
#include "atcmstyle.h"
#ifdef TARGET_ARM
#include "app_logprint.h"
#include "cross_table_utility.h"
#else
#if 0
#include <QtProperty>
#include "crosstableeditfactory.h"
#include "crosstablemanager.h"
#endif
#endif

ATCMcombobox::ATCMcombobox(QWidget *parent) :
    QComboBox(parent)
{
    m_fBusy = false;
    m_value = "";
    m_variable = "";
    m_status = UNK;
    m_CtIndex = -1;
    m_CtVisibilityIndex = 0;
    m_objectstatus = false;
    m_visibilityvar = "";
    m_writeAcknowledge = false;

    m_bgcolor = BG_COLOR_DEF;
    m_fontcolor = FONT_COLOR_DEF;
    m_bordercolor = BORDER_COLOR_DEF;
    m_borderwidth = BORDER_WIDTH_DEF;
    m_borderradius = BORDER_RADIUS_DEF;
    m_refresh = DEFAULT_PLUGIN_REFRESH;

#if 0
#ifndef TARGET_ARM
    CrossTableManager *filePathManager;
    CrossTableEditFactory *fileEditFactory;
    QtProperty *example;

    filePathManager = new CrossTableManager;
    example = filePathManager->addProperty("Example");

    filePathManager->setValue(example, "main.cpp");
    filePathManager->setFilter(example, "Source files (*.cpp *.c)");

    fileEditFactory = new CrossTableEditFactory;
    browser->setFactoryForManager(filePathManager, fileEditFactory);
    addSubProperty(example);
#endif
#endif

    //setMinimumSize(QSize(150,50));
    setFocusPolicy(Qt::NoFocus);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setEditable(false);
    QStyledItemDelegate *itDelegate = new QStyledItemDelegate();
    setItemDelegate(itDelegate);
    setStyle(new ATCMStyle);

#ifdef TARGET_ARM
    setToolTip("");
#endif

    setMapping(m_mapping);
    m_remapping = false;

    /*
     * put there a default stylesheet
     */
    setStyleSheet(
            #ifndef ENABLE_STYLESHEET
                ""
            #else
                "/*"
                "QAbstractItemView {\n"
                "    padding: 5 5 16 5;\n"
                "    font-size: 20px;\n"
                "}\n"
                "QScrollBar:vertical {\n"
                "    width: 30px;\n"
                "    margin: 0px 0px 0px 0px;\n"
                "}\n"
                "QScrollBar::handle:vertical {\n"
                "    min-height:30px;\n"
                "    background-color: rgb(0, 0, 0);\n"
                "}\n"
                "QScrollBar::add-line:vertical {\n"
                "    height: 0px;\n"
                "    subcontrol-position: bottom;\n"
                "    subcontrol-origin: margin;\n"
                "}\n"
                "QScrollBar::sub-line:vertical {\n"
                "    height: 0px;\n"
                "    subcontrol-position: top;\n"
                "    subcontrol-origin: margin;\n"
                "}\n"
                "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {\n"
                "    background: none;\n"
                "}\n"
                "QComboBox {\n"
                "    padding-left: 5px;\n"
                "    font-size: 20px;\n"
                "    border:2px solid black;\n"
                "    border-radius:4px;\n"
                "    background-color: rgb(230, 230, 230);\n"
                "}\n"
                "QComboBox::drop-down\n"
                "{\n"
                "    image: url(:/down.png);\n"
                "}\n"
                "*/"
            #endif
                );
    m_parent = parent;
#ifdef TARGET_ARM
    if (m_refresh > 0)
    {
        connect(m_parent, SIGNAL(varRefresh()), this, SLOT(updateData()));
    }
#endif
}

ATCMcombobox::~ATCMcombobox()
{
}

void ATCMcombobox::paintEvent(QPaintEvent * e)
{
    Q_UNUSED( e );
    QPalette palette = this->palette();

    QStyleOptionComboBox opt;
    opt.init(this);

    /* button color */
    palette.setColor(QPalette::Button, m_bgcolor);
    /* font color */
    palette.setColor(QPalette::ButtonText, m_fontcolor);
    /* border color */
    palette.setColor(QPalette::Foreground, m_bordercolor);

    if (m_apparence == QFrame::Raised)
    {
        opt.state = QStyle::State_Raised;
    }
    else if (m_apparence == QFrame::Sunken)
    {
        opt.state = QStyle::State_Sunken;
    }

#ifdef TARGET_ARM
    if (m_viewstatus)
    {
        /* draw the background color in funtion of the status */
        palette.setColor(QPalette::Foreground, Qt::red);
        switch(m_status)
        {
        case DONE:
            palette.setColor(QPalette::Foreground, Qt::green);
            break;
        case BUSY:
            palette.setColor(QPalette::Foreground, Qt::yellow);
            break;
        case ERROR:
            palette.setColor(QPalette::Foreground, Qt::red);
            break;
        default /*UNKNOWN*/:
            palette.setColor(QPalette::Foreground, Qt::gray);
            break;
        }
    }
#endif

    opt.palette = palette;
    _diameter_ = m_borderradius;
    _penWidth_ = m_borderwidth;
    opt.currentText = currentText();

    QPainter painter(this);
    style()->drawComplexControl(QStyle::CC_ComboBox, &opt, &painter, this);
}

void ATCMcombobox::unsetVariable()
{
    setVariable("");
}

void ATCMcombobox::unsetRefresh()
{
    setRefresh(DEFAULT_PLUGIN_REFRESH);
}

void ATCMcombobox::unsetViewStatus()
{
    setViewStatus(false);
}

void ATCMcombobox::unsetVisibilityVar()
{
    setVisibilityVar("");
}

void ATCMcombobox::unsetMapping()
{
    setMapping("");
}

void ATCMcombobox::unsetwriteAcknowledge()
{
    setWriteAcknowledge(false);
}

void ATCMcombobox::unsetApparence()
{
    setApparence(QFrame::Plain);
}

void ATCMcombobox::setViewStatus(bool status)
{
    m_viewstatus = status;
}

void ATCMcombobox::setWriteAcknowledge(bool status)
{
    m_writeAcknowledge = status;
}

bool ATCMcombobox::setVisibilityVar(QString visibilityVar)
{
    if (visibilityVar.trimmed().length() == 0)
    {
        m_visibilityvar.clear();
        m_CtVisibilityIndex = 0;
        return true;
    }
    else
    {
#ifdef TARGET_ARM
        int CtIndex;
        if (Tag2CtIndex(visibilityVar.trimmed().toAscii().data(), &CtIndex) == 0)
        {
            LOG_PRINT(verbose_e,"visibilityVar '%s', CtIndex %d\n", m_visibilityvar.toAscii().data(), m_CtVisibilityIndex);
            m_CtVisibilityIndex = CtIndex;
#endif
            m_visibilityvar = visibilityVar.trimmed();
            if (m_refresh == 0)
            {
                setRefresh(DEFAULT_PLUGIN_REFRESH);
            }
            return true;
#ifdef TARGET_ARM
        }
        else
        {
            m_CtVisibilityIndex = 0;
            LOG_PRINT(error_e,"visibilityVar '%s', CtIndex %d\n", visibilityVar.trimmed().toAscii().data(), CtIndex);
            return false;
        }
#endif
    }
}

/* Write variable */
bool ATCMcombobox::writeValue(QString value)
{
    if (m_variable.length() == 0)
    {
        return false;
    }
#ifdef TARGET_ARM
    bool ret_val = true;

    m_fBusy = true;

    if (m_writeAcknowledge == false || QMessageBox::question(this, trUtf8("Confirm Writing"), trUtf8("Do you want to save new value: '%1'?").arg(value), QMessageBox::Ok, QMessageBox::Cancel) == QMessageBox::Ok)
    {
        m_value = mapped2value(value);
        ret_val =  setFormattedVarByCtIndex(m_CtIndex, m_value.toAscii().data());
        //fprintf(stderr, "WRITING %d %s -> %s\n", m_CtIndex, value.toAscii().data(), m_value.toAscii().data());
    }

    setcomboValue();

    m_fBusy = false;

    return ret_val;
#else
    Q_UNUSED( value );
    return true;
#endif
}

/* Activate variable */
bool ATCMcombobox::setVariable(QString variable)
{
    /* if the acual variable is empty activate it */
    if (variable.trimmed().length() > 0)
    {
#ifdef TARGET_ARM
        m_variable = variable.trimmed();
        if (Tag2CtIndex(m_variable.toAscii().data(), &m_CtIndex) != 0)
        {
            LOG_PRINT(error_e, "cannot extract ctIndex\n");
            m_status = ERROR;
            m_value = VAR_UNKNOWN;
            m_CtIndex = -1;
        }
        LOG_PRINT(verbose_e, "'%s' -> ctIndex %d\n", m_variable.toAscii().data(), m_CtIndex);
#else
        m_variable = variable.trimmed();
#endif
    }

    if (m_status != ERROR)
    {
#ifndef TARGET_ARM
        setToolTip(m_variable);
#else
        setToolTip("");
#endif
        return true;
    }
    else
    {
        return false;
    }
}

QColor ATCMcombobox::bgColor() const
{
    return m_bgcolor;
}

void ATCMcombobox::setBgColor(const QColor& color)
{
    m_bgcolor = color;
    update();
}

QColor ATCMcombobox::borderColor() const
{
    return m_bordercolor;
}

void ATCMcombobox::setBorderColor(const QColor& color)
{
    m_bordercolor = color;
    update();
}

QColor ATCMcombobox::fontColor() const
{
    return m_fontcolor;
}

void ATCMcombobox::setFontColor(const QColor& color)
{
    m_fontcolor = color;
    update();
}

void ATCMcombobox::setBorderWidth(int width)
{
    m_borderwidth = width;
    update();
}

void ATCMcombobox::setBorderRadius(int radius)
{
    m_borderradius = radius;
    update();
}

bool ATCMcombobox::setRefresh(int refresh)
{
    m_refresh = refresh;
    return true;
}

/* read variable */
void ATCMcombobox::updateData()
{
    bool isChanged = true;
#ifdef TARGET_ARM
    char value[TAG_LEN] = "";

    if (m_fBusy)
        return;

    if (m_CtVisibilityIndex > 0) {
        uint32_t visible = 0;
        if (readFromDbLock(m_CtVisibilityIndex, &visible) == 0) {
            m_status = DONE;
            if (visible && ! this->isVisible()) {
                this->setVisible(true);
            }
            else if (! visible && this->isVisible()) {
                this->setVisible(false);
            }
        }
    }
    if (! this->isVisible()) {
        return;
    }

    if (m_variable.length() > 0)
    {
        if (m_CtIndex >= 0)
        {
            if (formattedReadFromDb_string(m_CtIndex, value) == 0 && strlen(value) > 0)
            {
                QString new_value = value;
                isChanged = (m_value != new_value);
                m_status = DONE;
                m_value = value;
            }
            else
            {
                m_value = VAR_UNKNOWN;
                m_status = ERROR;
            }
        }
        else
        {
            m_status = ERROR;
            m_value = VAR_UNKNOWN;
            LOG_PRINT(verbose_e, "Invalid CtIndex %d for variable '%s'\n", m_CtIndex, m_variable.toAscii().data());
        }
    }
    LOG_PRINT(verbose_e, "'%s': '%s' status '%c' \n", m_variable.toAscii().data(), value, m_status);
#endif
    if (m_status == DONE && isChanged)
    {
        setcomboValue();
    }
    this->update();
}



enum QFrame::Shadow ATCMcombobox::apparence() const
{
    return m_apparence;
}

void ATCMcombobox::setApparence(const enum QFrame::Shadow apparence)
{
    m_apparence = apparence;
    update();
}

bool ATCMcombobox::setMapping(QString mapping)
{
    m_mapping = mapping;
    this->clear();
    if (m_mapping.length() > 0)
    {
        QStringList map = m_mapping.split(";");
        if (maxCount() < map.count()/2)
        {
            setMaxCount(map.count()/2);
        }
        for (int i = 1; i < map.count(); i+=2)
        {
#ifdef TARGET_ARM
            LOG_PRINT(verbose_e, "@@@@@@@@@@@@@@@ '%d' [%s]\n", i, map.at(i).toAscii().data());
#endif
            this->addItem(map.at(i));
        }
    }
    else
    {
        return false;
    }
    return true;
}

QString ATCMcombobox::value2mapped( QString value )
{
    int i;
    QStringList map = m_mapping.split(";");
#ifdef TARGET_ARM
    LOG_PRINT(verbose_e, "'%d' [%s]\n", map.count(), m_mapping.toAscii().data());
#endif
    for (i = 0; i + 1 < map.count(); i+=2)
    {
#ifdef TARGET_ARM
        LOG_PRINT(verbose_e, "compare '%s' [%s] vs '%s'\n", map.at(i).toAscii().data(), map.at(i+1).toAscii().data(), value.toAscii().data());
#endif
        if (map.at(i).trimmed().compare(value) == 0)
        {
#ifdef TARGET_ARM
            LOG_PRINT(verbose_e, "Found mapping '%s' -> '%s'.\n", map.at(i).toAscii().data(), map.at(i + 1).toAscii().data());
#endif
            return map.at(i + 1);
        }
    }
    return QString("");
}

QString ATCMcombobox::mapped2value( QString mapped )
{
    int i;
    QStringList map = m_mapping.split(";");
#ifdef TARGET_ARM
    LOG_PRINT(verbose_e, "'%d' [%s]\n", map.count(), m_mapping.toAscii().data());
#endif
    for (i = 0; i + 1 < map.count(); i+=2)
    {
#ifdef TARGET_ARM
        LOG_PRINT(verbose_e, "compare '%s' [%s] vs '%s'\n", map.at(i).toAscii().data(), map.at(i+1).toAscii().data(), mapped.toAscii().data());
#endif
        if (map.at(i + 1).compare(mapped) == 0)
        {
#ifdef TARGET_ARM
            LOG_PRINT(verbose_e, "Found mapping '%s' -> '%s'.\n", map.at(i).toAscii().data(), map.at(i + 1).toAscii().data());
#endif
            return map.at(i);
        }
    }
    return QString("");
}

bool ATCMcombobox::setcomboValue()
{
    QString mapped = value2mapped(m_value);
    int index = this->findText(mapped, Qt::MatchExactly);

#ifdef TARGET_ARM
#else
    /* code to manage a remapping value */
    if (index >= 0)
    {
        disconnect( this, SIGNAL( currentIndexChanged(QString) ), this, SLOT( writeValue(QString) ) );
        if (m_remapping == true)
        {
            m_remapping = false;
            setMapping(m_mapping);
            setcomboValue();
        }
        this->setCurrentIndex(index);
        connect( this, SIGNAL( currentIndexChanged(QString) ), this, SLOT( writeValue(QString) ) );
    }
#endif

    if (index == this->currentIndex())
    {
        return true;
    }
    if (index >= 0)
    {
        disconnect( this, SIGNAL( currentIndexChanged(QString) ), this, SLOT( writeValue(QString) ) );
        this->setCurrentIndex(index);
        connect( this, SIGNAL( currentIndexChanged(QString) ), this, SLOT( writeValue(QString) ) );
        return true;
    }

    /* if is not managed, put an empty string */
    /* if the actual status is an error, display error message */
    if (m_status == ERROR)
    {
#ifdef TARGET_ARM
        this->setEditText(VAR_UNKNOWN);
        LOG_PRINT(verbose_e,"unknown value '%s'\n", m_value.toAscii().data());
#endif
    }
    /* if the actual status is not expected, display the value */
    else
    {
#ifdef TARGET_ARM
        disconnect( this, SIGNAL( currentIndexChanged(QString) ), this, SLOT( writeValue(QString) ) );
        this->setEditText("");
        this->setCurrentIndex(-1);
        connect( this, SIGNAL( currentIndexChanged(QString) ), this, SLOT( writeValue(QString) ) );
        LOG_PRINT(error_e,"unknown value '%s' for variable '%s'\n", m_value.toAscii().data(), m_variable.toAscii().data());
#else
        index = this->findText(m_value);
        if (index < 0)
        {
            this->addItem(m_value);
            index = this->findText(m_value);
        }
        disconnect( this, SIGNAL( currentIndexChanged(QString) ), this, SLOT( writeValue(QString) ) );
        this->setCurrentIndex(index);
        connect( this, SIGNAL( currentIndexChanged(QString) ), this, SLOT( writeValue(QString) ) );
        m_remapping = true;
        //this->setEditText(m_value);
#endif
    }
    return false;
}

