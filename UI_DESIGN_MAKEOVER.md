
# DRM-SMRTVRB — UI Design Specification

**Document status:** Implementation specification  
**Applies to:** DRM-SMRTVRB plugin UI  
**Design direction:** Vintage studio hardware  
**Primary goal:** Improve the plugin's visual identity and control usability without changing its sound or DSP behavior.

---

## 1. Purpose and Scope

The plugin's audio engine is released and its sound is considered satisfactory. This work is a **UI and interaction redesign only**.

The interface should feel like a piece of vintage professional studio equipment: deliberate, tactile, readable, and restrained.

Use classic digital reverb hardware and analog studio processors as broad visual inspiration. Do not reproduce another manufacturer's exact product, panel, logo, or proprietary artwork.

### In Scope

- Replace parameter sliders with rotary controls where specified.
- Replace checkbox-style enable controls with clearly labeled ON/OFF switches.
- Make Mix a continuously adjustable control with no enable/disable state.
- Improve layout, hierarchy, typography, spacing, contrast, and visual consistency.
- Add tactile, vintage-inspired styling while keeping the interface legible and practical.
- Preserve existing host interaction, automation, presets, and parameter behavior.

### Out of Scope

- DSP changes, algorithm changes, or changes to the sound.
- Changes to parameter IDs, names, ranges, units, defaults, or scaling.
- Changes to preset format or preset compatibility.
- Adding features or controls not already supported by the plugin.
- Reworking the audio architecture merely to support the new UI.

If the current implementation or project documentation does not make a control's behavior clear, inspect the existing code and documentation before making assumptions.

---

## 2. Design Direction

### Core Concept

**Vintage studio rack hardware: classic digital reverb meets analog control-panel discipline.**

The UI should suggest a well-designed piece of late-1970s-to-1980s studio equipment, interpreted as a clean modern plugin interface.

Aim for:

- A dark graphite or charcoal chassis.
- Warm cream or muted grey labels.
- Restrained amber/orange status illumination.
- Tactile rotary knobs with clear position indicators.
- Compact, functional typography.
- Subtle panel divisions, borders, and engraved-label character.
- Clear visual hierarchy and balanced spacing.

Avoid:

- Generic web-dashboard styling.
- Excessive gradients, glossy effects, glassmorphism, or neon accents.
- Oversized decorative elements that compete with the controls.
- Fake hardware details that reduce readability.
- Excessive texture, noise, or low-contrast text.
- Copying a specific commercial plugin or hardware product.

The result should look credible in a professional DAW, not like a novelty retro skin.

---

## 3. Visual System

Treat these as design guidance, not rigid values. Adapt them to the existing UI framework and rendering capabilities.

### Color Palette

Use a restrained palette with consistent semantic meaning.

| Role | Suggested Appearance |
|---|---|
| Main chassis | Dark charcoal / graphite |
| Recessed control areas | Slightly darker graphite |
| Panel borders | Muted dark grey or brushed-metal grey |
| Primary labels | Warm off-white / aged ivory |
| Secondary labels | Muted warm grey |
| Active state | Amber / warm orange |
| Inactive state | Desaturated grey |
| Numeric readouts | Warm ivory or amber |
| Warning / clipping | Existing appropriate warning color, used sparingly |

Do not rely on color alone to communicate ON/OFF state. Switch position and/or text must make the state clear.

### Typography

- Use a legible compact sans-serif for parameter labels and values.
- Use restrained uppercase labels where they improve the hardware-panel feel.
- Use consistent font sizes and weights.
- Keep parameter names readable at the plugin's normal size.
- Use tabular numerals for values if supported.
- Avoid overly distressed, decorative, or novelty typefaces.

### Panel Treatment

- Organize controls into visually distinct sections.
- Use subtle separators, recessed areas, or restrained borders.
- Maintain consistent alignment and spacing.
- Keep decorative details secondary to functional controls.
- Ensure the panel remains readable at the minimum supported window size.

---

## 4. Control Design

### 4.1 Rotary Knobs

Replace existing sliders with rotary knobs for continuous parameters where appropriate.

The intended control set includes:

- Space
- Reverb Time
- BPM
- Pre-delay
- Tone
- Width
- Mix
- Tail

**Important:** Confirm the existing parameter list and behavior in the code before implementation. This list describes the intended visual controls; it does not authorize adding new parameters or changing parameter semantics.

#### Knob Appearance

- Use a clear pointer, line, or cap to indicate the current position.
- Make the current value easy to read.
- Use a restrained circular scale or arc if it improves readability.
- Give the knob a tactile, hardware-inspired appearance without excessive shading.
- Keep knob sizes visually consistent, except where hierarchy justifies a deliberate size difference.
- Make the primary Space control visually prominent, but do not let it dominate the whole interface.

#### Knob Interaction

- Support the framework's appropriate click-and-drag gesture.
- Support fine adjustment using the expected modifier key if the existing framework supports it.
- Support double-click reset to the parameter's existing default value, if consistent with the plugin's current interaction conventions.
- Display the current value and unit where applicable.
- Ensure mouse-wheel behavior, keyboard focus, and accessibility do not conflict with the host or existing conventions.
- Ensure knob gestures update the existing plugin parameter and host automation correctly.

Do not invent new value ranges, units, defaults, or nonlinear mappings. Preserve the existing parameter conversion and scaling behavior.

### 4.2 ON/OFF Switches

Replace checkbox-style enable controls with compact, clearly labeled ON/OFF switches.

Potential examples include Pre-delay, Tone, Width, and Tail **only where those controls are currently enable/disable parameters**.

Requirements:

- The parameter label must be integrated into, or immediately and unambiguously associated with, the switch.
- The current state must be obvious without relying on color alone.
- ON and OFF states must be visually distinct.
- Use a restrained amber indicator for active states where appropriate.
- Keep switch dimensions and spacing consistent.
- Ensure switches remain easy to operate at normal plugin window sizes.
- Preserve the existing parameter IDs, state behavior, and host automation.

Do not convert continuous parameters into switches. Do not create an enable switch for Mix.

### 4.3 Mix Control

Mix must be a **continuous rotary knob**.

- It must not have an ON/OFF checkbox, switch, or separate enable state.
- It should continuously control the existing wet/dry mix parameter.
- Its displayed value must reflect the existing parameter representation.
- Preserve its existing range, default, scaling, and automation behavior.

---

## 5. Layout and Hierarchy

Organize the existing controls into a coherent panel. The following is a suggested grouping; use the actual parameters and existing functionality as the source of truth.

### Header

- Plugin name / identity.
- Existing preset selector and preset-related controls.
- Any existing global status indicators.

Keep the header compact and functional.

### Main Controls

Prioritize the most important sound-shaping controls.

Suggested emphasis:

- Space
- Reverb Time
- Any existing primary character or source control

Use a clear hierarchy. Space may be the visual centerpiece if that matches its role in the current plugin.

### Tempo Section

Group existing tempo-related controls together, such as:

- Sync, if present.
- BPM.
- Division, if present.

Do not add tempo functionality that does not already exist.

### Advanced Controls

Group secondary shaping controls together, such as:

- Pre-delay
- Tone
- Width
- Tail

Use rotary knobs for continuous parameters and labeled switches for actual enable/disable parameters.

### Output Section

Keep existing output-related controls and visual feedback together, such as:

- Mix
- Input/output meters, if present.
- Existing envelope or response visualization, if present.

Do not invent meters or visualizations that require new DSP or audio analysis unless separately approved.

### Responsive Behavior

- Keep the layout balanced at the default window size.
- Support the plugin's existing resizing behavior.
- Prevent labels, values, knobs, and switches from overlapping.
- Preserve usability at the minimum supported size.
- Avoid controls becoming too small or cramped when resized.
- Prefer a deliberate rearrangement or scaling strategy over arbitrary clipping.

---

## 6. Functional and Compatibility Requirements

These requirements are mandatory.

1. **Do not modify DSP code or audio processing behavior.**
2. Preserve all existing parameter IDs.
3. Preserve parameter names, ranges, units, defaults, and scaling.
4. Preserve host automation and parameter notification behavior.
5. Preserve preset loading, saving, and compatibility with existing presets.
6. Preserve plugin state serialization and restoration.
7. Preserve VST3 compatibility and existing host integration.
8. Do not break existing parameter gestures or host-controlled parameter updates.
9. Ensure UI controls correctly reflect parameter changes made by automation, preset loading, or the host.
10. Do not introduce unnecessary dependencies.
11. Do not increase rendering or CPU overhead unnecessarily.
12. Keep the implementation consistent with the project's existing UI framework and architecture.

If a proposed UI change appears to require a DSP or parameter architecture change, stop and report why before proceeding. Find a UI-only solution wherever possible.

---

## 7. Accessibility and Usability

- Maintain sufficient contrast between labels, values, controls, and the panel.
- Do not communicate state through color alone.
- Make interactive areas large enough to operate reliably.
- Keep labels concise and unambiguous.
- Ensure values remain readable and do not get clipped.
- Provide clear visual feedback during interaction.
- Avoid excessive animation or decorative motion.
- Respect host scaling and high-DPI behavior supported by the project.

---

## 8. Implementation Workflow

Before changing code:

1. Read `AGENTS.md` and all relevant project documentation.
2. Read this document completely.
3. Inspect the current UI implementation and identify the UI framework and parameter-binding approach.
4. Inspect the current parameter definitions and confirm which parameters are continuous versus boolean.
5. Identify existing preset, automation, resizing, and host-notification pathways.
6. Record the files expected to change.
7. Confirm that the planned work can be completed without changing DSP or parameter compatibility.

During implementation:

1. Work on a dedicated feature branch, not directly on the release branch.
2. Make UI changes in small, reviewable steps.
3. Reuse existing parameter attachments and bindings where possible.
4. Keep DSP and parameter-definition files untouched unless a strictly necessary UI integration issue is identified and approved.
5. Do not make unrelated refactors.
6. Build and test after meaningful changes.

Suggested branch name:

`feature/vintage-ui-redesign`

---

## 9. Verification Checklist

Before considering the redesign complete, verify the following.

### Visual Review

- [ ] The UI reads as vintage studio hardware rather than a generic dashboard.
- [ ] The palette is restrained, cohesive, and legible.
- [ ] Continuous controls use rotary knobs where specified.
- [ ] Enable controls use labeled ON/OFF switches rather than checkboxes.
- [ ] Mix is a rotary control with no enable/disable state.
- [ ] Parameter names and values are readable.
- [ ] The layout has clear sections and visual hierarchy.
- [ ] Controls do not overlap or clip at supported sizes.
- [ ] The interface remains usable when resized.

### Functional Review

- [ ] Every knob controls the correct existing parameter.
- [ ] Every switch controls the correct existing boolean parameter.
- [ ] UI values update when parameters change through the host.
- [ ] Automation works in both directions: host to UI and UI to host.
- [ ] Double-click reset, if implemented, returns to the existing default.
- [ ] Presets load and save correctly.
- [ ] Existing presets retain their expected sound and parameter values.
- [ ] Plugin state restores correctly after closing and reopening the project.

### Regression Review

- [ ] DSP source and behavior are unchanged.
- [ ] Parameter IDs and definitions are unchanged.
- [ ] No unintended audio differences are introduced.
- [ ] VST3 builds successfully.
- [ ] The plugin loads and operates in REAPER.
- [ ] No new compiler warnings or runtime errors are introduced by the UI work.
- [ ] UI interaction does not cause audio-thread blocking or unnecessary real-time work.

---

## 10. Completion Report

When implementation is complete, provide a concise report containing:

- Summary of the UI changes.
- Files changed.
- Build and test commands executed.
- Hosts and plugin formats tested.
- Confirmation that DSP, parameter IDs, ranges, defaults, presets, and automation were preserved.
- Any known limitations or remaining visual issues.
- Screenshots of the redesigned interface at the default size and, if supported, a resized state.

Do not claim a compatibility or regression check passed unless it was actually performed.

---

## 11. Final Design Principle

**Make DRM-SMRTVRB feel like a piece of studio equipment, not a collection of software widgets.**

The interface should be tactile, restrained, readable, and purposeful. Visual character is important, but usability and compatibility come first.

The released sound is the foundation: improve the instrument's presentation without changing the instrument itself.
