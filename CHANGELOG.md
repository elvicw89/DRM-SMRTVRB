# Changelog

## Unreleased — output level fader

- New host parameter `outGain` (Output Level, -24 to +18 dB, default 0 dB; raised from the first +12 dB design for more makeup gain) with a horizontal fader strip at the bottom of the editor (double-click resets to 0 dB). Applied as a smoothed (25 ms) gain after the dry/wet sum, so output meters read post-gain. At the 0 dB default the audio is unchanged. It is not part of factory presets, so loading a preset leaves the level alone and it does not trigger MODIFIED. Existing projects without the parameter load at 0 dB. Editor minimum height is now 460 px (was 420). Verified by macOS build, 8/8 CTest and a standalone screenshot (`screenshots/ui-output-fader.png`); not yet checked in a DAW. The portable text codec (`StateCodec`) is unchanged.

## 0.2.1 — SSL-reference UI pass (Windows VST3)

- SPACE value box now shows whole percent (display only; parameter precision unchanged).

### SSL-reference UI pass

- Editor-only: brushed-metal faceplate, header plate, inset panels, machined-metal knob caps with bezel, illuminated amber push buttons (PRE-DELAY/TONE/WIDTH/TAIL, with ON/OFF caption), MODIFIED status lamp. Replaced the orange focus box with a soft knob glow on hover/drag/keyboard focus. Matched against `docs/references/ui_reference.png`; differences: no segmented meter texture, simpler title treatment. DSP/parameters/presets/state unchanged. Verified by macOS build, 8/8 CTest and standalone screenshots (`screenshots/ui-ssl-*.png`); no DAW check.

## 0.2.0 — vintage UI redesign

- Editor-only redesign per `UI_DESIGN_MAKEOVER.md`: rotary knobs, labelled ON/OFF pill switches (Pre-delay, Tone, Width, Tail), recessed SOURCE/SPACE/OUTPUT/ADVANCED panels, custom look-and-feel. The Mix ON switch is gone; moving the Mix knob sets `advMixOn`, double-click restores Mix 100% and clears `advMixOn`. A preset with Mix off shows 100% while the sound follows the SPACE macro. DSP, parameter IDs/ranges/defaults, presets and state are unchanged. Verified by build, 8/8 CTest and standalone screenshots only; no DAW host check of the new UI yet.

## 0.1.0 — Windows VST3 release

- Published v0.1.0 as a GitHub release, promoted from pre-release (Windows x64 VST3, unsigned, AGPLv3, source link bundled). macOS/Linux deferred.

## Unreleased — Phase 7 close-out

- Phase 7 exit review added to `PROJECT_STATUS.md`; stale licence/visibility/version statements reconciled. No code changes.

## Unreleased — release source compliance

- Release package now bundles `LICENSE` and `SOURCE.txt` (source commit link). Tag/version mismatch check verified with a scratch tag.

## Unreleased — first real-stem audition

- Rendered owner-supplied stems through all engines; owner's informal verdict positive. No DSP changes.

## Unreleased — licence

- Adopted AGPLv3 (owner decision; free JUCE option). Added `LICENSE`; updated status, known-issue and build docs.

## Unreleased — foundation

- Created the private DRM-SMRTVRB project repository and initial documentation set.
- Recorded the project identity as DRM-SMRTVRB by yeahwaah audio.
- No plugin functionality or release artifact is included.

## Unreleased — DSP engine baseline

- Added independent Room, Spring, and Gated wet-only DSP engines.
- Added CMake build and impulse-response tests for engine output, separation, mono handling, and gated tail reduction.
- This baseline has not been integrated into a VST3 plugin or auditioned with real drum recordings.

## Unreleased — Phase 1 validation tools

- Added `drmsmrtvrb_render`, an offline wet-only WAV renderer for Room, Spring, and Gated.
- Added mono/stereo PCM16/24/32 and float32 WAV input, float32 output, and configurable rendered tail.
- Expanded automated checks across sample rates, block sizes, and parameter extremes.
- Added WAV read/write tests and ignored the local `recordings/` folder.
- Added deterministic synthetic kick/shell, snare, tom, room-mic, and overhead fixture generation plus a 15-case render-matrix script.

## Unreleased — target profile baseline

- Added distinct initial profiles for all four targets across Room, Spring, and Gated.
- Added source-specific wet-send filtering, send level, and engine-specific density/wet-level controls.
- Added `ProfiledReverb` and automated checks for all twelve combinations, including mono input and block chunking.

## Unreleased — SPACE macro baseline

- Added twelve target/engine SPACE curves that vary multiple reverb controls and per-profile wet caps.
- Added a default 20 ms smoother and dry/wet processor; SPACE 0% returns the exact dry input.
- Added offline renderer support for `--target` and `--space` and expanded tests for range, smoothing, dry endpoint, and wet output.

## Unreleased — tempo synchronization calculations

- Added six musical division calculations for Free, Host, and Tap timing modes.
- Added last-valid/default BPM fallback, tap-tempo interval averaging, gated timing clamp reporting, and a timing smoother.
- Added tests for tempo changes and missing source timing. Host transport extraction remains pending a plugin wrapper.

## Unreleased — advanced controls and preset state

- Added optional Pre-delay, Tone, Width, Mix, and Tail override composition on top of SPACE.
- Added the eight named factory presets from the roadmap.
- Added versioned state serialization with bounds checks and forward-compatible unknown fields.
# Unreleased

## Added

- Initial JUCE 8.0.6 VST3 and standalone plugin targets with stable APVTS IDs.
- Host parameter state serialization, factory programs, host BPM lookup, tap tempo control, and a parameter-attached hardware-inspired editor.
- CMake plugin build option and local build instructions.
- Windows-only VST3 CI build and artifact upload configuration.
- Stereo input/output meter snapshots, meter hold/release, wet activity history, factory-preset modified indication, host BPM display, and advanced-toggle behavior.
- Added per-parameter value formatting for compact SPACE, BPM, time, and trim readouts.

## Fixed

- Added the missing editor factory implementation required to link the standalone and VST3 targets.

## Validation

- macOS Apple clang 21.0.1 Release build produced VST3 and standalone targets; local CTest passed 6/6.
- Windows CI run [45185893939](https://github.com/elvicw/DRM-SMRTVRB/actions/runs/45185893939) passed configure, build, all six tests, VST3 bundle staging, and artifact upload.
- Windows local build and CTest passed 6/6 with MSVC and Ninja.
- Windows REAPER 7.42/x64 verified: VST3 plugin scan/load, exact dry endpoint at SPACE 0 (max error 0.0), SPACE 65 wet response (delta RMS 0.09518), host tempo sync at 80 vs 160 BPM (delta RMS 0.00157), SPACE envelope automation, all eight factory presets, and project save/reopen state restore.
- Real running UI screenshots captured from standalone and REAPER-hosted instances into `screenshots/` at default and minimum sizes (640 x 420).
- JUCE distribution licensing terms and real drum recording listening auditions remain tracked for Phase 7.

## Unreleased — release packaging workflow

- Added `.github/workflows/release-package.yml` (tag `v*` or manual dispatch): builds and tests on Windows, checks the tag matches the CMake project version, and uploads a versioned `DRM-SMRTVRB-<version>-Windows-VST3.zip` with a SHA-256 file as a workflow artifact.
- It does not publish a GitHub Release; JUCE distribution terms are still unresolved. Manual dispatch run 45210138012 succeeded and produced the zip and matching SHA-256.

## Unreleased — audition analysis tool

- Added `drmsmrtvrb_analyze` (`Tools/AnalyzeWav.cpp`, `Tools/AudioMetrics.*`) for objective level, band-share, decay-time and stereo-stability measurements of rendered WAVs, plus CTest `AudioMetrics`. Verified on synthetic signals only; no real recordings used. See `TESTING.md`.

## Unreleased — stability soak test

- Added `drmsmrtvrb_soak_test` (`Tools/SoakTest.cpp`) and CTest `PluginSoak`: randomized block-size/sample-rate/layout/parameter/preset fuzzing of the processor with finite/bounded output and POSIX memory-growth checks. macOS: PASS over up to 1,440 s of simulated audio. See `TESTING.md`.

## Unreleased — plugin processor CPU benchmark

- Added `drmsmrtvrb_plugin_benchmark` (`Tools/PluginBenchmark.cpp`), a JUCE console target that runs the real processor over all eight factory presets, and an informational Windows CI step for it. macOS: worst case 0.312% of one core at 48 kHz/512, finite output. See `TESTING.md`.

## Unreleased — CPU benchmark

- Added `drmsmrtvrb_benchmark` (`Tools/Benchmark.cpp`), an offline CPU benchmark over all targets and engines including decaying tails. macOS result: worst case 0.363% of one core at 48 kHz/512 samples, finite output. DSP core only; see `TESTING.md`.
