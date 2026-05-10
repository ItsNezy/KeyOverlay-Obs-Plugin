const KeyEditor = {
    currentRowIdx: null,
    currentKeyIdx: null,
    currentKeyData: null,
    step: 0.25, // Nudge step in units

    onKeySelected(rowIdx, keyIdx, keyData) {
        this.currentRowIdx = rowIdx;
        this.currentKeyIdx = keyIdx;
        this.currentKeyData = keyData;

        const panel = document.getElementById('key-editor-panel');
        panel.classList.remove('hidden');

        const isSpacer = !!keyData.spacer;

        document.getElementById('ke-info').textContent = isSpacer
            ? `Spacer (Row ${rowIdx + 1}, #${keyIdx + 1})`
            : `${keyData.label || keyData.code} (Row ${rowIdx + 1})`;

        document.getElementById('ke-width').value = keyData.width || 1;
        document.getElementById('ke-width-display').textContent = (keyData.width || 1).toFixed(2) + 'u';
        document.getElementById('ke-target-row').value = rowIdx + 1;
        document.getElementById('ke-target-index').value = keyIdx + 1;

        // Show/hide label field
        const labelRow = document.getElementById('ke-label-row');
        if (isSpacer) {
            labelRow.classList.add('hidden');
            document.getElementById('ke-code').value = '';
        } else {
            labelRow.classList.remove('hidden');
            document.getElementById('ke-label').value = keyData.label || '';
            document.getElementById('ke-code').value = keyData.code || '';
        }

        this.updateDebugInfo();
    },

    onKeyDeselected() {
        this.currentRowIdx = null;
        this.currentKeyIdx = null;
        this.currentKeyData = null;
        document.getElementById('key-editor-panel').classList.add('hidden');
        this.resetDebugInfo();
    },

    nudgeWidth(delta) {
        if (this.currentKeyData === null) return;
        const newWidth = Math.max(0, (this.currentKeyData.width || 1) + delta);
        this.applyWidth(newWidth);
    },

    applyWidth(val) {
        if (this.currentRowIdx === null) return;
        const newWidth = Math.max(0, parseFloat(val) || 0);
        Keyboard.updateKeyData(this.currentRowIdx, this.currentKeyIdx, { width: newWidth });
    },

    applyLabel(val) {
        if (this.currentRowIdx === null || this.currentKeyData?.spacer) return;
        Keyboard.updateKeyData(this.currentRowIdx, this.currentKeyIdx, { label: val });
    },

    applyCode(val) {
        if (this.currentRowIdx === null || this.currentKeyData?.spacer) return;
        Keyboard.updateKeyData(this.currentRowIdx, this.currentKeyIdx, { code: val.trim() });
    },

    moveSelected(direction) {
        Keyboard.moveSelectedByArrow(direction);
    },

    moveSelectedToInputs() {
        if (this.currentRowIdx === null) return;

        const rowInput = document.getElementById('ke-target-row');
        const indexInput = document.getElementById('ke-target-index');
        const targetRowIdx = Math.max(0, (parseInt(rowInput.value, 10) || 1) - 1);
        const targetKeyIdx = Math.max(0, (parseInt(indexInput.value, 10) || 1) - 1);
        Keyboard.moveSelectedTo(targetRowIdx, targetKeyIdx);
    },

    updateDebugInfo() {
        if (this.currentRowIdx === null || this.currentKeyIdx === null) {
            this.resetDebugInfo();
            return;
        }

        const debug = Keyboard.getKeyDebugPosition(this.currentRowIdx, this.currentKeyIdx);
        document.getElementById('ke-debug-row').textContent = String(this.currentRowIdx + 1);
        document.getElementById('ke-debug-index').textContent = String(this.currentKeyIdx + 1);
        document.getElementById('ke-debug-x').textContent = debug ? `${debug.x}px` : '-';
        document.getElementById('ke-debug-y').textContent = debug ? `${debug.y}px` : '-';
    },

    resetDebugInfo() {
        document.getElementById('ke-debug-row').textContent = '-';
        document.getElementById('ke-debug-index').textContent = '-';
        document.getElementById('ke-debug-x').textContent = '-';
        document.getElementById('ke-debug-y').textContent = '-';
    },

    confirmOrProceed(message) {
        try {
            if (typeof window.confirm === 'function') {
                return window.confirm(message);
            }
        } catch (e) {
            console.warn('confirm() unavailable, proceeding without dialog.', e);
        }
        return true;
    },

    // Insert a spacer BEFORE the selected key
    insertSpacerBefore() {
        if (this.currentRowIdx === null) return;
        const row = Keyboard.currentLayoutData.rows[this.currentRowIdx];
        row.keys.splice(this.currentKeyIdx, 0, { code: '', label: '', width: 0.25, spacer: true });
        Keyboard.saveCustomLayout();
        Keyboard.render(Keyboard.currentLayoutData);
        this.onKeyDeselected();
    },

    // Delete the selected key/spacer
    deleteSelected() {
        Keyboard.deleteSelectedKey();
    },

    init() {
        // Width input
        document.getElementById('ke-width').addEventListener('change', (e) => {
            this.applyWidth(e.target.value);
        });
        document.getElementById('ke-width').addEventListener('input', (e) => {
            const val = parseFloat(e.target.value);
            document.getElementById('ke-width-display').textContent = (Number.isFinite(val) ? val : 0).toFixed(2) + 'u';
        });

        // Label input
        document.getElementById('ke-label').addEventListener('change', (e) => {
            this.applyLabel(e.target.value);
        });
        document.getElementById('ke-code').addEventListener('change', (e) => {
            this.applyCode(e.target.value);
        });

        document.getElementById('ke-apply-position').addEventListener('click', () => {
            this.moveSelectedToInputs();
        });
        document.getElementById('ke-target-row').addEventListener('change', () => {
            this.moveSelectedToInputs();
        });
        document.getElementById('ke-target-index').addEventListener('change', () => {
            this.moveSelectedToInputs();
        });

        // Nudge buttons
        document.getElementById('ke-width-minus').addEventListener('click', () => this.nudgeWidth(-this.step));
        document.getElementById('ke-width-plus').addEventListener('click', () => this.nudgeWidth(this.step));
        document.getElementById('ke-move-up').addEventListener('click', () => this.moveSelected('up'));
        document.getElementById('ke-move-left').addEventListener('click', () => this.moveSelected('left'));
        document.getElementById('ke-move-right').addEventListener('click', () => this.moveSelected('right'));
        document.getElementById('ke-move-down').addEventListener('click', () => this.moveSelected('down'));

        // Insert spacer
        document.getElementById('ke-add-spacer').addEventListener('click', () => this.insertSpacerBefore());

        // Delete key
        document.getElementById('ke-delete').addEventListener('click', () => {
            if (this.confirmOrProceed('Delete this key/spacer?')) this.deleteSelected();
        });

        // Reset layout
        document.getElementById('ke-reset').addEventListener('click', () => {
            if (this.confirmOrProceed('Reset layout to default?')) Keyboard.resetLayout();
        });

        // Arrow key nudging (when key editor is focused)
        document.addEventListener('keydown', (e) => {
            if (this.currentKeyData === null) return;
            // Only respond if not typing in an input
            if (e.target.tagName === 'INPUT' || e.target.tagName === 'TEXTAREA' || e.target.tagName === 'SELECT') return;

            if (e.shiftKey && e.key === 'ArrowLeft') {
                e.preventDefault();
                this.nudgeWidth(-this.step);
            } else if (e.shiftKey && e.key === 'ArrowRight') {
                e.preventDefault();
                this.nudgeWidth(this.step);
            } else if (e.key === 'ArrowLeft') {
                e.preventDefault();
                this.moveSelected('left');
            } else if (e.key === 'ArrowRight') {
                e.preventDefault();
                this.moveSelected('right');
            } else if (e.key === 'ArrowUp') {
                e.preventDefault();
                this.moveSelected('up');
            } else if (e.key === 'ArrowDown') {
                e.preventDefault();
                this.moveSelected('down');
            } else if (e.key === 'Escape') {
                Keyboard.deselectKey();
            }
        });

        // Click outside to deselect
        document.getElementById('keyboard-wrapper').addEventListener('click', (e) => {
            if (e.target.id === 'keyboard-wrapper' || e.target.classList.contains('keyboard-layout')) {
                Keyboard.deselectKey();
            }
        });
    }
};
