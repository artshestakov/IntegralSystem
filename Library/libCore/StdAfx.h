#pragma once
#ifndef _STDAFX_H_INCLUDED
#define _STDAFX_H_INCLUDED
//-----------------------------------------------------------------------------
#include <cstdio>
#include <iostream>
#include <csignal>
#include <limits>
#include <string>
#include <map>
//-----------------------------------------------------------------------------
#ifdef WIN32
#include "windows.h"
#else
#include <float.h>
#endif
//-----------------------------------------------------------------------------
#include <QtCore/QtGlobal>
#include <QtCore/QObject>
#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QDateTime>
#include <QtCore/QThread>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QChar>
#include <QtCore/QString>
#include <QtCore/QLatin1String>
#include <QtCore/QStringList>
#include <QtCore/QUuid>
#include <QtCore/QVector>
#include <QtCore/QMap>
#include <QtCore/QIODevice>
#include <QtCore/QByteArray>
#include <QtCore/QBuffer>
#include <QtCore/QSize>
#include <QtCore/QList>
#include <QtCore/QMargins>
#include <QtCore/QTextStream>
#include <QtCore/QRegExp>
#include <QtCore/QModelIndex>
#include <QtCore/QMetaType>
#include <QtCore/QMetaObject>
#include <QtCore/QMetaProperty>
#include <QtCore/QLibrary>
#include <QtCore/QVariant>
#include <QtCore/QVariantMap>
#include <QtCore/QVariantList>
#include <QtCore/QRect>
#include <QtCore/QTimer>
#include <QtCore/QEvent>
#include <QtCore/QPoint>
#include <QtCore/QPointF>
#include <QtCore/QDir>
#include <QtCore/QUrl>
#include <QtCore/QProcess>
#include <QtCore/QProcessEnvironment>
#include <QtCore/QMimeData>
#include <QtCore/QTextCodec>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QCryptographicHash>
#include <QtCore/QEventLoop>
#include <QtCore/QFuture>
#include <QtCore/QFutureWatcher>
#include <QtCore/QSettings>
#include <QtCore/QSizeF>
#include <QtCore/QMarginsF>
#include <QtCore/QMutex>
#include <QtCore/QQueue>
#include <QtCore/QTranslator>
#include <QtCore/QAbstractItemModel>
#include <QtCore/QItemSelectionModel>
#include <QtCore/QPropertyAnimation>
#include <QtCore/QStandardPaths>
#include <QtCore/QSharedMemory>
#include <QtCore/QSystemSemaphore>
//-----------------------------------------------------------------------------
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQueryModel>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlError>
#include <QtSql/QSqlDriver>
#include <QtSql/QSqlField>
//-----------------------------------------------------------------------------
#include <QtXml/QDomElement>
#include <QtXml/QDomNode>
#include <QtXml/QDomDocument>
#include <QtXml/QDomNamedNodeMap>
//-----------------------------------------------------------------------------
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QSslSocket>
#include <QtNetwork/QHostAddress>
//-----------------------------------------------------------------------------
#include <QtConcurrent/QtConcurrent>
//-----------------------------------------------------------------------------
//#ifdef GUI
//-----------------------------------------------------------------------------
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>
#include <QtWidgets/QLayout>
#include <QtWidgets/QBoxLayout>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QLabel>
#include <QtWidgets/QFrame>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QAction>
#include <QtWidgets/QActionGroup>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QScrollBar>
#include <QtWidgets/QSplashScreen>
#include <QtWidgets/QSizePolicy>
#include <QtWidgets/QTableView>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QMenu>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QListWidgetItem>
#include <QtWidgets/QStyledItemDelegate>
#include <QtWidgets/QStyle>
#include <QtWidgets/QDateEdit>
#include <QtWidgets/QCalendarWidget>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QDateTimeEdit>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QTreeWidgetItem>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QDesktopWidget>
#include <QtWidgets/QCompleter>
#include <QtWidgets/QDialog>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QTabBar>
#include <QtWidgets/QProgressDialog>
#include <QtWidgets/QFileIconProvider>
#include <QtWidgets/QSystemTrayIcon>
#include <QtWidgets/QToolTip>
#include <QtWidgets/QColorDialog>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QCommandLinkButton>
#include <QtWidgets/QAbstractItemView>
#include <QtWidgets/QWidgetAction>
#include <QtWidgets/QErrorMessage>
//-----------------------------------------------------------------------------
#include <QtGui/QKeyEvent>
#include <QtGui/QResizeEvent>
#include <QtGui/QShowEvent>
#include <QtGui/QMouseEvent>
#include <QtGui/QPaintEvent>
#include <QtGui/QCloseEvent>
#include <QtGui/QWheelEvent>
#include <QtGui/QDragLeaveEvent>
#include <QtGui/QDragEnterEvent>
#include <QtGui/QDropEvent>
#include <QtGui/QDragMoveEvent>
#include <QtGui/QFocusEvent>
#include <QtGui/QContextMenuEvent>
#include <QtGui/QPixmap>
#include <QtGui/QFont>
#include <QtGui/QIcon>
#include <QtGui/QColor>
#include <QtGui/QCursor>
#include <QtGui/QPalette>
#include <QtGui/QBrush>
#include <QtGui/QPainter>
#include <QtGui/QValidator>
#include <QtGui/QRegExpValidator>
#include <QtGui/QIntValidator>
#include <QtGui/QKeySequence>
#include <QtGui/QClipboard>
#include <QtGui/QDesktopServices>
#include <QtGui/QPen>
#include <QtGui/QTextCursor>
#include <QtGui/QTextDocument>
#include <QtGui/QMovie>
#include <QtGui/QAbstractTextDocumentLayout>
#include <QtGui/QPageSize>
#include <QtGui/QFontMetrics>
#include <QtGui/QTextCharFormat>
#include <QtGui/QImageReader>
#include <QtGui/QEnterEvent>
#include <QtGui/QGuiApplication>
#include <QtGui/QScreen>
#include <QtGui/QTextFrame>
#include <QtGui/QBitmap>
//-----------------------------------------------------------------------------
#include <QtPrintSupport/QPrinter>
#include <QtPrintSupport/QPrintPreviewDialog>
#include <QtPrintSupport/QPrintDialog>
//-----------------------------------------------------------------------------
#include <QtMultimedia/QMediaPlayer>
#include <QtMultimedia/QSound>
//-----------------------------------------------------------------------------
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
//-----------------------------------------------------------------------------
//#endif
//-----------------------------------------------------------------------------
#endif
