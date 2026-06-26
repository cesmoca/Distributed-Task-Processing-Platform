# Distributed Task Platform

> ⚠️ **Work in Progress**
>
> This project is being developed incrementally as a learning exercise to explore modern C++, concurrency, networking, distributed systems, fault tolerance, and AI-related infrastructure concepts.
>
> The implementation is intentionally divided into multiple phases:
>
> * Phase 1: In-memory scheduler and worker pool
> * Phase 2: TCP-based communication
> * Phase 3: Heartbeats and failure detection
> * Phase 4: Task retries and fault tolerance
> * Phase 5: Persistence and recovery
> * Phase 6: Command-line tooling
> * Phase 7: Metrics and observability
> * Phase 8: Web dashboard
> * Phase 9: AI-oriented workloads and vector search
>
> Each phase introduces new concepts while keeping the system functional and understandable.
>
> **Current progress:** Phase 1 is in progress. The project currently has an in-memory, templated thread-safe task queue, `std::jthread`-based workers and worker pool, a scheduler that tracks task state, and promise/future-based task-result waiting. The code is covered by GoogleTest unit tests. Networking has not been introduced yet.
>
> The sections below describe the long-term goals and architecture of the project.

---

## Overview

Distributed Task Platform is a small distributed computing system built from scratch in modern C++.

The goal is not to compete with production-grade systems such as Kubernetes, Celery, Ray, Kafka, Spark, or distributed job schedulers. Instead, the project focuses on understanding the fundamental building blocks behind those systems by implementing them manually.

The platform consists of:

* A central Scheduler
* Multiple Worker nodes
* Task distribution and execution
* Failure detection and recovery
* Observability and metrics
* Future AI-related workloads

The project serves as a practical playground for modern C++ and distributed systems engineering.

---

## Learning Goals

This project is intended to provide hands-on experience with:

### Modern C++

* C++20
* RAII
* Move semantics
* Smart pointers
* Templates
* STL containers and algorithms
* Ranges
* Error handling
* Unit testing

### Concurrency

* std::thread
* std::jthread
* std::mutex
* std::condition_variable
* std::atomic
* Thread pools
* Producer-consumer patterns

### Networking

* TCP sockets
* Client-server architectures
* Message serialization
* Connection handling
* Reconnection strategies

### Distributed Systems

* Worker registration
* Task scheduling
* Heartbeats
* Failure detection
* Retries
* Timeouts
* Recovery mechanisms

### Observability

* Structured logging
* Metrics
* Monitoring endpoints
* Dashboard visualizations

### AI Infrastructure Concepts

* Vector similarity search
* Embedding storage
* Nearest-neighbor retrieval
* Distributed computation workloads

---

## Planned Architecture

```text
                  +-------------+
                  | Scheduler   |
                  +-------------+
                         |
        +----------------+----------------+
        |                |                |
        v                v                v

    Worker 1        Worker 2        Worker 3
```

Workers register with the scheduler and periodically send heartbeats.

The scheduler assigns tasks, tracks progress, detects failures, and redistributes work when necessary.

---

## Project Phases

### Phase 1 — In-Memory Execution Engine

Goal:

* Scheduler
* Task queue
* Worker threads
* Basic task execution

Current task model:

* Generic callable tasks
* Task status and result tracking
* Promise/future-based result waiting

Status:

* 🟨 In Progress

---

### Phase 2 — TCP Communication

Goal:

* Scheduler process
* Worker process
* TCP communication
* JSON messages

Status:

* ⬜ Not Started

---

### Phase 3 — Heartbeats and Failure Detection

Goal:

* Worker registration
* Periodic heartbeats
* Dead worker detection
* Task reassignment

Status:

* ⬜ Not Started

---

### Phase 4 — Fault Tolerance

Goal:

* Task retries
* Execution timeouts
* Failure recovery

Status:

* ⬜ Not Started

---

### Phase 5 — Persistence

Goal:

* Persistent task state
* Recovery after restart
* Event logging

Status:

* ⬜ Not Started

---

### Phase 6 — CLI Tooling

Goal:

* Submit tasks
* Query task status
* List workers

Status:

* ⬜ Not Started

---

### Phase 7 — Metrics

Goal:

* Runtime metrics
* Performance counters
* Monitoring endpoints

Status:

* ⬜ Not Started

---

### Phase 8 — Dashboard

Goal:

* Worker overview
* Task overview
* System health visualization

Status:

* ⬜ Not Started

---

### Phase 9 — AI-Oriented Workloads

Goal:

* Similarity search
* Vector indexing
* Distributed processing jobs

Status:

* ⬜ Not Started

---

## Example Future Workflow

```text
Client
   |
   | Submit Task
   v

Scheduler
   |
   | Assign Task
   v

Worker
   |
   | Execute
   v

Result
```

Example:

```text
Submit task:
count_words("large_document.txt")

↓

Worker executes task

↓

Scheduler receives result

↓

Client queries task status
```

---

## Technology Stack

Current:

* C++20
* CMake
* GoogleTest

Planned:

* Asio / Boost.Asio
* nlohmann/json
* spdlog
* SQLite (optional)

---

## Building

```bash
mkdir build
cd build

cmake ..
cmake --build .
```

---

## Running Tests

```bash
ctest
```

---

## Current Status

This repository is under active development.

The focus at the moment is building the core in-memory scheduling engine before introducing networking and distributed-system features.

The implementation intentionally prioritizes clarity and learning over production-grade performance or feature completeness.

---

## License

MIT License
