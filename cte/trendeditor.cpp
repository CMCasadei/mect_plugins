#include "trendeditor.h"
#include "ui_trendeditor.h"
#include "utils.h"

#include <QColor>
#include <QLatin1String>
#include <QColorDialog>
#include <QPalette>
#include <QIcon>
#include <QDebug>
#include <QCheckBox>
#include <QLineEdit>
#include <QLabel>
#include <QComboBox>
#include <QFile>
#include <QTextStream>
#include <QValidator>
#include <QDoubleValidator>
#include <QFileDialog>
#include <QMessageBox>


#define LINE_SIZE 1024


const int nTrk1 = 1;
const int nTrk2 = 2;
const int nTrk3 = 3;
const int nTrk4 = 4;



const QString szTREND1FILE = QLatin1String("trend1.csv");
const QString szTRENDMASK =  QLatin1String("trend*.csv");
const QString szDEFCOLOR = QLatin1String("palegreen");
const QString szPORTRAIT = QLatin1String("P");
const QString szLANDSCAPE = QLatin1String("L");
const QString szTrendRelPath = QLatin1String("config/");
const QString szTrendSection = QLatin1String("customtrend.files");
const QString szTrendExt = QLatin1String(".csv");

enum trendFields
{   nTrendVisible = 0,
    nTrendVarName,
    nTrendColor,
    nTrendMin,
    nTrendMax,
    nTrendDescr,
    nTrendFieldsTotal
};


TrendEditor::TrendEditor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TrendEditor)
{
    QIcon cIco;

    ui->setupUi(this);
    // Filling Fixed Combos
    // Combo Orientation
    mapOrientation.insert(szPORTRAIT, trUtf8("Portrait"));
    mapOrientation.insert(szLANDSCAPE, trUtf8("Landscape"));
    cIco = QIcon(szPathIMG + QLatin1String("Portrait.png"));
    ui->cboOrientation->addItem(cIco, mapOrientation.value(szPORTRAIT), szPORTRAIT);
    cIco = QIcon(szPathIMG + QLatin1String("Landscape.png"));
    ui->cboOrientation->addItem(cIco, mapOrientation.value(szLANDSCAPE), szLANDSCAPE);
    ui->cboOrientation->setCurrentIndex(-1);
    // Validators
    ui->txtMin_1->setValidator(new QDoubleValidator(this));
    ui->txtMin_2->setValidator(new QDoubleValidator(this));
    ui->txtMin_3->setValidator(new QDoubleValidator(this));
    ui->txtMin_4->setValidator(new QDoubleValidator(this));
    ui->txtMax_1->setValidator(new QDoubleValidator(this));
    ui->txtMax_2->setValidator(new QDoubleValidator(this));
    ui->txtMax_3->setValidator(new QDoubleValidator(this));
    ui->txtMax_4->setValidator(new QDoubleValidator(this));
    // Path & file infos
    m_szTrendFile.clear();
    m_szTrendPath.clear();
    m_szTemplateFile.clear();
    m_szCurrentModel.clear();
    ui->cmdSave->setEnabled(false);
    ui->cmdSaveAs->setEnabled(false);
    lstSourceRows.clear();
}

TrendEditor::~TrendEditor()
{
    delete ui;
}

void TrendEditor::updateVarLists(const QStringList &lstTrendVars)
{
    int         nCur;
    int         nPos = -1;
    QString      szCurVar;
    QComboBox   *cboVar = 0;

    for (nCur = nTrk1; nCur <= nTrk4; nCur++)  {
        if (nCur == nTrk1)  {
            cboVar = ui->cboVariable_1;
        }
        else if (nCur == nTrk2)  {
            cboVar = ui->cboVariable_2;
        }
        else if (nCur == nTrk3)  {
            cboVar = ui->cboVariable_3;
        }
        else if (nCur == nTrk4)  {
            cboVar = ui->cboVariable_4;
        }
        // qDebug() << tr("Updated Variables Combo: %1") .arg(nCur);
        // Retrieve Current Item selected if Any
        szCurVar = cboVar->currentText();
        // Update Varlist

        cboVar->clear();
        cboVar->addItem(szEMPTY);
        cboVar->addItems(lstTrendVars);
        // Fetch previous value
        nPos = -1;
        if (!szCurVar.isEmpty())  {
            nPos = cboVar->findText(szCurVar);
            // nPos = lstTrendVars.indexOf(szCurVar);
        }
        cboVar->setCurrentIndex(nPos);
    }
}

void TrendEditor::on_cmdColor_1_clicked()
{
    getNewColor(nTrk1);
}

void TrendEditor::on_cmdColor_2_clicked()
{
    getNewColor(nTrk2);
}

void TrendEditor::on_cmdColor_3_clicked()
{
    getNewColor(nTrk3);
}

void TrendEditor::on_cmdColor_4_clicked()
{
    getNewColor(nTrk4);
}
bool    TrendEditor::getNewColor(int nTrack)
// Show Color dialog to select new Track color
{
    QString     szColor;
    QColor      cColor;
    QColor      newColor;
    QLabel      *destLabel = 0;
    bool        fRes = false;

    // Retrieve Colors and start color dialog
    if (nTrack == nTrk1)  {
        szColor = ui->lblColor_1->text();
        destLabel = ui->lblColor_1;
    }
    else if (nTrack == nTrk2)  {
        szColor = ui->lblColor_2->text();
        destLabel = ui->lblColor_2;
    }
    else if (nTrack == nTrk3)  {
        szColor = ui->lblColor_3->text();
        destLabel = ui->lblColor_3;
    }
    else if (nTrack == nTrk4)  {
        szColor = ui->lblColor_4->text();
        destLabel = ui->lblColor_4;
    }
    // Analisi del contenuto della label
    if (! szColor.isEmpty())  {
        szColor.prepend(szSHARP);
        if (QColor::isValidColor(szColor))  {
            cColor.setNamedColor(szColor);
            // qDebug() << tr("Color valid: %1") .arg(szColor);
        }
        else {
            cColor = QColor(szDEFCOLOR);
            // qDebug() << tr("Color invalid: %1 forced to: %2") .arg(szColor) .arg(cColor.name());
        }
    }
    if (! cColor.isValid()) {
        cColor = QColor(szDEFCOLOR);
    }
    // Open Color Dialog to select New Color
    newColor = QColorDialog::getColor(cColor, this, tr("Select Track %1 Color") .arg(nTrack));
    // Set New Color to Label
    if (newColor.isValid() && destLabel != 0)  {
        setObjectColor(destLabel, newColor);
        fRes = true;
    }
    return fRes;
}
bool TrendEditor::tokens2Iface(const QStringList &lstTokens, int nRow)
// Da CSV Tokens ad interfaccia
{
    // Puntatori ad oggetto interfaccia del Trend corrente
    QCheckBox       *chkVisible = 0;
    QComboBox       *cboVariable = 0;
    QLabel          *lblColor = 0;
    QLineEdit       *txtMin = 0;
    QLineEdit       *txtMax = 0;
    QLineEdit       *txtComment = 0;
    // Variabili di supporto per verifica valori
    QString         szTemp;
    double          dblVal = 0.0;
    int             nVal = 0;
    bool            fOK = false;
    QColor          cColTrend;
    bool            fRes = true;


    // Assegnazione delle caselle di lavoro
    if (nRow == nTrk1)  {
        chkVisible = ui->chkVisible_1;
        cboVariable = ui->cboVariable_1;
        lblColor = ui->lblColor_1;
        txtMin = ui->txtMin_1;
        txtMax = ui->txtMax_1;
        txtComment = ui->txtComment_1;
    } 
    else if (nRow == nTrk2)  {
        chkVisible = ui->chkVisible_2;
        cboVariable = ui->cboVariable_2;
        lblColor = ui->lblColor_2;
        txtMin = ui->txtMin_2;
        txtMax = ui->txtMax_2;
        txtComment = ui->txtComment_2;
    } 
    else if (nRow == nTrk3)  {
        chkVisible = ui->chkVisible_3;
        cboVariable = ui->cboVariable_3;
        lblColor = ui->lblColor_3;
        txtMin = ui->txtMin_3;
        txtMax = ui->txtMax_3;
        txtComment = ui->txtComment_3;
    } 
    else if (nRow == nTrk4)  {        
        chkVisible = ui->chkVisible_4;
        cboVariable = ui->cboVariable_4;
        lblColor = ui->lblColor_4;
        txtMin = ui->txtMin_4;
        txtMax = ui->txtMax_4;
        txtComment = ui->txtComment_4;
    }
    // Check Visible
    nVal = lstTokens[nTrendVisible].toInt(&fOK);
    if (fOK)  {
        fOK = nVal == 1;
        chkVisible->setChecked(fOK);
    }
    else
        chkVisible->setChecked(false);
    // Var Name
    nVal = cboVariable->findText(lstTokens[nTrendVarName]);
    if (nVal >= 0 && nVal < cboVariable->count())
        cboVariable->setCurrentIndex(nVal);
    else
        cboVariable->setCurrentIndex(-1);
    // Color Trend
    szTemp = lstTokens[nTrendColor];
    if (!szTemp.isEmpty())  {
        szTemp.prepend(szSHARP);
        if (QColor::isValidColor(szTemp))  {
            cColTrend.setNamedColor(szTemp);
        }
    }
    if (! cColTrend.isValid())
        cColTrend.setNamedColor(szDEFCOLOR);
    setObjectColor(lblColor, cColTrend);
    // MinValue
    dblVal = lstTokens[nTrendMin].toDouble(&fOK);
    // Controllo che si tratti davvero di un numero
    dblVal = fOK ? dblVal : 0.0;
    if (fOK)
        szTemp = lstTokens[nTrendMin];
    else
        szTemp = QString::number(dblVal, 'f', 3);
    txtMin->setText(szTemp);
    // MaxValue
    dblVal = lstTokens[nTrendMax].toDouble(&fOK);
    dblVal = fOK ? dblVal : 0.0;
    // Controllo che si tratti davvero di un numero
    if (fOK)
        szTemp = lstTokens[nTrendMax];
    else
        szTemp = QString::number(dblVal, 'f', 3);
    txtMax->setText(szTemp);
    // Comment
    txtComment->setText(lstTokens[nTrendDescr]);
    // Return value
    return fRes;
}
bool TrendEditor::iface2Tokens(QStringList &lstTokens, int nRow)
// Da Interfaccia a List per scrittura CSV
{
    bool            fRes = true;
    // Puntatori ad oggetto interfaccia del Trend corrente
    QCheckBox       *chkVisible = 0;
    QComboBox       *cboVariable = 0;
    QLabel          *lblColor = 0;
    QLineEdit       *txtMin = 0;
    QLineEdit       *txtMax = 0;
    QLineEdit       *txtComment = 0;


    // Assegnazione delle caselle di lavoro
    if (nRow == nTrk1)  {
        chkVisible = ui->chkVisible_1;
        cboVariable = ui->cboVariable_1;
        lblColor = ui->lblColor_1;
        txtMin = ui->txtMin_1;
        txtMax = ui->txtMax_1;
        txtComment = ui->txtComment_1;
    }
    else if (nRow == nTrk2)  {
        chkVisible = ui->chkVisible_2;
        cboVariable = ui->cboVariable_2;
        lblColor = ui->lblColor_2;
        txtMin = ui->txtMin_2;
        txtMax = ui->txtMax_2;
        txtComment = ui->txtComment_2;
    }
    else if (nRow == nTrk3)  {
        chkVisible = ui->chkVisible_3;
        cboVariable = ui->cboVariable_3;
        lblColor = ui->lblColor_3;
        txtMin = ui->txtMin_3;
        txtMax = ui->txtMax_3;
        txtComment = ui->txtComment_3;
    }
    else if (nRow == nTrk4)  {
        chkVisible = ui->chkVisible_4;
        cboVariable = ui->cboVariable_4;
        lblColor = ui->lblColor_4;
        txtMin = ui->txtMin_4;
        txtMax = ui->txtMax_4;
        txtComment = ui->txtComment_4;
    }
    // Salvataggio dei valori su Lista Tokens
    lstTokens.clear();
    int nCol = 0;
    for (nCol = 0; nCol < nTrendFieldsTotal; nCol++)  {
        lstTokens.append(szEMPTY);
    }
    // Check Box Visible
    lstTokens[nTrendVisible] = (chkVisible->isChecked() ? szTRUE : szFALSE);
    // Variabile Name
    lstTokens[nTrendVarName] = cboVariable->currentText().trimmed();
    lstTokens[nTrendVarName].append(szSpace(MAX_IDNAME_LEN));
    lstTokens[nTrendVarName] = lstTokens[nTrendVarName].left(MAX_IDNAME_LEN);
    // Color Name
    lstTokens[nTrendColor] = lblColor->text().trimmed();
    // Trend Min
    lstTokens[nTrendMin] = txtMin->text().trimmed();
    // Trend Max
    lstTokens[nTrendMax] = txtMax->text().trimmed();
    // Trend Descriprion
    lstTokens[nTrendDescr] = txtComment->text().trimmed();
    // Return value
    return fRes;
}
bool TrendEditor::trendFile2Iface(const QString &szFileTrend)
// Load a trends file in Interface Fields
{
    bool            fRes = false;
    QFile           fSource(szFileTrend);
    QString         szLine;
    QStringList     lstTokens;
    int             nPos = -1;
    int             nRow = 1;

    if (fSource.exists())  {
        qDebug() << tr("Loading Trend: %1") .arg(szFileTrend);
        fSource.open(QIODevice::ReadOnly | QIODevice::Text);
        if (fSource.isOpen())  {
            lstSourceRows.clear();
            QTextStream txtTrend(&fSource);
            // Legge la prima riga per l'orientamento del Trend
            if (!txtTrend.atEnd())  {                
                szLine = txtTrend.readLine().trimmed();
                lstSourceRows.append(szLine);
                if (! szLine.isEmpty())  {
                    if (mapOrientation.contains(szLine))  {
                        szLine = mapOrientation.value(szLine);
                        nPos = ui->cboOrientation->findText(szLine, Qt::MatchFixedString);
                    }
                }
            }
            if (nPos >= 0 && nPos < ui->cboOrientation->count())
                ui->cboOrientation->setCurrentIndex(nPos);
            else
                ui->cboOrientation->setCurrentIndex(-1);
            // Cerca nel file le altre 4 righe 1 per ogni trend
            while(! txtTrend.atEnd() && nRow <= nTrk4) {
                szLine = txtTrend.readLine().trimmed();
                lstSourceRows.append(szLine);
                // Divide la riga letta in Tokens
                lstTokens = szLine.split(szSEMICOL);
                if (lstTokens.count() >= nTrendFieldsTotal)  {
                    // Trim dei Tokens
                    for (nPos = 0; nPos < lstTokens.count(); nPos++)
                        lstTokens[nPos] = lstTokens[nPos].trimmed();
                    // Tokens ad elementi interfaccia
                    fRes = tokens2Iface(lstTokens, nRow);
                }
                else
                    fRes = false;
                if (! fRes)
                    break;
                // Next Line
                nRow++;
            }
            // Close File
            fSource.close();
        }
    }
    else {
        qDebug() << tr("Error Loading Trend: %1") .arg(szFileTrend);
    }
    // Return value
    return fRes;
}
bool TrendEditor::iface2TrendFile(const QString &szFileTrend)
// Write Interface Fields to a trends file
{
    bool            fOk = false;
    QFile           fDest(szFileTrend);
    QFileInfo       infoDest;
    QString         szLine;
    QStringList     lstTokens;
    int             nRow = 1;

    // Apertura del File in modalità scrittura troncata
    fDest.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate);
    if (fDest.isOpen())  {
        lstSourceRows.clear();
        infoDest.setFile(fDest);
        QTextStream txtTrend(&fDest);
        // Trend Orientation
        szLine = mapOrientation.key(ui->cboOrientation->currentText());
        if (szLine.isEmpty())
            szLine = szLANDSCAPE;
        txtTrend << szLine << endl;
        lstSourceRows.append(szLine);
        // Trend Fields
        for (nRow = nTrk1; nRow <= nTrk4; nRow++)  {
            fOk = iface2Tokens(lstTokens, nRow);
            if (fOk)  {
                szLine = lstTokens.join(szSEMICOL);
                txtTrend << szLine << endl;
                lstSourceRows.append(szLine);
            }
            else
                break;
        }
        txtTrend.flush();
        szLine = fDest.fileName();
        fDest.close();
    }
    // Update the Template file if needed
    if (fOk)  {
        // Nome del file senza Path
        szLine = infoDest.fileName();
        // Nome file Trend relativo al percorso progetto
        szLine.prepend(szTrendRelPath);
        // Aggiorna la sezione customtrends del file template.pri
        fOk = updateTemplateFile(szLine);

    }
    // Return value
    return fOk;
}

void TrendEditor::on_cmdLoad_clicked()
{
    QString szSourceFile = currentTrendFile();


    szSourceFile = QFileDialog::getOpenFileName(this, tr("Open Trend File"),
                                              szSourceFile, tr("Trends Files (trend*.csv)"));
    if (! szSourceFile.isEmpty())  {
        QFile fTrend(szSourceFile);
        if (fTrend.exists())  {
            // Carica il nuovo file
            if (trendFile2Iface(szSourceFile))  {
                // Aggiorna il nome del File corrente ma non il path
                QFileInfo infoSource(szSourceFile);
                m_szTrendFile = infoSource.fileName();
                fillTrendsCombo(m_szTrendPath);
            }
        }
    }
}
void TrendEditor::on_cmdSave_clicked()
{
    saveTrend(true);
}

void TrendEditor::on_cmdSaveAs_clicked()
{
    QString szDestFile = currentTrendFile();

    // Checking interface Values
    if (! checkFields())
        return;
    // Ask Dialog file name.
    // FIXME: Restrict QFileDialog to current trend specific directory
    szDestFile = QFileDialog::getSaveFileName(this, tr("Save Trend File"),
                                              szDestFile, tr("Trends Files (trend*.csv)"));
    if (! szDestFile.isEmpty())  {
        // Copia di salvataggio
        fileBackUp(szDestFile);
        // Scrittura nuovi valori
        if (iface2TrendFile(szDestFile))  {
            QFileInfo infoDestination(szDestFile);
            m_szTrendFile = infoDestination.fileName();
            fillTrendsCombo(m_szTrendPath);
            // Ripristinato per versione 3.1.3
            m_szMsg = trUtf8("Trend File has been successfully Saved As File:\n%1").arg(szDestFile);
            notifyUser(this, szMectTitle, m_szMsg);
        }
    }
}
void TrendEditor::setTrendsParameters(const QString szModel, const QString &szTrendsPath, const QString szNewFile, const QString szTemplateFile)
// Force Model to new received model and enable Save - Save As
{

    m_szCurrentModel = szModel.trimmed();
    ui->cmdSave->setEnabled(! m_szCurrentModel.isEmpty());
    ui->cmdSaveAs->setEnabled(! m_szCurrentModel.isEmpty());

    // Force Trends Path to new received path
    m_szTrendPath = szTrendsPath;
    m_szTrendFile = szNewFile;
    m_szTemplateFile = szTemplateFile;
    // If is the received trend file name is empty, set it to default
    if (m_szTrendFile.isEmpty())  {
        m_szTrendFile = szTREND1FILE;
    }
    // Fills Trends Combo
    ui->cboTrendName->clear();
    fillTrendsCombo(m_szTrendPath);
}
void TrendEditor::setObjectColor(QLabel *destObject, QColor newColor)
// Imposta background e foreground di una label con il colore newColor
{
    QString     szColor;
    QColor      cTextColor;

    if (destObject != 0 && newColor.isValid())  {
        QPalette    palLabel = destObject->palette();
        palLabel.setColor(destObject->backgroundRole(), newColor);
        // Calcola un colore opportuno per il Testo della Label
        cTextColor = getIdealTextColor(newColor);
        palLabel.setColor(destObject->foregroundRole(), cTextColor);
        destObject->setAutoFillBackground(true);
        destObject->setPalette(palLabel);
        // Update Label content
        szColor = newColor.name();
        szColor.remove(szSHARP);
        szColor = szColor.toUpper();
        destObject->setText(szColor);
    }
}
bool TrendEditor::checkFields()
// Controllo del contenuto dei campi
{
    bool            fRes = false;
    bool            fOk = false;
    bool            fVisible = false;
    int             nRow = 0;
    QStringList     lstTokens;
    double          dblMin = 0;
    double          dblMax = 0;

    // Combo Orientation
    if(ui->cboOrientation->currentIndex() < 0)
    {
        QMessageBox::critical(0,trUtf8("Error"),trUtf8("'Orientation' must be selected"));
        goto exitCheck;
    }
    for (nRow = nTrk1; nRow <= nTrk4; nRow++)  {
        fOk = iface2Tokens(lstTokens, nRow);
        // Acquisizione valore check Visible
        fVisible = lstTokens[nTrendVisible] == szTRUE;
        // Variable Name
        if (fVisible && lstTokens[nTrendVarName].isEmpty())  {
            m_szMsg = tr("Track: %1. If Visible associated Variable must be selected") .arg(nRow);
            QMessageBox::critical(0,trUtf8("Error"),m_szMsg);
            goto exitCheck;
        }
        // Trend Color
        if (fVisible && lstTokens[nTrendColor].isEmpty())  {
            m_szMsg = tr("Track: %1. If Visible associated Display Color must be selected") .arg(nRow);
            QMessageBox::critical(0,trUtf8("Error"),m_szMsg);
            goto exitCheck;
        }
        // Trend Min
        dblMin = lstTokens[nTrendMin].toDouble(&fOk);
        if (fVisible && ! fOk)  {
            m_szMsg = tr("Track: %1. If Visible associated Min Value must be specified") .arg(nRow);
            QMessageBox::critical(0,trUtf8("Error"),m_szMsg);
            goto exitCheck;
        }
        // Trend Max
        dblMax = lstTokens[nTrendMax].toDouble(&fOk);
        if (fVisible && ! fOk)  {
            m_szMsg = tr("Track: %1. If Visible associated Max Value must be specified") .arg(nRow);
            QMessageBox::critical(0,trUtf8("Error"),m_szMsg);
            goto exitCheck;
        }
        // Trend Min > Trend Max
        if (fVisible && dblMin >= dblMax)  {
            m_szMsg = tr("Track: %1. If Visible associated Max Value must be greater than Min Value") .arg(nRow);
            QMessageBox::critical(0,trUtf8("Error"),m_szMsg);
            goto exitCheck;
        }
    }
    //------------------------
    // All check are Ok, return true
    //------------------------
    fRes = true;

exitCheck:
    return fRes;

}

bool TrendEditor::updateTemplateFile(const QString &szTrendFile)
// Aggiorna la sezione customtrends del file template.pri
{
    bool        fRes = true;
    bool        fAdded = false;
    QFile       fTemplate(m_szTemplateFile);
    QString     szLine;
    QStringList lstSourceRows;
    QStringList lstDestRows;
    int         nRow = 0;

    if (fTemplate.exists())  {
        lstSourceRows.clear();
        lstDestRows.clear();
        // Lettura del file Template
        fTemplate.open(QIODevice::ReadOnly | QIODevice::Text);
        if (fTemplate.isOpen())  {
            QTextStream streamTemplate(&fTemplate);
            lstSourceRows.clear();
            // Read entire file
            while (! streamTemplate.atEnd())  {
                szLine = streamTemplate.readLine(LINE_SIZE);
                lstSourceRows.append(szLine);
            }
            fTemplate.close();
            // Search Template row (Must already exist)
            szLine.clear();
            for (nRow = 0; nRow < lstSourceRows.count(); nRow++)  {
                szLine = lstSourceRows[nRow];
                // Ricerca della selezione
                if (szLine.contains(szTrendSection))  {
                    // Join splitted rows
                    while (szLine.endsWith(chBACKSLASH))  {
                        szLine.remove(chBACKSLASH);
                        nRow++;
                        if (nRow < lstSourceRows.count())  {
                            szLine.append(lstSourceRows[nRow].trimmed());
                        }
                    }
                    // qDebug() << tr("Template before: %1") .arg(szLine);
                    if (! szLine.contains(szTrendFile))  {
                        szLine.append(szSpace(1));
                        szLine.append(szTrendFile);
                        // qDebug() << tr("Template after: %1") .arg(szLine);
                        fAdded = true;
                    }
                }
                lstDestRows.append(szLine);
            }
            // Rewrite of file
            if (fAdded)  {
                // Backup file
                fileBackUp(m_szTemplateFile);
                // Rewrite Updated file
                fTemplate.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text);
                if (fTemplate.isOpen())  {
                    QTextStream streamOut(&fTemplate);
                    for (nRow = 0; nRow < lstDestRows.count(); nRow ++)  {
                        streamOut << lstDestRows[nRow] << endl;
                    }
                    fTemplate.close();
                }
                else
                    fRes = false;
            }
        }
        else
            fRes = false;

    }
    // Return value
    return fRes;
}
QString TrendEditor::currentTrendFile()
{
    return (m_szTrendPath + m_szTrendFile);
}
bool TrendEditor::isModified()
// Compares Interface values with source values in lstSourceRows
{
    QStringList     lstIfaceRows;
    int             nRow = 0;
    bool            fEqual = true;


    // Prepare Interface Fields
    iface2MemList(lstIfaceRows);
    for (nRow = 0; nRow < lstIfaceRows.count(); nRow ++)  {
        if (nRow >= lstSourceRows.count() || lstIfaceRows[nRow] != lstSourceRows[nRow]) {
            // qDebug() << tr("Source: %1 Interface: %2") .arg(lstSourceRows[nRow]) .arg(lstIfaceRows[nRow]);
            fEqual = false;
            break;
        }
    }
    // qDebug() << tr("Trends Modified (Rows %2):  %1") .arg(!fEqual) .arg(nRow);
    return ! fEqual;
}
void    TrendEditor::iface2MemList(QStringList &lstMemVars)
// Copy Interface Objects to Memory Var List
{
    QString         szLine;
    QStringList     lstTokens;
    int             nRow = 0;
    bool            fOk = false;

    lstMemVars.clear();
    // Trend Orientation
    szLine = mapOrientation.key(ui->cboOrientation->currentText());
    if (szLine.isEmpty())
        szLine = szLANDSCAPE;
    lstMemVars.append(szLine);
    // Trend Fields
    for (nRow = nTrk1; nRow <= nTrk4; nRow++)  {
        fOk = iface2Tokens(lstTokens, nRow);
        if (fOk)  {
            szLine = lstTokens.join(szSEMICOL);
            lstMemVars.append(szLine);
        }
        else
            break;
    }
}
void TrendEditor::saveTrend(bool notifUser)
{
    QString         szDestFile = currentTrendFile();

    QFile fTrend(szDestFile);
    if (fTrend.exists())  {
        fileBackUp(szDestFile);
    }
    // Write File
    if (! iface2TrendFile(szDestFile))  {
        m_szMsg = tr("Error Saving Trend File:\n%1") .arg(m_szTrendFile);
        warnUser(this, szMectTitle, m_szMsg);
    }
    else  {
        if (notifUser)  {
            m_szMsg = tr("Trend file has been successfully Saved:\nFile: %1") .arg(m_szTrendFile);
            notifyUser(this, szMectTitle, m_szMsg);
        }
    }
}
void TrendEditor::fillTrendsCombo(const QString szTrendsPath)
// Refresh-Refill combo of trend files
{
    int             nPos = -1;
    int             i = 0;
    bool            oldState = ui->cboTrendName->blockSignals(true);
    QDir            dirTrends(szTrendsPath);
    QStringList     lstTrendFilters;
    QFileInfoList   lstInfoTrendFiles;
    QString         szCurrentTrend = m_szTrendFile;

    // Preparazione della Lista di file trends presente in directory
    lstTrendFilters.clear();
    lstInfoTrendFiles.clear();
    if (dirTrends.exists())  {
        lstTrendFilters << szTRENDMASK;
        lstInfoTrendFiles = dirTrends.entryInfoList(lstTrendFilters, QDir::Files | QDir::NoDotAndDotDot | QDir::Writable, QDir::Name);
    }

    // Memorizza posizione del Trend corrente nella Combo
    szCurrentTrend = m_szTrendFile;
    szCurrentTrend = szCurrentTrend.replace(szTrendExt, szEMPTY);
    nPos = ui->cboTrendName->currentIndex();
    // qDebug() << tr("Prev Trend Index: %1") .arg(nPos);
    // caricamento dei nomi files nella combo
    ui->cboTrendName->clear();
    for (i = 0; i < lstInfoTrendFiles.count(); i ++)  {
        QFileInfo fInfo = lstInfoTrendFiles[i];
        ui->cboTrendName->addItem(fInfo.completeBaseName());
    }
    // Riattiva Segnali Combo
    ui->cboTrendName->blockSignals(oldState);
    // Ricerca trend corrente nella combo ed eventualmente riapre il file
    if (nPos < 0 || nPos != ui->cboTrendName->findText(szCurrentTrend))  {
        nPos = ui->cboTrendName->findText(szCurrentTrend);
        // qDebug() << tr("Current Trend Index: %1") .arg(nPos);
        on_cboTrendName_currentIndexChanged(nPos);
    }

}

void TrendEditor::on_cboTrendName_currentIndexChanged(int index)
{
    QString szSourceFile = m_szTrendPath;

    // Selezionato un nuovo trend
    if (index >= 0 && index < ui->cboTrendName->count())  {
        szSourceFile.append(ui->cboTrendName->currentText());
        szSourceFile.append(szTrendExt);
        QFile fTrend(szSourceFile);
        if (fTrend.exists())  {
            if (trendFile2Iface(szSourceFile))  {
                m_szTrendFile = ui->cboTrendName->currentText().trimmed() + szTrendExt;
            }
        }
    }
    qDebug() << tr("Combo Trend Index: %1 File Trend: %2") .arg(index) .arg(m_szTrendFile);
    // Abilitazione bottone di Save se esiste un Trend corrente
    ui->cmdSave->setEnabled(ui->cboTrendName->currentIndex() >= 0);
}
