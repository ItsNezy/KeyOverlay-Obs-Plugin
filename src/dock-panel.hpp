#pragma once

#include <QDockWidget>

class QWebEngineView;
class QLineEdit;
class QPushButton;

class DockPanel : public QDockWidget {
    Q_OBJECT
public:
    explicit DockPanel(QWidget* parent = nullptr);
    ~DockPanel();

    void init();

private slots:
    void onUrlChanged();
    void onResetUrl();

private:
    QWebEngineView* webView_ = nullptr;
    QLineEdit* urlBar_ = nullptr;
    QPushButton* resetBtn_ = nullptr;

    QString getLocalUrl() const;
    QString getSavedUrl() const;
    void saveUrl(const QString& url);
    void loadUrl(const QString& url);
};
