#include "dock-panel.hpp"
#include <QWebEngineView>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QUrl>
#include <obs-frontend-api.h>
#include <obs-module.h>
#include <util/config-file.h>

DockPanel::DockPanel(QWidget* parent) : QDockWidget(parent) {
    setObjectName("KeyOverlayDock");
    setWindowTitle("KeyOverlay");
    setMinimumSize(320, 480);

    auto* contentWidget = new QWidget(this);
    auto* mainLayout = new QVBoxLayout(contentWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Toolbar layout
    auto* toolbarLayout = new QHBoxLayout();
    toolbarLayout->setContentsMargins(4, 4, 4, 4);

    urlBar_ = new QLineEdit(contentWidget);
    urlBar_->setPlaceholderText("Enter overlay URL...");
    
    resetBtn_ = new QPushButton("Reset", contentWidget);

    toolbarLayout->addWidget(urlBar_);
    toolbarLayout->addWidget(resetBtn_);

    // Web view
    webView_ = new QWebEngineView(contentWidget);

    mainLayout->addLayout(toolbarLayout);
    mainLayout->addWidget(webView_);

    setWidget(contentWidget);

    connect(urlBar_, &QLineEdit::returnPressed, this, &DockPanel::onUrlChanged);
    connect(resetBtn_, &QPushButton::clicked, this, &DockPanel::onResetUrl);
}

DockPanel::~DockPanel() {
}

void DockPanel::init() {
    obs_frontend_add_dock_by_id("KeyOverlayDock", "KeyOverlay", this);
    
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
    webView_->setUrl(QUrl(url));
}
