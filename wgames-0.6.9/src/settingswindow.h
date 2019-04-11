#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QDialog>
#include <QString>

#ifdef Q_WS_MAEMO_5
    #include <QMaemo5ValueButton>
#else
    #include <QPushButton>
class QMaemo5ValueButton : public QPushButton
{
	Q_OBJECT
	Q_PROPERTY(QString valueText READ valueText WRITE setValueText NOTIFY valueTextChanged)

	public:
	enum ValueLayout_e
	{
    ValueUnderTextCentered
	};

	public:
		QMaemo5ValueButton(QWidget *parent = 0);
		virtual ~QMaemo5ValueButton();

		void setValueText(const QString &text);
		QString valueText() const;
		void setValueLayout(ValueLayout_e l);
		void setText(const QString &text);
		QString text() const;

Q_SIGNALS:
		void valueTextChanged(const QString &valueText);

	private:
		void SetupRenderText();

	private:
		QString sValueText;
		QString sText;
		Q_DISABLE_COPY(QMaemo5ValueButton)
};
#endif

namespace Ui {
    class SettingsWindow;
}

class SettingsWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsWindow(QWidget *parent = 0);
    ~SettingsWindow();

private slots:
    void on_saveButton_clicked();
    void on_restoreButton_clicked();
    void on_cancelButton_clicked();
    void downloadsDirButton_clicked();
    void savesDirButton_clicked();
    void binDirButton_clicked();
    void dataDirButton_clicked();

private:
    Ui::SettingsWindow *ui;
    QString _origDataDir;

#if defined(Q_WS_MAEMO_5) || defined(_HARMATTAN)
    QMaemo5ValueButton *downloadsDirButton;
    QMaemo5ValueButton *savesDirButton;
    QMaemo5ValueButton *binDirButton;
    QMaemo5ValueButton *dataDirButton;
#else
    QPushButton *downloadsDirButton;
    QPushButton *savesDirButton;
    QPushButton *binDirButton;
    QPushButton *dataDirButton;
#endif

    void LoadSettings();
    void SaveSettings();
};

#endif // SETTINGSWINDOW_H
