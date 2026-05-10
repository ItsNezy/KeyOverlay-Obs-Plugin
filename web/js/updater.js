/* ═══════════════════════════════════════════════════════
   KeyOverlay — Update Checker
   Checks the GitHub Releases API for a newer version
   and shows a banner in the editor if one is available.
   ═══════════════════════════════════════════════════════ */
const Updater = {
    CURRENT_VERSION: '1.0.0',
    REPO: 'Rubim1/KeyOverlay-Obs-Plugin',
    CHECK_INTERVAL: 60 * 60 * 1000, // 1 hour

    init() {
        this.check();
        setInterval(() => this.check(), this.CHECK_INTERVAL);
    },

    async check() {
        try {
            const resp = await fetch(
                `https://api.github.com/repos/${this.REPO}/releases/latest`,
                { cache: 'no-cache' }
            );
            if (!resp.ok) return;

            const data = await resp.json();
            const latestTag = (data.tag_name || '').replace(/^v/, '');
            if (!latestTag) return;

            if (this.isNewer(latestTag, this.CURRENT_VERSION)) {
                this.showBanner(latestTag, data.html_url);
            }
        } catch (e) {
            // Network error — silently ignore
        }
    },

    /** Returns true if `a` is semantically greater than `b` (semver). */
    isNewer(a, b) {
        const pa = a.split('.').map(Number);
        const pb = b.split('.').map(Number);
        for (let i = 0; i < 3; i++) {
            if ((pa[i] || 0) > (pb[i] || 0)) return true;
            if ((pa[i] || 0) < (pb[i] || 0)) return false;
        }
        return false;
    },

    showBanner(version, url) {
        const banner = document.getElementById('update-banner');
        if (!banner) return;
        banner.innerHTML = `
            <span>🚀</span>
            <span>Update <strong>v${version}</strong> available!</span>
            <a href="${url}" target="_blank" rel="noopener">Download →</a>
        `;
        banner.classList.add('visible');
    }
};
