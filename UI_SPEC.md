# DRM-SMRTVRB Interface Specification

## Status

> Redesign in progress (branch `feature/vintage-ui-redesign`): the layout below predates `UI_DESIGN_MAKEOVER.md`; the redesign uses rotary knobs, ON/OFF pill switches and no Mix enable switch. See CHANGELOG.


Phase 6 design with an initial JUCE editor implementation in `Source/PluginEditor.*`. The editor binds to actual APVTS parameters. JUCE 8.0.6 is selected for implementation; distribution license terms remain to be resolved before release.

## Main window

- Default content size: 760 x 500 logical pixels; keep all controls usable at 640 x 420.
- Dark charcoal faceplate, restrained brushed-metal detail, warm amber active accents, high-contrast labels, and compact studio-hardware styling.
- Use a clear top-to-bottom hierarchy: preset and identity, source/character choices, SPACE, timing, then optional advanced controls and meters.
- All controls remain visible without scrolling at the default size. Advanced controls may open a compact side panel or expand below the main controls at the minimum size.

## Layout and behavior

| Region | Controls / display | Required behavior |
|---|---|---|
| Header | DRM-SMRTVRB, yeahwaah audio, preset selector | Preset selection recalls the complete versioned state; modified state is indicated until saved or another preset is recalled. |
| Source | Shells, Snare, Room, Overheads | Exclusive selection; changing target updates the active profile and is host-automatable. |
| Character | Room, Spring, Gated | Exclusive selection; changing character updates the active profile and is host-automatable. |
| Main | Large SPACE rotary, numeric percent readout | Dominant control; 0% is exact dry, 100% reaches the profile's intended maximum. Drag, mouse wheel, keyboard arrows, and double-click default are supported. |
| Timing | Free / Host / Tap, BPM readout, division selector, tap button | Host mode displays host tempo or the documented fallback; Tap measures taps; divisions are enabled only when relevant. Gated tail timing follows the selected division and the engine's safe maximum. |
| Advanced | Pre-delay, Tone, Width, Mix, Tail; each with enable/bypass | Values edit only when enabled. Disabled values remain stored. Engine/profile constraints are shown in the value range and respected by DSP. |
| Output level | Horizontal fader strip at the bottom, -24 to +12 dB, double-click resets to 0 dB | Parameter `outGain`; post dry/wet gain, not stored in factory presets. |
| Meter strip | Input and output peak meters | Show stereo channels independently, with peak hold and decay; meter handoff must be lock-free and safe for the audio callback. |
| Reverb activity | Compact envelope/history display | Visualize the recent wet output envelope from audio-thread data; provide a static reduced-motion fallback and no audio-thread drawing or allocation. |

## Interaction and accessibility

- Every control has a persistent text label, readable value, keyboard focus, and accessible name/role/value through the chosen framework.
- Use distinct selected states beyond color alone; maintain text/background contrast and visible focus rings.
- Rotary controls support click-drag, fine adjustment with a modifier, wheel adjustment, arrow-key adjustment, and double-click reset to the documented default.
- Preset changes, source/character changes, and advanced enable switches are undoable through normal host automation/state recall behavior where supported.
- Avoid modal dialogs for routine parameter editing. Tooltips explain the audible purpose in plain language.
- Reduce animation when the operating system requests reduced motion; meters must not be the only indication of signal presence.

## Parameter and state contract

- Stable host parameter IDs are assigned before UI implementation and remain stable across releases.
- UI reads and writes the same parameter/state model used by the processor; it does not keep a second authoritative copy.
- Target, character, SPACE, sync mode, division, free decay, and advanced values/enable flags persist through host state.
- Host BPM and transport are read-only displays supplied by the processor; Tap BPM remains editable only in Tap mode.
- Preset labels map to the eight existing factory states. User presets, preset file browsing, and preset deletion are outside the initial V1 UI unless the roadmap is revised.
- The editor polls or receives copied meter/envelope snapshots at a bounded UI rate. It never calls DSP processing methods or touches mutable audio buffers.

## Acceptance checks for implementation

1. All 12 target/character choices select the corresponding processor profile.
2. SPACE updates audible processing continuously, gives exact dry at zero, and stays synchronized when restored from host state.
3. Free/Host/Tap and division controls reflect valid BPM/timing and graceful missing-host-tempo fallback.
4. Each advanced control is disabled by default unless the state says otherwise; toggles and values survive save/restore.
5. All eight factory presets recall the exact portable state and update every visible control.
6. Input/output meters and reverb activity show live audio without UI access to audio-thread buffers.
7. Controls are operable by mouse and keyboard at the default and minimum window sizes.
8. Validate in a Windows VST3 host, including resizing, automation, preset/state restore, and editor reopen.

## Framework-dependent work remaining

The editor implements the main selectors, SPACE, timing controls, advanced overrides, factory preset choice, stereo meter/activity history, and preset modified indication. The standalone window was reviewed at default size and 640 x 420 content size; layout overlap and caption clipping were corrected. Accessibility inspection reports names for preset, source, character, sync, and division selectors, and names/values for SPACE, Free Decay, and advanced sliders. Native keyboard interaction changed SPACE by one step with the Right arrow, and its amber focus outline remains clear of Character.

Mac REAPER 7.34 checks verified SPACE 0 exact dry, SPACE 65 wet/tail behavior, host BPM response at 80/160 BPM, SPACE automation, saved-project reopen, and all eight host factory programs against their expected target/character/SPACE/sync/division values. Windows CI run [45185893939](https://github.com/elvicw/DRM-SMRTVRB/actions/runs/45185893939) passed build/tests and staged the x64 VST3 artifact. Native Windows REAPER 7.42 testing confirmed the editor loads, displays correctly, and resizes down to the minimum 640 x 420 bounds without overlap. Standalone and hosted screenshots are archived under `screenshots/`. Audition with real drum recordings and settlement of JUCE distribution terms remain tracked for Phase 7.
