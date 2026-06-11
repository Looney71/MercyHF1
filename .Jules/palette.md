
## 2025-02-12 - Missing explicit labels on UI controls
**Learning:** Found multiple instances of `role="switch"` and icon-only buttons missing `aria-label` or `title` attributes. Svelte's `a11y_consider_explicit_label` rule flags these to ensure they are accessible via keyboard navigation and screen readers.
**Action:** Always verify custom interactive components (like switches and icon buttons) explicitly declare their purpose using `aria-label` or `title` attributes (leveraging `$t` translation where applicable) to pass a11y checks and provide good screen reader support.
