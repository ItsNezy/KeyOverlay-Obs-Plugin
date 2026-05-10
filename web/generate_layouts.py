import json
import os

os.makedirs("layouts", exist_ok=True)


def row(keys):
    return {"keys": keys}


def key(code, label, width=1):
    return {"code": code, "label": label, "width": width}


def spacer(width=1):
    return {"code": "", "label": "", "width": width, "spacer": True}


# 60% Alphas (Standard)
row1_60 = [
    key("Escape", "Esc"),
    key("Digit1", "1"),
    key("Digit2", "2"),
    key("Digit3", "3"),
    key("Digit4", "4"),
    key("Digit5", "5"),
    key("Digit6", "6"),
    key("Digit7", "7"),
    key("Digit8", "8"),
    key("Digit9", "9"),
    key("Digit0", "0"),
    key("Minus", "-"),
    key("Equal", "="),
    key("Backspace", "Back", 2),
]
row2_60 = [
    key("Tab", "Tab", 1.5),
    key("KeyQ", "Q"),
    key("KeyW", "W"),
    key("KeyE", "E"),
    key("KeyR", "R"),
    key("KeyT", "T"),
    key("KeyY", "Y"),
    key("KeyU", "U"),
    key("KeyI", "I"),
    key("KeyO", "O"),
    key("KeyP", "P"),
    key("BracketLeft", "["),
    key("BracketRight", "]"),
    key("Backslash", "\\", 1.5),
]
row3_60 = [
    key("CapsLock", "Caps", 1.75),
    key("KeyA", "A"),
    key("KeyS", "S"),
    key("KeyD", "D"),
    key("KeyF", "F"),
    key("KeyG", "G"),
    key("KeyH", "H"),
    key("KeyJ", "J"),
    key("KeyK", "K"),
    key("KeyL", "L"),
    key("Semicolon", ";"),
    key("Quote", "'"),
    key("Enter", "Enter", 2.25),
]
row4_60 = [
    key("ShiftLeft", "Shift", 2.25),
    key("KeyZ", "Z"),
    key("KeyX", "X"),
    key("KeyC", "C"),
    key("KeyV", "V"),
    key("KeyB", "B"),
    key("KeyN", "N"),
    key("KeyM", "M"),
    key("Comma", ","),
    key("Period", "."),
    key("Slash", "/"),
    key("ShiftRight", "Shift", 2.75),
]
row5_60 = [
    key("ControlLeft", "Ctrl", 1.25),
    key("MetaLeft", "Win", 1.25),
    key("AltLeft", "Alt", 1.25),
    key("Space", "", 6.25),
    key("AltRight", "Alt", 1.25),
    key("MetaRight", "Win", 1.25),
    key("ContextMenu", "Menu", 1.25),
    key("ControlRight", "Ctrl", 1.25),
]

layout_60 = {
    "name": "60%",
    "rows": [row(row1_60), row(row2_60), row(row3_60), row(row4_60), row(row5_60)],
}

# 65% (Adds arrows and right column)
# RShift = 1.75u so ↑ aligns directly above ↓ in the inverted-T arrow cluster.
row1_65 = row1_60[:-1] + [key("Backspace", "Back", 2), spacer(0.25), key("Delete", "Del")]
row2_65 = row2_60 + [spacer(0.25), key("PageUp", "PgUp")]
row3_65 = row3_60 + [spacer(0.25), key("PageDown", "PgDn")]
row4_65 = [
    key("ShiftLeft", "Shift", 2.25),
    key("KeyZ", "Z"),
    key("KeyX", "X"),
    key("KeyC", "C"),
    key("KeyV", "V"),
    key("KeyB", "B"),
    key("KeyN", "N"),
    key("KeyM", "M"),
    key("Comma", ","),
    key("Period", "."),
    key("Slash", "/"),
    key("ShiftRight", "Shift", 1.75),
    spacer(0),
    key("ArrowUp", "↑"),
]
row5_65 = [
    key("ControlLeft", "Ctrl", 1.25),
    key("MetaLeft", "Win", 1.25),
    key("AltLeft", "Alt", 1.25),
    key("Space", "", 6.25),
    key("AltRight", "Alt", 1),
    key("MetaRight", "Fn", 1),
    key("ControlRight", "Ctrl", 1),
    spacer(0.25),
    key("ArrowLeft", "←"),
    key("ArrowDown", "↓"),
    key("ArrowRight", "→"),
]

layout_65 = {
    "name": "65%",
    "rows": [row(row1_65), row(row2_65), row(row3_65), row(row4_65), row(row5_65)],
}

# 75% (Adds F-row) — number row starts with Backquote since Esc is on the F-row
row0_75 = [
    key("Escape", "Esc"),
    spacer(1),
    key("F1", "F1"),
    key("F2", "F2"),
    key("F3", "F3"),
    key("F4", "F4"),
    spacer(0.5),
    key("F5", "F5"),
    key("F6", "F6"),
    key("F7", "F7"),
    key("F8", "F8"),
    spacer(0.5),
    key("F9", "F9"),
    key("F10", "F10"),
    key("F11", "F11"),
    key("F12", "F12"),
    spacer(0.25),
    key("Delete", "Del"),
]
row1_75_numrow = [
    key("Backquote", "`"),
    key("Digit1", "1"),
    key("Digit2", "2"),
    key("Digit3", "3"),
    key("Digit4", "4"),
    key("Digit5", "5"),
    key("Digit6", "6"),
    key("Digit7", "7"),
    key("Digit8", "8"),
    key("Digit9", "9"),
    key("Digit0", "0"),
    key("Minus", "-"),
    key("Equal", "="),
    key("Backspace", "Back", 2),
    spacer(0.25),
    key("Home", "Home"),
]
row2_75 = row2_65
row3_75 = row3_65
row4_75 = row4_65
row5_75 = row5_65

layout_75 = {
    "name": "75%",
    "rows": [row(row0_75), row(row1_75_numrow), row(row2_75), row(row3_75), row(row4_75), row(row5_75)],
}

# TKL — number row starts with Backquote since Esc is on F-row
nav1 = [key("Insert", "Ins"), key("Home", "Home"), key("PageUp", "PgUp")]
nav2 = [key("Delete", "Del"), key("End", "End"), key("PageDown", "PgDn")]
arrows = [spacer(1), key("ArrowUp", "↑"), spacer(1)]
arrows2 = [key("ArrowLeft", "←"), key("ArrowDown", "↓"), key("ArrowRight", "→")]

row1_tkl_numrow = [
    key("Backquote", "`"),
    key("Digit1", "1"),
    key("Digit2", "2"),
    key("Digit3", "3"),
    key("Digit4", "4"),
    key("Digit5", "5"),
    key("Digit6", "6"),
    key("Digit7", "7"),
    key("Digit8", "8"),
    key("Digit9", "9"),
    key("Digit0", "0"),
    key("Minus", "-"),
    key("Equal", "="),
    key("Backspace", "Back", 2),
]

row0_tkl = row0_75[:-2] + [spacer(0.25), key("PrintScreen", "Prt"), key("ScrollLock", "Scr"), key("Pause", "Pau")]
row1_tkl = row1_tkl_numrow + [spacer(0.5)] + nav1
row2_tkl = row2_60 + [spacer(0.5)] + nav2
row3_tkl = row3_60 + [spacer(3.5)]
row4_tkl = row4_60 + [spacer(0.5)] + arrows
row5_tkl = row5_60 + [spacer(0.5)] + arrows2

layout_tkl = {
    "name": "TKL (87-key)",
    "rows": [row(row0_tkl), row(row1_tkl), row(row2_tkl), row(row3_tkl), row(row4_tkl), row(row5_tkl)],
}

# Numpad
np1 = [key("NumLock", "Num"), key("NumpadDivide", "/"), key("NumpadMultiply", "*"), key("NumpadSubtract", "-")]
np2 = [key("Numpad7", "7"), key("Numpad8", "8"), key("Numpad9", "9"), {"code": "NumpadAdd", "label": "+", "width": 1, "height": 2}]
np3 = [key("Numpad4", "4"), key("Numpad5", "5"), key("Numpad6", "6")]
np4 = [key("Numpad1", "1"), key("Numpad2", "2"), key("Numpad3", "3"), {"code": "NumpadEnter", "label": "Ent", "width": 1, "height": 2}]
np5 = [key("Numpad0", "0", 2), key("NumpadDecimal", ".")]

layout_numpad = {
    "name": "Numpad",
    "rows": [row(np1), row(np2), row(np3), row(np4), row(np5)],
}

# Full
row0_full = row0_tkl
row1_full = row1_tkl + [spacer(0.5)] + np1
row2_full = row2_tkl + [spacer(0.5)] + np2
row3_full = row3_tkl + [spacer(0.5)] + np3
row4_full = row4_tkl + [spacer(0.5)] + np4
row5_full = row5_tkl + [spacer(0.5)] + np5

layout_full = {
    "name": "Full (104-key)",
    "rows": [row(row0_full), row(row1_full), row(row2_full), row(row3_full), row(row4_full), row(row5_full)],
}

layouts = {
    "60": layout_60,
    "65": layout_65,
    "75": layout_75,
    "tkl": layout_tkl,
    "numpad": layout_numpad,
    "full": layout_full,
}

for name, layout in layouts.items():
    with open(f"layouts/{name}.json", "w", encoding="utf-8") as f:
        json.dump(layout, f, indent=2, ensure_ascii=False)

print("Layouts generated successfully.")
