# Particle Physics Reconstruction

A C++ data analysis framework that reconstructs particle decay events, fits statistical distributions, and outputs results to ROOT histograms. Built as part of a university physics computing course.

The physics domain is particle decay, but the engineering focus is on **clean architecture**, **design patterns**, and **handling large volumes of structured data**.

---

## What it does

The program reads a stream of particle collision events (from file or simulation), identifies which type of particle decayed in each event, and runs two independent analyses:

- **Mass reconstruction** — computes the invariant mass of each event and accumulates mean and RMS statistics
- **Lifetime fit** — collects decay times and fits an exponential lifetime via a log-likelihood scan, minimized with a quadratic fitter

Results are printed to stdout and saved as ROOT histograms.

---

## Design patterns used

| Pattern | Where |
|---|---|
| Singleton | `ParticleReco`, `ProperTime` — one shared reconstruction instance per run |
| Active/Lazy Observer | Analyzers subscribe to the event stream; computation is either immediate or deferred until requested |
| Abstract Factory | `AnalysisFactory`, `SourceFactory` — analyzers and data sources are registered and created by name at runtime |
| Lazy evaluation | Results are only computed when first requested, then cached |

---

## Project structure

The codebase is organized into four packages, each compiled into its own dynamic library:

```
.
├── AnalysisFramework/          # Core event loop and infrastructure
│   ├── main.cc
│   ├── AnalysisInfo            # CLI argument parser
│   ├── AnalysisSteering        # Base class for analyzers
│   ├── AnalysisFactory         # Plugin-style abstract factory
│   ├── SourceFactory           # Factory for event sources
│   ├── Event                   # Data model for a single collision event
│   ├── EventSource             # Abstract event loop
│   ├── EventReadFromFile       # File-based event source
│   └── EventSim                # Monte Carlo event generator
│
├── AnalysisUtilities/          # Stateless helpers and math tools
│   ├── Constants               # Physical constants
│   ├── Utilities               # Energy and invariant mass helpers
│   └── QuadraticFitter         # Least-squares quadratic fitter
│
├── AnalysisObjects/            # Physics reconstruction and statistics
│   ├── ParticleReco            # Reconstruction logic (Singleton + LazyObserver)
│   ├── ProperTime              # Decay time calculation (Singleton + LazyObserver)
│   ├── MassMean                # Running mean/RMS accumulator
│   └── LifetimeFit             # Log-likelihood lifetime fitter
│
└── AnalysisPlugins/            # Analyzers, one dynamic library each
    ├── EventDump               # Prints raw event content to stdout
    ├── ParticleMass            # Mass analysis pipeline (ActiveObserver)
    └── ParticleLifetime        # Lifetime analysis pipeline (ActiveObserver)
```

Each plugin in `AnalysisPlugins` is compiled as a **separate shared library** and loaded at runtime based on the command line arguments, so the executable itself has no compile-time dependency on any specific analyzer.

---

## Build


**Standard:** C++11 

Requires [ROOT 6.38.04](https://root.cern) and a C++11-compatible compiler.

Use ./compile bash script or :

```bash
# Source ROOT environment first
source <root_install_dir>/bin/thisroot.sh

# Build all packages
cd AnalysisFramework && make
cd ../AnalysisUtilities && make
cd ../AnalysisObjects && make
cd ../AnalysisPlugins && make

# Make libraries discoverable at runtime
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}":"
```

---

## Synopsis

```
./analysis (input <file> | sim <N> [seed <N>]) [plot <file>] [lifetime <file>] [output <name>] [dump]
```

## Usage

```bash
# Run on real data
./analysis input data.txt plot mass.txt lifetime lifetime.txt output results

# Run on simulated data (10000 events, seed 42)
./analysis sim 10000 seed 42 plot mass.txt lifetime lifetime.txt output results

# Dump raw event content
./analysis input data.txt dump
```

---

## Other info:


- **Decoupled pipeline** — the event source, reconstruction, and analysis layers have no direct dependencies on each other; communication goes entirely through a typed Dispatcher
- **Two observer variants** — the framework implements both an `ActiveObserver`, which processes every event immediately, and a `LazyObserver`, which defers computation until its result is actually requested. `ParticleReco` and `ProperTime` are lazy: they receive the event notification but only run the reconstruction when a downstream consumer calls `par_mass()` or `decayTime()`. This avoids redundant computation when multiple analyzers share the same reconstruction result
- **Plugin architecture** — each analyzer in `AnalysisPlugins` compiles to its own shared library. Adding a new analyzer requires no changes to the core framework, only a new class and a one-line static self-registration
- **Layered package design** — the four packages have a strict one-way dependency: `AnalysisPlugins` → `AnalysisObjects` → `AnalysisUtilities` → `AnalysisFramework`, with no circular dependencies
- **Separation of concerns** — data reading, physics reconstruction, statistical fitting, and output are all independent modules
- **Deliberate C++11 memory model** — the codebase uses raw pointers and manual heap allocation throughout, following the HEP (High Energy Physics) community standard. Copy constructors and assignment operators are explicitly deleted everywhere to prevent accidental copies and make ownership unambiguous

---

## License

MIT
