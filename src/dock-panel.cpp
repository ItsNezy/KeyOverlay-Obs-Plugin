#include "dock-panel.hpp"
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QUrl>
#include <QApplication>
#include <QClipboard>
#include <obs-frontend-api.h>
#include <obs-module.h>

DockPanel::DockPanel(QWidget* parent) : QDockWidget(parent) {
    setObjectName("KeyOverlayDock");
    setWindowTitle("KeyOverlay Settings");
    setMinimumSize(320, 200);

    auto* contentWidget = new QWidget(this);
    auto* mainLayout = new QVBoxLayout(contentWidget);
    mainLayout->setContentsMargins(16, 16, 16, 16);
    mainLayout->setSpacing(12);

    infoLabel_ = new QLabel("To use KeyOverlay, add a Browser Source in OBS and paste this URL:", contentWidget);
    infoLabel_->setWordWrap(true);
    mainLayout->addWidget(infoLabel_);

    auto* urlLayout = new QHBoxLayout();
    urlBar_ = new QLineEdit(contentWidget);
    urlBar_->setPlaceholderText("Enter overlay URL...");
    copyBtn_ = new QPushButton("Copy", contentWidget);
    
    urlLayout->addWidget(urlBar_);
    urlLayout->addWidget(copyBtn_);
    mainLayout->addLayout(urlLayout);

    resetBtn_ = new QPushButton("Reset to Default URL", contentWidget);
    mainLayout->addWidget(resetBtn_);
    mainLayout->addStretch();

    setWidget(contentWidget);

    connect(urlBar_, &QLineEdit::returnPressed, this, &DockPanel::onUrlChanged);
    connect(resetBtn_, &QPushButton::clicked, this, &DockPanel::onResetUrl);
    connect(copyBtn_, &QPushButton::clicked, this, &DockPanel::onCopyUrl);
}

DockPanel::~DockPanel() {
}

void DockPanel::init() {
    obs_frontend_add_dock(this);
    
    QString savedUrl = getSavedUrl();
    if (!savedUrl.isEmpty()) {
        loadUrl(savedUrl);
    } else {
        loadUrl(getLocalUrl());
    }
}

void DockPanel::onUrlChanged() {
    QString url = urlBar_->text().trimmed();
    if (url.isEmpty()) return;

    saveUrl(url);
    loadUrl(url);
    blog(LOG_INFO, "[KeyOverlay] UI URL changed to: %s", url.toUtf8().constData());
}

void DockPanel::onResetUrl() {
    config_set_string(obs_frontend_get_global_config(), "KeyOverlay", "UIUrl", "");
    QString localUrl = getLocalUrl();
    loadUrl(localUrl);
    blog(LOG_INFO, "[KeyOverlay] UI URL reset to local default");
}

void DockPanel::onCopyUrl() {
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(urlBar_->text());
}

QString DockPanel::getLocalUrl() const {
    char* pluginDataPath = obs_module_file("ui/index.html");
    QString path = QString::fromUtf8(pluginDataPath);
    bfree(pluginDataPath);
    return QUrl::fromLocalFile(path).toString();
}

QString DockPanel::getSavedUrl() const {
    const char* saved = config_get_string(obs_frontend_get_global_config(), "KeyOverlay", "UIUrl");
    if (saved && saved[0] != '\0') {
        return QString::fromUtf8(saved);
    }
    return QString();
}

void DockPanel::saveUrl(const QString& url) {
    config_set_string(obs_frontend_get_global_config(), "KeyOverlay", "UIUrl", url.toUtf8().constData());
}

void DockPanel::loadUrl(const QString& url) {
    urlBar_->setText(url);
}
