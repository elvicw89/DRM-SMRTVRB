# Agent Handover

## Output level fader (Claude Sonnet 5.5)

Branch `feature/output-gain-fader`: new `outGain` parameter (-24..+12 dB, default 0) plus a bottom fader strip in the editor; docs updated. macOS build + 8/8 CTest + standalone screenshot done; Windows package built via dispatch for owner testing. Not done: DAW check of the fader, release/version bump (owner decision). Continue on this branch; do not create a new one. Note: the AGENTS.md token/usage check has no accessible API in this environment and was not performed.

## Vintage UI redesign (Claude Sonnet 5.5)

Branch `feature/vintage-ui-redesign`. Only `Source/PluginEditor.{h,cpp}` changed (plus docs/screenshots). macOS build + 8/8 CTest + standalone screenshots (`screenshots/ui-makeover-*.png`) done. Not done: Windows CI result, REAPER check of the new UI, tag/release (ask owner; a new version is needed). Mix knob engages `advMixOn` on user change (see CHANGELOG).

## SSL-reference UI pass (Claude Sonnet 5.5)

Branch `feature/ui-ssl-reference-pass`; only `Source/PluginEditor.{h,cpp}` plus docs/screenshots. Not released/tagged. Not done: DAW check of the new look.

## v0.1.0 promoted (2026-11-24, Claude Sonnet 5.5)

- Owner approved promoting v0.1.0 from pre-release to a normal (latest) release; notes/title updated accordingly. 1.0.0 should wait for at least one more DAW and ideally signing.

## v0.1.0 Windows pre-release (2026-11-24, Claude Sonnet 5.5)

- Tagged `v0.1.0` on `main` (`c840672`); run 45233325683 passed; published pre-release with zip + SHA-256. Owner chose to skip macOS/Linux for now; PR #9 (unix-build.yml) closed unmerged. Known for later: Linux link failure needs `POSITION_INDEPENDENT_CODE ON` on `drmsmrtvrb_dsp`; macOS universal build passed.
- Not done: other DAWs, in-DAW CPU, code signing, installer, macOS/Linux packaging.

## Phase 7 close-out (2026-11-24, Claude Sonnet 5.5)

- Branch `phase7/close-out`: docs reconciliation only. Local plugin build + CTest 8/8 passed. Exit review table is in `PROJECT_STATUS.md`. Owner authorised self-review and merge of this PR.
- Carried forward: other DAW hosts, in-DAW CPU, macOS/Linux packaging, signing, installer, published release. Next phase: owner to choose.

## Release source link + tag check (2026-11-24, Claude Sonnet 5.5)

- Branch `phase7/release-source-link`: `release-package.yml` bundles `LICENSE` + `SOURCE.txt`. Dispatch run 45225739734 passed and the artifact was inspected. Tag mismatch check verified (run 45225737575 failed as intended; scratch tag deleted). Repo is public.
- Not done: no GitHub Release published (owner action). Next: owner tags `v0.1.0` (matches CMake) when ready; review release notes.

## First real-stem audition (2026-11-24, Claude Sonnet 5.5)

- Owner supplied five 24-bit/44.1 kHz stems (kick, snare, ride, overheads, room) in git-ignored `recordings/`. Rendered through all three engines at SPACE 65 (`recordings/real/`) and analysed with `drmsmrtvrb_analyze`. Metrics: kick/snare show clear engine differences (snare RT60 est. Spring 0.18 s, Room 0.30 s, Gated 1.02 s); ride/overheads/room stems are dominated by their own sustain, so the metrics barely separate engines.
- Owner's subjective verdict: likes how it sounds. Informal, one listener, defaults only; no per-profile tuning changes were made.
- AGPLv3 adopted (PR #5 merged). Release still needs the repo public and a source link.

## Licence decision: AGPLv3 (2026-11-24, Claude Sonnet 5.5)

- Owner chose a free/open-source route: project is AGPLv3, `LICENSE` added (branch `docs/adopt-agpl3`). Before distributing binaries: make the repo public and link source in releases.
- Stale branch `phase7/audition-analysis-tool` is merged into main; deletion awaits owner go-ahead. Owner will supply drum stems in git-ignored `recordings/`.

## Audition analysis tool (2026-11-24, Claude Sonnet 5.5)

- Branch `phase7/audition-analysis-tool` (from `main` at `5f94845`): added `drmsmrtvrb_analyze`, `drmsmrtvrb_metrics` library and CTest `AudioMetrics`. macOS DSP-only build: CTest 7/7. Note: these commits (`5dd587a`, `acbcd34`) were committed directly to `main` by mistake (HEAD had moved off the branch); the owner accepted keeping them, and Windows CI on `main` passed. Ready for real recordings: render with `drmsmrtvrb_render` then analyze with `--dry`.
- Not done: real-recording auditions (none supplied), JUCE licence decision. The analyzer gives numbers only, no thresholds.

## Stability soak (2026-11-24, Claude Sonnet 5.5)

- Branch `phase7/stability-soak` (from `main` at `893ee5d`): added `Tools/SoakTest.cpp`, target `drmsmrtvrb_soak_test`, CTest `PluginSoak`. macOS: passes (default and 1,440 s/seed 777 runs). Windows CI run 45215377733 passed all 7 CTest targets including PluginSoak (4.86 s).
- Not covered: DAW host, editor, real-time scheduling, Windows memory growth. Open: real-recording auditions, JUCE licence decision.

## Plugin processor CPU benchmark (2026-11-24, Claude Sonnet 5.5)

- Branch `phase7/plugin-wrapper-cpu` (from `main` at `4a38a2d`): added `Tools/PluginBenchmark.cpp` / `drmsmrtvrb_plugin_benchmark` and an informational Windows CI step. macOS 0.312% worst case; Windows CI run 45212922040 passed, worst case 0.751%. All output finite.
- Not measured: editor drawing, in-DAW CPU, host playhead lookup. Open: real-recording auditions, JUCE licence decision.
- Next: owner reviews/merges this branch.

## Phase 7 Windows CPU benchmark (2026-11-23, Claude Sonnet 5.5)

- `build/release-packaging` was merged into `main` (fast-forward, `e5c97f2`) on the owner's instruction. Branch `phase7/windows-cpu-benchmark-ci` adds an informational benchmark step to `.github/workflows/windows-build.yml`.
- Verified: windows-build dispatch run 45210114347 passed with the benchmark (worst case 3.813% of one core); release-package dispatch run 45210138012 passed and its zip/SHA-256 were inspected and matched.
- Not verified: tag-mismatch check; plugin-in-host CPU. Open: real-recording auditions, JUCE licence decision.
- Next: owner reviews/merges this branch.

## Release packaging (2026-11-23, Claude Sonnet 5.5)

- Branch `build/release-packaging` (from `main` at `83c1913`). Added `.github/workflows/release-package.yml`; updated `CHANGELOG.md`, `BUILD_INSTRUCTIONS.md`, `PROJECT_STATUS.md`.
- Also added `Tools/Benchmark.cpp` + `drmsmrtvrb_benchmark` CMake target (DSP CPU benchmark; results in `TESTING.md`). macOS Release build and CTest 6/6 passed; benchmark ran, all output finite.
- Checks: workflow YAML parses (ruby). At that time it had not been run on GitHub; it has since passed manual dispatch (run 45210138012, see the Windows CPU benchmark entry above).
- Usage check: no exact figure was accessible; the owner authorized proceeding, and an estimate (well above 20% remaining) was used.
- Next: push the branch, run the workflow via `workflow_dispatch`, inspect the artifact, then owner review/merge. JUCE licensing and real-recording auditions remain open.

## Latest handoff (2026-11-23 — Windows Host Verification Completed)

- **Phase 6 Host Validation Completed:** Native Windows testing conducted in REAPER 7.42/win64 on Windows x64.
  - VST3 scanned from `%LOCALAPPDATA%\Programs\Common\VST3` and `C:\Program Files\Common Files\VST3`, registering `DRM-SMRTVRB (yeahwaah audio)` in `reaper-vstplugins64.ini`.
  - All 22 exposed parameters enumerated, queried, and verified against APVTS definitions.
  - All eight factory programs verified (Tight Shells, Big Snare, Explosive Snare, Small Studio, Large Room, Wide Overheads, Metallic Spring, Vintage Gated) with exact target, character, SPACE, sync mode, division, and tail values.
  - Controlled SPACE 0% render vs dry source confirmed exact sample-for-sample match (`max error = 0.0000000000` across all 288,000 frames).
  - Controlled SPACE 65% render verified wet reverb contribution (`delta RMS = 0.09517980`), with output finite and bounded (peak 0.512281).
  - Host tempo sync verified at 80 BPM vs 160 BPM (`delta RMS = 0.00156543`, `max sample diff = 0.00897741`).
  - SPACE automation envelope (0% -> 65% -> 0%) verified with clean return to dry (delta RMS 4.5e-7).
  - Project save/reopen round-trip verified complete restoration of parameter states, modified flag, and envelope points.
  - VST3 editor window opened in REAPER, verified dark faceplate with amber accents, and tested resize down to 640 x 420.
  - Official running screenshots captured in `screenshots/drm-space-main.png`, `screenshots/drm-space-advanced.png`, `screenshots/reaper-vst3-editor.png`, and `screenshots/reaper-vst3-editor-min.png`.
- **Phase transition:** Phase 6 (Hardware-inspired UI) is complete. Transitioning to Phase 7 (Testing, optimization, real drum listening, and release preparation).
- **Next steps for Phase 7:**
  1. Audition with actual drum recordings (kick, snare, toms, room, overheads) when user supplies them.
  2. Confirm JUCE licensing plan (commercial vs. AGPLv3) with the repository owner before public binary distribution.
  3. Prepare versioned release packaging and workflow.

## Prior handoffs (reference)

- Branch `main` is clean; latest code commit `782d1e7` passed Windows CI run [45053765301](https://github.com/elvicw/DRM-SMRTVRB/actions/runs/45053765301). The docs-only follow-up `f4b7986` records that result.
- Minimum-size (640 x 420), accessible selector names, SPACE keyboard adjustment, and its visible focus outline were verified in the macOS standalone. The ring/Character overlap was corrected.
- REAPER 7.34/macOS arm64 smoke check: temporarily installed the VST3, found and loaded `VST3: DRM-SMRTVRB`, opened the editor, imported `recordings/synthetic-kick-shells.wav`, and played it through the track. REAPER track and master meters showed signal. This confirms host scan/load and signal passing through a track with the VST3 inserted; it does not verify wet output quality, plugin meter/history behavior, tempo sync, automation, or state round-trip. The capture happened during exploratory parameter interaction, so do not treat it as a controlled wet/dry comparison.
- Cleanup completed: stopped the temporary REAPER instance (none remains running) and removed only the temporary `~/Library/Audio/Plug-Ins/VST3/DRM-SMRTVRB.vst3` copy. The existing `WaveMorph.vst3` install was left in place. The REAPER test project was unsaved and discarded.
- Next: repeat host playback with a controlled saved test project and SPACE > 0, compare SPACE 0 and wet output, test host BPM changes, parameter automation and project save/reopen, then run the Windows VST3 in Windows REAPER. Audition real drum recordings and resolve JUCE distribution terms before release.
- Do not claim Windows host validation or audio-quality validation from the macOS smoke test.

## Continuation note (2026-11-23)

- A controlled offline render used the existing synthetic kick fixture at SPACE 0 and 65. SPACE 0 matched input sample-for-sample over the input duration; SPACE 65 differed (delta RMS 0.09518). Outputs were finite and below 0 dBFS. See `TESTING.md` for commands and scope.
- A controlled REAPER wet/dry project was not completed. A temporary plugin bundle was installed for discovery and then removed; the REAPER instance started for this attempt was closed without saving a project. The existing `WaveMorph.vst3` installation was left untouched.
- Next: complete the controlled wet/dry, host BPM, automation, and state round-trip checks in REAPER; then validate in Windows REAPER. Keep the offline synthetic result separate from host and listening claims.

## Resumed handover (2026-11-22)

The user resumed work after the pause. The local build/tests pass and the first Windows plugin build succeeded; continue verification below.

### Current state

- Branch `main`, tracking `origin/main`; latest pushed commit is `8b15eab` (`fix: improve editor accessibility and narrow layout`).
- JUCE 8.0.6 VST3/standalone wrapper, APVTS state/parameters, eight factory programs, and editor are implemented locally.
- Windows run [45045203823](https://github.com/elvicw/DRM-SMRTVRB/actions/runs/45045203823) passed configure, build, all six tests, bundle staging, and artifact upload for the latest editor patch. Download inspection confirmed `DRM-SMRTVRB.vst3/Contents/x86_64-win/DRM-SMRTVRB.vst3` and `Contents/Resources/moduleinfo.json`.
- Latest additions include per-channel meter snapshots, atomic wet history, preset modified-state display, tail smoothing, host BPM display, advanced-control disabling, and populated choice menus.
- Corrected the tail smoother to a 25 ms ramp (`0.025` seconds).
- Current macOS `cmake --build build-plugin -j 4` links VST3 and standalone. Windows CI run 45045203823 passed all six CTest targets after the latest editor patch; the latest macOS CTest pass predates it.
- The standalone UI was visually inspected at default size; the choice menus are populated. The latest accessibility and minimum-size findings are recorded below.
- Follow-up UI review found that combo boxes and sliders needed explicit accessible titles; Source, Character, Tempo Sync, SPACE, Free Decay, and advanced slider names are observed in the REA accessibility tree. The preset status label now moves with editor width to avoid clipping at 640 px. The native resize action did not reach 640 x 420, so verify that size and keyboard operation when a reliable host resize path is available.
- Parameter formatting is supplied by APVTS parameter attributes so JUCE attachments preserve concise values and units; the standalone screenshot now shows SPACE as `28%` and tail as `0.48 s`.
- REAPER 7.34/macOS arm64 instantiated the VST3 and opened its editor; audio was not played. One capture showed “Tight Shells” with different parameter values and MODIFIED. Source inspection shows this is consistent with retaining the selected preset and marking edited values; no defect was established. Revisit only if a fresh host reproduction shows inconsistent restored state.
- The temporary plugin install and REAPER cache entry were removed; the REAPER instance was closed.
- Latest Windows CI and artifact layout are verified; Windows host validation remains outstanding. No real-recording audition exists. JUCE distribution terms remain unresolved.

### Next steps

1. Validate 640 x 420 layout, keyboard operation, meter decay/hold, wet history, and preset modified/reset behavior. Accessibility titles are observed on the enabled source/character/sync choices and sliders; preset/division selector accessibility still needs review.
2. In Windows REAPER, test audio, all profiles, SPACE 0 dry endpoint, host tempo/fallback, automation, state/preset restore, editor reopen, and resizing.
3. Resolve JUCE commercial versus AGPL distribution terms before binaries are distributed; audition real drum recordings before audio-quality claims.
4. Keep status docs current as each validation milestone completes.

### Do not claim

- Windows host validation or REAPER audio quality.
- Real drum-recording audition or release readiness.

## Current continuation (2026-11-23)

- See **Resumed handover (2026-11-22)** above for the latest build, test, host observations, and Windows artifact verification.
- 640 x 420 content-size standalone capture verified the minimum layout and exposed two defects: SPACE overlapped Character, and the Wet Envelope caption reached the bottom edge. `Source/PluginEditor.cpp` now centers the shrinking SPACE knob under its label and shortens/repositions the history region/caption. The revised capture is clear.
- Sliders now request keyboard focus and use `DRMFocusableSlider` to draw an amber focus outline. Native mouse/key input moved SPACE from 28% to 29%; it was restored to 28%, and the focus state/ring were observed. That screenshot exposed the ring touching Character at default size. `Source/PluginEditor.cpp` shifts the SPACE rotary and its label together to clear the selector; local CMake build succeeds.
- Windows CI run [45053765301](https://github.com/elvicw/DRM-SMRTVRB/actions/runs/45053765301) passed configure, build, all six tests, VST3 bundle staging, and artifact upload for commit `782d1e7`. Downloaded artifact inspection confirmed both the Windows VST3 binary and `moduleinfo.json` under the expected bundle root.
- Standalone accessibility inspection reports selector names for preset, source, character, sync, and division, plus slider values for SPACE and the advanced controls. Native keyboard adjustment and the focus ring are verified. The updated default-size view clears the Character selector; at minimum size the rotary left edge remains at x=356, preserving its clearance.
- REAPER smoke-test details and cleanup are recorded in **Latest handoff (2026-11-23)** above.

## Current state

- **Phase:** 6 — Hardware-inspired UI.
- **Last completed milestone:** Advanced override composition, eight factory presets, and versioned state codec pass local tests; Phase 2–4 DSP/timing modules are also implemented.
- **Active milestone:** Validate the initial Phase 6 JUCE wrapper/editor and complete its remaining UI/host integration work.
- **Implementation status:** DSP library, offline renderer, target profiles, SPACE, tempo calculator/tap tracker, advanced controls, presets, APVTS wrapper, host BPM lookup, and JUCE editor are implemented. A local editor-only accessibility/layout patch builds; the prior six-test CTest pass predates that patch. Windows and host behavior validation remain incomplete.

## Completed work

- Repository: https://github.com/elvicw/DRM-SMRTVRB (private).
- Product name: DRM-SMRTVRB. Company: yeahwaah audio.
- The incoming roadmap was moved to ROADMAP.md with product naming updated.
- Adjacent `../drum-shaper` is DRM-RESHAPE and uses CMake with JUCE 8.0.6 FetchContent. This is a reference only.
- `Source/ReverbEngines.*` implements early-reflection/tank Room, resonant/dispersive Spring, and separately gated feedback engines. Processing does not allocate; delay memory is allocated in `prepare`.
- `Source/TargetProfiles.*` defines twelve distinct profiles with source-specific wet-send filtering and engine controls.
- `Source/SpaceMacro.*` maps 0–100% through twelve independent curves, equal-power dry/wet gains, and a default 20 ms smoother. `SpaceMacroProcessor` returns exactly dry at 0% and processes parameters in 32-sample quanta.
- `Source/TempoSync.*` calculates six musical divisions for Free/Host/Tap modes, retains valid fallback BPM values, handles tap intervals, and smooths timing changes. The JUCE wrapper reads host BPM; transport play/stop remains unused.
- `Source/PluginState.*` composes advanced overrides after SPACE and contains the eight roadmap presets plus a versioned text state codec. The codec is not connected to host state callbacks.
- `Source/PluginProcessor.*` maps APVTS parameters into the DSP and serializes host state as JUCE XML. Host tempo BPM is read; transport play/stop is not used.
- `Source/PluginEditor.*` binds controls to APVTS and includes stereo meters, activity history, preset dirty-state display, host BPM display, and toggle-based advanced control enabling.
- `UI_SPEC.md` captures the framework-independent Phase 6 layout, interaction, parameter/state contract, and implementation acceptance checks.
- `Tools/RenderWav.cpp` and `Tools/WavFile.*` provide an offline mono/stereo WAV renderer. Supported input is PCM16/24/32 and float32; manual engine mode writes wet-only float32, while profile/SPACE mode applies its dry/wet mix.
- `Tools/GenerateTestRecordings.py` creates five procedural drum-like fixtures (kick/shells, snare, toms, room-mic, overheads); `Tools/RenderFixtureMatrix.py` renders all 15 profile/engine pairs at SPACE 65. Generated WAVs are local under ignored `recordings/`.
- `Tests/ReverbEnginesTests.cpp`, `Tests/TargetProfilesTests.cpp`, and `Tests/SpaceMacroTests.cpp` cover engine safety, all twelve profiles, SPACE range/smoothing/exact dry endpoint, mono, and block chunking.
- `Tests/TempoSyncTests.cpp` and `Tests/PluginStateTests.cpp` cover timing divisions/fallback/tap tracking, advanced override interactions, factory presets, and state serialization.
- `Tests/WavFileTests.cpp` checks float32 round trip and PCM16/24/32 decoding.
- Synthetic outputs were finite and below 0 dBFS in the generated matrix. They are not real recordings; no actual drum recordings have been supplied, so real-material listening is still outstanding.

## Build and test status

- Machine: macOS; Apple clang 21.0.1, CMake 4.5.0, Ninja.
- Build: `cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release && cmake --build build`
- Test: `ctest --test-dir build --output-on-failure`
- Render help: `./build/drmsmrtvrb_render --help`
- Latest verified plugin result before the pending editor-only patch: macOS VST3 and standalone linked; CTest passed 6/6. The pending editor patch has since linked successfully; CTest has not been rerun.
- Synthetic renderer result: all five synthetic inputs rendered through all twelve source/engine profiles at SPACE 65; all output peaks remained below 0 dBFS. This says nothing about the new wrapper or real-recording quality.
- Windows CI run 45008104694 passed configure, build, and all six CTest targets including Phase 5. Windows run 45008427219 passed configure, build, and all six CTest targets after commit `02c0273`.
- REAPER 7.34/macOS arm64 instantiated the VST3 and opened its editor, but no audio was played. A capture with a selected factory name and MODIFIED status matches the editor's documented dirty-preset behavior; no defect was established. Windows VST3 host validation remains required.

## GitHub status

- Owner/account: `elvicw`.
- Repository URL: https://github.com/elvicw/DRM-SMRTVRB.
- Repository is private; current branch is `main`, tracking `origin/main`.
- Latest pushed commit before the pending editor patch: `2e277e3` (`docs: clarify modified factory preset behavior`).
- Current working tree contains the accessible-title/responsive-status editor patch and updated UI notes.

## Next steps

Follow the numbered **Next steps** in the Resumed handover above.

## Next agent instruction

Continue from **Resumed handover (2026-11-22)**. The latest editor source builds locally; accessible control names and responsive preset-status positioning were added after the verified Windows artifact run. Run Windows CI for this editor change, then complete minimum-size/keyboard and Windows REAPER checks. Do not claim Windows host validation or REAPER audio quality without evidence.
