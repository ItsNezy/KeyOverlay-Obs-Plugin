const Editor = {
    init() {
        this.populateValues();
        this.bindEvents();
        this.initCopyModal();
    },

    buildShareUrl() {
        const url = new URL(window.location.href);
        for (const key of [...url.searchParams.keys()]) {
            url.searchParams.delete(key);
        }

        url.searchParams.set('overlay', 'true');
        url.searchParams.set('config', encodeURIComponent(JSON.stringify(Config.current)));

        const layoutPayload = typeof Keyboard !== 'undefined' ? Keyboard.getShareLayoutPayload() : null;
        if (layoutPayload) {
            url.searchParams.set('layoutName', layoutPayload.layoutName);
            url.searchParams.set('layoutData', encodeURIComponent(JSON.stringify(layoutPayload.layoutData)));
        }

        return url.toString();
    },

    populateValues() {
        const c = Config.current;
        document.getElementById('theme-select').value = c.theme;
        document.getElementById('layout-select').value = c.layout;
        document.getElementById('scale-slider').value = c.scale;
        document.getElementById('scale-value').innerText = c.scale.toFixed(1) + 'x';
        document.getElementById('pos-y').value = c.position.y;
        document.getElementById('pos-x').value = c.position.x;

        const rootStyles = getComputedStyle(document.documentElement);
        document.getElementById('color-bg').value = this.toHex(c.colors.bg) || '#000000';
        document.getElementById('color-key-bg').value = this.toHex(c.colors.keyBg || rootStyles.getPropertyValue('--key-bg').trim());
        document.getElementById('color-text').value = this.toHex(c.colors.keyText || rootStyles.getPropertyValue('--key-text').trim());
        document.getElementById('color-active').value = this.toHex(c.colors.keyActive || rootStyles.getPropertyValue('--key-active').trim());
    },

    bindEvents() {
        document.getElementById('theme-select').addEventListener('change', (e) => {
            Config.update({ theme: e.target.value, colors: { bg: Config.current.colors.bg, keyBg: '', keyText: '', keyActive: '' } });
            setTimeout(() => this.populateValues(), 50);
        });

        document.getElementById('layout-select').addEventListener('change', (e) => {
            Config.update({ layout: e.target.value });
        });

        document.getElementById('scale-slider').addEventListener('input', (e) => {
            const val = parseFloat(e.target.value);
            document.getElementById('scale-value').innerText = val.toFixed(1) + 'x';
            Config.update({ scale: val });
        });

        document.getElementById('pos-y').addEventListener('change', () => {
            Config.update({ position: { y: document.getElementById('pos-y').value, x: document.getElementById('pos-x').value } });
        });
        document.getElementById('pos-x').addEventListener('change', () => {
            Config.update({ position: { y: document.getElementById('pos-y').value, x: document.getElementById('pos-x').value } });
        });

        document.getElementById('color-bg').addEventListener('input', (e) => {
            Config.update({ colors: { bg: e.target.value } });
        });
        document.getElementById('color-key-bg').addEventListener('input', (e) => {
            Config.update({ colors: { keyBg: e.target.value } });
        });
        document.getElementById('color-text').addEventListener('input', (e) => {
            Config.update({ colors: { keyText: e.target.value } });
        });
        document.getElementById('color-active').addEventListener('input', (e) => {
            Config.update({ colors: { keyActive: e.target.value } });
        });

        document.getElementById('btn-preview').addEventListener('click', () => {
            window.open(this.buildShareUrl(), '_blank');
        });

        document.getElementById('btn-copy-url').addEventListener('click', () => {
            const shareUrl = this.buildShareUrl();

            navigator.clipboard.writeText(shareUrl).then(() => {
                this.showCopyModal();
            }).catch(() => {
                prompt('Copy this URL:', shareUrl);
            });
        });
    },

    /* ── Copy URL Instruction Modal ──────────────── */
    initCopyModal() {
        const overlay = document.getElementById('copy-modal');
        const closeBtn = document.getElementById('copy-modal-close');
        if (!overlay || !closeBtn) return;

        closeBtn.addEventListener('click', () => {
            overlay.classList.remove('visible');
        });
        overlay.addEventListener('click', (e) => {
            if (e.target === overlay) overlay.classList.remove('visible');
        });
    },

    showCopyModal() {
        const overlay = document.getElementById('copy-modal');
        if (overlay) overlay.classList.add('visible');
    },

    /* ── WebSocket Connection Status ─────────────── */
    updateWsStatus(connected) {
        const dot = document.getElementById('ws-dot');
        const text = document.getElementById('ws-status-text');
        if (!dot || !text) return;

        if (connected) {
            dot.classList.add('connected');
            text.textContent = 'Connected to OBS';
            text.style.color = '#86efac';
        } else {
            dot.classList.remove('connected');
            text.textContent = 'Disconnected';
            text.style.color = '#71717a';
        }
    },

    toHex(val) {
        if (!val) return null;
        val = val.trim();
        if (val.startsWith('#')) return val.length === 4 ? '#' + val[1] + val[1] + val[2] + val[2] + val[3] + val[3] : val;
        if (val === 'transparent' || val === 'rgba(0, 0, 0, 0)') return '#000000';

        const match = val.match(/rgba?\((\d+),\s*(\d+),\s*(\d+)/);
        if (!match) return '#000000';

        const r = parseInt(match[1], 10).toString(16).padStart(2, '0');
        const g = parseInt(match[2], 10).toString(16).padStart(2, '0');
        const b = parseInt(match[3], 10).toString(16).padStart(2, '0');
        return `#${r}${g}${b}`;
    }
};
