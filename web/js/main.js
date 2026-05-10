document.addEventListener('DOMContentLoaded', () => {
    const urlParams = new URLSearchParams(window.location.search);
    const isOverlay = urlParams.get('overlay') === 'true';

    // Init Config (loads from localStorage/URL and renders keyboard)
    Config.load();

    // Init Keypress listening
    Keypress.init();

    if (isOverlay) {
        // OBS / Preview mode — hide editor, transparent bg
        document.getElementById('editor-container').classList.add('hidden');
        document.body.classList.add('overlay-mode');
    } else {
        // Editor mode (DEFAULT)
        document.getElementById('editor-container').classList.remove('hidden');
        document.body.classList.add('editor-mode');
        Editor.init();
        KeyEditor.init();
    }
});
