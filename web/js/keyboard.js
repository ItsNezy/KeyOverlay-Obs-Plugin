const Keyboard = {
    currentLayoutName: null,
    currentLayoutData: null,
    keyElements: {},
    selectedKey: null, // { rowIdx, keyIdx, el }
    dragSource: null,

    getCustomLayoutStorageKey(layoutName) {
        return `keyoverlay_layout_${layoutName}`;
    },

    getUrlLayoutOverride(layoutName) {
        const params = new URLSearchParams(window.location.search);
        const encodedLayoutName = params.get('layoutName');
        const encodedLayoutData = params.get('layoutData');
        if (!encodedLayoutData) {
            return null;
        }

        if (encodedLayoutName && encodedLayoutName !== layoutName) {
            return null;
        }

        try {
            return JSON.parse(decodeURIComponent(encodedLayoutData));
        } catch (e) {
            console.error('Failed to parse layoutData from URL', e);
            return null;
        }
    },

    getShareLayoutPayload() {
        if (!this.currentLayoutName || !this.currentLayoutData) {
            return null;
        }

        return {
            layoutName: this.currentLayoutName,
            layoutData: this.currentLayoutData
        };
    },

    loadLayout(layoutName) {
        if (!window.KeyLayouts || !window.KeyLayouts[layoutName]) {
            console.error('Layout not found:', layoutName);
            return;
        }
        this.currentLayoutName = layoutName;
        // Deep clone so edits don't mutate the original
        this.currentLayoutData = JSON.parse(JSON.stringify(window.KeyLayouts[layoutName]));

        const urlOverride = this.getUrlLayoutOverride(layoutName);
        if (urlOverride) {
            this.currentLayoutData = urlOverride;
            this.render(this.currentLayoutData);
            return;
        }
        
        // Check if there's a custom layout saved
        const customKey = this.getCustomLayoutStorageKey(layoutName);
        const saved = window.StorageUtil.get(customKey);
        if (saved) {
            try {
                this.currentLayoutData = JSON.parse(saved);
            } catch(e) { /* ignore */ }
        }
        
        this.render(this.currentLayoutData);
    },

    saveCustomLayout() {
        if (!this.currentLayoutName || !this.currentLayoutData) return;
        const customKey = this.getCustomLayoutStorageKey(this.currentLayoutName);
        window.StorageUtil.set(customKey, JSON.stringify(this.currentLayoutData));
    },

    clearCustomLayout(layoutName = this.currentLayoutName) {
        if (!layoutName) return;
        const customKey = this.getCustomLayoutStorageKey(layoutName);
        window.StorageUtil.remove(customKey);
    },

    resetLayout() {
        if (!this.currentLayoutName) return;
        this.clearCustomLayout(this.currentLayoutName);
        this.currentLayoutData = JSON.parse(JSON.stringify(window.KeyLayouts[this.currentLayoutName]));
        this.render(this.currentLayoutData);
        this.deselectKey();
    },

    render(layoutData) {
        const wrapper = document.getElementById('keyboard-wrapper');
        wrapper.innerHTML = '';
        this.keyElements = {};
        this.dragSource = null;

        const container = document.createElement('div');
        container.className = 'keyboard-layout';
        const isEditor = document.body.classList.contains('editor-mode');

        layoutData.rows.forEach((row, rowIdx) => {
            const rowEl = document.createElement('div');
            rowEl.className = 'key-row';
            rowEl.dataset.rowIdx = rowIdx;

            row.keys.forEach((keyData, keyIdx) => {
                const keyEl = document.createElement('div');
                
                const widthMultiplier = keyData.width || 1;
                const baseWidth = 48;
                const gap = 4;
                const calculatedWidth = (baseWidth * widthMultiplier) + (gap * (widthMultiplier - 1));
                keyEl.style.width = `${calculatedWidth}px`;

                if (keyData.height && keyData.height > 1) {
                    const heightMultiplier = keyData.height;
                    const calculatedHeight = (baseWidth * heightMultiplier) + (gap * (heightMultiplier - 1));
                    keyEl.style.height = `${calculatedHeight}px`;
                    keyEl.style.marginBottom = `-${calculatedHeight - baseWidth}px`;
                    keyEl.style.zIndex = "10";
                }

                if (keyData.spacer) {
                    keyEl.className = 'key-spacer';
                    // In editor mode, spacers are clickable too
                    if (isEditor) {
                        this.decorateEditorKey(keyEl, rowIdx, keyIdx, true);
                    }
                } else {
                    keyEl.className = 'key';
                    keyEl.id = `key-${keyData.code}`;
                    keyEl.textContent = keyData.label;
                    this.keyElements[keyData.code] = keyEl;

                    if (isEditor) {
                        this.decorateEditorKey(keyEl, rowIdx, keyIdx, false);
                    }
                }

                rowEl.appendChild(keyEl);
            });

            container.appendChild(rowEl);
        });

        wrapper.appendChild(container);
    },

    decorateEditorKey(keyEl, rowIdx, keyIdx, isSpacer) {
        keyEl.dataset.rowIdx = rowIdx;
        keyEl.dataset.keyIdx = keyIdx;
        keyEl.dataset.isSpacer = isSpacer ? 'true' : 'false';
        keyEl.style.pointerEvents = 'auto';
        keyEl.style.cursor = 'pointer';
        keyEl.draggable = true;

        if (isSpacer) {
            keyEl.classList.add('editor-spacer');
        }

        keyEl.addEventListener('click', (e) => {
            e.stopPropagation();
            this.selectKey(rowIdx, keyIdx, keyEl);
        });

        keyEl.addEventListener('dragstart', (e) => {
            this.dragSource = { rowIdx, keyIdx };
            keyEl.classList.add('dragging');
            if (e.dataTransfer) {
                e.dataTransfer.effectAllowed = 'move';
                e.dataTransfer.setData('text/plain', `${rowIdx}:${keyIdx}`);
            }
        });

        keyEl.addEventListener('dragend', () => {
            this.clearDropTargets();
            keyEl.classList.remove('dragging');
            this.dragSource = null;
        });

        keyEl.addEventListener('dragover', (e) => {
            e.preventDefault();
            this.clearDropTargets();
            keyEl.classList.add('drop-target');
            if (e.dataTransfer) e.dataTransfer.dropEffect = 'move';
        });

        keyEl.addEventListener('dragleave', () => {
            keyEl.classList.remove('drop-target');
        });

        keyEl.addEventListener('drop', (e) => {
            e.preventDefault();
            this.clearDropTargets();
            this.handleDrop(rowIdx, keyIdx);
        });
    },

    clearDropTargets() {
        document.querySelectorAll('.drop-target').forEach((el) => el.classList.remove('drop-target'));
        document.querySelectorAll('.dragging').forEach((el) => el.classList.remove('dragging'));
    },

    handleDrop(targetRowIdx, targetKeyIdx) {
        if (!this.dragSource) return;
        this.moveKey(this.dragSource.rowIdx, this.dragSource.keyIdx, targetRowIdx, targetKeyIdx);
    },

    getRowCount() {
        return this.currentLayoutData?.rows?.length || 0;
    },

    getRowLength(rowIdx) {
        return this.currentLayoutData?.rows?.[rowIdx]?.keys?.length || 0;
    },

    selectKey(rowIdx, keyIdx, el) {
        // Deselect previous
        if (this.selectedKey && this.selectedKey.el) {
            this.selectedKey.el.classList.remove('selected');
        }
        
        el.classList.add('selected');
        this.selectedKey = { rowIdx, keyIdx, el };
        
        const keyData = this.currentLayoutData.rows[rowIdx].keys[keyIdx];
        
        // Notify editor
        if (typeof KeyEditor !== 'undefined') {
            KeyEditor.onKeySelected(rowIdx, keyIdx, keyData);
        }
    },

    deselectKey() {
        if (this.selectedKey && this.selectedKey.el) {
            this.selectedKey.el.classList.remove('selected');
        }
        this.selectedKey = null;
        if (typeof KeyEditor !== 'undefined') {
            KeyEditor.onKeyDeselected();
        }
    },

    updateKeyData(rowIdx, keyIdx, newData) {
        const row = this.currentLayoutData.rows[rowIdx];
        if (!row) return;
        Object.assign(row.keys[keyIdx], newData);
        this.saveCustomLayout();
        this.render(this.currentLayoutData);
        this.reselectKey(rowIdx, keyIdx);
    },

    reselectKey(rowIdx, keyIdx) {
        // Re-select the same key
        const allKeys = document.querySelectorAll(`[data-row-idx="${rowIdx}"][data-key-idx="${keyIdx}"]`);
        if (allKeys.length) {
            this.selectKey(rowIdx, keyIdx, allKeys[0]);
        }
    },

    clampMoveTarget(rowIdx, keyIdx) {
        const maxRowIdx = this.getRowCount() - 1;
        const safeRowIdx = Math.max(0, Math.min(rowIdx, maxRowIdx));
        const maxKeyIdx = this.getRowLength(safeRowIdx);
        const safeKeyIdx = Math.max(0, Math.min(keyIdx, maxKeyIdx));
        return { rowIdx: safeRowIdx, keyIdx: safeKeyIdx };
    },

    moveKey(fromRowIdx, fromKeyIdx, toRowIdx, toKeyIdx) {
        if (!this.currentLayoutData?.rows?.[fromRowIdx] || !this.currentLayoutData?.rows?.[toRowIdx]) return;

        const sourceRow = this.currentLayoutData.rows[fromRowIdx];
        const targetRow = this.currentLayoutData.rows[toRowIdx];
        const [movedKey] = sourceRow.keys.splice(fromKeyIdx, 1);
        if (!movedKey) return;

        let insertIdx = toKeyIdx;
        if (fromRowIdx === toRowIdx && fromKeyIdx < toKeyIdx) {
            insertIdx -= 1;
        }
        insertIdx = Math.max(0, Math.min(insertIdx, targetRow.keys.length));
        targetRow.keys.splice(insertIdx, 0, movedKey);

        this.saveCustomLayout();
        this.render(this.currentLayoutData);
        this.reselectKey(toRowIdx, insertIdx);
    },

    moveSelectedTo(targetRowIdx, targetKeyIdx) {
        if (!this.selectedKey) return;
        const { rowIdx, keyIdx } = this.selectedKey;
        const safeTarget = this.clampMoveTarget(targetRowIdx, targetKeyIdx);
        this.moveKey(rowIdx, keyIdx, safeTarget.rowIdx, safeTarget.keyIdx);
    },

    moveSelectedByArrow(direction) {
        if (!this.selectedKey) return;

        const { rowIdx, keyIdx } = this.selectedKey;
        const rows = this.currentLayoutData.rows;

        if (direction === 'left' && keyIdx > 0) {
            this.moveKey(rowIdx, keyIdx, rowIdx, keyIdx - 1);
            return;
        }

        if (direction === 'right' && keyIdx < rows[rowIdx].keys.length - 1) {
            this.moveKey(rowIdx, keyIdx, rowIdx, keyIdx + 2);
            return;
        }

        if (direction === 'up' && rowIdx > 0) {
            const targetIdx = Math.min(keyIdx, rows[rowIdx - 1].keys.length);
            this.moveSelectedTo(rowIdx - 1, targetIdx);
            return;
        }

        if (direction === 'down' && rowIdx < rows.length - 1) {
            const targetIdx = Math.min(keyIdx, rows[rowIdx + 1].keys.length);
            this.moveSelectedTo(rowIdx + 1, targetIdx);
        }
    },

    getKeyDebugPosition(rowIdx, keyIdx) {
        const keyEl = document.querySelector(`[data-row-idx="${rowIdx}"][data-key-idx="${keyIdx}"]`);
        const wrapper = document.getElementById('keyboard-wrapper');
        if (!keyEl || !wrapper) {
            return null;
        }

        const keyRect = keyEl.getBoundingClientRect();
        const wrapperRect = wrapper.getBoundingClientRect();

        return {
            row: rowIdx + 1,
            index: keyIdx + 1,
            x: Math.round(keyRect.left - wrapperRect.left),
            y: Math.round(keyRect.top - wrapperRect.top)
        };
    },

    deleteSelectedKey() {
        if (!this.selectedKey || !this.currentLayoutData?.rows?.[this.selectedKey.rowIdx]) {
            return;
        }

        const { rowIdx, keyIdx } = this.selectedKey;
        const row = this.currentLayoutData.rows[rowIdx];
        if (!row.keys[keyIdx]) {
            return;
        }

        row.keys.splice(keyIdx, 1);
        this.saveCustomLayout();
        this.render(this.currentLayoutData);

        if (row.keys.length === 0) {
            this.deselectKey();
            return;
        }

        const nextIndex = Math.min(keyIdx, row.keys.length - 1);
        this.reselectKey(rowIdx, nextIndex);
    },

    highlightKey(code, active) {
        const el = this.keyElements[code];
        if (el) {
            if (active) el.classList.add('active');
            else el.classList.remove('active');
        }
    },

    clearAllHighlights() {
        Object.values(this.keyElements).forEach((el) => {
            el.classList.remove('active');
        });
    }
};
