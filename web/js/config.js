window.StorageUtil = {
    get(key) {
        try {
            const localValue = localStorage.getItem(key);
            if (localValue !== null) {
                return localValue;
            }
        } catch (e) { /* ignore */ }

        const cookiePrefix = `${encodeURIComponent(key)}=`;
        const cookies = document.cookie ? document.cookie.split('; ') : [];
        for (const cookie of cookies) {
            if (cookie.startsWith(cookiePrefix)) {
                return decodeURIComponent(cookie.slice(cookiePrefix.length));
            }
        }
        return null;
    },

    set(key, value, days = 365) {
        try {
            localStorage.setItem(key, value);
        } catch (e) { /* ignore */ }

        const expires = new Date(Date.now() + (days * 24 * 60 * 60 * 1000)).toUTCString();
        document.cookie = `${encodeURIComponent(key)}=${encodeURIComponent(value)}; expires=${expires}; path=/; SameSite=Lax`;
    },

    remove(key) {
        try {
            localStorage.removeItem(key);
        } catch (e) { /* ignore */ }

        document.cookie = `${encodeURIComponent(key)}=; expires=Thu, 01 Jan 1970 00:00:00 GMT; path=/; SameSite=Lax`;
    }
};

const Config = {
    defaultConfig: {
        theme: 'dark',
        layout: '60',
        scale: 1.0,
        position: { y: 'bottom', x: 'center-x' },
        colors: {
            bg: 'transparent',
            keyBg: '',
            keyText: '',
            keyActive: ''
        }
    },

    current: null,

    load() {
        // Try query params first
        const params = new URLSearchParams(window.location.search);
        if (params.has('config')) {
            try {
                this.current = this._deepMerge(this.defaultConfig, JSON.parse(decodeURIComponent(params.get('config'))));
                this.apply();
                return;
            } catch (e) {
                console.error("Failed to parse config from URL", e);
            }
        }

        // Try local storage
        const saved = window.StorageUtil.get('keyoverlay_config');
        if (saved) {
            try {
                this.current = this._deepMerge(this.defaultConfig, JSON.parse(saved));
            } catch(e) {
                this.current = JSON.parse(JSON.stringify(this.defaultConfig));
            }
        } else {
            this.current = JSON.parse(JSON.stringify(this.defaultConfig));
        }
        
        this.apply();
    },

    save() {
        window.StorageUtil.set('keyoverlay_config', JSON.stringify(this.current));
    },

    update(newConfig) {
        this.current = this._deepMerge(this.current, newConfig);
        this.save();
        this.apply();
    },

    apply() {
        if (!this.current) return;
        
        // Apply theme
        document.body.setAttribute('data-theme', this.current.theme);

        // Apply custom colors ON BODY so they override theme styles
        // (theme uses body[data-theme] selector, inline style on body wins)
        if (this.current.colors.keyBg) {
            document.body.style.setProperty('--key-bg', this.current.colors.keyBg);
        } else {
            document.body.style.removeProperty('--key-bg');
        }
        
        if (this.current.colors.keyText) {
            document.body.style.setProperty('--key-text', this.current.colors.keyText);
        } else {
            document.body.style.removeProperty('--key-text');
        }
        
        if (this.current.colors.keyActive) {
            document.body.style.setProperty('--key-active', this.current.colors.keyActive);
        } else {
            document.body.style.removeProperty('--key-active');
        }

        // Apply scale
        document.documentElement.style.setProperty('--scale', this.current.scale);

        // Apply position
        const overlay = document.getElementById('overlay-container');
        if (overlay) {
            // Strip old pos- classes
            const classes = [...overlay.classList].filter(c => !c.startsWith('pos-'));
            overlay.className = classes.join(' ');
            overlay.classList.add(`pos-${this.current.position.y}`);
            overlay.classList.add(`pos-${this.current.position.x}`);

            // Update transform-origin based on position
            const wrapper = document.getElementById('keyboard-wrapper');
            if (wrapper) {
                const originX = this.current.position.x === 'left' ? 'left' : this.current.position.x === 'right' ? 'right' : 'center';
                const originY = this.current.position.y === 'top' ? 'top' : this.current.position.y === 'bottom' ? 'bottom' : 'center';
                wrapper.style.transformOrigin = `${originX} ${originY}`;
            }
        }

        // Load layout if changed
        if (Keyboard.currentLayoutName !== this.current.layout) {
            Keyboard.loadLayout(this.current.layout);
        }
    },

    _deepMerge(target, source) {
        const result = JSON.parse(JSON.stringify(target));
        for (const key of Object.keys(source)) {
            if (source[key] && typeof source[key] === 'object' && !Array.isArray(source[key])) {
                result[key] = this._deepMerge(result[key] || {}, source[key]);
            } else {
                result[key] = source[key];
            }
        }
        return result;
    }
};
