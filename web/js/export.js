document.addEventListener('DOMContentLoaded', () => {
    const btnExport = document.getElementById('btn-export');
    const btnImport = document.getElementById('file-import');

    if (btnExport) {
        btnExport.addEventListener('click', () => {
            const exportPayload = {
                config: Config.current,
                customLayout: typeof Keyboard !== 'undefined' ? Keyboard.getShareLayoutPayload() : null
            };

            const dataStr = 'data:text/json;charset=utf-8,' + encodeURIComponent(JSON.stringify(exportPayload, null, 2));
            const downloadAnchorNode = document.createElement('a');
            downloadAnchorNode.setAttribute('href', dataStr);
            downloadAnchorNode.setAttribute('download', 'keyoverlay_config.json');
            document.body.appendChild(downloadAnchorNode);
            downloadAnchorNode.click();
            downloadAnchorNode.remove();
        });
    }

    if (btnImport) {
        btnImport.addEventListener('change', (e) => {
            const file = e.target.files[0];
            if (!file) return;

            const reader = new FileReader();
            reader.onload = (loadEvent) => {
                try {
                    const parsed = JSON.parse(loadEvent.target.result);
                    const importedConfig = parsed.config || parsed;

                    if (parsed.customLayout?.layoutName && parsed.customLayout?.layoutData) {
                        const storageKey = Keyboard.getCustomLayoutStorageKey(parsed.customLayout.layoutName);
                        window.StorageUtil.set(storageKey, JSON.stringify(parsed.customLayout.layoutData));
                    }

                    Config.update(importedConfig);
                    if (typeof Editor !== 'undefined') {
                        Editor.populateValues();
                    }
                    alert('Configuration imported successfully!');
                } catch (err) {
                    console.error('Error parsing JSON', err);
                    alert('Invalid JSON file.');
                }
            };
            reader.readAsText(file);
        });
    }
});
