## 2026-05-30 - Icon-Only Button Accessibility in Search Components
**Learning:** Icon-only buttons (like arrows for previous/next matches and X for closing) in utility components like search bars are frequently missed for accessibility annotations. Users relying on screen readers hear "button" without context, and mouse users miss out on helpful tooltips.
**Action:** Always add `aria-label` (for screen readers) and `title` (for mouse users/tooltips) to any icon-only button, especially in compact UI elements like search bars where space constraints prevent text labels.
