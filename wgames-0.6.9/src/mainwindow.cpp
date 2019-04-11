#ifdef Q_WS_MAEMO_5
#include <QtMaemo5>
#endif

#include <QDebug>
#include <QImage>
#include <QPainter>
#include <QMenuBar>
#include <QSettings>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "managewindow.h"
#include "settingswindow.h"
#include "saveswindow.h"
#include "config.h"
#include "trackercfg.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
#ifdef Q_WS_MAEMO_5
    this->setAttribute(Qt::WA_Maemo5StackedWindow);
    this->setWindowFlags(Qt::Window);
#endif

    _games = new GamesData();

    QAction * settingsAction = new QAction(tr("Settings"), this);
    connect(settingsAction, SIGNAL(triggered()), this, SLOT(Settings()));

    QMenuBar * menuBar = new QMenuBar(this);
    menuBar->addAction(settingsAction);

#ifdef _HARMATTAN
		{
			QAction *action;
			
			action = new QAction(tr("Quit"), this);
			connect(action, SIGNAL(triggered()), this, SLOT(close()));

			menuBar->addAction(action);
		}
		menuBar->setStyleSheet(
				"QMenuBar{"
				"background: rgba(255, 255, 255, 0);"
				"color: #ffffff;"
				"}"
				);
#endif

    UpdateLabels();

#ifdef Q_WS_MAEMO_5
    ui->installButton->setIcon(QIcon("/etc/hildon/theme/backgrounds/app_install_games.png"));
    ui->savesButton->setIcon(QIcon("/etc/hildon/theme/backgrounds/app_install_multimedia.png"));
#else
#ifdef _HARMATTAN
#ifdef _DBG
#define THEME_PATH "./maemofiles/theme/"
#else
#define THEME_PATH BASE_DIR"/theme/"
#endif
    ui->installButton->setIcon(QIcon(THEME_PATH"app_install_games.png"));
    ui->savesButton->setIcon(QIcon(THEME_PATH"app_install_multimedia.png"));
#else
#if 0
		QString styleSheet(
				"QPushButton{"
				"color: #ffffff;"
				"font-size: 32px;"
				"}"
				"QPushButton:hover{"
				"color: #000000;"
				"font-size: 32px;"
				"}"
				);
		ui->installButton->setStyleSheet(styleSheet);
		ui->savesButton->setStyleSheet(styleSheet);
#endif
    ui->installButton->setText(tr("Install"));
    ui->savesButton->setText(tr("Saves"));
#endif
#endif

    UpdateTracker();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::UpdateTracker()
{
    QSettings settings(ORGANIZATION_NAME,
                       APPLICATION_NAME);

    QString dataDir(settings.value("Main/DataDir",
                                   DATA_DIR_ROOT).toString());
    TrackerCfg tracker(this, dataDir, dataDir);
    tracker.Update();
}

void MainWindow::Settings()
{
    SettingsWindow * window = new SettingsWindow(this);
#ifndef _HARMATTAN
    window->show();
#else
		window->showFullScreen();
#endif
}

void MainWindow::on_installButton_clicked()
{
    ManageWindow * window = new ManageWindow(this, *_games);
#ifdef Q_WS_MAEMO_5
    window->setAttribute(Qt::WA_Maemo5StackedWindow);
    window->setWindowFlags(Qt::Window);
#endif
#ifndef _HARMATTAN
    window->show();
#else
		window->showFullScreen();
#endif
    connect(window, SIGNAL(destroyed()), this, SLOT(UpdateLabels()));
}

void MainWindow::on_savesButton_clicked()
{
    SavesWindow * window = new SavesWindow(this, *_games);
#ifdef Q_WS_MAEMO_5
    window->setAttribute(Qt::WA_Maemo5StackedWindow);
    window->setWindowFlags(Qt::Window);
#endif
#ifndef _HARMATTAN
    window->show();
#else
		window->showFullScreen();
#endif
}

void MainWindow::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.drawImage(this->rect(),
                      QImage(GRAPHICS_DIR"background.png"));
}

void MainWindow::UpdateLabels()
{
    ui->infoLabel->setText("<span style='color:#9c9c9c;'>" +
                           QString(
                               tr("%n games supported", "", _games->TotalCount()) +
                               tr(", %n games installed", "", _games->InstalledCount()))
                           + "</span>"
                           );
}
