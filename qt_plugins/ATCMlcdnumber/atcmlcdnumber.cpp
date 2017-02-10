/**
 * @file
 * @author Emiliano Bergamaschini <ebergamaschini@mect.it>
 *
 * @section LICENSE
 * Copyright Mect s.r.l. 2013
 *
 * @brief ATCM lcdnumber base class
 */
#include <QPainter>
#include <QVBoxLayout>
#include <QStyleOption>

#include "atcmlcdnumber.h"
#include "common.h"
#ifdef TARGET_ARM
#include "cross_table_utility.h"
#include "app_logprint.h"
#endif

ATCMlcdnumber::ATCMlcdnumber(QWidget *parent) :
	QLCDNumber(parent)
{
	m_value = "";
	m_variable = "";
	m_status = UNK;

    //setMinimumSize(QSize(150,50));
    setFocusPolicy(Qt::NoFocus);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	/*
	 * put there a default stylesheet
	 */
	setStyleSheet(
#ifndef ENABLE_STYLESHEET
			""
#else
			"QLCDNumber {\n"
			"    background-color: rgb(230, 230, 230);\n"
			"    border:2px solid black;\n"
			"    border-radius:4px;\n"
			"}"
#endif
			);
    m_parent = parent;
#ifdef TARGET_ARM
    connect(m_parent, SIGNAL(varRefresh()), this, SLOT(updateData()));
#endif
}

ATCMlcdnumber::~ATCMlcdnumber()
{
}

void ATCMlcdnumber::paintEvent(QPaintEvent * e)
{
	QPainter p(this);

	if (m_viewstatus)
	{
		/* draw the background color in funtion of the status */
		QBrush brush(Qt::red);
		switch(m_status)
		{
			case DONE:
				brush.setColor(Qt::green);
				break;
			case BUSY:
				brush.setColor(Qt::yellow);
				break;
			case ERROR:
				brush.setColor(Qt::red);
				break;
			default /*UNKNOWN*/:
				brush.setColor(Qt::gray);
				break;
		}
		p.setBrush(brush);
		p.drawRect(this->rect());
	}
	/* propagate the stylesheet set by QtCreator */
	QStyleOption opt;
	opt.init(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
	/* propagate the paint event to the parent widget */
	QLCDNumber::paintEvent(e);
}

/* Write variable */
bool ATCMlcdnumber::writeValue(QString value)
{
	m_value = value;
	this->display(m_value);
	if (m_value.length() == 0 || m_variable.length() == 0)
	{
		return true;
	}
#ifdef TARGET_ARM
	return setFormattedVar(m_variable.toAscii().data(), m_value.toAscii().data());
#else
	return true;
#endif
}

/* Activate variable */
bool ATCMlcdnumber::setVariable(QString variable)
{
    m_variable = variable.trimmed();
#ifdef TARGET_ARM
//    if (m_variable.isEmpty()) {
//        m_status = ERROR;
//        m_value = "";
//        m_CtIndex = 0;
//        LOG_PRINT(verbose_e, "empty variable\n");
//    }
//    else if (Tag2CtIndex(m_variable.toAscii().data(), &m_CtIndex) != 0)
//    {
//        m_status = ERROR;
//        m_value = "";
//        m_CtIndex = 0;
//        LOG_PRINT(error_e, "unknown variable '%s'\n", variable.trimmed().toAscii().data());
//    }
//    else
//    {
        m_status = UNK; // not read yet
        m_value =  "";
//        LOG_PRINT(info_e, "set variable #%d '%s'\n", m_CtIndex, m_variable.toAscii().data());
//    }
    setToolTip("");
#else
    setToolTip(m_variable);
#endif
    return false;
}

bool ATCMlcdnumber::setRefresh(int refresh)
{
	m_refresh = refresh;
	return true;
}

/* read variable */
void ATCMlcdnumber::updateData()
{
#ifdef TARGET_ARM
	int CtIndex;
    char value[42] = "";

	if (this->isVisible() == false)
	{
		return;
	}

	if (m_variable.length() == 0)
	{
		return;
	}

	if (Tag2CtIndex(m_variable.toAscii().data(), &CtIndex) != 0)
	{
		LOG_PRINT(error_e, "cannot extract ctIndex\n");
		m_status = ERROR;
		m_value = VAR_UNKNOWN;
	}
	else
	{
        m_status = DONE;
        if (formattedReadFromDb_string(CtIndex, value) == 0 && strlen(value) > 0)
		{
			char statusMsg[TAG_LEN];
            m_status = pIODataStatusAreaI[CtIndex];
			switch (m_status)
			{
				case BUSY:
					m_value = value;
					break;
				case ERROR:
					if (strlen(statusMsg) > 0) m_value = statusMsg;
					break;
				case DONE:
					m_value = value;
					break;
				default:
					m_value = VAR_UNKNOWN;
					break;
			}
		}
		else
		{
			m_value = VAR_UNKNOWN;
			m_status = ERROR;
		}
	}
	LOG_PRINT(verbose_e, "'%s': '%s' status '%c' (BUSY '%c' - ERROR '%c' - DONE '%c')\n", m_variable.toAscii().data(), value, m_status, BUSY, ERROR, DONE);
#endif
	this->update();
	this->display(m_value);
}

bool ATCMlcdnumber::setViewStatus(bool status)
{
	return (m_viewstatus = status);
}
