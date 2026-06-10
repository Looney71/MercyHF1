## 2024-05-18 - Added aria-label to toggle switches
**Learning:** Found Svelte a11y warnings for `role="switch"` without accessible names. The `aria-checked` attribute isn't enough for screen readers; they need an `aria-label` or `aria-labelledby` to know what is being toggled.
**Action:** Always add `aria-label` to custom toggle switches and buttons that lack visible text labels.
