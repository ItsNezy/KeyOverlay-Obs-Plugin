#pragma once

#include <QDockWidget>

class QLineEdit;
class QPushButton;
class QLabel;

class DockPanel : public QDockWidget {
    Q_OBJECT
public:
    explicit DockPanel(QWidget* parent = nullptr);
    ~DockPanel();

    void init();

private slots:
    void onUrlChanged();
    void onResetUrl();
    void onCopyUrl();

private:
    QLineEdit* urlBar_ = nullptr;
    QPushButton* resetBtn_ = nullptr;
    QPushButton* copyBtn_ = nullptr;
    QLabel* infoLabel_ = nullptr;

    QString getLocalUrl() const;
    QString getSavedUrl() const;
    void saveUrl(const QString& url);
    void loadUrl(const QString& url);
};
