#pragma once

#include <QDockWidget>
#include <QTimer>

class QLineEdit;
class QPushButton;
class QLabel;
class WsServer;

class DockPanel : public QDockWidget {
    Q_OBJECT
public:
    explicit DockPanel(WsServer* wsServer, QWidget* parent = nullptr);
    ~DockPanel();

    void init();

private slots:
    void onUrlChanged();
    void onResetUrl();
    void onCopyUrl();
    void updateStatus();

private:
    WsServer* wsServer_;
    QLineEdit* urlBar_ = nullptr;
    QPushButton* resetBtn_ = nullptr;
    QPushButton* copyBtn_ = nullptr;
    QLabel* infoLabel_ = nullptr;
    QLabel* statusLabel_ = nullptr;
    QTimer* statusTimer_ = nullptr;

    QString getSavedUrl() const;
    void saveUrl(const QString& url);
    void loadUrl(const QString& url);
};
