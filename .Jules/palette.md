## 2024-06-14 - Icon-only Search Bar Buttons missing ARIA labels
**Learning:** Icon-only buttons used for secondary/inline features like in-chat search often miss ARIA labels, creating accessibility gaps for screen readers. Using the existing `$t()` function provides clean translations for these labels.
**Action:** Always check newly found icon-only UI components for missing ARIA labels and title attributes, and integrate them gracefully with existing `i18n.ts` keys.
