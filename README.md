# Particle Reconstruction

C++ framework for the analysis of neutral particle decays. Reads a stream of decay
events, reconstructs each decay under two competing mass hypotheses, and measures the
invariant mass and the mean proper lifetime of each species. Results are written to
ROOT histograms.

Written for the Object-Oriented Programming course in Physics at @unipd.
The `util/` directory was provided as course material.

---

## Physics

Each event contains a decay vertex and its charged daughter tracks. Only two-track
events with zero total charge are reconstructed; anything else is tagged `unknown`.

Two decay hypotheses are tested per event:

| Hypothesis | Daughters | PDG mass (GeV/c²) |
|---|---|---|
| K⁰ | π⁺ π⁻ | 0.497611 |
| Λ⁰ | p π⁻ | 1.115683 |

For each hypothesis the daughter energies are computed from the measured momenta and
the assumed masses, and the invariant mass of the pair is reconstructed. The hypothesis
whose invariant mass falls closer to its PDG value is assigned to the event.

The proper decay time follows from the flight distance `d` between the origin and the
decay vertex:

```
t = d · m / (p · c)
```


Two independent measurements run over the event stream:

- **Invariant mass** — running mean and RMS per species, over a configurable mass window.
- **Lifetime** — decay times are collected per species and fitted to an exponential by
  scanning the negative log-likelihood over a range of candidate lifetimes; the minimum
  is located by fitting a parabola to the scan (`QuadraticFitter`).

### Known limitation

The Λ⁰ hypothesis assigns the proton mass to the positive track only, so Λ̄⁰ (p̄ π⁺) is
not reconstructed correctly. The sample is assumed to contain particles, not
antiparticles.

---

## Requirements

- A C++11-compatible compiler
- ROOT ([root.cern](https://root.cern)), *or* the `fakeROOT` stub bundled in `util/`

`util/fakeROOT` provides minimal stand-ins for `TH1F`, `TFile` and `TDirectory`, enough
to run the analysis and dump the histograms without a full ROOT installation. Install it
with `util/fakeROOT/install` and source `util/fakeROOT/bin/thisroot.sh` instead of the
real one.

---

## Build

```bash
source <root_install_dir>/bin/thisroot.sh
./compile
```

The script builds three shared libraries (`libAnalysisFramework.so`,
`libAnalysisUtilities.so`, `libAnalysisObjects.so`), one library per plugin, and four
executables that differ only in which analyzers they link:

| Executable | Analyzers linked |
|---|---|
| `runDump` | `EventDump` |
| `runPlot` | `ParticleMass` |
| `runLife` | `ParticleLifetime` |
| `runAll` | all three |

Plugins are loaded from the current directory, so:

```bash
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:.
```

---

## Usage

```
./runAll (input <file> | sim <N> [seed <N>]) [plot <file>] [lifetime <file>] [output <name>] [dump]
```

| Argument | Meaning |
|---|---|
| `input <file>` | Read events from file |
| `sim <N>` | Generate `N` events with the built-in Monte Carlo |
| `seed <N>` | RNG seed for `sim` |
| `plot <file>` | Mass-analysis configuration (see below) |
| `lifetime <file>` | Lifetime-fit configuration (see below) |
| `output <name>` | ROOT output file, written as `<name>.root` (default `data.root`) |
| `dump` | Print raw event content to stdout |

Each analyzer is selected by the presence of its keyword: `plot` activates the mass
analysis, `lifetime` the lifetime fit, `dump` the event dump. Omitting the keyword
disables that analyzer.

```bash
# Full analysis on the bundled sample
./runAll input particle_events.txt plot massRanges lifetime particleFitters output results

# Simulated sample, 10000 events, fixed seed
./runAll sim 10000 seed 42 plot massRanges lifetime particleFitters output results

# Inspect the input
./runDump input particle_events.txt dump
```

---

## File formats

### Event file

Whitespace-separated, free-format. Per event, a header followed by one line per track:

```
<id> <x> <y> <z> <nTracks>
<charge> <px> <py> <pz>
...
```

Vertex coordinates in cm, momenta in GeV/c. `particle_events.txt` is a bundled sample.

### Mass configuration (`plot`)

One line per species — the mass window used both for the running statistics and for the
histogram range:

```
<name> <massMin> <massMax>
```

See `massRanges`.

### Lifetime configuration (`lifetime`)

One line per species:

```
<name> <massMin> <massMax> <tMin> <tMax> <scanMin> <scanMax> <scanStep>
```

`massMin`/`massMax` select which events enter the fit, `tMin`/`tMax` set the histogram
range in ps, and `scanMin`/`scanMax`/`scanStep` define the likelihood scan over the
candidate lifetime. See `particleFitters`.

---

## Project structure

```
.
├── AnalysisFramework/          # Event loop and infrastructure
│   ├── AnalysisInfo            # Command line parser
│   ├── AnalysisSteering        # Base class for analyzers
│   ├── AnalysisFactory         # Analyzer factory
│   ├── SourceFactory           # Event source factory
│   ├── Event                   # Decay vertex + tracks
│   ├── EventSource             # Abstract event loop
│   ├── EventReadFromFile       # File-based source
│   └── EventSim                # Monte Carlo generator
│
├── AnalysisUtilities/          # Stateless helpers
│   ├── Constants               # Particle masses, c
│   ├── Utilities               # Energy and invariant mass
│   └── QuadraticFitter         # Least-squares parabola fit
│
├── AnalysisObjects/            # Reconstruction and statistics
│   ├── ParticleReco            # Mass hypotheses, type assignment
│   ├── ProperTime              # Decay time
│   ├── MassMean                # Running mean/RMS
│   └── LifetimeFit             # Log-likelihood lifetime fit
│
├── AnalysisPlugins/            # Analyzers, one library each
│   ├── EventDump
│   ├── ParticleMass
│   └── ParticleLifetime
│
└── util/                       # Course material, not written by me
    ├── include/                # Singleton, Dispatcher, Active/LazyObserver, Random
    ├── fakeROOT/               # Minimal ROOT stand-in
    └── src/random/             # Random number generators
```

Dependencies run one way, `AnalysisPlugins` → `AnalysisObjects` → `AnalysisUtilities` →
`AnalysisFramework` → `util`, with no cycles.

---

## Design notes

| Pattern | Where |
|---|---|
| Singleton | `ParticleReco`, `ProperTime` — one instance per run |
| Abstract Factory | `AnalysisFactory`, `SourceFactory` — analyzers and sources created by name at runtime |
| Observer (active) | Analyzers, via `AnalysisSteering`: process each event as dispatched |
| Observer (lazy) | `ParticleReco`, `ProperTime`: reconstruction deferred until requested, then cached |

**Dispatcher.** Event source, reconstruction and analyzers never reference each other;
all communication goes through the typed dispatcher in `util/include`.

**Lazy reconstruction.** `ParticleReco` and `ProperTime` receive the event notification
but compute nothing until a consumer calls `par_mass()`, `par_energy()` or
`decayTime()`; `check()` recomputes only when the event has changed. The mass and
lifetime analyzers therefore share a single reconstruction per event, and `ProperTime`
is itself a consumer of `ParticleReco`, so the chain resolves on demand.

**Runtime plugin registration.** Each analyzer compiles to its own shared library and
registers a concrete factory under a name (`"plot"`, `"lifetime"`, `"dump"`) through a
static object constructed before `main`. The framework has no compile-time knowledge of
any analyzer; adding one means a new class and one static declaration.

**Memory model.** Raw pointers and manual heap allocation throughout, following HEP
conventions and the C++11 target. Copy constructors and assignment operators are deleted
so that ownership stays unambiguous.
