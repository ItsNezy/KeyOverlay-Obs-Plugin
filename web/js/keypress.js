const Keypress = {
    activeCodes: new Set(),

    isTypingTarget(target) {
        if (!target) return false;
        const tagName = target.tagName;
        return tagName === 'INPUT' || tagName === 'TEXTAREA' || tagName === 'SELECT' || target.isContentEditable;
    },

    highlight(code) {
        if (!code || typeof Keyboard === 'undefined') return;
        this.activeCodes.add(code);
        Keyboard.highlightKey(code, true);
    },

    release(code) {
        if (!code || typeof Keyboard === 'undefined') return;
        this.activeCodes.delete(code);
        Keyboard.highlightKey(code, false);
    },

    clearAllActiveKeys() {
        if (typeof Keyboard !== 'undefined') {
            Keyboard.clearAllHighlights();
        }
        this.activeCodes.clear();
    },

    init() {
        window.addEventListener('keydown', (e) => {
            if (this.isTypingTarget(e.target)) {
                return;
            }

            // Keep browser/OS shortcuts from sticking the overlay state.
            this.highlight(e.code);

            // In editor mode we only suppress default for normal keys,
            // not for OS/meta combos that can steal focus and skip keyup.
            const isMetaCombo = e.metaKey || e.key === 'Meta' || e.code === 'MetaLeft' || e.code === 'MetaRight';
            if (!isMetaCombo) {
                e.preventDefault();
            }
        });

        window.addEventListener('keyup', (e) => {
            this.release(e.code);
        });

        window.addEventListener('blur', () => {
            this.clearAllActiveKeys();
        });

        window.addEventListener('focus', () => {
            this.clearAllActiveKeys();
        });

        window.addEventListener('contextmenu', () => {
            this.clearAllActiveKeys();
        });

        document.addEventListener('visibilitychange', () => {
            if (document.hidden) {
                this.clearAllActiveKeys();
            }
        });

        // Connect to OBS Plugin WebSocket server for global key events
        this.connectWebSocket();
    },

    connectWebSocket() {
        const wsUrl = 'ws://127.0.0.1:9001';
        const ws = new WebSocket(wsUrl);

        ws.onopen = () => {
            console.log('[KeyOverlay] Connected to WebSocket server');
            if (typeof Editor !== 'undefined' && Editor.updateWsStatus) {
                Editor.updateWsStatus(true);
            }
        };

        ws.onmessage = (event) => {
            try {
                const data = JSON.parse(event.data);
                const code = data.key; // C++ backend sends the code in 'key' field

                if (data.type === 'keydown') {
                    this.highlight(code);
                } else if (data.type === 'keyup') {
                    this.release(code);
                }
            } catch (e) {
                console.error('[KeyOverlay] Failed to parse message', e);
            }
        };

        ws.onclose = () => {
            console.log('[KeyOverlay] WebSocket disconnected. Reconnecting in 2s...');
            if (typeof Editor !== 'undefined' && Editor.updateWsStatus) {
                Editor.updateWsStatus(false);
            }
            setTimeout(() => this.connectWebSocket(), 2000);
        };

        ws.onerror = () => {
            // Silently handle error, onclose will trigger reconnect
        };
    }
};
