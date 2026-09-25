# Limit Order Book and Matching Engine

A C++20 program that takes buy and sell orders, keeps them in an order book, and matches them when prices cross, producing trades.

> **Status:** Work in progress. See the [roadmap](#roadmap) for what's done and what's next.

## Overview

Every stock exchange runs an **order book**: a list of everyone who wants to buy (bids) and everyone who wants to sell (asks), sorted by price. When a new buy order's price is at or above the lowest ask, or a new sell order's price is at or below the highest bid, the **matching engine** turns the overlap into trades.

This project implements that core. The goal is to demonstrate C++ with a focus on data structures, memory layout, performance, and correctness under edge cases.

## Features

- [ ] **Limit orders:** buy or sell at a set price or better
- [ ] **Market orders:** buy or sell immediately at the best available price
- [ ] **Cancel** a resting order by ID
- [ ] **Modify** an order (cancel and replace)
- [ ] **Price-time priority:** best price fills first; at the same price, the earlier order fills first
- [ ] **Partial fills:** one order can fill against several resting orders, and any leftover quantity stays in the book
- [ ] **Event output:** trades, acknowledgements, and cancel confirmations sent through a listener interface
- [ ] **File replay:** run orders from a CSV file for repeatable tests and benchmarks

## Design highlights

The design below is planned. Once each piece is built, it will be measured against simpler alternatives.

- **Price levels:** bids and asks are each kept sorted by price. The baseline uses `std::map`, to be compared with a flat, vector-based layout that is friendlier to the CPU cache.
- **Orders within a level:** an intrusive doubly linked list preserves arrival order and allows O(1) removal.
- **O(1) cancels:** a hash map from order ID to its node jumps straight to the order without searching the book.
- **No allocation on the hot path:** orders come from a pre-allocated object pool rather than `new`/`delete`.
- **Compile-time side selection:** the book is templated on side (bid/ask), so the price comparison has no runtime branch.
- **Strong types:** `Price`, `Quantity`, and `OrderId` are distinct types, so they can't be mixed up by accident.
- **Modern C++:** concepts, `std::variant` for events, `std::span`, and `constexpr` where they make the code clearer.

## Project layout

```
.
├── include/orderbook/   # Public headers (types, orders, OrderBook, CSV reader)
├── src/                 # Engine implementation
├── apps/                # CLI for replaying order files
├── tests/               # Unit tests (GoogleTest)
├── bench/               # Performance benchmarks (Google Benchmark)
├── data/                # Sample order files
├── cmake/               # Shared compiler warnings and sanitizer flags
└── CMakeLists.txt
```

## Building and running

Requires a C++20 compiler (Clang 15+, GCC 12+, or Apple Clang) and CMake 3.20+. GoogleTest and Google Benchmark are downloaded automatically at configure time.

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
./build/apps/orderbook_replay data/sample_orders.csv
```

### Order file format

One command per line. Prices are integer ticks (for example, cents). Blank lines, `#` comments, and a header row are skipped.

```
ADD,<id>,<BUY|SELL>,<LIMIT|MARKET>,<price>,<quantity>
CANCEL,<id>
```

The price may be left empty for market orders, for example `ADD,5,BUY,MARKET,,150`.

### Build options

| Option | Default | Effect |
|---|---|---|
| `ORDERBOOK_BUILD_TESTS` | `ON` | Build the unit tests |
| `ORDERBOOK_BUILD_BENCHMARKS` | `OFF` | Build `orderbook_bench` |
| `ORDERBOOK_SANITIZE` | `OFF` | Compile with AddressSanitizer and UndefinedBehaviorSanitizer |

## Testing

- **Unit tests:** every matching case, including partial fills, sweeping several price levels, cancelling mid-queue, and empty-book market orders.
- **Randomized tests:** the engine is fed millions of random orders and its output is compared with a deliberately simple reference engine. Any mismatch is a bug.
- **Sanitizers:** a separate build runs the test suite under AddressSanitizer and UndefinedBehaviorSanitizer.

```bash
ctest --test-dir build --output-on-failure

# Sanitizer build
cmake -S . -B build-asan -DCMAKE_BUILD_TYPE=Debug -DORDERBOOK_SANITIZE=ON
cmake --build build-asan
ctest --test-dir build-asan --output-on-failure
```

## Benchmarks

Results will be added once the engine is implemented. Each optimization will be recorded with before-and-after numbers.

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DORDERBOOK_BUILD_BENCHMARKS=ON
cmake --build build
./build/bench/orderbook_bench
```

| Version | Throughput (orders/sec) | p50 latency | p99 latency | Machine |
|---|---|---|---|---|
| Baseline (`std::map` + `std::list`) | TBD | TBD | TBD | TBD |
| + Object pool and intrusive lists | TBD | TBD | TBD | TBD |
| + Flat price levels | TBD | TBD | TBD | TBD |

## Roadmap

1. **Core engine:** single-symbol book with limit, market, and cancel orders, plus full unit tests
2. **Usability:** O(1) cancel index, CSV replay, and a CLI that prints the book and trades
3. **Performance:** baseline benchmarks, then the object pool and intrusive lists, measured before and after
4. **Polish:** CI on GitHub Actions, sanitizer builds, and a finished README

**Stretch goals**
- Multiple symbols, each on its own thread, fed by a lock-free queue
- A TCP front end so clients can send orders over the network
- Stop orders, and immediate-or-cancel / fill-or-kill options
- A market-data feed publishing the best bid and ask prices

## Design decisions

This section will record the reasoning behind key tradeoffs as they're made, including what was measured, what alternatives were considered, and why one was chosen.

## Author

**Emily Adams**
