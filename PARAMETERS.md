# Parameters

The JUCE wrapper now exposes the host parameters below. IDs are the stable identifiers introduced in the initial wrapper; do not rename them after release. Profile values remain provisional pending listening checks with real drum recordings.

## Host parameters

| ID | Name | Type/range | Default |
|---|---|---|---|
| `target` | Target | Choice: Shells, Snare, Room, Overheads | Shells |
| `engine` | Character | Choice: Room, Spring, Gated | Room |
| `space` | SPACE | 0–100% | 0 |
| `sync` | Sync | Choice: Free, Host, Tap | Host |
| `bpm` | Tap BPM | 20–300 BPM | 120 |
| `division` | Division | 1/16, 1/8, 1/4, 1/2, 1 bar, 2 bars | 1/4 |
| `freeDecay` | Free Decay | 0.08–8 s | 1.2 |
| `advPreDelayOn`, `advToneOn`, `advWidthOn`, `advMixOn`, `advTailOn` | Advanced enables | Boolean | Off |
| `advPreDelay` | Advanced Pre-delay | 0–200 ms | 12 |
| `advTone` | Advanced Tone | -1–1 | 0 |
| `advWidth` | Advanced Width | 0–2 | 1 |
| `advMix` | Advanced Mix | 0–1 | 1 |
| `advTail` | Advanced Tail | 0.08–8 s | 1.2 |
| `outGain` | Output Level | -24–+12 dB | 0 |

The wrapper uses JUCE APVTS parameter state and XML host serialization. Host tempo is read-only and not a parameter. Tap BPM is editable in Tap mode. The TAP button updates the BPM parameter from tap intervals. The portable versioned text codec remains separate from host serialization.

## Internal controls

| Name | Type/range | Function |
|---|---|---|
| Decay | seconds, 0.08–8.0 | Reverb feedback decay |
| Pre-delay | ms, 0–200 | Delay before wet excitation |
| Damping | 0–1 | Frequency-dependent decay damping |
| Width | 0–2 | Stereo side gain |
| Gate | seconds, 0.03–4.0 | Gated engine termination |
| Density | 0–1 | Engine-specific diffusion/feedback density |
| Wet level | 0–2 | Wet output gain |
| Send high-pass / low-pass | Hz | Source-specific band limiting before reverb |
| Send level | linear, 0–1 | Source-specific wet excitation |
| SPACE | percent, 0–100 | Per-profile macro for decay, pre-delay, damping, density, width, gate, and wet/dry gains |
| SPACE smoothing | ms, 1–100; default 20 | Standalone macro processor transition ramp |
| Sync mode | Free / Host / Tap | Selects unrestricted, host BPM, or tapped BPM timing |
| BPM | 20–300 | Host/tap musical timing; missing source falls back to last valid BPM or 120 |
| Division | 1/16, 1/8, 1/4, 1/2, 1 bar, 2 bars | Converts BPM to timing milliseconds |
| Advanced Pre-delay | ms, 0–200; optional | Overrides the macro pre-delay when enabled |
| Advanced Tone | -1–1; optional | Dark-to-bright damping offset |
| Advanced Width | 0–2; optional | Overrides macro stereo width |
| Advanced Mix | 0–1; optional | Overrides profile wet cap while SPACE sets the amount |
| Advanced Tail | seconds, 0.08–8; optional | Overrides natural decay; Gated termination clamps to 4 s |

## Initial twelve source/engine profiles

Values are `(HP Hz / LP Hz / Send / Decay s / Pre-delay ms / Damping / Width / Gate s / Density / Wet)`.

| Target | Room | Spring | Gated |
|---|---|---|---|
| Shells | `85 / 15000 / .65 / .80 / 9 / .60 / .75 / .35 / .58 / .72` | `100 / 14000 / .58 / 1.00 / 12 / .42 / .85 / .38 / .72 / .68` | `85 / 16000 / .78 / .58 / 4 / .52 / .90 / .30 / .84 / .80` |
| Snare | `70 / 17000 / .76 / 1.35 / 16 / .42 / 1.05 / .32 / .72 / .80` | `90 / 16000 / .70 / 1.65 / 22 / .30 / 1.12 / .42 / .80 / .74` | `80 / 17000 / .92 / .85 / 6 / .44 / 1.16 / .28 / .92 / .96` |
| Room | `90 / 13000 / .52 / 1.50 / 4 / .62 / .90 / .40 / .82 / .62` | `130 / 12000 / .45 / 1.15 / 9 / .52 / .82 / .45 / .68 / .55` | `110 / 11000 / .45 / .75 / 1 / .70 / .80 / .22 / .72 / .48` |
| Overheads | `170 / 9000 / .38 / .72 / 5 / .82 / .72 / .40 / .65 / .48` | `200 / 8500 / .32 / .82 / 8 / .72 / .70 / .45 / .62 / .38` | `180 / 9000 / .38 / .48 / 0 / .85 / .66 / .16 / .68 / .43` |

The source-controlled table is in `Source/TargetProfiles.cpp`; `Tests/TargetProfilesTests.cpp` verifies distinctness, ranges, finite output, and mono/stereo processing. Twelve SPACE curves are in `Source/SpaceMacro.cpp`; `Tests/SpaceMacroTests.cpp` verifies endpoints, input clamps, smoothing, and exact dry output at zero.

Factory presets are defined in `Source/PluginState.cpp`: Tight Shells, Big Snare, Explosive Snare, Small Studio, Large Room, Wide Overheads, Metallic Spring, and Vintage Gated. Portable state format version 1 serializes the selected target/engine, SPACE, sync/division, free decay, advanced override enable flags, and values. The JUCE wrapper saves/restores host state as APVTS XML; the portable text codec is not connected to that host format.

Audio-thread automation is read from APVTS atomics. SPACE is smoothed; profile, timing, and advanced parameters are currently sampled at block/control-quantum boundaries. Test automation and state round trips in a Windows host before release.
