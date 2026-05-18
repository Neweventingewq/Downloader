# Volchay Downloader

A serious, full-featured desktop video downloader written in **C++17 + Qt 6 / QML**.

It wraps `yt-dlp` and `ffmpeg` behind a modern Acri-style UI with an amber-orange accent and three carefully tuned themes (**Snow**, **Dark**, **Blackout**), a flexible Settings page, a real download queue with progress / cancel / retry, and a cross-platform CMake build.

---

## Project rules (mandatory, read first)

1. **Every step of development is documented in this README — in exhaustive detail.** Whenever a new file, class, signal, build flag, dependency, design decision or workaround is introduced, the corresponding section of this README is updated in the same commit. Nothing is left to "git archeology": if you want to know why a file exists, this document explains it.
2. **This README never links to source-control hosts, mirrors, archives, or any other repository URLs.** No `github.com/…`, no `gitlab.com/…`, no shortened links to repos. Tool home-pages (e.g. *yt-dlp*, *ffmpeg*, *Qt*) are mentioned **by name only**, not as URLs.
3. **This README never names other consumer downloader applications.** No comparisons such as "like X" or "the Y alternative", no logos, no feature-by-feature tables against shipping products. The application is described on its own terms.
4. **After every completed task the project owner suggests one or more concrete additions to the project**, drawn either from open ideas already in this document or from features common in serious downloader programs (without naming them). This rule is to keep the project moving — every closed task seeds the next one.
5. **This is a serious, complete program — not a toy.** Code quality, error handling, accessibility, theming consistency, cross-platform builds and reproducibility are all treated as first-class concerns.

These rules are repeated in `CONTRIBUTING` workflow (PR template / commit messages) and are non-negotiable.

---

## What it does

- Paste a video URL, pick a quality, hit **Download**. yt-dlp does the actual extraction; the application drives it.
- Multiple sites supported via yt-dlp's extractors (the long tail of supported services is not enumerated here on purpose — see `yt-dlp --list-extractors` for the live list).
- Per-job progress: percentage, bytes, speed, ETA, live status (`Resolving`, `Downloading`, `Post-processing`, `Done`, `Failed`, `Canceled`).
- True download queue with configurable parallelism (1–8 concurrent jobs).
- Video pipeline: pick a max height (Best / 2160 / 1440 / 1080 / 720 / 480 / 360) and container (mp4 / mkv / webm). yt-dlp merges the best video + audio streams via ffmpeg.
- Audio extraction pipeline: pick a container (mp3 / m4a / opus / wav / flac / aac / vorbis) and a quality (0 = best). One click on **Audio only** in the Home page.
- Optional add-ons toggled in Settings: write / embed subtitles (with language list), write / embed thumbnail, embed metadata, embed chapters, SponsorBlock skip-sponsor.
- Network knobs: speed limit (KB/s), retries, proxy URL, cookies from a chosen browser, cookies file, custom User-Agent.
- Three themes:
  - **Snow** — pure white floor, neutral grays, slightly deeper amber accent so the brand stays visible on light surfaces.
  - **Dark** — graphite floor with amber accent. The default.
  - **Blackout** — pure `#000000` floor for OLED panels and matte-black setups.
- A **Compact** mode that collapses the sidebar to a 64-px rail of icons and shrinks the window.
- Bilingual UI (Russian / English), switchable at runtime without restart.
- Frameless window with a custom title bar and corner resize handle.

---

## Design philosophy

The UI is intentionally **not a clone** of any other downloader. The reference points are:

- **Acri-style surfaces.** Frosted, slightly translucent cards (`AcrylicCard.qml`) sit on a soft amber-tinted background (`AcrylicBackground.qml`) with three "lamp" gradients to give the floor depth without using a blur effect (the QtGraphicalEffects blur module is not always available on every Qt distribution, so the design replaces it with overlaid gradients and a 1-px top highlight on every card).
- **One accent colour.** Everything brand-related uses `#F59E0B` (amber). Hover and pressed states only adjust lightness; we never introduce a second hue for actions, only for status (`success = #22C55E`, `warning = #F59E0B`, `error = #F43F5E`).
- **Calm typography.** A single font stack (Inter → Segoe UI → sans-serif) at three sizes (11 / 13 / 22) carries every screen. Headlines are 22 / DemiBold; section labels are 10 / Bold / 1.4 letter-spacing / uppercase; everything else is 13 / Regular.
- **The sidebar is the navigation.** No tabs, no breadcrumbs. The active row gets the amber stripe + accent-tinted background; everything else is hover-only.
- **Settings are flat, not nested.** Six grouped cards on a single scrollable page (`SettingsPage.qml`). No tabs, no "advanced" hidden behind a checkbox — just clearly-headed sections.

---

## Architecture

```
Volchaydownloader/
├── CMakeLists.txt              # Qt6 project, executable + windeployqt staging
├── LICENSE                     # MIT
├── .gitignore                  # CMake / Qt / IDE / OS artifacts
├── src/                        # C++ backend
│   ├── main.cpp                # QGuiApplication, QML context properties, engine
│   ├── Settings.{h,cpp}        # QSettings-backed flexible configuration
│   ├── Theme.{h,cpp}           # Snow / Dark / Blackout palette
│   ├── Locale.{h,cpp}          # Static RU/EN translation table, i18n.t()
│   ├── ToolsLocator.{h,cpp}    # Finds yt-dlp / ffmpeg on disk
│   ├── DownloadJob.{h,cpp}     # Plain struct describing one job
│   ├── DownloadQueueModel.{h,cpp}  # QAbstractListModel exposing the queue
│   └── DownloadManager.{h,cpp} # Spawns yt-dlp QProcesses, parses progress
├── qml/
│   ├── Main.qml                # Root window, title bar, sidebar, page stack
│   ├── components/             # Reusable widgets (cards, buttons, fields…)
│   └── pages/                  # HomePage, QueuePage, HistoryPage, SettingsPage, AboutPage
├── resources/
│   ├── qml.qrc                 # Qt resource manifest (QML + icons)
│   ├── app.rc                  # Windows resource (icon + version info)
│   └── icons/                  # PNG/ICO icon set
└── .github/workflows/
    ├── build-linux.yml         # Linux smoke build (Qt6 from apt)
    └── build-windows.yml       # Windows .exe + windeployqt + bundled tools
```

### Backend (C++)

| Class | Role | Notes |
|---|---|---|
| `Settings` | Persistent user configuration, exposed to QML as `settings`. | Every property uses `Q_PROPERTY` + `NOTIFY` + a QSettings-backed setter. Saving happens on the same call that mutates the value, so the file on disk is always consistent. `Q_INVOKABLE` helpers (`formatChoices()`, `audioFormatChoices()` …) feed the comboboxes. |
| `Theme` | Dynamic colour palette, exposed to QML as `theme`. | Re-emits `paletteChanged` whenever `settings.themeMode` changes. All colours come from a switch on the current mode — no QtQuickControls styling hacks required. |
| `Locale` | Built-in RU/EN translation table, exposed as `i18n`. | We don't use `tr()` / `QTranslator` because the catalogue is small and live language switching is required. `i18n.t("nav.home")` looks up by key; missing keys fall back to themselves so untranslated strings are immediately visible. `main.cpp` re-binds the `i18n` context property and calls `engine.retranslate()` whenever the language changes. |
| `ToolsLocator` | Finds `yt-dlp` and `ffmpeg`. | Search order: (1) user override from Settings, (2) directory next to the executable (this is how the Windows release ships its tools), (3) `$PATH`. Reports version strings by running the binaries once on startup. Exposed as `tools`. |
| `DownloadJob` | POD describing one download (id / url / title / progress / status / log tail …). | Lives only inside the model. |
| `DownloadQueueModel` | `QAbstractListModel` of `DownloadJob`. | Single source of truth for queue state. Exposes named roles (`title`, `progress`, `status`, …) so QML delegates can bind directly. Counts (`activeCount`, `queuedCount`, `finishedCount`, `failedCount`) are bindable Q_PROPERTYs. |
| `DownloadManager` | Owns the per-job QProcess pool, exposed as `manager`. | Public entry points: `enqueue(url)`, `enqueueWithFormat(url, choice)`, `cancelJob(id)`, `retryJob(id)`, `removeJob(id)`, `revealInFileManager(path)`, `openOutputDir()`, `looksLikeSupportedUrl(url)`. |

### Progress parsing

yt-dlp prints one progress line per chunk thanks to `--newline`. We add `--progress-template` with a fixed format that uses the Unit Separator (`U+001F`) between fields:

```
VDPROG<US>downloaded_bytes<US>total_bytes_or_estimate<US>speed<US>eta<US>status<US>title<US>uploader<US>duration<US>thumbnail
```

`DownloadManager::parseProgressLine()` splits on the separator, updates the corresponding `DownloadJob` fields in the model, and the QML delegate re-binds via `dataChanged`. We also pass `--print after_move:VDPROGFINAL<US>%(filepath)s` so we know the on-disk path after any merge/extract step and can open the file from the queue.

`status == "downloading"` flips the row to `Downloading`, `status == "finished"` flips it to `PostProcessing` (yt-dlp is still going through ffmpeg merge / audio extraction at this point), and `QProcess::finished` flips it to `Finished` / `Failed` / `Canceled`.

### Queue scheduling

`DownloadManager::pump()` is the only scheduler. Every time a new job is enqueued, a running job finishes, or `settings.maxConcurrent` changes, we walk the model in row order and start jobs while `m_active.size() < limit`. There is no separate background thread — `QProcess` is fully asynchronous and lives on the GUI thread.

### Settings persistence

`QSettings` writes platform-native storage:
- Windows → `HKCU\Software\Volchay\Volchay-Downloader`
- Linux → `~/.config/Volchay/Volchay-Downloader.conf`
- macOS → `~/Library/Preferences/com.Volchay.Volchay-Downloader.plist`

The setter macro (`VD_DEFINE_SETTER`) writes through to QSettings on every change, so even an unexpected crash will not lose the user's most recent click.

### QML structure

- `Main.qml` is a `FramelessWindow` (`Qt.FramelessWindowHint`) with our own `TitleBar.qml` (drag handle delegates to `Window::startSystemMove()` on platforms that support it, and falls back to manual `mapToGlobal` math otherwise). A 16 × 16 invisible bottom-right `MouseArea` handles resize via `startSystemResize()`.
- `AcrylicBackground.qml` paints the vertical gradient floor and three soft circular "lamps" tinted with `theme.accent`. On the Snow theme the lamps drop to ~5 % opacity so they don't tint the white floor; on Blackout they drop to ~4 % so the OLED look is preserved.
- `AcrylicCard.qml` is the universal surface: rounded 14-px corners, 1-px border in `theme.border`, optional 3-px amber left stripe (`accentStripe: true`) used by active queue cards.
- `pages/HomePage.qml` is one URL row + one quality / output-dir card + three status chips (yt-dlp version, ffmpeg version, queue counts). Hitting Enter or clicking **Download** enqueues and switches to the Queue page.
- `pages/SettingsPage.qml` is six grouped cards (Appearance / Output / Format / Extras / Network / Advanced) on a single scrollable column. Every control binds directly to a `settings.<property>`.

---

## Building

### Prerequisites

| OS | Toolchain | Qt |
|---|---|---|
| Linux | `cmake` ≥ 3.21, `ninja`, GCC ≥ 11 | Qt 6.2+ (`qt6-base-dev qt6-declarative-dev qt6-svg-dev qt6-tools-dev` and the matching `qml6-module-*` runtime modules) |
| Windows | Visual Studio 2022 (MSVC v143) **or** MSVC build tools, `cmake`, `ninja` | Qt 6.6+ for MSVC 64-bit (online installer / `aqt`); the GitHub workflow uses the `jurplel/install-qt-action` |
| macOS | Xcode CLT, Homebrew `cmake ninja qt` | Qt 6.6+ |

External runtime tools (the application **does not bundle them on Linux/macOS** — install via your package manager):
- `yt-dlp` (Python 3.8+)
- `ffmpeg` (any recent build)

On Windows the release artefact built by the workflow includes `yt-dlp.exe` and `ffmpeg.exe` / `ffprobe.exe` next to the `.exe` — no separate install required.

### Local Linux build

```bash
sudo apt-get install -y \
  build-essential ninja-build cmake pkg-config \
  qt6-base-dev qt6-declarative-dev qt6-tools-dev qt6-svg-dev \
  qml6-module-qtquick qml6-module-qtquick-controls \
  qml6-module-qtquick-layouts qml6-module-qtquick-window \
  qml6-module-qtquick-shapes qml6-module-qtquick-dialogs \
  qml6-module-qtqml-workerscript

cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
./build/Volchay-Downloader
```

### Local Windows build (VS 2022 64-bit prompt)

```powershell
cmake -S . -B build -G "Ninja" -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
windeployqt --release --no-translations --qmldir qml build\Volchay-Downloader.exe
```

### CI

Two workflows live in `.github/workflows/`:

- `build-linux.yml` — Ubuntu 22.04, apt-installed Qt 6, `cmake + ninja`, runs on every push / PR. Acts as the canonical syntactic-correctness smoke test (any new C++ or QML mistake fails CI within a couple of minutes).
- `build-windows.yml` — Windows Server 2022, Qt 6.6.3 MSVC 2019 64-bit (installed via `jurplel/install-qt-action`), MSVC env via `ilammy/msvc-dev-cmd`, `cmake + ninja`. After build it runs `windeployqt`, downloads the latest `yt-dlp.exe` and an *essentials* ffmpeg build, zips everything next to the `.exe`, and uploads the bundle as a release artefact.

---

## Detailed development log

This section grows over time. Every change to the code base is summarised here so the reasoning is preserved in-tree.

### Step 1 — Project rules and scaffold

- Created `LICENSE` (MIT) and `.gitignore` (covers CMake / Qt build artefacts, IDE files, OS junk).
- Wrote the top-level `CMakeLists.txt`. Minimum CMake 3.21 — we need `qt_add_executable` (Qt 6 way) and target-level `BUNDLE` properties. C++17, `AUTOMOC / AUTORCC / AUTOUIC` on, default Release build. Required Qt6 components: `Core Gui Qml Quick QuickControls2 Svg Network`. The executable target is named `VolchayDownloader` internally but the binary is named `Volchay-Downloader` via `OUTPUT_NAME` so the on-disk artefact uses the brand spelling.
- Windows-only branch picks up `resources/app.rc` so the compiled `.exe` carries a real icon + version metadata.
- `qt_add_executable(... WIN32 MACOSX_BUNDLE ...)` — `WIN32` removes the console window on Windows; `MACOSX_BUNDLE` produces an `.app` on macOS.

### Step 2 — Application entry point

- `src/main.cpp` creates a `QGuiApplication` (we don't need `QApplication` because we're QML-only and don't use widgets), sets organisation/application/version metadata, forces the **Basic** Quick Controls style so our flat custom widgets are not overridden by any platform-native default style, and antialiases the default font.
- Backend instances are stack-allocated and the lifetime is owned by `main` for unambiguous destruction order: `Settings → Locale → Theme → ToolsLocator → DownloadQueueModel → DownloadManager`.
- All instances are registered with the QML root context as `settings`, `i18n`, `theme`, `tools`, `queue`, `manager`. Re-binding the `i18n` property + calling `engine.retranslate()` on language change forces every `i18n.t("…")` binding to re-evaluate.

### Step 3 — Flexible Settings

- `Settings` exposes 30+ Q_PROPERTYs covering theme, language, output (dir, template, ASCII restriction, organise-by-uploader), format (default quality, video container, audio format, audio quality, prefer-free-formats), extras (subs / auto-subs / embed subs / sub languages / thumbnail / embed thumbnail / metadata / chapters / SponsorBlock), network (max concurrent, speed limit, retries, proxy URL, cookies-from-browser, cookies file, User-Agent) and advanced (yt-dlp path override, ffmpeg path override, free-form extra args).
- The `VD_DEFINE_SETTER` macro keeps every setter consistent: guard against no-op assignments, save through QSettings, emit the corresponding `…Changed()` signal.
- `resetToDefaults()` clears QSettings and re-runs the constructor in place so the in-memory state and the on-disk state are guaranteed to agree afterwards. Every change signal is fired so QML re-binds.

### Step 4 — Theme

- `Theme` returns colours via switches on `Settings::themeMode`. It connects to `Settings::themeModeChanged` and re-emits `paletteChanged()` — every Q_PROPERTY in the class lists that signal in its `NOTIFY`, so a single mode switch repaints the entire window.
- The amber accent is `#F59E0B` (Tailwind amber-500) on Dark / Blackout, deepened to `#D97706` on Snow so it stays readable on white.
- `accentTextOn = #1A1612` (near-black amber-tinted) — bright amber + dark text is the highest-contrast pairing across all three themes.
- Soft amber (`accentSoft`) is used for the active sidebar background and theme-pick chips. Different alpha for blackout vs the others so the OLED look is preserved.

### Step 5 — Locale (i18n)

- One Russian / English string per UI key, stored in a `QHash<key, QHash<lang, text>>`. `t("foo")` returns the language-specific value (falling back to RU, then to the key for diagnostics).
- `t1(key, arg1)` / `t2(key, arg1, arg2)` are thin wrappers around `QString::arg`. They cover the few strings that need parameter substitution.

### Step 6 — ToolsLocator

- Three-step lookup (override → app dir → `$PATH`). On Windows we add `.exe`; on Linux/macOS the suffix is empty.
- Versions are fetched by running each binary once with the appropriate flag (`yt-dlp --version`, `ffmpeg -version`) and capturing the first line. Three-second start timeout, five-second finish timeout — generous enough to handle a slow first-run yt-dlp self-update check.
- Re-runs on every change to the path overrides in Settings.

### Step 7 — DownloadJob / DownloadQueueModel

- `DownloadJob` is a POD with a strong `Status` enum.
- The model exposes 22 named roles so QML can bind directly: `jobId`, `url`, `title`, `uploader`, `thumbnailUrl`, `durationMs`, `totalBytes`, `downloadedBytes`, `speedBps`, `etaSec`, `progress`, `status`, `statusText`, `outputFile`, `outputDir`, `formatChoice`, `errorText`, `startedAt`, `finishedAt`, `logTail`, `isActive`, `isTerminal`.
- Mutators (`addJob`, `updateJob`, `mergeJob`, `removeJobById`) are the only way to change a row. `mergeJob` takes a lambda so the caller can mutate the live job in place — `DownloadManager` uses this almost exclusively to keep partial updates atomic.
- `clearFinished` / `clearFailed` / `clearAll` are exposed `Q_INVOKABLE` so the QML toolbar buttons can drive them.

### Step 8 — DownloadManager

- Owns a `QHash<int /*jobId*/, Active>` where `Active` holds the live `QProcess` plus newline carry-over buffers for stdout / stderr.
- `buildYtDlpArgs()` is where every Settings property becomes a yt-dlp flag. `--newline --no-colors --encoding utf-8` keep parsing simple. `--progress-template` writes the custom progress format described in *Architecture*. `--print after_move:VDPROGFINAL\x1f%(filepath)s` reveals the destination after merge.
- Format choices map as follows:
  - `best` → `bv*+ba/b`
  - `2160 / 1440 / 1080 / 720 / 480 / 360` → `bv*[height<=N]+ba/b[height<=N]/b`
  - `audio` → `-f bestaudio/best -x --audio-format <Settings.audioFormat>`
- `--merge-output-format` keeps the final container consistent with Settings.containerVideo when merging streams.
- Optional flags are gated on the corresponding Settings boolean (`--write-subs`, `--embed-subs`, `--embed-metadata`, `--sponsorblock-remove sponsor`, …).
- Cookies: `--cookies-from-browser <name>` (when the user picked a browser) or `--cookies <path>` (when they pointed at a cookies.txt file).
- `--ffmpeg-location` is set to the directory of the resolved ffmpeg so yt-dlp's internal merge step uses the same binary the application reports under About.
- Extra args from `settings.extraArgs` are split with a single regex that respects double and single quotes.
- `cancelJob()` marks the row as `Canceled` *before* terminating the QProcess so the `finished` handler doesn't mis-classify the non-zero exit as a failure.
- `retryJob()` clears progress + error fields and bumps the status back to `Queued`; the next pump picks it up.

### Step 9 — Main.qml: window, title bar, sidebar, page stack

- Frameless window, `Qt.FramelessWindowHint`. A 1-px `Rectangle` rim sits at `z: 1000` above the content for a clean window edge on every theme.
- `TitleBar.qml` is a 38-px row with the app logo, app name, a `MouseArea` drag handle (`startSystemMove()` on supported platforms, manual `mapToGlobal` fallback otherwise), and three trafficlight buttons (minimise / maximise / close). The close button hover state turns red (`#F43F5E`-ish) so it's instantly recognisable.
- Sidebar is a 232-px column (collapses to 64 px in compact mode) of `SidebarItem.qml` rows. Each row shows an inline `SidebarIcon.qml` (vector glyph drawn with QtQuick.Shapes — no SVG file dependency) and the localised label, plus a small amber badge when the row is "Queue" and there are active or pending jobs.
- Page switching is `currentPage: string` + a `StackLayout`. Animations are kept light (200 ms cubic out on the sidebar width, no slide-in/out between pages) so the app feels snappy.
- Bottom-right resize handle is a 16 × 16 `MouseArea` that calls `startSystemResize(Qt.BottomEdge | Qt.RightEdge)` first and falls back to manual width/height updates if the platform doesn't support native resize.

### Step 10 — Reusable QML components

- `AcrylicBackground.qml` — gradient floor + three soft amber lamps. Lamp opacities adapt to theme.
- `AcrylicCard.qml` — universal surface; optional accent stripe, optional cheap shadow.
- `AmberButton.qml` / `GhostButton.qml` — primary and secondary buttons, both 38 px tall.
- `TextFieldA.qml` / `NumberField.qml` / `ComboField.qml` / `PathField.qml` / `SwitchA.qml` — themed input controls. Focus state shows a 1.5-px amber border instead of the platform-default ring.
- `ProgressBar.qml` — 4-px amber fill with a separate indeterminate animation for the *Resolving* / *Post-processing* phases.
- `QueueItemCard.qml` — single row in the queue list. Shows title + uploader + status + (cancel | retry | open | remove) buttons + progress bar + numeric line (`xx % · 12 MB / 100 MB · 4.5 MB/s · ETA 1:23`).
- `StatusToast.qml` — fade-in / fade-out chip at the bottom centre; bound to `manager.toastEmitted`.
- `SectionHeader.qml` — small all-caps section label used in the Settings page.

### Step 11 — Pages

- `HomePage.qml` — URL row (paste / clear), quality + output-dir form, "Add to queue" + primary "Download" buttons, three status chips. Enter on the URL field triggers the primary action.
- `QueuePage.qml` — scrollable list of `QueueItemCard`s, empty state, "Clear finished" / "Clear errors" toolbar.
- `HistoryPage.qml` — same model, filtered to terminal jobs (Finished / Failed / Canceled).
- `SettingsPage.qml` — six grouped cards (Appearance, Output, Format & quality, Extras, Network & concurrency, Advanced) on one scrollable column. A "Reset to defaults" button at the bottom asks for confirmation before calling `settings.resetToDefaults()`.
- `AboutPage.qml` — version, description, and a small panel with the resolved paths + versions of `yt-dlp` and `ffmpeg`.

### Step 12 — Resources

- `resources/qml.qrc` bundles every QML file under `qrc:/qml/…` and the icon set under `qrc:/icons/…`. Aliases keep the in-resource paths short.
- `resources/icons/app.png` (used as the window icon) and `resources/icons/app.ico` (used by the Windows resource) are generated programmatically — a rounded amber-tinted square with the amber play triangle. No external image editor in the loop.
- `resources/app.rc` declares the icon + the embedded version block (`FILEVERSION`, `CompanyName`, `ProductName`, …) used by Windows Explorer when right-clicking the `.exe`.

### Step 13 — Robust cookies-from-browser (yt-dlp lock workaround)

- `--cookies-from-browser chrome` (and its sister flags `edge`, `brave`, `opera`, `vivaldi`, `chromium`) routinely fails on Windows with the warning *"Could not copy Chrome cookie database. See https://github.com/yt-dlp/yt-dlp/issues/7271 for more info"*. The root cause is that running Chromium-based browsers hold their `Cookies` SQLite database open with an exclusive write lock, so yt-dlp's own `shutil.copy` can't take a snapshot to read from. Downloads that need authentication (age-restricted YouTube, member videos, channel-only content) then fall back to anonymous requests and fail with a 403 or "Sign in to confirm your age".
- `src/CookiesPreparer.{h,cpp}` is a small helper that sidesteps the issue from the application's side. Before each download that requests `cookies-from-browser`, the preparer:
  1. Resolves the default user-data directory for the selected browser using OS-aware paths (`%LOCALAPPDATA%\Google\Chrome\User Data` on Windows, `~/.config/google-chrome` on Linux, `~/Library/Application Support/Google/Chrome` on macOS, plus the corresponding paths for Edge / Brave / Opera / Vivaldi / Chromium).
  2. Snapshots the few files yt-dlp actually reads — `Local State` for the DPAPI-wrapped AES key plus the `Default/Cookies` SQLite and its `-journal` / `-wal` / `-shm` sidecars (and the `Network/Cookies` variant used by recent Chrome builds) — into a per-job temporary directory.
  3. Uses `QFile::copy` with a five-attempt retry/backoff loop (100 / 200 / 300 / 400 / 500 ms). Qt's `CopyFileExW` on Windows opens both ends with `FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE`, and the retries ride out the brief windows when Chromium briefly takes an exclusive lock for a write transaction, which is the failure mode `shutil.copy` hits.
  4. Rewrites the yt-dlp invocation to `--cookies-from-browser <browser>:<abs/path/to/temp/profile>`. yt-dlp accepts an absolute path as the PROFILE argument and reads `Local State` from the parent of that path, so our staged layout (`<tmp>/Local State` + `<tmp>/Default/Cookies`) plugs in without further changes on the yt-dlp side.
  5. Hands the temp directory's lifetime to `DownloadManager`, which calls `CookiesPreparer::cleanupForJob()` from `onProcessFinished()` whether the job succeeded, failed, or was cancelled. The destructor's `cleanupAll()` mops up any orphans on app exit.
- If the snapshot fails outright (e.g. the browser holds the lock for the full backoff window, the user-data dir doesn't exist on this machine, or the profile is named something other than `Default`), the preparer falls back to passing `--cookies-from-browser <browser>` unchanged so the user is never worse off than before this step.
- Firefox / Safari are passed through unchanged — Firefox's SQLite WAL mode allows shared reads and Safari is read via macOS APIs that don't touch the database file directly, so they don't hit the lock problem.
- `DownloadManager::parseStderrLine()` watches for the literal `"Could not copy … cookie database"` line (case-insensitive substring) and tags the job with `errorKey = "error.cookiesLocked"`. The QML `QueueItemCard` prefers `i18n.t(errorKey)` over the raw `errorText`, so the user sees a translated, actionable sentence (*"Закрой Chrome / Edge / Brave полностью…"*) instead of yt-dlp's English warning with a GitHub link.
- The Settings page's *Cookies from browser* dropdown now renders an inline 11 px hint underneath whenever the selected browser is a Chromium-based one, explaining the limitation and pointing at the *Cookies file (Netscape)* alternative for users who prefer to keep their browser running.
- New role `errorKey` on `DownloadQueueModel`, new field `DownloadJob::errorKey` and a matching reset in `retryJob()` so a retried row starts clean.

### Step 14 — GitHub Actions

- `build-linux.yml` installs the same apt packages listed in the *Building* section, configures CMake with Ninja, builds, and asserts the resulting binary exists and is executable. Fast: the whole job typically completes in well under five minutes and acts as the canonical "did I break the build?" gate.
- `build-windows.yml` installs Qt 6.6.3 via `jurplel/install-qt-action` (with module cache), pulls in MSVC via `ilammy/msvc-dev-cmd`, configures CMake with Ninja + `cl.exe`, builds Release, then runs `windeployqt --qmldir qml` against the freshly-built `.exe`. After that it downloads the latest official `yt-dlp.exe` and an *essentials* ffmpeg build, copies `ffmpeg.exe` / `ffprobe.exe` next to the `.exe`, zips the result, and uploads the artefact (and the raw `dist/` tree). The job retains the artefact for 30 days.

---

## Suggested next features

These are explicit follow-up ideas — every closed task should pick at least one (project rule #4):

- **Drag-and-drop URL targets** — accept dropped links and `.txt` link lists on the Home page.
- **System tray icon** with quick "paste & download from clipboard", and pause/resume of the queue.
- **Per-job rate limit** (override the global speed limit on a single row).
- **Subtitle-only mode** — fetch only `.vtt` / `.srt` for a list of URLs without downloading the video.
- **Playlist range / item picker** — UI for `--playlist-items 1-5,8,12-15` and `--match-filter`.
- **Built-in updater for yt-dlp** — invoke `yt-dlp -U` (or download the latest release on Windows) directly from About.
- **Theming knobs** — accent-colour picker (any hue, keeping the same surface system), font-size scale, sidebar width.
- **Format inspector** — show the JSON returned by `yt-dlp -j` for the current URL in a side panel so power users can pick a specific format by id.
- **Browser-extension companion** — small Manifest V3 extension that pings the running app over a localhost socket to enqueue the current tab's URL.

---

## License

This project is licensed under the MIT License — see the `LICENSE` file for details.
