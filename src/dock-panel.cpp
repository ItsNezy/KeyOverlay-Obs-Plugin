#include "dock-panel.hpp"
#include "wsserver.hpp"
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QApplication>
#include <QClipboard>
#include <obs-frontend-api.h>
#include <obs-module.h>
#include <util/config-file.h>

DockPanel::DockPanel(WsServer* wsServer, QWidget* parent) : QDockWidget(parent), wsServer_(wsServer) {
    setObjectName("KeyOverlayDock");
    setWindowTitle("KeyOverlay Settings");
    setMinimumSize(320, 200);

    auto* contentWidget = new QWidget(this);
    auto* mainLayout = new QVBoxLayout(contentWidget);
    mainLayout->setContentsMargins(16, 16, 16, 16);
    mainLayout->setSpacing(12);

    infoLabel_ = new QLabel("Add a Browser Source in OBS and paste this URL:", contentWidget);
    infoLabel_->setWordWrap(true);
    mainLayout->addWidget(infoLabel_);

    urlBar_ = new QLineEdit(contentWidget);
    urlBar_->setPlaceholderText("Enter overlay URL...");
    mainLayout->addWidget(urlBar_);

    auto* btnLayout = new QHBoxLayout();
    copyBtn_ = new QPushButton("Copy URL", contentWidget);
    resetBtn_ = new QPushButton("Reset to Default", contentWidget);
    btnLayout->addWidget(copyBtn_);
    btnLayout->addWidget(resetBtn_);
    mainLayout->addLayout(btnLayout);

    mainLayout->addStretch();

    statusLabel_ = new QLabel("Connected clients: 0", contentWidget);
    mainLayout->addWidget(statusLabel_);

    setWidget(contentWidget);

    connect(urlBar_, &QLineEdit::textChanged, this, &DockPanel::onUrlChanged);
    connect(resetBtn_, &QPushButton::clicked, this, &DockPanel::onResetUrl);
    connect(copyBtn_, &QPushButton::clicked, this, &DockPanel::onCopyUrl);

    statusTimer_ = new QTimer(this);
    connect(statusTimer_, &QTimer::timeout, this, &DockPanel::updateStatus);
    statusTimer_->start(2000);
}

DockPanel::~DockPanel() {
}

void DockPanel::init() {
    obs_frontend_add_dock_by_id("KeyOverlayDock", "KeyOverlay", this);
    
    QString savedUrl = getSavedUrl();
    if (!savedUrl.isEmpty()) {
        loadUrl(savedUrl);
    } else {
        loadUrl("http://127.0.0.1:9000");
    }
}

void DockPanel::onUrlChanged() {
    QString url = urlBar_->text().trimmed();
    if (url.isEmpty()) return;

    saveUrl(url);
    blog(LOG_INFO, "[KeyOverlay] UI URL changed to: %s", url.toUtf8().constData());
}

void DockPanel::onResetUrl() {
    config_set_string(obs_frontend_get_global_config(), "KeyOverlay", "UIUrl", "http://127.0.0.1:9000");
    loadUrl("http://127.0.0.1:9000");
    blog(LOG_INFO, "[KeyOverlay] UI URL reset to http://127.0.0.1:9000");
}

void DockPanel::onCopyUrl() {
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(urlBar_->text());
}

void DockPanel::updateStatus() {
    if (wsServer_) {
        statusLabel_->setText(QString("Connected clients: %1").arg(wsServer_->clientCount()));
    }
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
    bool wasBlocked = urlBar_->blockSignals(true);
    urlBar_->setText(url);
    urlBar_->blockSignals(wasBlocked);
}
