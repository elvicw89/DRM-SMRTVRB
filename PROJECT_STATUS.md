# Project Status

- **Product:** DRM-SMRTVRB
- **Company:** yeahwaah audio
- **Version:** 0.1.0 (Windows VST3 release published (promoted from pre-release 2026-11-24): https://github.com/elvicw/DRM-SMRTVRB/releases/tag/v0.1.0)
- **Phase:** 7 — Testing, optimization, and release preparation
- **Completed:** Standalone engines/profiles/SPACE; Free/Host/Tap timing calculations; five synthetic fixtures; optional advanced overrides; eight factory presets; portable versioned state codec; JUCE 8.0.6 VST3 and standalone wrapper; full Windows REAPER 7.42 host acceptance (exact dry endpoint, wet SPACE response, host tempo synchronization, automation envelopes, all eight factory programs, state/project save/reopen, UI rendering and resize down to 640 x 420).
- **Active tasks:** Phase 7 close-out complete (see Phase 7 exit review below). Next phase to be chosen by the owner.
- **CPU:** DSP-core benchmark on macOS: worst case 0.363% of one core (48 kHz, 512 samples); Windows CI runner worst case 3.813% of one core; plugin processor (all eight presets, no editor/host) worst case 0.312% on macOS and 0.751% on the Windows CI runner; in-DAW CPU unmeasured.
- **Stability:** `PluginSoak` (randomized block sizes 1–4096, 22.05–192 kHz, mono/stereo, all parameters, presets, re-prepare) passes on macOS; no non-finite output, peak ≤ 1.484, no memory growth in the leak phase. Windows CI run 45215377733 passed all 7 CTest targets including `PluginSoak` (memory check is POSIX-only).
- **Audition tooling:** `drmsmrtvrb_analyze` provides objective measurements for rendered WAVs (unit-tested on synthetic signals; not yet used on real recordings).
- **Outstanding:** Release source-link bundling and tag check verified; real drum recording auditions (first informal pass done 2026-11-24: owner liked the sound; further tuning/listening optional); JUCE licence decided (owner): project is AGPLv3 (see `LICENSE`); no paid JUCE licence. Public source hosting and releases must follow AGPLv3.
- **Latest successful build:** Windows x64 MSVC (Visual Studio 2022 BuildTools) + Ninja Release; VST3 and standalone built and verified locally on Windows; macOS Release also linked.
- **Latest test evidence:** Local Windows CTest passed 6/6 (ReverbEngines, TargetProfiles, SpaceMacro, TempoSync, PluginState, WavFile); Windows REAPER 7.42 passed exact dry (max error 0.0), SPACE 65 wet (delta RMS 0.09518), host tempo sync (80 vs 160 BPM delta RMS 0.00157), envelope automation, all 8 factory programs, and project save/reopen.
- **Plugin screenshots:** Verified captures of running standalone and REAPER-hosted plugin saved in `screenshots/drm-space-main.png`, `screenshots/drm-space-advanced.png`, and `screenshots/reaper-vst3-editor.png`.
- **Repository:** https://github.com/elvicw/DRM-SMRTVRB (public, AGPLv3)

## Phase 7 exit review (2026-11-24)

| Item | Result | Evidence |
| --- | --- | --- |
| Automated tests | 8/8 CTest pass locally (macOS, plugin on); Windows CI passes all targets | local `ctest`; Windows CI on `main` |
| CPU | Worst case 0.363% (DSP, macOS), 3.813% (DSP, Windows CI), 0.312% / 0.751% (processor, macOS / Windows CI) | `drmsmrtvrb_benchmark`, `drmsmrtvrb_plugin_benchmark` |
| Stability | Fuzz soak passes on macOS and Windows CI; no memory growth (POSIX) | `PluginSoak` |
| Windows host | REAPER 7.42 acceptance complete | `TESTING.md` |
| Real-recording audition | One informal pass on five owner stems (kick, snare, ride, overheads, room), default settings; owner liked the sound | `recordings/real/` (git-ignored) |
| Licensing | AGPLv3 adopted; repository public | `LICENSE` |
| Release packaging | Versioned zip + SHA-256 + `LICENSE` + `SOURCE.txt`; tag/version check exercised | runs 45225739734, 45225737575 |

Not verified (carried forward): in-DAW CPU, other hosts (Cubase, Ableton, FL Studio, Bitwig, Studio One, etc.), macOS/Linux release packaging, signed/notarized builds, extended listening beyond one pass, installer, and an actual published release. `v0.1.0` was tagged and published as a Windows-only, unsigned GitHub release (initially pre-release, promoted to a normal release by owner decision) (build run 45233325683, zip SHA-256 `bc5bf4d7…0a3d` verified, contains `LICENSE` and `SOURCE.txt`). macOS and Linux are deferred by owner decision; a trial Linux build failed linking because `drmsmrtvrb_dsp` is a non-PIC static library linked into the shared VST3 (fix: `POSITION_INDEPENDENT_CODE ON`), while a universal macOS build passed in CI (PR #9, closed unmerged).

## Environment observations

- Current machine: macOS (Apple clang 21.0.1, CMake 4.5.0, Ninja available).
- Git 2.52.0 and GitHub CLI 2.104.0 are available; GitHub authentication is active for `elvicw`.
- No Windows cross-compiler was found in the initial PATH/Homebrew check.
- JUCE 8.0.6 is fetched through CMake FetchContent; clean builds require network access.
- JUCE licensing is settled (AGPLv3, public repository); the release package bundles `LICENSE` and `SOURCE.txt`.
- REAPER 7.34/macOS arm64 initially scanned and loaded the VST3 and passed a basic synthetic-audio signal-through smoke check. That exploratory check did not cover controlled output, host timing, automation, or state; later controlled results are recorded below.
- REAPER 7.34/macOS: matched SPACE dry/wet/tail, host tempo response at 80 vs 160 BPM, SPACE automation, project reopen, and all eight factory program values. Live meter/history drawing remains unverified because this session’s transport stopped immediately and its track peaks stayed at zero; Windows REAPER UI acceptance also remains open. See `TESTING.md`.
- The standalone UI was visually captured at default and 640 x 420 content sizes. Minimum-size review fixed SPACE overlap with Character and Wet Envelope caption clipping. A native mouse click plus Right arrow changed SPACE from 28% to 29%; it was restored to 28%. The SPACE control receives focus and shows an amber outline. That screenshot exposed the outline touching Character at default size, so the rotary and label were shifted together to leave a clear gap. The standalone accessibility tree reports names for preset, source, character, sync, and division selectors, plus slider names/values for SPACE, Free Decay, and advanced controls.
- The accessible-title/responsive-status editor change and handover updates are pushed in `8b15eab`.
- Offline WAV renderer is built as `drmsmrtvrb_render`; five local synthetic fixtures and fifteen profile renders are under ignored `recordings/`; no user recordings were available.
