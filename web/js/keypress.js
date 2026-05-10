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
    }
};
