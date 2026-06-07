# The Ultimate Computer Science, Backend & System Design Mastery

This repository documents a strictly sequential learning path from absolute Computer Science fundamentals to Expert-level System Design. The curriculum is designed to ensure no advanced architecture is learned without first deeply understanding the underlying mechanics of the operating system, memory, and network (checkout notes/ at each sub-phase for the mechanics and concepts).

## Repository Structure

The repository is divided into 4 main phases. Each phase contains sub-phases organized by specific concepts. Inside each sub-phase, you will find:
- Markdown notes detailing architectural theory and runtime mechanics.
- Downloaded architectural diagrams (Eraser.io / Mermaid).
- Source code (C++, Node.js) demonstrating the concept in an isolated environment.
- Docker configurations (when applicable) for testing infrastructure.

### Directory Layout

```text
system-design-mastery/
├── README.md
├── .gitignore
├── X_Phase_Name/
│   ├── X.Y_Sub_Phase_Name/
│   │   ├── notes.md                            # Notes in markdown form
│   │   ├── diagrams/                           # Concept diagram images
│   │   ├── src/                                # Implementation code
│   │   │   └── (e.g., main.cpp, server.js)
│   │   └── docker-compose.yml                  # If required for infrastructure
```

## Tools & Environment

- **OS**: Windows Subsystem for Linux (WSL2) - Ubuntu
- **Languages**: C++ (for low-level OS/memory mechanics), JavaScript/TypeScript (Node.js for backend runtime mechanics)
- **Linux Core Utilities**: strace, lsof, top
- **System Calls**: epoll, kqueue
- **Infrastructure**: Docker, Nginx, Redis, Kafka
- **Diagramming**: Eraser.io

## Learning Methodology

Every concept in this repository is learned and documented through a strict multi-stage process, starting from the absolute fundamental definitions before building up to complex systems:

1. **Setup Stage**: Configuring the local environment, installing required compilers (g++), runtimes (Node.js), or infrastructure tools (e.g., Docker) necessary for the specific sub-phase.
2. **Stage 1: Concept & Core Problem**: Defining the fundamental computer science concept from the absolute basics (e.g., defining what a process or TCP is before explaining how it scales) and mapping out the engineering problem via visual data-flow representations.
3. **Stage 2: Technical Walkthrough**: Understanding how the mechanism operates inside a real runtime environment (e.g., OS kernel scheduling, memory allocation boundaries, or V8 engine event loop).
4. **Stage 3: Implementation**: Writing bare-minimum, dependency-free code to demonstrate the system mechanics.
5. **Stage 4: Code Breakdown**: A line-by-line mapping of the code back to the architectural theory from Stage 1 and the system runtime mechanics from Stage 2.